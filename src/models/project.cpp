#include "models/project.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileSystemModel>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QThread>
#include <QUrl>

namespace o3prm
{

    struct Project::PrivateData 
    {
        QDir dir;
        QList<QString> paths;
        bool editable;

        QModelIndex rootIndex;

        void createProjectTree();
        void readO3prmpFile();
        void writeO3prmpFile();
    };

    Project::Project( const QString & projDir, QObject * parent ) :
        QStandardItemModel( parent ) 
    {
        d = new PrivateData;
        d->dir = QDir( projDir );
        d->paths << projDir;
        d->editable = false;

        // We create project files
        if ( ! d->dir.exists( d->dir.dirName()+"."+QString(EXTNAME_O3PRM_PROJECT)) ) 
        {
            d->createProjectTree();
        }
        else
        {
            d->readO3prmpFile();
        }

        auto filters = QStringList()
            << "*."+QString(EXTNAME_O3PRM)
            << "*."+QString(EXTNAME_O3PRM_RUN);
        setNameFilters( filters);

        setNameFilterDisables( false );
        setReadOnly( false );
        setRootPath( d->dir.absolutePath() );

        d->rootIndex = index( d->dir.absolutePath() );
    }

    Project::~Project()
    {
        close();
        delete d;
    }

    QString Project::name() const 
    {
        return d->dir.dirName();
    }

    QString Project::dir() const 
    {
        return d->dir.absolutePath();
    }

    bool Project::isEditable() const 
    {
        return d->editable;
    }

    void Project::setEditable( bool editable ) 
    {
        d->editable = editable;
    }

    bool Project::isInside( const QString & filePath ) const 
    {
        QFileInfo info( filePath );

        if ( info.isFile() )
        {
            return QDir::match( d->dir.absolutePath() + "*", info.absolutePath() );
        }
        else
        {
            return false;
        }
    }

    /// TODO Implement this !
    QString	Project::fileName ( const QModelIndex & index ) const
    {
        return QString("error");
    }

    /// TODO Implement this !
    QFileInfo Project::fileInfo ( const QModelIndex & index ) const
    {
        return QFileInfo(fileName(index));
    }

    /// TODO Implement this !
    bool Project::isDir(const QModelIndex& index) const
    {
        return false;
    }

    /// TODO Implement this !
    QModelIndex	Project::mkdir ( const QModelIndex & parent, const QString & name )
    {
        return parent;
    }

    /// TODO Implement this !
    bool Project::remove ( const QModelIndex & index ) const
    {
        return false;
    }

    /// TODO Implement this !
    void Project::setNameFilters ( const QStringList & filters )
    {

    }

    /// TODO Implement this !
    void Project::setNameFilterDisables ( bool enable )
    {

    }

    /// TODO Implement this !
    void Project::setReadOnly ( bool enable )
    {

    }

    /// TODO Implement this !
    QModelIndex Project::setRootPath ( const QString & newPath )
    {
        return QModelIndex();
    }

    /// TODO Implement this !
    QModelIndex	Project::index ( const QString & path, int column) const
    {
        return QModelIndex();
    }

    /// TODO Implement this !
    QString Project::filePath ( const QModelIndex & index ) const
    {
        return QString("error");
    }

    /// TODO Implement this !
    bool Project::rmdir ( const QModelIndex & index ) const
    {
        return false;
    }

    QList<QString> Project::files() const 
    {
        QList<QString> closeList;
        QList<QDir> openList;
        openList << d->dir;

        while ( ! openList.isEmpty() ) 
        {
            QDir current = openList.takeFirst();
            foreach( QFileInfo info, current.entryInfoList( QDir::AllEntries|QDir::NoDotAndDotDot ) ) 
            {
                if ( info.isDir() )
                {
                    openList << info.absoluteFilePath();
                }
                else
                {
                    closeList << info.absoluteFilePath();
                }
            }
        }
        return closeList;
    }

    bool Project::rmdirRec( const QModelIndex & parent ) 
    {
        if ( ! isDir( parent ) )
        {
            return false;
        }

        QDir dir( filePath( parent ) );
        bool result = true;

        foreach( QFileInfo info, dir.entryInfoList( QDir::AllEntries|QDir::NoDotAndDotDot ) ) 
        {
            if ( info.isDir() )
            {
                result &= rmdirRec( index( info.absoluteFilePath() ) );
            }
            else
            {
                result &= dir.remove( info.absoluteFilePath() );
            }
        }

        if (result)
        {
            return rmdir(parent);
        }
        return false;
    }

