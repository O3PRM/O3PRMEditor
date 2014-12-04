#include "parsers/o3prmlinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/o3prm/O3prmReader.h>

#include <QDebug>

using namespace gum::prm::o3prm;


//! Constructor.
O3prmlInterpretation::O3prmlInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
    AbstractParser( sci, parent ), m_reader( 0 ), prmChanged( false ) {
}

//! Destructor.
O3prmlInterpretation::~O3prmlInterpretation() {
  // Wait the run methods ends.
  wait();

  if ( m_reader )
    delete m_reader->prm();

  delete m_reader;
}

//! \reimp
void O3prmlInterpretation::run() {
  QString f = filename();
  std::string b = buffer().toStdString();

  if ( m_reader )
    delete m_reader->prm();

  delete m_reader;

  m_reader = new O3prmReader<double>();

  m_reader->setClassPath( classPaths().join( ";" ).toStdString() );

  m_reader->showElegantErrorsAndWarnings();

  if ( f.isEmpty() ) {
    m_reader->readString( b );
  } else
    m_reader->readFile( f.toStdString() );

  setErrors( m_reader->errorsContainer() );

  QSharedPointer<PRMTreeModel> ptr( new PRMTreeModel( m_reader->prm() ) );

  setPRM( ptr );
}

