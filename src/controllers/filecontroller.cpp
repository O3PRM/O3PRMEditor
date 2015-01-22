#include "filecontroller.h"

#include "uis/mainwindow.h"
#include "ui_mainwindow.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QList>
#include <QAction>
#include <QSignalMapper>
#include <QDate>
#include <QDebug>

namespace o3prm
{
    struct FileController::PrivateData 
    {
        int numberMaxOfRecentsFiles;
        QMenu * recentsFiles;
        QList<QString> recentsFilesList;
        QHash<QString,QsciScintillaExtended *> openFiles;
        QList<QsciScintillaExtended *> openDocuments;
        QSignalMapper * recentsFilesMapper;
        QString lastDir;
    };

    /// Constructor
    FileController::FileController( MainWindow * mw, QObject * parent ) :
        QObject( parent ), __mainWidget( mw ), d( new PrivateData )
    {
        d->recentsFiles = new QMenu( mw );
        d->recentsFilesMapper = new QSignalMapper( this );

        // Construct "Recent files" menu
        __mainWidget->ui->actionRecentFiles->setMenu( d->recentsFiles );

        d->lastDir = QDir::homePath();
    }

    void FileController::setupConnections()
    {
        connect(d->recentsFilesMapper, SIGNAL( mapped( QString ) ),
                this, SLOT( openFile( QString ) ) );

        connect(__mainWidget->ui->actionOpenFile, SIGNAL( triggered() ),
                this, SLOT( openFile() ));
        connect(__mainWidget->ui->actionSaveFile, SIGNAL( triggered() ),
                this, SLOT( saveFile() ));
        connect(__mainWidget->ui->actionSaveAsFile, SIGNAL( triggered() ),
                this, SLOT( saveAsFile() ));
        connect(__mainWidget->ui->actionSaveAllFiles, SIGNAL( triggered() ),
                this, SLOT( saveAllFiles() ));
        connect(__mainWidget->ui->actionCloseFile, SIGNAL( triggered() ),
                this, SLOT( closeFile() ));
        connect(__mainWidget->ui->actionCloseAllFiles, SIGNAL( triggered() ),
                this, SLOT( closeAllFiles() ));
        connect(__mainWidget->ui->actionUpdateMetadata, SIGNAL( triggered() ),
                this, SLOT( updateMetadata() ));
        connect(__mainWidget->ui->tabWidget, SIGNAL( currentChanged( int ) ),
                this, SLOT( onCurrentDocumentChanged( int ) ));
        connect(__mainWidget->ui->tabWidget, SIGNAL( tabCloseRequested( int ) ),
                this, SLOT( closeFile( int ) ));
        connect(__mainWidget->pc, SIGNAL(projectClosed()),
                this, SLOT(closeAllFiles()));

        QTimer::singleShot( 100, this, SLOT( triggerInit() ) );
    }

    /// Destructor
    FileController::~FileController()
    {
        delete d;
    }

    void FileController::triggerInit() 
    {
        QSettings settings;
        settings.beginGroup( "file" );

        d->numberMaxOfRecentsFiles = settings.value("numberMaxOfRecentsFiles", 5).toInt();

        // Read the last closed files in settings
        int size = settings.beginReadArray("recentsFiles");

        for ( int i = 0; i < size ; i++ )
        {
            settings.setArrayIndex( i );
            addToRecentFiles( settings.value("file").toString() );
        }

        settings.endArray();

        // Reopen files don't close last time.
        size = settings.beginReadArray("openTabs");

        for ( int i = 0 ; i < size ; i++ )
        {
            settings.setArrayIndex( i );
            openFile( settings.value("file").toString() );
        }

        settings.endArray();
    }

    /// Return all open documents.
    QList<QsciScintillaExtended *> FileController::openDocuments() const
    {
        return d->openDocuments;
    }

    /// Return the current document, the open tab, or 0 if any.
    QsciScintillaExtended * FileController::currentDocument() const
    {
        return qobject_cast<QsciScintillaExtended *>( __mainWidget->ui->tabWidget->currentWidget() );
    }

