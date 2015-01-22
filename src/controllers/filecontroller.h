#ifndef O3PRM_FILECONTROLLER_H
#define O3PRM_FILECONTROLLER_H

#include <QObject>
#include <QHash>
#include <QFileInfo>

#include "qsciscintillaextended.h"

// Cross reference, do not replace by an include directive.
class MainWindow; // uis/mainwindow.h

namespace o3prm
{
    /// FileController provides methods for managing files.
    /// All slots have an entry in the File menu.
    /// Public methods are for other controllers.
    /// This class controls the last open files too.
    class FileController : public QObject 
    {
        Q_OBJECT

        Q_PROPERTY( int numberMaxOfRecentsFiles \
                    READ numberMaxOfRecentsFiles \
                    WRITE setNumberMaxOfRecentsFiles );

        public:
            FileController( MainWindow * mw, QObject * parent = 0 );

            ~FileController();

            // Setup connections, call this once after all controlles in MainWindow
            // were properly initialised.
            void setupConnections();

            /// Return all open documents.
            QList<QsciScintillaExtended *> openDocuments() const;

            /// Return the current document, the open tab, or 0 if any.
            QsciScintillaExtended * currentDocument() const;

            /// Return true if there is at less one open document, false otherwise.
            bool hasCurrentDocument() const;

            /// Return true if \a file is open, false otherwise.
            bool isOpenFile( const QString & file ) const;

            /// Return the document corresponding to \a file, or 0 if this file is not open.
            QsciScintillaExtended * fileToDocument( const QString & file ) const;

            /// The max number of entry in the "recent files" list.
            int numberMaxOfRecentsFiles() const;

            void setNumberMaxOfRecentsFiles( int nb );

            QsciScintillaExtended * newDocument( const QString & title,
                    QsciScintillaExtended::Lexer lexer = QsciScintillaExtended::None );

        public slots:
            void newO3prmlFile();

            void newO3prmrFile();

            bool openFile( const QString & file = QString() );

            bool saveFile( QsciScintillaExtended * sci = 0, bool force = false );

            bool saveAsFile( QsciScintillaExtended * sci = 0, QString dir = "" );

            bool saveAllFiles();

            bool closeFile( int index = -1 );

            bool closeAllFiles();

            void updateMetadata( QsciScintillaExtended * sci = 0 );

            bool quit();

        signals:
            void fileOpened( const QString & filename, QsciScintillaExtended * document );

            void fileSaved( const QString & filename, QsciScintillaExtended * document );

            void fileRenamed( const QString & oldFilename,
                    const QString & newFilename,
                    QsciScintillaExtended * document );

            void fileClosed( const QString & filename );

        protected :
            /// Return a hashmap of open files,
            /// with the filename as key,
            /// and a pointer on the QsciScintillaExtended widget as value.
            const QHash<QString, QsciScintillaExtended *> & openFiles() const;

        protected slots:
            void addToRecentFiles( const QString & filename );

            void removeOfRecentFiles( const QString & filename );

            void saveFilesState();

            void onCurrentDocumentChanged( int index = -2 );

            void onDocumentRenamed( const QString & oldFilename, const QString & newFilename );

            void triggerInit();

        private:
            MainWindow * __mainWidget;

            struct PrivateData;
            PrivateData * d;

            /// Open the files include in filenameList.
            /// Returns false if a file could not be opened.
            void __openFiles(const QStringList& filenameList);

            /// Returns true if filename could be open, false otherwise.
            bool __openFile(const QFileInfo& filename);

            /// Show a warning dialog about the existence of a backup file.
            void __warnAboutBackupFile(const QFileInfo& file);

            /// Save a file and returns true if it succedded.
            bool __saveFile(QsciScintillaExtended * sci, const QString& filename);

            /// Return a path where the file should be saved.
            QString __findWhereToSave(QsciScintillaExtended * sci, const QString& dir);

            /// Shows a dialog starting at path that ask for a file name.
            QString __askForFileName(QsciScintillaExtended * sci, const QString& path);

            /// Returns the document and update index with the proper value.
            QsciScintillaExtended* __findDocument(int& index);

            /// Returns true if the file was saved or if the user did not want to save it.
            bool __askForSaveIfChanged(QsciScintillaExtended* sci, int index);

            /// Remove any existing metadata inforamtion in a file.
            void __cleanExistingMetadata(QsciScintillaExtended* sci);

            /// Generate the metadata information in a file.
            QString __generateMetadata();

            /// For each unsaved file, ask the user if he wants to save and saves it
            /// if necessary
            bool __askAndSaveFiles();

            /// Set the margin for the given document in the tab view.
            void __setMargin(QsciScintillaExtended* sci);
    };
}

#endif // O3PRM_FILECONTROLLER_H

