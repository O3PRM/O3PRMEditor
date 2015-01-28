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
            /// Open file at path in a new tab
            void openFile(QString path);

            /// Save the current document
            void saveFile();

            /// Copy the file somewhere
            void saveAsFile(QsciScintillaExtended *sci=0, QString dir="");

            /// Save all files
            void saveAllFiles();

            /// Close file at index or curent file if index == -1
            void closeFile(int index=-1);

            /// Close all files
            void closeAllFiles();

        signals:
            /// Emited when a file is opened
            void fileOpened(QString path, QsciScintillaExtended *sci);

            /// Emited when a file is saved 
            void fileSaved(QString path, QsciScintillaExtended *sci);

            /// Emited when a file is closed
            void fileClosed(QString path);

        private:
            MainWindow *__mainWidget;

            QHash<QString, QsciScintillaExtended *> __openFiles;

            /// Return the current document, the open tab, or 0 if any.
            QsciScintillaExtended* __currentDocument() const;

            /// Returns the document and update index with the proper value.
            QsciScintillaExtended* __findDocument(int& index);

            bool __openFile(const QFileInfo& fileinfo);

            void __toggleEditorMenus(bool show);

            void __initialize(QsciScintillaExtended* sci,
                             const QString& title,
                             const QString& path,
                             const QString& text);

            /// Save the file, even if it is unmodified.
            bool __saveFile(const QString& filename, QsciScintillaExtended * sci);

            /// Return a path where the file should be saved.
            QString __findWhereToSave(QsciScintillaExtended * sci, const QString& dir);

            /// Shows a dialog starting at path that ask for a file name.
            QString __askForFileName(QsciScintillaExtended * sci, const QString& path);

            /// Returns true if the file was saved or if the user did not want to save it.
            bool __askForSaveIfChanged(QsciScintillaExtended* sci, int index);
    };
}

#endif // O3PRM_EDITOR_CONTROLLER_H

