#include "controllers/editorController.h"

#include <QMessageBox>
#include <QFileDialog>

#include "uis/mainwindow.h"
#include "controllers/projectcontroller.h"
#include "controllers/viewcontroller.h"

namespace o3prm
{
    EditorController::EditorController(MainWindow *mw, QObject *parent):
        QObject(parent), __mainWidget(mw)
    {

    }

    EditorController::~EditorController()
    {

    }

    void EditorController::setupConnections()
    {
        connect(__mainWidget->projectController(), SIGNAL( fileOpened( QString ) ),
                this, SLOT( openFile(QString) ) );
    }

    void EditorController::openFile(QString path)
    {
        QFileInfo fileinfo(path);
        if (not __openFile(fileinfo))
        {
            auto title = tr("Error opening file");
            auto msg = tr("Could not open %1.").arg(path);
            QMessageBox::warning( __mainWidget, title, msg);
        }
    }

    void EditorController::saveFile()
    {
        // If argument is 0, get the current document
        auto sci = __currentDocument(); 

        // If there is any editor open
        if (sci)
        {
            QString filename = sci->filename();
            // If file never has been save, go to save as
            if ( filename.isEmpty() )
            {
                saveAsFile(sci);
            }
            if (not __saveFile(filename, sci))
            {
                auto title = tr("Error saving file");
                auto msg = tr("Could no save %1.").arg(filename) ;
                QMessageBox::warning(__mainWidget, title, msg);
            }
        }
    }

    void EditorController::saveAsFile(QsciScintillaExtended *sci, QString dir)
    {
        // If argument is 0, get the current editor
        if ( sci == 0 )
        {
            sci = __currentDocument();
        }

        if (sci)
        {
            auto path = __findWhereToSave(sci, dir);
            auto filename = __askForFileName(sci, path);

            if (not filename.isEmpty())
            {
                sci->setFilename(filename);

                if (not __saveFile(filename, sci))
                {
                    auto title = tr("Error saving file");
                    auto msg = tr("Could no save %1.").arg(filename) ;
                    QMessageBox::warning(__mainWidget, title, msg);
                }
            }
        }
    }

    void EditorController::closeFile(int index)
    {
        auto sci = __findDocument(index); // updates index

        // if there is an open file then of file has changed, ask for save
        if (sci and __askForSaveIfChanged(sci, index))
        {
            __mainWidget->mainwindow()->tabWidget->removeTab( index );

            if ( __mainWidget->mainwindow()->tabWidget->count() == 0 ) 
            {
                __toggleEditorMenus(false);
            }
            else if ( index == 0 )
            {
                auto new_index = (index?index-1:index); // previous tab or last one left
                __mainWidget->mainwindow()->tabWidget->widget( new_index )->setFocus();
            }

            QFileInfo info(sci->filename()); // TODO: this may not be necessary
            emit fileClosed(info.absolutePath());
            sci->deleteLater();
        }
    }

    void EditorController::saveAllFiles()
    {
        bool success = true;
        for ( int i = 0; i < __mainWidget->mainWindow()->tabWidget->count(); i++ ) 
        {
            auto sci = __findDocument(i);
            success = success and __saveFile(sci->filename(), sci);
        }

        if (not success)
        {
            auto title = tr("Error saving files");
            auto msg = tr("Could not save all files.");
            QMessageBox::warning(__mainWidget, title, msg);
        }
    }

    void EditorController::closeAllFiles()
    {
        for ( int i = __mainWidget->mainWindow()->tabWidget->count() - 1; i >= 0 ; i-- ) 
        {
            closeFile(i);
        }
    }

    QsciScintillaExtended* EditorController::__currentDocument() const
    {
        return qobject_cast<QsciScintillaExtended *>( __mainWidget->mainwindow()->tabWidget->currentWidget() );
    }

    QsciScintillaExtended* EditorController::__findDocument(int& index)
    {
        if (index >= 0 and index < __mainWidget->mainWindow()->tabWidget->count())
        {
            index = __mainWidget->mainWindow()->tabWidget->currentIndex();
            return __currentDocument();
        }
        auto obj = __mainWidget->mainWindow()->tabWidget->widget(index);
        return qobject_cast<QsciScintillaExtended*>(obj);
    }

    void EditorController::__toggleEditorMenus(bool show)
    {
        // Restore editable menu
        __mainWidget->mainWindow()->menuEdit->setEnabled( true );
        __mainWidget->mainWindow()->menuSearch->setEnabled( true );
    }

