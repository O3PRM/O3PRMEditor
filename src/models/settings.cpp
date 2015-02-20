#include "models/settings.h"

#include <QApplication>
#include <QAction>
#include <QDir>
#include <QMenu>

namespace o3prm
{

    Settings::Settings(QObject *parent):
        __memory(5)
    {
        __settingsFile = QApplication::applicationDirPath().left(1) + ":/o3prmsettings.ini";
        __lastDir = QDir::homePath();
    }

    Settings::~Settings()
    {

    }

    void Settings::addProject(QString path)
    {
        int index = __recentProjects.indexOf(path);
        if (index != -1)
        {
            __recentProjects.removeAt(index);
        }
        __recentProjects.push_front(path);

        if (__recentProjects.count() > __memory)
        {
            __recentProjects.pop_back();
        }
    }

    const QList<QString>& Settings::recentProjects() const
    {
        return __recentProjects;
    }

    QString Settings::lastDir() const
    {
        return __lastDir;
    }

    void Settings::lastDir(const QString& dir)
    {
        __lastDir = dir;
    }

    void Settings::loadSettings()
    {
        QSettings settings("LIP6", "O3PRM Editor");
        int size = settings.beginReadArray("recent_projects");
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            auto path = settings.value("path").toString();
            addProject(path);
        }
        settings.endArray();

        lastDir(settings.value("last_dir").toString());
    }

    void Settings::saveSettings()
    {
        QSettings settings("LIP6", "O3PRM Editor");
        settings.beginWriteArray("recent_projects", __recentProjects.count());
        for (int i = 0; i < __recentProjects.count(); ++i)
        {
            settings.setArrayIndex(i);
            auto path = __recentProjects.at(i);
            settings.setValue("path", path);
        }
        settings.endArray();

        settings.setValue("last_dir", __lastDir);
    }
}