    /// Return true if there is at less one open document, false otherwise.
    bool FileController::hasCurrentDocument() const 
    {
        return __mainWidget->ui->tabWidget->currentIndex() > -1;
    }

    /// Return true if \a file is open, false otherwise.
    bool FileController::isOpenFile( const QString & file ) const 
    {
        // Handle absolute and relative file path.
        QFileInfo info( file );

        if ( ! info.isFile() )
            return false;

        return openFiles().contains( info.absoluteFilePath() );
    }

    /// Return the document corresponding to \a file, or 0 if this file is not open.
    /// \a file must be an absoltue path.
    QsciScintillaExtended * FileController::fileToDocument( const QString & file ) const 
    {
        return openFiles().value( file,0 );
    }

    /// The max number of entry in the "recent files" list.
    int FileController::numberMaxOfRecentsFiles() const 
    {
        return d->numberMaxOfRecentsFiles;
    }

    /// The max number of entry in the "recent files" list.
    void FileController::setNumberMaxOfRecentsFiles( int nb ) 
    {
        for ( int i = d->numberMaxOfRecentsFiles ; i > nb && i > 0 ; i-- )
        {
            removeOfRecentFiles( d->recentsFilesList.last() );
        }

        if ( nb >= 0 ) 
        {
            d->numberMaxOfRecentsFiles = nb;
        } 
        else 
        {
            d->numberMaxOfRecentsFiles = 0;
        }
    }

    ///  Return a hashmap of open files,
    ///  with the filename as key,
    ///  and a pointer on the QsciScintillaExtended widget as value.
    const QHash<QString, QsciScintillaExtended *> & FileController::openFiles() const 
    {
        return d->openFiles;
    }

    ///
    /// Add a new tab and a new empty file.
    ///
    void FileController::newO3prmlFile() 
    {
        // We add a tab in the tabbar,
        newDocument( tr( "New O3prml" ), QsciScintillaExtended::O3prml );

        // and give it the focus
        auto idx = __mainWidget->ui->tabWidget->count() - 1;
        __mainWidget->ui->tabWidget->setCurrentIndex(idx);
        __mainWidget->ui->tabWidget->currentWidget()->setFocus();
    }

    ///  Add a new tab and a new empty file.
    void FileController::newO3prmrFile() 
    {
        // We add a tab in the tabbar,
        newDocument( tr( "New O3prmr" ), QsciScintillaExtended::O3prmr );

        // and give it the focus
        __mainWidget->ui->tabWidget->setCurrentIndex( __mainWidget->ui->tabWidget->count() - 1 );
        __mainWidget->ui->tabWidget->currentWidget()->setFocus();
    }

    ///  Open the file specified in argument,
    ///  or open a file dialog.
    ///  In the file dialog, several files may be open.
    ///  Return false if the file can not be open, true otherwise.
    bool FileController::openFile( const QString & filename ) 
    {
        QStringList filenameList;
        bool success = true;

        // If the filename is passed in argument, we open this file
        // otherwise we open a dialog
        if (filename.isEmpty())
        {
            auto msg = tr("Select files to open");
            auto filter = tr("O3prm Files(*.o3prml *.o3prmr)");
            filenameList = QFileDialog::getOpenFileNames(__mainWidget, msg, d->lastDir, filter);

            // If no files were selected, we stop here
            if (filenameList.isEmpty())
            {
                return false;
            }
        }
        else
        {
            filenameList.append(filename);
        }

        __openFiles(filenameList);
        saveFilesState();

        return success;
    }

    void FileController::__openFiles(const QStringList& filenameList)
    {
        // For each filename
        foreach(QString filename, filenameList)
        {
            QFileInfo fileinfo( filename );

            // If the file is already open, switch to it.
            if (openFiles().contains(fileinfo.absoluteFilePath()))
            {
                auto qsciExt = openFiles().value(fileinfo.absoluteFilePath());
                __mainWidget->ui->tabWidget->setCurrentWidget(qsciExt);
            }

            // Otherwhise try to open it
            else if (not __openFile(filename))
            {
                auto title = QString("Error opening file");
                auto msg = tr("Could not open %1.").arg(filename);
                QMessageBox::warning( __mainWidget, title, msg);
            }
        }
    }

