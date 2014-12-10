#include "o3prmrinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

#include <QDebug>

using namespace std;

using namespace gum::prm::o3prmr;

struct O3prmrInterpretation::PrivateData {
  QMutex *                        mutex;
  gum::prm::o3prmr::O3prmrInterpreter *   interpreter;
  QString                               command;
  vector<QueryResult>                   result;
};

////////////////////////////////////////////////////////////////////////////////

//! Constructor. Create o3prmr interpreter.
O3prmrInterpretation::O3prmrInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
    AbstractParser( sci, parent ), d( new PrivateData ) {
  d->interpreter = new O3prmrInterpreter();
  d->mutex = new QMutex();
}

//! Destructor. Delete o3prmr interpreter.
O3prmrInterpretation::~O3prmrInterpretation() {
  // Wait the run methods ends.
  wait();
  delete d->mutex;
  delete d->interpreter;
}

//! Return last command.
QString O3prmrInterpretation::command() const {
  QMutexLocker locker( d->mutex );
  return d->command;
}


//! Return result of execution.
vector<gum::prm::o3prmr::QueryResult> O3prmrInterpretation::results() const {
  QMutexLocker locker( d->mutex );
  return d->result;
}

//! \reimp
void O3prmrInterpretation::parse( Priority priority ) {
  QMutexLocker locker( d->mutex );
  d->command.clear();
  locker.unlock();

  AbstractParser::parse( priority );
}

//! Parse a single command
void O3prmrInterpretation::parseCommand( const QString & command, Priority priority ) {
  QMutexLocker locker( d->mutex );
  d->command = command;
  locker.unlock();

  AbstractParser::parse( priority );
}


//! \reimp
void O3prmrInterpretation::run() {
  QString f = filename();
  QString b = buffer();

  d->interpreter->setSyntaxMode( isSyntaxMode() );
  d->interpreter->clearPaths();
  foreach( QString s, classPaths() )
  d->interpreter->addPath( s.toStdString() );
  d->interpreter->setSyntaxMode( isSyntaxMode() );
  //d->interpreter->setVerboseMode(true);

  QMutexLocker locker( d->mutex );
  QString command = d->command;
  locker.unlock();

  bool result;

  if ( ! command.isEmpty() ) {
    if ( ! d->interpreter || ! d->interpreter->prm() ) { // On a pas encore parsé le fichier
      if ( ! f.isEmpty() ) {
        result = d->interpreter->interpretFile( f.toStdString() );
      } else {
        result = d->interpreter->interpretLine( b.toStdString() );
      }
    }

    result = d->interpreter->interpretLine( command.toStdString() );
  } else if ( ! f.isEmpty() )
    result = d->interpreter->interpretFile( f.toStdString() );
  else
    result = d->interpreter->interpretLine( b.toStdString() );

// qDebug() << "in O3prmrInterpretation::run() :" << command.isEmpty() << f.isEmpty()
//   << b.count('\n') << result << d->interpreter->getErrorsContainer().error_count << d->interpreter->prm();

// if ( d->interpreter->getErrorsContainer().error_count > 0 )
//  d->interpreter->getErrorsContainer().showElegantErrorsAndWarnings();

  setErrors( d->interpreter->errorsContainer() );

  if ( d->interpreter->prm() == 0 )
    return;

  locker.relock();

  d->result = d->interpreter->results();

  QSharedPointer<PRMTreeModel> ptr( new PRMTreeModel( d->interpreter->prm(),d->interpreter->getContext() ) );

  setPRM( ptr );
}
