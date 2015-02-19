#include "buildcontroller.h"

#include "uis/mainwindow.h"
#include "ui_mainwindow.h"
#include "projectcontroller.h"
#include "qsciscintillaextended.h"
#include "parsers/o3prmlinterpretation.h"
#include "parsers/o3prmrinterpretation.h"

#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

#include <QDir>
#include <QListWidget>
#include <QProcess>
#include <QMessageBox>
#include <QMutex>
#include <QDebug>

using namespace gum::prm::o3prm;

using namespace gum::prm::o3prmr;

namespace o3prm
{
    struct BuildController::PrivateData 
    {
        QListWidget * buildList;
        QListWidget * execList;
        QLineEdit * commandLine;
        QPushButton * commandButton;

        bool autoSyntaxCheck;
        bool isAuto;
        bool isCommand;
        AbstractParser * fileParser;

        O3prmrInterpretation * projectParser;

        QTimer timer;

        QSharedPointer<PRMTreeModel> prmModel;

        void showStartParsing( const QString & of );
        void showStartExecution( const QString & of );
    };

    void BuildController::PrivateData::showStartParsing( const QString & of ) 
    {
        buildList->addItem( "" );
        buildList->addItem( tr( "Check syntax of '%1' ..." ).arg( of ) );

        int i = buildList->count() - 1;
        buildList->item( i )->setTextColor( Qt::blue );
        i--;

        while ( i >= 0 && buildList->item( i )->textColor() != Qt::gray )
        {
            buildList->item( i-- )->setTextColor( Qt::gray );
        }

        buildList->scrollToBottom();
    }

    void BuildController::PrivateData::showStartExecution( const QString & of ) 
    {
        showStartParsing( of );

        // Clear exec dock and inform start of process
        execList->addItem( "" );
        execList->addItem( tr( "Executing '%1' ..." ).arg( of ) );
        int i = execList->count() - 1;
        execList->item( i )->setTextColor( Qt::blue );
        i--;

        while ( i >= 0 && execList->item( i )->textColor() != Qt::gray )
            execList->item( i-- )->setTextColor( Qt::gray );

        execList->scrollToBottom();
    }

    /// Contructor
    BuildController::BuildController( MainWindow * mw, QObject *parent ) :
        QObject( parent ), mw( mw ), d( new PrivateData ) 
    {
        //d->commandButton = mw->ui->validCommandButton;
        //d->commandLine = mw->ui->commandLineEdit;
        d->isAuto = false;
        d->isCommand = false;
        d->fileParser = 0;
        d->projectParser = 0;
        d->autoSyntaxCheck = false;

        d->timer.setSingleShot( true );
        d->timer.setInterval( 1000 );

        // ###########################################################
        // Set local to "C" instead local setting (Qt change it) to
        // read o3prml files, to use '.' instead ',' for float number.
        setlocale( LC_NUMERIC, "C" );
        // ###########################################################
    }

    void BuildController::setupConnections()
    {
        // connect(d->buildList, SIGNAL(doubleClicked( QModelIndex)),
        //         this, SLOT(onMsgDoubleClick(QModelIndex)));
        // connect(mw->ui->actionDockVisibility, SIGNAL(triggered()),
        //         this, SLOT(hideBuildMessage()));

        // connect(mw->ui->actionBuild, SIGNAL(triggered()),
        //         this, SLOT(checkSyntax()));
        // connect(mw->ui->actionExecute, SIGNAL(triggered()),
        //         this, SLOT(execute()));
        // connect(d->commandButton, SIGNAL(clicked()),
        //         this, SLOT(onCommandValided()));

        // connect(&(d->timer), SIGNAL(timeout()),
        //         this, SLOT(startParsing()));
        // connect(mw->fc, SIGNAL(fileSaved(QString, QsciScintillaExtended*)),
        //         this, SLOT(startParsing()));
        // connect(mw->fc, SIGNAL(fileClosed(QString)),
        //         this, SLOT(onDocumentClosed(QString)));
        // connect(mw->fc, SIGNAL(fileRenamed(QString, QString, QsciScintillaExtended*)),
        //         this, SLOT(onDocumentClosed(QString)));

        // // Must be start after project triggerInit
        // QTimer::singleShot(500, this, SLOT(triggerInit()));
    }

