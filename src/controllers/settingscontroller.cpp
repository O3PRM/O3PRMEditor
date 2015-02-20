#include "controllers/settingscontroller.h"

#include <QAction>

#include "uis/mainwindow.h"
#include "controllers/projectcontroller.h"

namespace o3prm
{
    SettingsController::SettingsController(MainWindow* mw, QObject *parent):
        QObject(parent), __mw(mw), __model(new Settings(this))
    {

    }

    SettingsController::~SettingsController()
    {

    }

    void SettingsController::setupConnections()
    {
        connect(__mw->projectController(), SIGNAL(projectLoaded(QString)),
                this, SLOT(__projectOpened(QString)));

        connect(__mw->projectController(), SIGNAL(projectLoaded(QString)),
                this, SLOT(__lastDir(QString)));

        connect(__mw, SIGNAL(started()),
                this, SLOT(__onStart()));
        connect(__mw, SIGNAL(closing()),
                this, SLOT(__onQuit()));
    }

    void SettingsController::__onStart()
    {
        __model->loadSettings();

        __updateRecentProjects();
        emit __lastDir(__model->lastDir());
    }

    void SettingsController::__onQuit()
    {
        __model->saveSettings();
    }

    void SettingsController::__projectOpened(QString path)
    {
        __model->addProject(path);

        __updateRecentProjects();
    }

    void SettingsController::__updateRecentProjects()
    {
        auto menu = __mw->mainWindow()->menuRecentProjects;
        menu->clear();

        for (int i = 0; i < __model->recentProjects().count(); ++i)
        {
            auto action = menu->addAction(__model->recentProjects()[i]);
            connect(action, SIGNAL(triggered()),
                    this, SLOT(__onRecentProjectClicked()));
        }
    }

    void SettingsController::__lastDir(QString path)
    {
        QFileInfo info(path);
        if (info.exists())
        {
            QString dir = path;
            if (not info.isDir())
            {
                dir = info.dir().absolutePath();
            }
            __model->lastDir(path);
            emit lastDirChanged(dir);
        }
    }

    void SettingsController::__onRecentProjectClicked()
    {
        QAction* action = static_cast<QAction*>(QObject::sender());
        QString path = action->text();
        emit recentProjectClicked(path);
    }

}
