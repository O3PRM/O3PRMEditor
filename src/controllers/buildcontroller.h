#ifndef O3PRM_BUILDCONTROLLER_H
#define O3PRM_BUILDCONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include <QSharedPointer>
#include <QListWidget>

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"

/// Cross reference, do not replace by an include directive
class MainWindow; // uis/mainwindow.h

namespace gum {

  class ErrorsContainer;
}

namespace o3prm
{
    class BuildController : public QObject 
    {
        Q_OBJECT

        public:

            explicit BuildController( MainWindow * mw, QObject *parent = 0 );
            ~BuildController();

            void setupConnections();

            bool isAutoSyntaxCheck() const;

            QSharedPointer<PRMTreeModel> projectModel();
            const QSharedPointer<PRMTreeModel> projectModel() const;

        public slots:
            void setAutoSyntaxCheck( bool isAuto );

            void checkSyntax( QsciScintillaExtended * sci = 0 );
            void checkSyntax( const QString & filename );
            void execute( QsciScintillaExtended * sci = 0 );
            void execute( const QString & filename );

            /// Parse all o3prml files in project.
            void parseProject();
            /// Update project model with newer information if possible.
            void updateModel();

            void hideBuildMessage();

        signals:
            void projectModelChanged();
            void modelCanBeUpdated();

        protected slots:
            /*!
              This slot is called when the user switch to another document
              or when correction is finished. 
             */
            void startParsing( bool isAuto = true, bool isExecution = false );
            void onParsingFinished();
            void onExecutionFinished();
            void onProjectParseFinished();

            void onCommandValided();
            void onCommandParsed();

            void onMsgDoubleClick( QModelIndex index );
            void onDocumentClosed( const QString & filename );

            void executeClass( QsciScintillaExtended * sci );
            void executeSystem( QsciScintillaExtended * sci );
            void executeRequest(QsciScintillaExtended* sci);

        private slots:
            void triggerInit();

        private:
            bool __createNewParser();

            MainWindow * mw;

            struct PrivateData;
            PrivateData * d;

            QsciScintillaExtended* __checkDocument(QsciScintillaExtended* sci, bool &ok);
            void __startParsing(bool isAuto, bool isExecution);
            void __parseErrors(const gum::ErrorsContainer& errors);
            void __execInterupted(QListWidget* list);
    };

} // o3prm

#endif // O3PRM_BUILDCONTROLLER_H