    /// Destructor
    BuildController::~BuildController()
    {
        d->prmModel.clear();
        delete d;
    }

    void BuildController::triggerInit() 
    {
        // // Start it in case there is only one document
        // connect( mw->ui->tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( startParsing() ) );

        // connect( mw->ui->tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( parseProject() ) );
        // //connect( mw->fc, SIGNAL( fileRenamed( QString,QString,QsciScintillaExtended* ) ), this, SLOT( parseProject() ) );
        // parseProject();

        // mw->ui->commandLineEdit->setCompleter( mw->ec->completer() );
    }

    /* ************************************************************************* */

    bool BuildController::isAutoSyntaxCheck() const
    {
        // return d->autoSyntaxCheck;
        return false;
    }

    void BuildController::setAutoSyntaxCheck( bool isAuto ) 
    {
        // if ( isAuto == d->autoSyntaxCheck )
        // {
        //     return;
        // }

        // d->autoSyntaxCheck = isAuto;

        // if ( isAuto )
        // {
        //     startParsing( -1 );
        // }
        // else
        // {
        //     d->timer.stop();
        //     d->prmModel.clear();
        // }
    }

    QSharedPointer<PRMTreeModel> BuildController::projectModel()
    {
        return d->prmModel;
    }

    const QSharedPointer<PRMTreeModel> BuildController::projectModel() const 
    {
        return d->prmModel;
    }

    void BuildController::checkSyntax(const QString & filename) 
    {
        // // Open file, check it, and close it if it is not already open.
        // if ( mw->fc->isOpenFile( filename ) )
        // {
        //     checkSyntax( mw->fc->fileToDocument( filename ) );
        // }
        // else
        // {
        //     int currentIndex = mw->ui->tabWidget->currentIndex();

        //     // If can't open file, do nothing
        //     if ( mw->fc->openFile( filename ) ) 
        //     {
        //         checkSyntax();

        //         mw->fc->closeFile();
        //         mw->ui->tabWidget->setCurrentIndex( currentIndex );
        //     }
        // }
    }

    void BuildController::checkSyntax(QsciScintillaExtended * sci) 
    {
        // bool ok;
        // sci = __checkDocument(sci, ok);

        // if (ok)
        // {
        //     startParsing(false, false);
        //     d->showStartParsing( sci->title() );

        //     // Erase all error marks in editors
        //     foreach(QsciScintillaExtended * doc, mw->fc->openDocuments())
        //     {
        //         doc->clearAllErrors();
        //     }

        //     mw->vc->setBuildDockVisibility(true);
        // }
    }

    QsciScintillaExtended* BuildController::__checkDocument(QsciScintillaExtended* sci, bool &ok)
    {
        // ok = true;
        // if (sci == 0)
        // {
        //     if (mw->fc->hasCurrentDocument())
        //     {
        //         sci = mw->fc->currentDocument();
        //     }
        //     else
        //     {
        //         ok = false;
        //     }
        // }
        // return sci;
        return 0;
    }

    void BuildController::execute(const QString & filename) 
    {
        // // Open file, check it, and close it if it is not already open.
        // if (mw->fc->isOpenFile(filename)) 
        // {
        //     execute( mw->fc->fileToDocument(filename) );
        // }
        // else
        // {
        //     int currentIndex = mw->ui->tabWidget->currentIndex();

        //     // If can't open file, do nothing
        //     if ( mw->fc->openFile(filename) )
        //     {
        //         execute(); // => currentDocument

        //         mw->fc->closeFile();
        //         mw->ui->tabWidget->setCurrentIndex( currentIndex );
        //     }
        // }
    }

    void BuildController::execute(QsciScintillaExtended * sci) 
    {
        // bool ok;
        // sci = __checkDocument(sci, ok);

        // if (ok)
        // {
        //     switch (sci->lexerEnum())
        //     {
        //         case QsciScintillaExtended::O3prml:
        //             {
        //                 if (sci->block().first == "class")
        //                 {
        //                     executeClass(sci);
        //                 }
        //                 else if (sci->block().first == "system")
        //                 {
        //                     executeSystem(sci);
        //                 }
        //                 break;
        //             }
        //         case QsciScintillaExtended::O3prmr:
        //             {
        //                 executeRequest(sci);
        //             }
        //         default:
        //             {
        //                 // Do nothing
        //                 break;
        //             }
        //     }
        // }
    }

