#include "parsers/o3prmlinterpretation.h"

#include "models/qsciscintillaextended.h"
#include "models/prmtreemodel.h"
#include <agrum/PRM/o3prm/O3prmReader.h>

#include <QDebug>

using namespace gum::prm::o3prm;

O3prmlInterpretation::O3prmlInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
    AbstractParser( sci, parent ), m_reader( 0 ), prmChanged( false )
{

}

O3prmlInterpretation::~O3prmlInterpretation() 
{
    // Wait for the run methods to end
    wait();

    if ( m_reader )
    {
        delete m_reader->prm();
    }

    delete m_reader;
}

void O3prmlInterpretation::run() 
{
    QString f = filename();
    std::string b = buffer().toStdString();

    if ( m_reader )
    {
        delete m_reader->prm();
        delete m_reader;
    }

    m_reader = new O3prmReader<double>();
    m_reader->setClassPath( classPaths().join( ";" ).toStdString() );
    m_reader->showElegantErrorsAndWarnings();

    if ( f.isEmpty() ) 
    {
        m_reader->readString( b );
    }
    else
    {
        m_reader->readFile( f.toStdString() );
    }

    setErrors( m_reader->errorsContainer() );
    QSharedPointer<PRMTreeModel> ptr( new PRMTreeModel( m_reader->prm() ) );
    setPRM( ptr );
}