    bool FileController::__openFile(const QFileInfo& fileinfo)
    {
        if (not fileinfo.isFile())
        {
            return false;
        }

        // Warn if there is a backup file
        if ( QFile::exists( fileinfo.absoluteFilePath() + ".bak" ) ) 
        {
            __warnAboutBackupFile(fileinfo);
        }

        // We open the file
        QFile file( fileinfo.absoluteFilePath() );

        if (file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            // We charge the file,
            auto text = QString::fromUtf8(file.readAll());
            file.close();

            auto sci = newDocument( fileinfo.fileName() );
            sci->setFilename( fileinfo.absoluteFilePath() );
            d->openFiles.insert( fileinfo.absoluteFilePath(), sci );
            sci->setText( text );
            sci->setModified( false );

            // If current tab is empty, open in it
            if ( hasCurrentDocument() && currentDocument()->text().isEmpty() ) 
            {
                int index = __mainWidget->ui->tabWidget->currentIndex();
                __mainWidget->ui->tabWidget->removeTab( index );
                __mainWidget->ui->tabWidget->insertTab( index, sci, fileinfo.fileName() );
            }

            // and give it the focus
            __mainWidget->ui->tabWidget->setCurrentWidget( sci );
            sci->setFocus();
            removeOfRecentFiles( fileinfo.absoluteFilePath() );

            emit fileOpened( fileinfo.absoluteFilePath(), sci );

            // Update last dir
            d->lastDir = fileinfo.absolutePath();
            return true;
        }
        return false;
    }

    void FileController::__warnAboutBackupFile(const QFileInfo& fileinfo)
    {
        auto title = QString( "File backup found" );
        auto msg = tr( "The file %1 was not properly closed. "
                "Restore or delete the %1.bak file to supress this warning."
                ).arg(fileinfo.fileName());

        QMessageBox::warning(__mainWidget, title, msg);
    }

    /// Save the file passed in argument, or the current tab.
    /// If the file has never been save, go to save as.
    bool FileController::saveFile(QsciScintillaExtended * sci, bool force) 
    {
        // If argument is 0, get the current document
        if ( sci == 0 )
        {
            sci = currentDocument();
        }

        // If there is any editor open
        if ( sci == 0 )
        {
            return false;
        }
        else if ( ! force && ( sci->text().isEmpty() || ! sci->isModified() ) )
        {
            return true;
        }

        QString filename = sci->filename();

        // If file never has been save, go to save as.
        if ( filename.isEmpty() )
        {
            return saveAsFile( sci );
        }
        if (not __saveFile(sci, filename))
        {
            auto title = tr("Error saving file");
            auto msg = tr("Could no save %1.").arg(filename) ;
            QMessageBox::warning(__mainWidget, title, msg);
            return false;
        }
        return true;
    }

    bool FileController::__saveFile(QsciScintillaExtended * sci, const QString& filename)
    {
        // Update metadatas ?
        QSettings settings;
        settings.beginGroup( "preferences/metadata" );

        if ( settings.value( "updateDateChecked",false ).toBool() )
        {
            updateMetadata( sci );
        }

        // Open the file in write mode
        QFile file( filename );

        if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        {
            return false;
        }

        // write inside
        file.write( sci->text().toUtf8() );
        file.close();
        sci->setModified( false );
        d->lastDir = QFileInfo( filename ).dir().path();

        emit fileSaved( filename, sci );
        saveFilesState();

        return true;
    }

    /// Ask a filename for the file, and save it.
    bool FileController::saveAsFile(QsciScintillaExtended * sci, QString dir ) 
    {
        // If argument is 0, get the current editor
        if ( sci == 0 )
        {
            sci = currentDocument();
        }

        if ( sci == 0 )
        {
            return false;
        }

        auto path = __findWhereToSave(sci, dir);
        auto filename = __askForFileName(sci, path);

        if (filename.isEmpty())
        {
            return false;
        }

        d->openFiles.remove(sci->filename()); // Do nothing if sci was not already saved.
        sci->setFilename(filename);
        d->openFiles.insert(filename, sci);

        return saveFile( sci, true );
    }