    void BuildController::executeRequest(QsciScintillaExtended* sci)
    {
        // d->showStartExecution( sci->title() );

        // // Erase all error marks in editors
        // foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
        // {
        //     sci->clearAllErrors();
        // }

        // startParsing(false, true);

        // // Show exec dock
        // mw->vc->setExecuteDockVisibility( true );
    }

    void BuildController::parseProject()
    {
        // // If no project or still running
        // if (mw->pc->hasProject() and d->projectParser == 0)
        // {
        //     QString document;
        //     const o3prm::Project * project = mw->pc->currentProject();

        //     foreach( QString filename, project->files() )
        //     {
        //         if ( filename.endsWith( ".o3prml" ) ) 
        //         {
        //             QString relFilename = QDir( project->dir() ).relativeFilePath( filename );
        //             relFilename.replace( '/','.' );
        //             relFilename = relFilename.left( relFilename.length() - 6 );
        //             document += "import " + relFilename + ";\n";
        //         }
        //     }

        //     QsciScintillaExtended * projectDoc = new QsciScintillaExtended( QsciScintillaExtended::O3prmr, mw );

        //     projectDoc->setFilename( project->dir().absoluteFilePath("/projectFile.o3prmr") );
        //     projectDoc->append( document );

        //     d->projectParser = new O3prmrInterpretation( projectDoc, this );
        //     d->projectParser->setSyntaxMode( true );

        //     connect(d->projectParser, SIGNAL(finished()),
        //             this, SLOT(onProjectParseFinished()));
        //     connect(d->projectParser, SIGNAL(finished()),
        //             projectDoc, SLOT(deleteLater()));

        //     d->projectParser->parse();
        // }
    }

    bool BuildController::__createNewParser()
    {
        // QsciScintillaExtended * sci = mw->fc->currentDocument();

        // if ( sci)
        // {
        //     // If a previous thread is still running we reconnect it to self delete
        //     if ( d->fileParser )
        //     {
        //         disconnect( d->fileParser, 0, this, 0 );
        //         connect(d->fileParser, SIGNAL(finished()),
        //                 d->fileParser, SLOT(deleteLater()));

        //         if ( d->fileParser->isFinished() )
        //         {
        //             d->fileParser->deleteLater();
        //         }
        //     }

        //     d->fileParser = 0;

        //     // Create a new thread
        //     switch (sci->lexerEnum())
        //     {
        //         case QsciScintillaExtended::O3prmr:
        //             {
        //                 // Create new document and connectit
        //                 d->fileParser = new O3prmrInterpretation( sci, this );
        //                 break;
        //             }
        //         case QsciScintillaExtended::O3prml:
        //             {
        //                 // Create new document and connect it
        //                 d->fileParser = new O3prmlInterpretation( sci, this );
        //                 break;

        //             }
        //         default:
        //             {
        //                 return false;
        //             }
        //     }

        //     // Set paths
        //     QString filename = sci->filename();
        //     if (mw->pc->hasProject() and mw->pc->currentProject()->isInside(filename))
        //     {
        //         d->fileParser->addClassPaths( mw->pc->currentProject()->paths() );
        //     }
        //     return true;
        // }
        return false;
    }

    void BuildController::startParsing( bool isAuto, bool isExecution ) 
    {
        // // startParsing( bool isAuto = true, bool isExecution = false )
        // // return if no currentDocument
        // if ( mw->fc->hasCurrentDocument() and not ( not isAuto or d->autoSyntaxCheck ) )
        // {
        //     auto sci = mw->fc->currentDocument();

        //     // Stop timer if running, to prevent multiple thread.
        //     d->timer.stop();

        //     // If a previous thread is still running, wait again
        //     if ( isAuto and d->fileParser and d->fileParser->isRunning() ) 
        //     {
        //         d->timer.start();
        //     }
        //     else if (__createNewParser())
        //     {
        //         __startParsing(isAuto, isExecution);
        //     }
        // }
    }

