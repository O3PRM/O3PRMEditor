#include "controllers/editorcontroller.h"

#include <iostream>

#include <QMessageBox>
#include <QFileDialog>

#include "uis/mainwindow.h"
#include "controllers/projectcontroller.h"

namespace o3prm
{

    //////////////////////////////////////////////////////////////////////////
    //                            PUBLIC METHODS                            //
    //////////////////////////////////////////////////////////////////////////

    EditorController::EditorController(MainWindow *mw):
        QObject(mw), __mainWidget(mw)
    {

    }

    EditorController::~EditorController()
    {

    }

    void EditorController::setupConnections()
    {
        connect(__mainWidget->projectController(), SIGNAL( fileOpened( QString ) ),
                this, SLOT( openFile(QString) ) );

        // Connecting all the Edit entries from the main window
        connect(__mainWidget->mainWindow()->actionUndo, SIGNAL( triggered() ),
                this, SLOT( undo() ) );
        connect(__mainWidget->mainWindow()->actionRedo, SIGNAL( triggered() ),
                this, SLOT( redo() ) );
        connect(__mainWidget->mainWindow()->actionCut, SIGNAL( triggered() ),
                this, SLOT( cut() ) );
        connect(__mainWidget->mainWindow()->actionCopy, SIGNAL( triggered() ),
                this, SLOT( copy() ) );
        connect(__mainWidget->mainWindow()->actionPaste, SIGNAL( triggered() ),
                this, SLOT( paste() ) );
        connect(__mainWidget->mainWindow()->actionComment, SIGNAL( triggered() ),
                this, SLOT( switchComment() ) );
        connect(__mainWidget->mainWindow()->actionUncomment, SIGNAL( triggered() ),
                this, SLOT( switchComment() ) );
        connect(__mainWidget->mainWindow()->actionIncreaseIndentation, SIGNAL( triggered() ),
                this, SLOT( increaseIndentation() ) );
        connect(__mainWidget->mainWindow()->actionDecreaseIndentation, SIGNAL( triggered() ),
                this, SLOT( decreaseIndentation() ) );
        connect(__mainWidget->mainWindow()->actionAutocomplete, SIGNAL( triggered() ),
                this, SLOT( autocomplete() ) );
        connect(__mainWidget->mainWindow()->actionCloseFile, SIGNAL( triggered() ),
                this, SLOT( closeFile() ) );

        // Connection some of the File entries from the main window.
        connect(__mainWidget->mainWindow()->actionSave, SIGNAL( triggered() ),
                this, SLOT( saveFile() ));
        connect(__mainWidget->mainWindow()->actionSaveAll, SIGNAL( triggered() ),
                this, SLOT( saveAllFiles() ));

        // Connecting to the tab widget
        connect(__mainWidget->mainWindow()->tabWidget, SIGNAL( currentChanged( int ) ),
                this, SLOT( onCurrentDocumentChanged( int ) ));
        connect(__mainWidget->mainWindow()->tabWidget, SIGNAL( tabCloseRequested( int ) ),
                this, SLOT( closeFile( int ) ));

        // Connecting to the project controller.
        connect(__mainWidget->projectController(), SIGNAL(projectClosed()),
                this, SLOT(closeAllFiles()));
        connect(__mainWidget->projectController(), SIGNAL( fileRenamed(QString, QString) ),
                this, SLOT( onDocumentRenamed(QString, QString) ));
        connect(__mainWidget->projectController(), SIGNAL( packageRenamed(QString, QString) ),
                this, SLOT( onPackageRenamed(QString, QString) ));
        connect(__mainWidget->projectController(), SIGNAL( fileRemoved(QString) ),
                this, SLOT( closeFiles(QString) ));
        connect(__mainWidget->projectController(), SIGNAL( packageRemoved(QString) ),
                this, SLOT( closeFiles(QString) ));
        connect(__mainWidget->projectController(), SIGNAL(beforeInference()),
                this, SLOT( saveAllFiles() ));
    }

    QsciScintillaExtended* EditorController::currentDocument() const
    {
        auto obj = __mainWidget->mainWindow()->tabWidget->currentWidget();
        if (obj)
        {
            return qobject_cast<QsciScintillaExtended *>(obj);
        }
        return 0;
    }

    // TODO Changing index is ugly, need to refactor this
    QsciScintillaExtended* EditorController::findDocument(int& index)
    {
        if (index < 0 or index >= __mainWidget->mainWindow()->tabWidget->count())
        {
            index = __mainWidget->mainWindow()->tabWidget->currentIndex();
            return currentDocument();
        }
        auto obj = __mainWidget->mainWindow()->tabWidget->widget(index);
        return qobject_cast<QsciScintillaExtended*>(obj);
    }

    QsciScintillaExtended* EditorController::fileToDocument( const QString & file ) const 
    {
        return __openFiles.value(file, 0);
    }

    bool EditorController::quit() 
    {
        closeAllFiles();
        return __mainWidget->mainWindow()->tabWidget->count() == 0;
    }

