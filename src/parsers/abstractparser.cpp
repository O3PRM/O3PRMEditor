#include "abstractparser.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"

#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/PRM.h>

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

struct AbstractParser::PrivateData 
{
    const QsciScintillaExtended * sci;
    QStringList paths;
    QMutex *  mutex;
    bool   syntaxMode;
    QString filename;
    QString buffer;
    bool isDocModified;
    gum::ErrorsContainer errors;
    QSharedPointer<PRMTreeModel> prm;
};

AbstractParser::AbstractParser( const QsciScintillaExtended * sci, QObject *parent ) :
    QThread( parent ), d( new PrivateData )
{
    d->sci = sci;
    d->syntaxMode = true;
    d->mutex = new QMutex();
}

AbstractParser::~AbstractParser() 
{
    wait();
    delete d->mutex;
    delete d;
}

void AbstractParser::parse( Priority priority ) 
{
    if ( d->sci == 0 || isRunning() )
    {
        return;
    }

    setFilename( d->sci->filename() );
    d->isDocModified = d->sci->isModified();

    // If file is not saved in a file
    if ( filename().isEmpty() ) 
    {
        // Parse code
        setBuffer( d->sci->text() );
    } 
    // If file is saved in a file and has been modified
    else if ( d->isDocModified )
    {
        // Save modifications in a temporary file
        setFilename( filename() + ".bak" );
        QFile file( filename() );

        if ( file.open( QFile::WriteOnly ) ) 
        {
            file.write( d->sci->text().toUtf8() );
            file.close();
            // If modifications can't be saved, parse code
        }
        else 
        {
            setBuffer( d->sci->text() );
            setFilename( QString() );
            addClassPath( QFileInfo( d->sci->filename() ).path() );
        }
    }
    start( priority );
}

QStringList AbstractParser::classPaths() const {
    QMutexLocker locker( d->mutex );
    return d->paths;
}

void AbstractParser::setClassPaths( const QStringList & paths ) 
{
    QMutexLocker locker( d->mutex );
    d->paths.clear();
    locker.unlock();
    addClassPaths( paths );
}

void AbstractParser::addClassPath( const QString & path ) 
{
    QMutexLocker locker( d->mutex );

    if ( path.endsWith( "/" ) )
    {
        d->paths += path;
    }
    else
    {
        d->paths += path + "/";
    }
}

void AbstractParser::addClassPaths( const QStringList & paths ) 
{
    foreach( QString path, paths )
    {
        addClassPath( path );
    }
}

bool AbstractParser::isSyntaxMode() const 
{
    QMutexLocker locker( d->mutex );
    return d->syntaxMode;
}

void AbstractParser::setSyntaxMode( bool active ) 
{
    QMutexLocker locker( d->mutex );
    d->syntaxMode = active;
}

const QsciScintillaExtended * AbstractParser::document() const 
{
    QMutexLocker locker( d->mutex );
    return d->sci;
}

gum::ErrorsContainer AbstractParser::errors() const 
{
    QMutexLocker locker( d->mutex );
    return d->errors;
}

QSharedPointer<PRMTreeModel> AbstractParser::prm() const 
{
    QMutexLocker locker( d->mutex );
    return d->prm;
}

QString AbstractParser::filename() const 
{
    QMutexLocker( d->mutex );
    return d->filename;
}

void AbstractParser::setFilename( const QString & filename ) 
{
    QMutexLocker( d->mutex );
    d->filename = filename;
}

QString AbstractParser::buffer() const 
{
    QMutexLocker( d->mutex );
    return d->buffer;
}

void AbstractParser::setBuffer( const QString & buffer ) 
{
    QMutexLocker( d->mutex );
    d->buffer = buffer;
}

void AbstractParser::setPRM( const QSharedPointer<PRMTreeModel> & prm ) 
{
    QMutexLocker( d->mutex );
    d->prm.clear();
    d->prm = prm;
    d->prm->moveToThread( QApplication::instance()->thread() );
}

void AbstractParser::setErrors( const gum::ErrorsContainer & errors ) 
{
    QMutexLocker( d->mutex );
    d->errors = errors;
}