    void BuildController::__startParsing(bool isAuto, bool isExecution)
    {
        // //  create a new thread if there is not or isExecution is true or thread is still running or document change
        // // if ( ! d->parser || isExecution || d->parser->document() != sci )
        // if ( isExecution )
        // {
        //     connect( d->fileParser, SIGNAL( finished() ), this, SLOT( onExecutionFinished() ) );
        // }
        // else
        // {
        //     connect( d->fileParser, SIGNAL( finished() ), this, SLOT( onParsingFinished() ) );
        // }

        // // Start it
        // d->isAuto = isAuto;
        // d->fileParser->setSyntaxMode( ! isExecution );

        // if ( isAuto )
        // {
        //     d->fileParser->parse( QThread::LowPriority );
        // }
        // else
        // {
        //     d->fileParser->parse();
        // }
    }

    void BuildController::onParsingFinished()
    {
        // if (d->fileParser == 0
        //         or (d->isAuto and d->fileParser->document() != mw->fc->currentDocument() ))
        // {
        //     return;
        // }

        // auto errors = d->fileParser->errors();
        // if ( errors.error_count == 0 )
        // {
        //     emit modelCanBeUpdated();
        // }

        // //updateModel();

        // if ( d->isAuto )
        // {
        //     mw->fc->currentDocument()->clearAllSyntaxErrors();
        // }

        // __parseErrors(errors);

        // if (d->autoSyntaxCheck and not d->timer.isActive())
        // {
        //     d->timer.start();
        // }

        // if (not d->isAuto)
        // {
        //     if ( errors.error_count == 0 )
        //     {
        //         d->buildList->addItem( tr( "Syntaxe vérifiée. Il n'y a pas d'erreur." ) );
        //         d->buildList->item( d->buildList->count() - 1 )->setTextColor( Qt::blue );
        //     }
        //     else
        //     {
        //         __execInterupted(d->buildList);
        //     }
        //     d->buildList->scrollToBottom();
        // }
    }

    void BuildController::__parseErrors(const gum::ErrorsContainer& errors)
    {
        // for ( int i = 0, size = errors.count() ; i < size ; i++ ) 
        // {
        //     const gum::ParseError & err = errors.error( i );
        //     const QString & filename = d->fileParser->document()->filename();
        //     QString errFilename = QString::fromStdString( err.filename );

        //     if ( d->isAuto and ( errFilename.isEmpty() or errFilename.contains( filename,Qt::CaseSensitive ) ) )
        //     {
        //         mw->fc->currentDocument()->setSyntaxError( err.line - 1 );
        //     }
        //     else if ( not d->isAuto ) 
        //     {
        //         int line = err.line;
        //         QString s = QString::fromStdString( err.toString() );
        //         QString relFilename;

        //         if ( errFilename.isEmpty() ) 
        //         {
        //             if ( mw->pc->hasProject() )
        //             {
        //                 relFilename = QDir( mw->pc->currentProject()->dir() ).relativeFilePath( filename );
        //             }
        //             else
        //             {
        //                 relFilename = filename;
        //             }

        //             s.prepend( relFilename + ":" );
        //             errFilename = filename;
        //         }
        //         else if ( errFilename == "anonymous buffer" or errFilename.endsWith( ".bak" ) )
        //         {
        //             if ( mw->pc->hasProject() )
        //             {
        //                 relFilename = QDir( mw->pc->currentProject()->dir() ).relativeFilePath( filename );
        //             }
        //             else
        //             {
        //                 relFilename = filename;
        //             }

        //             s.replace( errFilename, relFilename );
        //             errFilename = filename;
        //         }
        //         else if ( mw->pc->hasProject() ) 
        //         {
        //             relFilename = QDir( mw->pc->currentProject()->dir() ).relativeFilePath( errFilename );
        //             s.replace( errFilename, relFilename );
        //         }

        //         QListWidgetItem * item = new QListWidgetItem( s, d->buildList ) ;

        //         if ( not d->isCommand ) 
        //         {
        //             item->setData( Qt::UserRole, errFilename );
        //             item->setData( Qt::UserRole + 1, line );
        //         }
        //     }
        // }
    }