    QString FileController::__findWhereToSave(QsciScintillaExtended * sci, const QString& dir)
    {
        if ( dir.isEmpty() && ! sci->filename().isEmpty() )
        {
            return sci->filename();
        }
        else if ( dir.isEmpty() && __mainWidget->pc->currentProject() != 0 )
        {
            return __mainWidget->pc->currentProject()->dir().absolutePath();
        }
        else  // if ( dir.isEmpty() ) or everything else failed
        {
            return d->lastDir;
        }
    }

    QString FileController::__askForFileName(QsciScintillaExtended * sci, const QString& dir)
    {
        const QString o3prmlFilter = tr( "Class or system files (*.o3prml)" );
        const QString o3prmrFilter = tr( "Request files (*.o3prmr)" );

        QString filters, selectedFilter;

        // Set pre-selectionned filter
        if ( sci->lexerEnum() == QsciScintillaExtended::O3prmr )
        {
            filters = o3prmrFilter + ";;" + o3prmlFilter;
        }
        else
        {
            filters = o3prmlFilter + ";;" + o3prmrFilter;
        }

        auto title = tr( "Save file as..." );
        auto filename = QFileDialog::getSaveFileName(__mainWidget,
                title, dir, filters, &selectedFilter );

        if (selectedFilter == o3prmlFilter && ! filename.endsWith(".o3prml"))
        {
            filename += ".o3prml";
        }
        else if (selectedFilter == o3prmrFilter && ! filename.endsWith(".o3prmr"))
        {
            filename += ".o3prmr";
        }

        return filename;
    }

    /// Process saveFile on all open editor.
    /// Return true if all files have been saved, false otherwise.
    bool FileController::saveAllFiles() 
    {
        bool success = true;

        // Save the curretn file
        QsciScintillaExtended * temp = currentDocument();

        // Change current index allow to view the file if we must saveAs it.
        for ( int i = 0, size = __mainWidget->ui->tabWidget->count() ; i < size ; i++ ) 
        {
            __mainWidget->ui->tabWidget->setCurrentIndex( i ); // Set the document i to the current file
            success = success and saveFile();
        }

        // Reshow the previous current file.
        __mainWidget->ui->tabWidget->setCurrentWidget( temp );

        return success;
    }

    /// Close the current editor.
    /// If the file has been modified, ask for save it.
    /// Add the closed file to the "recentFiles" list.
    bool FileController::closeFile( int index )
    {
        QsciScintillaExtended * sci = __findDocument(index); // updates index

        // if there is an open file then of file has changed, ask for save
        if (sci and __askForSaveIfChanged(sci, index))
        {
            // Add the file to the "recentFiles" list.
            if ( ! sci->filename().isEmpty() )
            {
                addToRecentFiles( sci->filename() );
                d->openFiles.remove( sci->filename() );
            }

            d->openDocuments.removeOne( sci );
            __mainWidget->ui->tabWidget->removeTab( index );

            if ( __mainWidget->ui->tabWidget->count() == 0 ) 
            {
                __mainWidget->ui->menuEdit->setEnabled( false );
                __mainWidget->ui->menuSearch->setEnabled( false );
            }
            else if ( index == 0 )
            {
                __mainWidget->ui->tabWidget->widget( index )->setFocus();
            }
            else
            {
                __mainWidget->ui->tabWidget->widget( index-1 )->setFocus();
            }

            emit fileClosed( sci->filename() );

            sci->deleteLater();
            saveFilesState();

            return true;
        }
        return false;
    }

    QsciScintillaExtended* FileController::__findDocument(int& index)
    {
        QsciScintillaExtended* sci= 0;
        if ( index == -1 )
        {
            sci = currentDocument();
            index = __mainWidget->ui->tabWidget->currentIndex();
        }
        else
        {
            sci = qobject_cast<QsciScintillaExtended*>( __mainWidget->ui->tabWidget->widget( index ) );
        }
        return sci;
    }