    QList<QsciScintillaExtended*> EditorController::openDocuments() const
    {
        return __openFiles.values();
    }

    bool EditorController::hasCurrentDocument() const
    {
        return __mainWidget->mainWindow()->tabWidget->count();
    }

    //////////////////////////////////////////////////////////////////////////
    //                            PUBLIC SLOTS                              //
    //////////////////////////////////////////////////////////////////////////

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
        auto sci = currentDocument(); 

        // If there is any editor open
        if (sci)
        {
            // If file never has been save, go to save as
            if ( sci->filename().isEmpty() )
            {
                saveAsFile(sci);
            }
            if (not __saveFile(sci))
            {
                auto title = tr("Error saving file");
                auto msg = tr("Could no save %1.").arg(sci->filename()) ;
                QMessageBox::warning(__mainWidget, title, msg);
            }
        }
    }

    void EditorController::saveAsFile(QsciScintillaExtended *sci, QString dir)
    {
        // If argument is 0, get the current editor
        if ( sci == 0 )
        {
            sci = currentDocument();
        }

        if (sci)
        {
            auto path = __findWhereToSave(sci, dir);
            auto filename = __askForFileName(sci, path);

            if (not filename.isEmpty())
            {
                sci->setFilename(filename);

                if (not __saveFile(sci))
                {
                    auto title = tr("Error saving file");
                    auto msg = tr("Could no save %1.").arg(filename) ;
                    QMessageBox::warning(__mainWidget, title, msg);
                }
            }
        }
    }

    void EditorController::closeFiles(QString path)
    {
        QList<QString> keys(__openFiles.keys());
        for (int i = 0; i < keys.count(); ++i)
        {
            if (keys[i].startsWith(path))
            {
                auto sci = __openFiles.value(keys[i]);
                auto index = __mainWidget->mainWindow()->tabWidget->indexOf( sci );
                closeFile(index);
            }
        }
    }

    void EditorController::closeFile(int index)
    {
        auto sci = findDocument(index); // updates index

        // if there is an open file then of file has changed, ask for save
        if (sci and __askForSaveIfChanged(sci, index))
        {
            __mainWidget->mainWindow()->tabWidget->removeTab( index );

            if ( __mainWidget->mainWindow()->tabWidget->count() == 0 ) 
            {
                __toggleEditorMenus(false);
            }
            else if ( index == 0 )
            {
                auto new_index = (index?index-1:index); // previous tab or last one left
                __mainWidget->mainWindow()->tabWidget->widget( new_index )->setFocus();
            }

            emit fileClosed(sci->filename());
            __openFiles.remove(sci->filename());
            sci->deleteLater();
        }
    }

    void EditorController::saveAllFiles()
    {
        bool success = true;
        int count = __mainWidget->mainWindow()->tabWidget->count();
        for ( int i = 0; i < count; i++ ) 
        {
            auto sci = findDocument(i);
            success = success and __saveFile(sci);
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

    void EditorController::onDocumentRenamed(QString oldPath, QString newPath )
    {
        std::cout << "onDocumentRenamed(" << oldPath.toStdString() << ", " << newPath.toStdString() << ")" << std::endl;
        auto sci = __openFiles.value(oldPath, 0);
        if (sci)
        {
            auto index = __mainWidget->mainWindow()->tabWidget->indexOf( sci );
            closeFile(index);
            QFileInfo info(newPath);
            __openFile(newPath, index);
        }
    }

    void EditorController::onPackageRenamed(QString oldPath, QString newPath )
    {
        QList<QString> keys(__openFiles.keys());
        for (int i = 0; i < keys.count(); ++i)
        {
            if (keys[i].startsWith(oldPath))
            {
                auto sci = __openFiles.value(keys[i]);
                auto index = __mainWidget->mainWindow()->tabWidget->indexOf( sci );
                closeFile(index);
                __openFile(keys[i].replace(oldPath, newPath), index);
            }
        }
    }

    void EditorController::onCurrentDocumentChanged( int index ) 
    {
        QsciScintillaExtended * sci = 0;

        // if index == -2, signal was emit by a QsciScintilla
        if ( index == -2 ) 
        {
            sci = qobject_cast<QsciScintillaExtended*>( sender() );
            index = __mainWidget->mainWindow()->tabWidget->indexOf( sci );
        }
        // else, by QTabWidget
        else if (index >= 0 and index < __mainWidget->mainWindow()->tabWidget->count())
        {
            auto obj = __mainWidget->mainWindow()->tabWidget->widget( index );
            sci = qobject_cast<QsciScintillaExtended*>(obj);
        }
        else
        {
            sci = currentDocument();
            index = __mainWidget->mainWindow()->tabWidget->indexOf( sci );
        }

        if (sci) 
        {
            __mainWidget->setWindowFilePath( sci->title() );
            __mainWidget->setWindowModified( sci->isModified() );
            __mainWidget->mainWindow()->tabWidget->setTabText( index, sci->title() + ( sci->isModified()?"*":"" ) );
            __mainWidget->mainWindow()->actionBuild->setEnabled( true );
            __mainWidget->mainWindow()->actionExecute->setEnabled( true );
            //__mainWidget->viewController()->setCommandWidgetVisible( sci->lexerEnum() == QsciScintillaExtended::O3prmr );
        }
        else
        {
            __mainWidget->setWindowFilePath( QString() );
            __mainWidget->setWindowModified( false );
            __mainWidget->mainWindow()->actionExecute->setEnabled( false );
            __mainWidget->mainWindow()->actionBuild->setEnabled( false );
            //__mainWidget->viewController()->setCommandWidgetVisible( false );
        }
    }

    void EditorController::undo()
    {
        if ( currentDocument()) 
        {
            currentDocument()->undo();
        }
    }

    void EditorController::redo() 
    {
        if ( currentDocument()) 
        {
            currentDocument()->redo();
        }
    }

    void EditorController::cut() 
    {
        if ( currentDocument()) 
        {
            currentDocument()->cut();
        }
    }

    void EditorController::copy() 
    {
        if ( currentDocument()) 
        {
            currentDocument()->copy();
        }
    }

    void EditorController::paste() 
    {
        if ( currentDocument()) 
        {
            currentDocument()->paste();
        }
    }

    void EditorController::selectAll()
    {
        if ( currentDocument()) 
        {
            currentDocument()->selectAll();
        }
    }

    void EditorController::switchComment() {
        if ( currentDocument()) 
        {
            currentDocument()->switchComment();
        }
    }

    void EditorController::increaseIndentation()
    {
        if ( currentDocument()) 
        {
            currentDocument()->indent();
        }
    }

    void EditorController::decreaseIndentation()
    {
        if ( currentDocument()) 
        {
            currentDocument()->unindent();
        }
    }

    void EditorController::autocomplete() 
    {
        // if ( currentDocument() )
        // {
        //     if ( ! d->prmModel.isNull() && mw->pc->hasProject() ) {
        //         mw->bc->updateModel();

        //         if ( mw->ui->commandLineEdit->hasFocus() ) {
        //             mw->ui->commandLineEdit->setCompleter( d->completer );
        //             mw->ui->commandLineEdit->autocomplete();
        //         } else {
        //             mw->fc->currentDocument()->setCompleter( d->completer );
        //             mw->fc->currentDocument()->autoCompleteFromCompleter();
        //         }
        //     } else if ( ! mw->ui->commandLineEdit->hasFocus() )
        //         mw->fc->currentDocument()->autoCompleteFromAll();
        // }
    }

    //////////////////////////////////////////////////////////////////////////
    //                           PRIVATE METHODS                            //
    //////////////////////////////////////////////////////////////////////////

    void EditorController::__toggleEditorMenus(bool show)
    {
        // Restore editable menu
        __mainWidget->mainWindow()->editMenu->setEnabled( true );
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

        //if ( ! __mainWidget->viewController()->isLineNumbersVisible() )
        //{
        //    sci->setMarginWidth( 0,0 );
        //}

        //if ( ! __mainWidget->viewController()->isBookmarksVisible() )
        //{
        //    sci->setMarginWidth( 1,0 );
        //}

        //if ( __mainWidget->viewController()->isUnprintableCharactersVisible() ) 
        //{
        //    sci->setWhitespaceVisibility(( QsciScintillaExtended::WhitespaceVisibility ) true );
        //    sci->setEolVisibility( true );
        //}

        QSettings settings;
        sci->setTabWidth( settings.value( "preferences/editor/tabwidth" ).toInt() );
        sci->setIndentationWidth( settings.value( "preferences/editor/indentWidth" ).toInt() );
        sci->setAutoIndent( settings.value( "preferences/editor/autoIndent" ).toBool() );
        sci->setTabIndents( settings.value( "preferences/editor/tabIndents" ).toBool() );
        sci->setBackspaceUnindents( settings.value( "preferences/editor/backspaceUnindents" ).toBool() );
        sci->setIndentationsUseTabs( settings.value( "preferences/editor/indentUseTabs" ).toBool() );

        connect(sci, SIGNAL( modificationChanged( bool ) ),
                this, SLOT( onCurrentDocumentChanged() ) );
        connect(sci, SIGNAL( filenameChanged( QString,QString ) ),
                this, SLOT( onDocumentRenamed( QString,QString ) ) );
    }

    bool EditorController::__openFile(const QFileInfo& fileinfo, int index)
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
                __mainWidget->mainWindow()->tabWidget->insertTab(index, sci, fileinfo.fileName());
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

    bool EditorController::__saveFile(QsciScintillaExtended * sci)
    {
        QFile file( sci->filename() );

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
        const QString o3prmlFilter = tr( "Class or system files (*.o3prm)" );
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

        if (selectedFilter == o3prmlFilter and not filename.endsWith(".o3prm"))
        {
            filename += ".o3prm";
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
                        if ( __saveFile(sci) )
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