    void BuildController::onExecutionFinished() 
    {
        // if ( mw->fc->currentDocument() == d->fileParser->document() )
        // {
        //     // We show all results
        //     auto o3prmrParser = qobject_cast<O3prmrInterpretation *>( d->fileParser );

        //     if (o3prmrParser) 
        //     {
        //         // We show errors and warnings.
        //         onParsingFinished();

        //         // If there was errors or it's not an execution, return.
        //         if ( o3prmrParser->errors().error_count != 0 ) 
        //         {
        //             __execInterupted(d->execList);
        //         }
        //         else
        //         {
        //             const std::vector<QueryResult> & results = o3prmrParser->results();

        //             for ( size_t i = 0 ; i < results.size() ; i++ ) 
        //             {
        //                 const QString & query = QString::fromStdString( results[i].command );
        //                 double time = results[i].time;
        //                 d->execList->addItem( tr( "%1 [%2s]" ).arg( query ).arg( time ) );
        //                 d->execList->item( d->execList->count() - 1 )->setTextColor( Qt::darkYellow );
        //                 const std::vector<SingleResult> & result = results[i].values;

        //                 for ( size_t j = 0 ; j < result.size() ; j++ ) 
        //                 {
        //                     float val = result[j].p;
        //                     d->execList->addItem( tr( "%1 : %2" ).arg( QString::fromStdString( result[j].label ) ).arg( val ) );
        //                 }
        //             }

        //             d->execList->addItem( tr( "Exécution terminée." ) );
        //             d->execList->item( d->execList->count() - 1 )->setTextColor( Qt::blue );
        //         }
        //         d->execList->scrollToBottom();
        //     }
        // }
    }

    void BuildController::__execInterupted(QListWidget* list)
    {
        // list->addItem( tr( "Exécution interrompue. Il y a des erreurs." ) );
        // int i = list->count() - 1;
        // list->item( i )->setTextColor( Qt::red );
        // i--;

        // while ( i >= 0 && list->item(i)->textColor() != Qt::gray )
        // {
        //     i--;
        // }

        // list->item( i+1 )->setTextColor( Qt::red );
        // mw->vc->setBuildDockVisibility( true );
    }

    void BuildController::onProjectParseFinished() 
    {
        // if (d->projectParser)
        // {
        //     d->prmModel.clear();
        //     d->prmModel = d->projectParser->prm();

        //     emit projectModelChanged();

        //     // Update icons in project explorator
        //     // Clear all errors
        //     //for ( int i = 0, size = d->projectParser->errors().size() ; i < size ; i++ ) {
        //     //  QString filename = d->projectParser->errors().getError(i).filename();
        //     //  mw->pc->setErrorIcon( filename );
        //     //}

        //     QFile::remove( d->projectParser->document()->filename() + ".bak" );

        //     d->projectParser->deleteLater();
        //     d->projectParser = 0;
        // }
    }

    void BuildController::updateModel() 
    {
        // if ( not (d->prmModel.isNull() or d->fileParser == 0 or d->fileParser->errors().error_count != 0))
        // {
        //     d->prmModel->update( d->fileParser->prm(), mw->fc->currentDocument() );
        //     d->prmModel->sort( 0 );
        // }
    }

    void BuildController::hideBuildMessage() 
    {
        // // Erase all error marks in editors
        // foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
        // {
        //     sci->clearAllErrors();
        // }
        // mw->vc->setDockVisibility( false );
    }

    void BuildController::onMsgDoubleClick( QModelIndex index ) 
    {
        // // Do anything if it is not an error message.
        // if ( index.isValid() and index.data( Qt::UserRole ).isValid() )
        // {
        //     QString filename = index.data( Qt::UserRole ).toString();
        //     int line = index.data( Qt::UserRole + 1 ).toInt() - 1; // lines start at 0 in scintilla

        //     // Open the file (if filename is empty take the current document)
        //     if (mw->fc->openFile( filename ) )
        //     {
        //         // Go to the line
        //         QsciScintillaExtended * sci = mw->fc->currentDocument();
        //         if (sci)
        //         {
        //             sci->setCursorPosition( line, 0 );
        //             sci->setError( line );
        //             sci->setFocus();
        //         }
        //     }
        // }
    }

