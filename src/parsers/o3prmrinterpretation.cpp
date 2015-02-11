#include "o3prmrinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

#include <QDebug>

using namespace std;

using namespace gum::prm::o3prmr;

struct O3prmrInterpretation::PrivateData 
{
    QMutex *mutex;
    gum::prm::o3prmr::O3prmrInterpreter *interpreter;
    QString command;
    vector<QueryResult> result;
};

O3prmrInterpretation::O3prmrInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
    AbstractParser( sci, parent ), d( new PrivateData )
{
    d->interpreter = new O3prmrInterpreter();
    d->mutex = new QMutex();
}

O3prmrInterpretation::~O3prmrInterpretation() 
{
    // Wait for the run methods to end
    wait();
    delete d->mutex;
    delete d->interpreter;
}

QString O3prmrInterpretation::command() const 
{
    QMutexLocker locker( d->mutex );
    return d->command;
}

vector<gum::prm::o3prmr::QueryResult> O3prmrInterpretation::results() const 
{
    QMutexLocker locker( d->mutex );
    return d->result;
}

void O3prmrInterpretation::parse( Priority priority ) 
{
    QMutexLocker locker( d->mutex );
    d->command.clear();
    locker.unlock();

    AbstractParser::parse( priority );
}

void O3prmrInterpretation::parseCommand( const QString & command, Priority priority ) 
{
    QMutexLocker locker( d->mutex );
    d->command = command;
    locker.unlock();

    AbstractParser::parse( priority );
}

void O3prmrInterpretation::run() 
{
    QString f = filename();
    QString b = buffer();

    d->interpreter->setSyntaxMode( isSyntaxMode() );
    d->interpreter->clearPaths();

    foreach( QString s, classPaths() )
    {
        d->interpreter->addPath( s.toStdString() );
    }

    d->interpreter->setSyntaxMode( isSyntaxMode() );

    QMutexLocker locker( d->mutex );
    QString command = d->command;
    locker.unlock();

    bool result;

    if ( ! command.isEmpty() )
    {
        // File not parsed yet
        if ( ! d->interpreter || ! d->interpreter->prm() )
        {
            if ( ! f.isEmpty() ) 
            {
                result = d->interpreter->interpretFile( f.toStdString() );
            }
            else
            {
                result = d->interpreter->interpretLine( b.toStdString() );
            }
        }

        result = d->interpreter->interpretLine( command.toStdString() );
    }
    else if ( ! f.isEmpty() )
    {
        result = d->interpreter->interpretFile( f.toStdString() );
    }
    else
    {
        result = d->interpreter->interpretLine( b.toStdString() );
    }

    setErrors( d->interpreter->errorsContainer() );

    if ( d->interpreter->prm() == 0 )
    {
        return;
    }

    locker.relock();
    d->result = d->interpreter->results();
    auto model = new PRMTreeModel( d->interpreter->prm(),d->interpreter->getContext() );
    QSharedPointer<PRMTreeModel> ptr(model);

    setPRM( ptr );
}