    void EditorController::__initialize(QsciScintillaExtended* sci,
                                        const QString& title,
                                        const QString& path,
                                        const QString& text)
    {
        sci->setTitle( title );
        sci->setFilename( path );
        sci->setText( text );
        sci->setModified( false );

        if ( ! __mainWidget->viewController()->isLineNumbersVisible() )
        {
            sci->setMarginWidth( 0,0 );
        }

        if ( ! __mainWidget->viewController()->isBookmarksVisible() )
        {
            sci->setMarginWidth( 1,0 );
        }

        if ( ! __mainWidget->viewController()->isFoldMargingVisible() )
        {
            sci->setFolding( QsciScintillaExtended::NoFoldStyle );
        }

        if ( ! __mainWidget->viewController()->isIndentationGuidesVisible() )
        {
            sci->setIndentationGuides( false );
        }

        if ( __mainWidget->viewController()->isUnprintableCharactersVisible() ) 
        {
            sci->setWhitespaceVisibility(( QsciScintillaExtended::WhitespaceVisibility ) true );
            sci->setEolVisibility( true );
        }

        QSettings settings;
        sci->setTabWidth( settings.value( "preferences/editor/tabwidth" ).toInt() );
        sci->setIndentationWidth( settings.value( "preferences/editor/indentWidth" ).toInt() );
        sci->setAutoIndent( settings.value( "preferences/editor/autoIndent" ).toBool() );
        sci->setTabIndents( settings.value( "preferences/editor/tabIndents" ).toBool() );
        sci->setBackspaceUnindents( settings.value( "preferences/editor/backspaceUnindents" ).toBool() );
        sci->setIndentationsUseTabs( settings.value( "preferences/editor/indentUseTabs" ).toBool() );

        connect( sci, SIGNAL( modificationChanged( bool ) ), this, SLOT( onCurrentDocumentChanged() ) );
        connect( sci, SIGNAL( filenameChanged( QString,QString ) ), this, SLOT( onDocumentRenamed( QString,QString ) ) );
    }

    bool EditorController::__openFile(const QFileInfo& fileinfo)
    {
        // If already opend, set focus on it
        if (__openFiles.contains(fileinfo.absoluteFilePath()))
        {
            auto sci = __openFiles.value(fileinfo.absoluteFilePath());
            __mainWidget->mainWindow()->tabWidget->setCurrentWidget(sci);
            return true;
        }
        // File not open, lets try to open it
        else
        {
            if (not fileinfo.isFile())
            {
                return false;
            }

            // We open the file
            QFile file( fileinfo.absoluteFilePath() );

            if (file.open( QIODevice::ReadOnly | QIODevice::Text ) )
            {
                // We charge the file,
                auto text = QString::fromUtf8(file.readAll());
                file.close();

                // We create the QScintilla view of the file
                auto lexer = QsciScintillaExtended::None;
                auto sci =  new QsciScintillaExtended( lexer, __mainWidget->mainWindow()->tabWidget );
                __initialize(sci, fileinfo.fileName(), fileinfo.absoluteFilePath(), text);

                // Add the file to the view
                int index = __mainWidget->mainWindow()->tabWidget->addTab( sci, fileinfo.fileName() );
                __mainWidget->mainWindow()->tabWidget->setCurrentWidget( sci );
                sci->setFocus();

                // Update members and view
                __openFiles.insert( fileinfo.absoluteFilePath(), sci );
                __toggleEditorMenus(true);

                emit fileOpened(fileinfo.absolutePath(), sci);
                return true;
            }
            return false;
        }
    }

    /// TODO: first arg should be removed
    bool EditorController::__saveFile(const QString& filename, QsciScintillaExtended * sci)
    {
        QFile file( filename );

        if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
        {
            file.write( sci->text().toUtf8() );
            file.close();
            sci->setModified( false );

            QFileInfo info(file);
            emit fileSaved(info.absolutePath(), sci);

            return true;
        }
        return false;
    }

    QString EditorController::__findWhereToSave(QsciScintillaExtended * sci, const QString& dir)
    {
        if ( dir.isEmpty() and not sci->filename().isEmpty() )
        {
            return sci->filename();
        }
        else if ( dir.isEmpty() and __mainWidget->projectController()->currentProject() )
        {
            return __mainWidget->projectController()->currentProject()->dir().absolutePath();
        }
        else  // if ( dir.isEmpty() ) or everything else failed
        {
            return QDir::homePath();
        }
    }

    QString EditorController::__askForFileName(QsciScintillaExtended * sci, const QString& dir)
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
        auto filename = QFileDialog::getSaveFileName(__mainWidget, title, dir, filters, &selectedFilter);

        if (selectedFilter == o3prmlFilter and not filename.endsWith(".o3prml"))
        {
            filename += ".o3prml";
        }
        else if (selectedFilter == o3prmrFilter and not filename.endsWith(".o3prmr"))
        {
            filename += ".o3prmr";
        }

        return filename;
    }

    bool EditorController::__askForSaveIfChanged(QsciScintillaExtended* sci, int index)
    {
        if ( sci->isModified() )
        {
            auto title = tr( "Save file ?" );
            auto filename = __mainWidget->mainWindow()->tabWidget->tabText(index);
            auto msg = tr( "%1 has unsaved modifications, do you want to save them ?" ).arg(filename);
            auto buttons = QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel;

            switch ( QMessageBox::question( __mainWidget, title, msg, buttons, QMessageBox::Ok ))
            {
                case QMessageBox::Ok :
                    {
                        // If we can't save the file, we don't close it.
                        if ( __saveFile(sci->filename(), sci) )
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

} // o3prm

