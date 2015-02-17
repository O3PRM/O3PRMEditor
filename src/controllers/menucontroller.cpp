#include "controllers/menucontroller.h"

#include <QStatusBar>

#include "uis/mainwindow.h"
#include "controllers/editorcontroller.h"

namespace o3prm
{

    MenuController::MenuController(MainWindow* mw, QObject* parent):
        QObject(parent), __mw(mw)
    {

    }

    MenuController::~MenuController()
    {

    }

    void MenuController::setupConnections()
    {
        connect(__mw->mainWindow()->actionShowToolbar, SIGNAL(toggled(bool)),
                this, SLOT(setToolBarVisibility(bool)));

        connect(__mw->mainWindow()->actionShowStatusBar, SIGNAL(toggled(bool)),
                this, SLOT(setStatusBarVisibility(bool)));

        connect(__mw->mainWindow()->actionShowProjectExplorator, SIGNAL(toggled(bool)),
                this, SLOT(setProjectExploratorVisibility(bool)));

        connect(__mw->mainWindow()->actionBigger, SIGNAL(triggered()),
                this, SLOT(increaseZoom()));
        connect(__mw->mainWindow()->actionSmaller, SIGNAL(triggered()),
                this, SLOT(decreaseZoom()));
        connect(__mw->mainWindow()->actionDefaultSize, SIGNAL(triggered()),
                this, SLOT(normalZoom()));

        connect(__mw->mainWindow()->actionShowLineNumbers, SIGNAL(toggled(bool)),
                this, SLOT(setLineNumbersVisibility(bool)));
        connect(__mw->mainWindow()->actionShowBookmarks, SIGNAL(toggled(bool)),
                this, SLOT(setBookmarksVisibility(bool)));
        connect(__mw->mainWindow()->actionShowInvisibleCharacters, SIGNAL(toggled(bool)),
                this, SLOT(setUnprintableCharactersVisibility(bool)));
    }

    void MenuController::setStatusBarVisibility( bool visible )
    {
        if (visible)
        {
            __mw->statusBar()->show();
        }
        else
        {
            __mw->statusBar()->hide();
        }
    }

    void MenuController::setToolBarVisibility( bool visible )
    {
        if (visible)
        {
            __mw->mainWindow()->mainToolBar->show();
        }
        else
        {
            __mw->mainWindow()->mainToolBar->hide();
        }        
    }

    void MenuController::setProjectExploratorVisibility( bool visible )
    {
        if (visible)
        {
            __mw->mainWindow()->projectExplorator->show();
        }
        else
        {
            __mw->mainWindow()->projectExplorator->hide();
        }        
    }

    void MenuController::increaseZoom()
    {
        if ( __mw->editorController()->hasCurrentDocument() )
        {
            __mw->editorController()->currentDocument()->zoomIn();
        }
    }

    void MenuController::decreaseZoom()
    {
        if ( __mw->editorController()->hasCurrentDocument() )
        {
            __mw->editorController()->currentDocument()->zoomOut();
        }
    }

    void MenuController::normalZoom()
    {
        if ( __mw->editorController()->hasCurrentDocument() )
        {
            __mw->editorController()->currentDocument()->resetZoom();
        }
    }

    void MenuController::setLineNumbersVisibility( bool checked )
    {
    }

    void MenuController::setBookmarksVisibility( bool checked )
    {
    }

    void MenuController::setUnprintableCharactersVisibility( bool checked )
    {
    }

} // O3PRM
