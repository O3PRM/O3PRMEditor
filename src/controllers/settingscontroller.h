#ifndef O3PRM_SETTINGS_CONTROLLER_H
#define O3PRM_SETTINGS_CONTROLLER_H

#include <QObject>

#include "models/project.h"
#include "models/settings.h"

class MainWindow;

namespace o3prm
{
    class SettingsController: public QObject
    {
        Q_OBJECT

        public:
            explicit SettingsController(MainWindow *mw, QObject *parent=0);
            ~SettingsController();

            void setupConnections();

        signals:
            /// Emited when a recent project was clicked and must be opened.
            void recentProjectClicked(QString);

            void lastDirChanged(QString);

        private slots:

            void __onStart();

            void __onQuit();

            void __projectOpened(QString path);

            void __lastDir(QString path);

            void __onRecentProjectClicked();

        private:
            MainWindow *__mw;

            Settings *__model;

            void __updateRecentProjects();
    };

}

#endif // O3PRM_SETTINGS_CONTROLLER_H