    bool FileController::__askForSaveIfChanged(QsciScintillaExtended* sci, int index)
    {
        if ( sci->isModified() )
        {
            auto title = tr( "Save file ?" );
            auto filename = __mainWidget->ui->tabWidget->tabText(index);
            auto msg = tr( "%1 has unsaved modifications, do you want to save them ?" ).arg(filename);
            auto buttons = QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel;

            switch ( QMessageBox::question( __mainWidget, title, msg, buttons, QMessageBox::Ok ))
            {
                case QMessageBox::Ok :
                    {
                        // If we can't save the file, we don't close it.
                        if ( saveFile( sci ) )
                        {
                            return true;
                        }
                    }
                case QMessageBox::Discard :
                    {
                        return true;
                    }
                default:
                    {
                        return false;
                    }
            }
        }
        return true;
    }

    /// Process closeFile successively for each tab.
    /// Return true if all files have been closed, false otherwise.
    bool FileController::closeAllFiles()
    {
        bool success = true;

        for ( int i = 0, size = __mainWidget->ui->tabWidget->count() ; i < size ; i++ )
        {
            // Set the document i to the current file
            __mainWidget->ui->tabWidget->setCurrentIndex( i );

            // Close the current file
            if ( ! closeFile() ) 
            {
                success = false;
            }
        }

        return success;
    }

    /// Include comments at the beginning of the file,
    /// with licence info, autor info, and project info.
    void FileController::updateMetadata( QsciScintillaExtended * sci )
    {
        // If argument is 0, get the current editor
        if ( sci == 0 )
        {
            sci = currentDocument();
        }

        // If there is any editor open
        if ( sci || not sci->text().isEmpty() )
        {
            // If there is already metadatas erase them.
            __cleanExistingMetadata(sci);

            /// First line of metadata (comments)
            QString comment;
            comment += "/// ";
            for (int i = 0; i < 75; ++i)
            {
                comment += "*";
            }
            comment += "\n";

            auto metadata = __generateMetadata();
            foreach( QString s, metadata.split( "\n" ) )
            {
                comment += "///   " + s + "\n";
            }

            /// Last line of metadata (comments)
            comment += "/// ";
            for (int i = 0; i < 75; ++i)
            {
                comment += "*";
            }
            comment += "\n";

            // Add metadatas to document.
            sci->insertAt( comment,0,0 );
        }
    }

    void FileController::__cleanExistingMetadata(QsciScintillaExtended* sci)
    {
        if ( sci->text(0).startsWith("/// *****") )
        {
            sci->beginUndoAction();

            while (sci->text(0).startsWith("/// ") )
            {
                sci->setSelection(0, 0, 1, 0);
                // Remove the line
                sci->removeSelectedText(); 
            }

            sci->setSelection(0, 0, 1, 0);

            // Remove the blanc line
            sci->removeSelectedText(); 
            sci->endUndoAction();
        }
    }

    QString FileController::__generateMetadata()
    {
        QString metadata( "\n" );

        if ( __mainWidget->pc->isOpenProject() )
        {
            auto msg = tr("THIS FILE IS PART OF '%1' PROJECT.\n");
            metadata += msg.arg(__mainWidget->pc->currentProject()->name());
            metadata += "\n";
        }

        QSettings settings;
        settings.beginGroup( "preferences/metadata" );

        if (not settings.value( "autor", "" ).toString().isEmpty())
        {
            auto msg = tr("\tAUTOR(S) : %1\n");
            metadata += msg.arg(settings.value( "autor").toString());
            metadata += "\n";
        }

        if (not settings.value( "licence", "" ).toString().isEmpty())
        {
            metadata += tr("\tLICENCE :\n");
            metadata += settings.value( "licence" ).toString();
            metadata += "\n\n";
        }

        metadata += tr("\tLAST MODIFICATION : ");

        metadata += QDate::currentDate().toString(Qt::DefaultLocaleLongDate);
        metadata += "\n";
        return metadata;
    }

    /// Process saveAllFiles and, if success,
    /// close the application.
    /// Remove also all temp file from building.
    bool FileController::quit() 
    {
        __askAndSaveFiles();

        // Emit signal fileClosed to others controllers proceed to cleaning
        // if necessary (build controller clean temp file for exemple).
        foreach( const QString & s, d->openFiles.keys() )
        {
            emit fileClosed( s );
        }

        return true;
    }

