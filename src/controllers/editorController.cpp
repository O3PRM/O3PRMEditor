#include "controllers/editorController.h"

#include <QMessageBox>

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

                return true;
            }
            return false;
        }
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

    void EditorController::saveFile(QString path)
    {

    }

    void EditorController::closeFile(QString path)
    {

    }

    void EditorController::saveAllFiles()
    {

    }

    void EditorController::closeAllFiles()
    {
    }

} // o3prm

