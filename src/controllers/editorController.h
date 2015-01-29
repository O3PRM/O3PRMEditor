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
            /// Class constructor
            explicit EditorController(MainWindow * mw);

            /// Class destructor
            ~EditorController();

            /// Setup connections, call this once after all controlles in MainWindow
            /// were properly initialised.
            void setupConnections();

            /// Return the list of opend documents
            QList<QsciScintillaExtended*> openDocuments() const;

            /// Return true if at least one document is opened
            bool hasCurrentDocument() const;

            /// Return the current document, the open tab, or 0 if any.
            QsciScintillaExtended* currentDocument() const;

            /// Return the document corresponding to \a file, or 0 if this file is not open.
            QsciScintillaExtended * fileToDocument( const QString & file ) const;

            /// Returns the document and update index with the proper value.
            QsciScintillaExtended* findDocument(int& index);

            /// Close all files, return true if it succeeded
            bool quit();

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

            /// Close old tab and open on renamed file
            void onDocumentRenamed(QString oldPath, QString newPath );

            /// Update tab to show that the document was changed
            void onCurrentDocumentChanged(int index=-2);

            /// Do an undo on the current document
            void undo();

            /// Do a redo on the current document
            void redo();

            /// Do a cut in the pastebin in the current document
            void cut();

            /// Do a copy in the pastebin in the current document
            void copy();

            /// Do a paste from the pastebin in the current document
            void paste();

            /// Select all the text in the current document.
            void selectAll();

            /// Add or removes comments from the selection in the current document
            void switchComment();

            /// Increase indentation in the current document
            void increaseIndentation();

            /// Decrease indentation in the current document
            void decreaseIndentation();

            /// Autocomplete the current word in the current document
            void autoComplete();

        signals:
            /// Emited when a file is opened
            void fileOpened(QString path, QsciScintillaExtended *sci);

            /// Emited when a file is saved 
            void fileSaved(QString path, QsciScintillaExtended *sci);

            /// Emited when a file is closed
            void fileClosed(QString path);

        private:
            /// The app main window.
            MainWindow* __mainWidget;

            /// A hash of all currently opened files.
            QHash<QString, QsciScintillaExtended *> __openFiles;

            /// Return the current document, the open tab, or 0 if any.
            QsciScintillaExtended* __currentDocument() const;

            /// Returns the document and update index with the proper value.
            QsciScintillaExtended* __findDocument(int& index);

            /// Open a file in the given position in the tab widget.
            bool __openFile(const QFileInfo& fileinfo, int pos=-1);

            /// Show/hide the editor menu.
            void __toggleEditorMenus(bool show);

            /// Initialize a QsciScintillaExtended instance before adding it to
            /// the tab widget.
            void __initialize(QsciScintillaExtended* sci,
                             const QString& title,
                             const QString& path,
                             const QString& text);

            /// Save the file, even if it is unmodified.
            bool __saveFile(QsciScintillaExtended * sci);

            /// Return a path where the file should be saved.
            QString __findWhereToSave(QsciScintillaExtended * sci, const QString& dir);

            /// Shows a dialog starting at path that ask for a file name.
            QString __askForFileName(QsciScintillaExtended * sci, const QString& path);

            /// Returns true if the file was saved or if the user did not want to save it.
            bool __askForSaveIfChanged(QsciScintillaExtended* sci, int index);
    };
}

#endif // O3PRM_EDITOR_CONTROLLER_H