    bool FileController::__askAndSaveFiles()
    {
        for ( int i = 0 ; i < __mainWidget->ui->tabWidget->count(); i++ ) 
        {
            __mainWidget->ui->tabWidget->setCurrentIndex( i );
            QsciScintillaExtended * sci = currentDocument();

            if ( sci->isModified() )
            {
                auto title = tr( "Save file ?" );
                auto filename = __mainWidget->ui->tabWidget->tabText(i);
                auto msg = tr( "%1 has unsavec modification, do you want to save it ?" ).arg(filename);
                auto buttons = QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel;

                int answer = QMessageBox::question(__mainWidget, title, msg, buttons, QMessageBox::Ok);

                switch (answer)
                {
                    case QMessageBox::Ok:
                        {
                            saveFile(sci);
                            break;
                        }
                    case QMessageBox::Discard:
                        {
                            break;
                        }
                    default:
                        {
                            return false;
                        }


                }
            }
        }
        return true;
    }

    void FileController::onCurrentDocumentChanged( int index ) 
    {
        QsciScintillaExtended * sci = 0;

        // if index == -2, signal was emit by a QsciScintilla
        if ( index == -2 ) 
        {
            sci = qobject_cast<QsciScintillaExtended*>( sender() );
            index = __mainWidget->ui->tabWidget->indexOf( sci );
        }
        // else, by QTabWidget
        else
        {
            sci = qobject_cast<QsciScintillaExtended*>( __mainWidget->ui->tabWidget->widget( index ) );
        }

        if ( sci != 0 ) 
        {
            __mainWidget->setWindowFilePath( sci->title() );
            __mainWidget->setWindowModified( sci->isModified() );
            __mainWidget->ui->tabWidget->setTabText( index, sci->title() + ( sci->isModified()?"*":"" ) );
            __mainWidget->ui->actionBuild->setEnabled( true );
            __mainWidget->ui->actionExecute->setEnabled( true );
            __mainWidget->vc->setCommandWidgetVisible( sci->lexerEnum() == QsciScintillaExtended::O3prmr );

        }
        else
        {
            __mainWidget->setWindowFilePath( QString() );
            __mainWidget->setWindowModified( false );
            __mainWidget->ui->actionExecute->setEnabled( false );
            __mainWidget->ui->actionBuild->setEnabled( false );
            __mainWidget->vc->setCommandWidgetVisible( false );
        }
    }

    /// When document are rename :
    ///     - if filename change, change all old filename reference to the new one
    ///       (filename often represent class/interface/system name);
    ///     - if path change, change package.
    ///
    /// TODO : change these references in all files.
    void FileController::onDocumentRenamed( const QString & oldFilename, const QString & newFilename ) 
    {
        QsciScintillaExtended * sci = qobject_cast<QsciScintillaExtended *>( sender() );

        if ( sci and not oldFilename.isEmpty() )
        {
            onCurrentDocumentChanged( __mainWidget->ui->tabWidget->indexOf( sci ) );

            // We update openFiles
            d->openFiles.remove( oldFilename );
            d->openFiles.insert( newFilename, sci );

            emit fileRenamed( oldFilename, newFilename, sci );
        }
    }

    /// Add the file the "recentFiles" list.
    void FileController::addToRecentFiles( const QString & filename ) 
    {
        if ( not (filename.isEmpty() || d->recentsFilesList.contains(filename)) )
        {
            d->recentsFilesList.append( filename );

            QAction * action = new QAction( QFileInfo( filename ).fileName(), __mainWidget );
            action->setData( filename );

            connect( action, SIGNAL( triggered() ), d->recentsFilesMapper, SLOT( map() ) );

            d->recentsFilesMapper->setMapping( action, filename );

            if ( d->recentsFiles->actions().isEmpty() )
            {
                d->recentsFiles->addAction( action );
            }
            else
            {
                d->recentsFiles->insertAction( d->recentsFiles->actions().first(), action );
            }

            if ( d->recentsFiles->actions().size() > d->numberMaxOfRecentsFiles )
            {
                d->recentsFiles->removeAction( d->recentsFiles->actions().last() );
            }
        }
    }