    void Project::addPath( const QString & path ) 
    {
        d->paths << path;
    }

    void Project::addPaths( const QList<QString> & paths ) 
    {
        d->paths << paths;
    }

    void Project::removePath( const QString & path ) 
    {
        d->paths.removeOne( path );
    }

    void Project::clearPaths() 
    {
        d->paths.clear();
    }

    QList<QString> Project::paths() const 
    {
        return d->paths;
    }

    void Project::PrivateData::createProjectTree() 
    {
        writeO3prmpFile();
        dir.mkdir( tr( "classes" ) );
        dir.mkdir( tr( "requests" ) );
        dir.mkdir( tr( "systems" ) );
    }

    void Project::close() 
    {
        d->writeO3prmpFile();
    }

    void Project::PrivateData::readO3prmpFile() 
    {
        QString filename = dir.absolutePath() + QDir::separator() + dir.dirName()+".o3prmp";
        QFile file( filename );

        if ( ! file.open( QFile::ReadOnly ) )
        {
            auto msg = tr( "Échec lors de l'ouverture du fichier projet %1" ).arg( filename );
            QMessageBox::warning( 0, tr( "Erreur non fatal" ), msg );
            return;
        }

        QDataStream in( &file );

        QMultiMap<QString, QString> map;
        in >> map;

        // If file is corrupted
        if ( map.size() == 0 )
        {
            auto msg = tr( "Échec lors de la récupération des information : %1 est corrompu." );
            msg = msg.arg( filename );
            QMessageBox::warning( 0, tr( "Erreur non fatal" ), msg );
        }
        else
        {
            paths = map.values( "paths" );
        }
    }

    void Project::PrivateData::writeO3prmpFile()
    {
        QString filename = dir.absolutePath() + "/" + dir.dirName() + ".o3prmp";
        QFile file( filename );

        if ( ! file.open( QFile::WriteOnly ) )
        {
            auto msg = tr( "Échec lors de l'ouverture du fichier projet %1" ).arg( filename );
            QMessageBox::warning( 0, tr( "Erreur non fatal" ), msg );
            return;
        }

        QDataStream out( &file );
        QMultiMap<QString, QString> map;
        foreach( QString s, paths )
            map.insertMulti( "paths", s );
        out << map;
    }

    QModelIndex Project::root() const 
    {
        return d->rootIndex;
    }

    int Project::columnCount( const QModelIndex & ) const 
    {
        return 1;
    }


    /// TODO Implement this !
    QVariant Project::data( const QModelIndex & index, int role ) const 
    {
        if ( role == Qt::DecorationRole && index.data().toString().endsWith( ".o3prml" ) ) 
        {
            return QIcon( ":/icons/icons/new.png" );
        }
        else if ( role == Qt::DecorationRole && index.data().toString().endsWith( ".o3prmr" ) ) 
        {
            return QIcon( ":/icons/icons/gear-icon.png" );
        }
        // return QFileSystemModel::data( index,role );
        return QString("Error");
    }

    /// TODO Implement this !
    QVariant Project::headerData( int section, Qt::Orientation orientation, int role ) const 
    {
        if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            return d->dir.dirName();
        }
        else
        {
            // return QFileSystemModel::headerData( section, orientation, role );
            return QString("Error");
        }
    }

    /// TODO Implement this !
    bool Project::dropMimeData( const QMimeData * data,
            Qt::DropAction action,
            int row,
            int column,
            const QModelIndex & parent ) 
    {
        // bool result = QFileSystemModel::dropMimeData( data, action, row, column, parent );
        bool result = false;

        if ( result && action == Qt::MoveAction && data->hasUrls() )
        {
            foreach( QUrl url, data->urls() ) 
            {
                QFileInfo info( url.toLocalFile() );
                emit fileMoved( info.absoluteFilePath(), filePath( parent )+"/"+info.fileName() );
            } 
        }

        return result;
    }

    /// TODO Implement this !
    Qt::ItemFlags Project::flags( const QModelIndex & index ) const 
    {
        // Qt::ItemFlags flags = QFileSystemModel::flags( index );
        Qt::ItemFlags flags = Qt::NoItemFlags;
        if ( ! d->editable )
        {
            flags &= ~ Qt::ItemIsEditable;
        }
        return flags;
    }
}
