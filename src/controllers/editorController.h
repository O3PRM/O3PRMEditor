#ifndef O3PRM_EDITOR_CONTROLLER_H
#define O3PRM_EDITOR_CONTROLLER_H

#include <QObject>
#include <QHash>
#include <QFileInfo>

#include "qsciscintillaextended.h"

// Cross reference, do not replace by an include directive.
class MainWindow; // uis/mainwindow.h

namespace o3prm
{
    /// This controller handles all the events regarding files currently edited.
    class EditorController : public QObject 
    {
        Q_OBJECT

        public:
            EditorController(MainWindow * mw, QObject *parent=0);

            ~EditorController();

            // Setup connections, call this once after all controlles in MainWindow
            // were properly initialised.
            void setupConnections();

        public slots:
            void openFile(QString path);

            void saveFile(QString path);

            void closeFile(QString path);

            void saveAllFiles();

            void closeAllFiles();

        private:
            MainWindow *__mainWidget;

            QHash<QString, QsciScintillaExtended *> __openFiles;

            bool __openFile(const QFileInfo& fileinfo);

            void __toggleEditorMenus(bool show);

            void __initialize(QsciScintillaExtended* sci,
                             const QString& title,
                             const QString& path,
                             const QString& text);
    };
}

#endif // O3PRM_EDITOR_CONTROLLER_H