    /// Remove the file of the "recentFiles" list.
    void FileController::removeOfRecentFiles( const QString & filename ) 
    {
        if ( not (filename.isEmpty() || not d->recentsFilesList.contains( filename )) )
        {
            d->recentsFilesList.removeOne( filename );
            QAction * a = qobject_cast<QAction *>( d->recentsFilesMapper->mapping( filename ) );

            if ( a != 0 ) 
            {
                d->recentsFilesMapper->removeMappings( a );
                d->recentsFiles->removeAction( a );
            }
        }
    }


    /// Save files open and closed in settings.
    void FileController::saveFilesState() 
    {
        // Save the open files in settings
        QSettings settings;
        settings.beginGroup( "file" );

        settings.beginWriteArray( "openTabs" );
        int tabCount = __mainWidget->ui->tabWidget->count();

        for ( int i = 0, j = 0 ; i < tabCount ; i++ ) 
        {
            auto ptr = __mainWidget->ui->tabWidget->widget(i);
            auto sci = qobject_cast<QsciScintillaExtended*>(ptr);

            if ( ! sci->filename().isEmpty() ) 
            {
                settings.setArrayIndex( j++ );
                settings.setValue( "file",sci->filename() );
            }
        }
        settings.endArray();

        // Save the last closed files in settings
        int size = d->recentsFiles->actions().size();
        settings.beginWriteArray( "recentsFiles",size );

        for ( int i = 0 ; i < size ; i++ ) 
        {
            settings.setArrayIndex( i );
            settings.setValue( "file",d->recentsFiles->actions().at( i )->data() );
        }
        settings.endArray();
    }

    /// Create a new empty document, with all options, at end.
    QsciScintillaExtended * FileController::newDocument( const QString & title, QsciScintillaExtended::Lexer lexer ) 
    {
        // We add a tab in the tabbar,
        auto sci =  new QsciScintillaExtended( lexer, __mainWidget->ui->tabWidget );
        sci->setTitle( title );
        __mainWidget->ui->tabWidget->addTab( sci, title );
        connect( sci, SIGNAL( modificationChanged( bool ) ), this, SLOT( onCurrentDocumentChanged() ) );
        connect( sci, SIGNAL( filenameChanged( QString,QString ) ), this, SLOT( onDocumentRenamed( QString,QString ) ) );

        // Restore editable menu
        __mainWidget->ui->menuEdit->setEnabled( true );
        __mainWidget->ui->menuSearch->setEnabled( true );

        __setMargin(sci);

        QSettings settings;
        sci->setTabWidth( settings.value( "preferences/editor/tabwidth" ).toInt() );
        sci->setIndentationWidth( settings.value( "preferences/editor/indentWidth" ).toInt() );
        sci->setAutoIndent( settings.value( "preferences/editor/autoIndent" ).toBool() );
        sci->setTabIndents( settings.value( "preferences/editor/tabIndents" ).toBool() );
        sci->setBackspaceUnindents( settings.value( "preferences/editor/backspaceUnindents" ).toBool() );
        sci->setIndentationsUseTabs( settings.value( "preferences/editor/indentUseTabs" ).toBool() );

        d->openDocuments.append( sci );

        return sci;
    }

    void FileController::__setMargin(QsciScintillaExtended* sci)
    {
        if ( ! __mainWidget->vc->isLineNumbersVisible() )
        {
            sci->setMarginWidth( 0,0 );
        }

        if ( ! __mainWidget->vc->isBookmarksVisible() )
        {
            sci->setMarginWidth( 1,0 );
        }

        if ( ! __mainWidget->vc->isFoldMargingVisible() )
        {
            sci->setFolding( QsciScintillaExtended::NoFoldStyle );
        }

        if ( ! __mainWidget->vc->isIndentationGuidesVisible() )
        {
            sci->setIndentationGuides( false );
        }

        if ( __mainWidget->vc->isUnprintableCharactersVisible() ) 
        {
            sci->setWhitespaceVisibility(( QsciScintillaExtended::WhitespaceVisibility ) true );
            sci->setEolVisibility( true );
        }
    }

} // o3prm

