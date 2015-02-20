#ifndef O3PRM_SETTINGS_MODEL_H
#define O3PRM_SETTINGS_MODEL_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QList>
#include <QHash>

#include "models/project.h"

namespace o3prm
{
    class Settings
    {
        public:
            explicit Settings(QObject *parent=0);
            ~Settings();

        public :
            /// Used to know if a project was opened to add to the list
            /// of recent projects.
            void addProject(QString path);

            const QList<QString>& recentProjects() const;

            QString lastDir() const;

            void lastDir(const QString& dir);

            void loadSettings();

            void saveSettings();

        private:
            int __memory;

            QList<QString> __recentProjects;

            /// The last directory in which the user opened something.
            QString __lastDir;

            QString __settingsFile;
    };
}

#endif // O3PRM_SETTINGS_MODEL_H