    void BuildController::onDocumentClosed( const QString & filename ) 
    {
        // QFile::remove( filename + ".bak" );
    }

    void BuildController::executeClass( QsciScintillaExtended * sci ) 
    {
        // qWarning() << "BuildController::executeClass must be reimplemented with new PRMTreeModel interface.";
    }

    void BuildController::executeSystem( QsciScintillaExtended * sci )
    {
        // QString name = QFileInfo( sci->filename() ).baseName();
        // // On crée un nouveau document o3prmr qu'on enregistre dans le package du système
        // QsciScintillaExtended * req = mw->fc->newDocument( name+"Request", QsciScintillaExtended::O3prmr );
        // req->setFilename( mw->pc->currentProject()->dir().absoluteFilePath("requests/" + req->title() + ".o3prmr") );

        // // On met à jour le package dans le document o3prmr,
        // if ( ! sci->package().isEmpty() )
        // {
        //     req->append( "\npackage requests;\n" );
        // }

        // // On ajoute l'import
        // req->append( "\nimport "+sci->package()+"."+name+" as default;\n\n" );

        // // On ajoute le block default
        // req->append( "request "+name+"Request {\n    engine SVE;\n}\n" );

        // // give it the focus,
        // mw->ui->tabWidget->setCurrentWidget( req );
        // req->setFocus();
    }

    void BuildController::onCommandValided() 
    {
        // QString command = d->commandLine->text().simplified();

        // if ( command.isEmpty() ) 
        // {
        //     d->commandLine->setStyleSheet( "" );
        //     return;
        // }

        // if ( ! command.contains( QRegExp( ";\\s*$" ) ) )
        // {
        //     command += ";";
        // }

        // // Create a new parser if necessary
        // if ( d->fileParser == 0 || d->fileParser->document() != mw->fc->currentDocument() )
        // {
        //     __createNewParser();
        // }

        // // If a previous request was send, wait a few second and return if too long
        // if ( d->fileParser or not d->fileParser->isRunning() or d->fileParser->wait( 500 ) )
        // {
        //     O3prmrInterpretation * parser = qobject_cast<O3prmrInterpretation *>( d->fileParser );
        //     if ( parser == 0 ) 
        //     {
        //         qWarning() << "in onCommandValided() : Error : parser == 0.";
        //         return;
        //     }

        //     disconnect( d->fileParser, 0, this, 0 );

        //     connect( d->fileParser, SIGNAL( finished() ), this, SLOT( onCommandParsed() ) );
        //     d->fileParser->setSyntaxMode( false );

        //     d->showStartExecution( command );
        //     d->isAuto = false;

        //     // Erase all error marks in editors
        //     foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
        //     {
        //         sci->clearAllErrors();
        //     }

        //     // Show exec dock
        //     mw->vc->setExecuteDockVisibility( true );

        //     parser->parseCommand( command, QThread::NormalPriority );
        //     d->commandLine->setStyleSheet( "" );
        // }
    }

    void BuildController::onCommandParsed() 
    {
        // d->isCommand = true;
        // onExecutionFinished();
        // d->isCommand = false;

        // O3prmrInterpretation * parser = qobject_cast<O3prmrInterpretation *>( d->fileParser );

        // if ( parser == 0 ) 
        // {
        //     qWarning() << "in onCommandParsed() : Error : parser == 0.";
        //     return;
        // }

        // if ( parser->errors().error_count == 0 ) 
        // {
        //     d->commandLine->clear();
        //     QsciScintillaExtended * sci = mw->fc->currentDocument();

        //     if ( sci->findFirst( "}", false, false, false, true, false, 0, 0 ) ) 
        //     {
        //         QString indent( sci->indentation( sci->currentLine()-1 ),QChar( ' ' ) );
        //         QString newCommand = indent + parser->command() + "\n}";
        //         sci->replace( newCommand );
        //         sci->setSelection( 0,0,0,0 );
        //         sci->ensureLineVisible( sci->lines()-1 );
        //     }
        // }
        // else 
        // {
        //     d->commandLine->setStyleSheet( "background : red;" );
        // }
    }

} // o3prm

