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

//    Project::Project( const QString & projDir, QObject * parent ) :
//        QStandardItemModel( parent )
//    {
//        static int counter = 0;
//        __name = projDir + QString(counter++);
//    }
//
//    Project::~Project()
//    {
//    }
//
//    QString Project::name() const 
//    {
//        return __name;
//    }
//
//    QString Project::dir() const 
//    {
//        return QString("Error");
//    }

    bool Project::isEditable() const 
    {
        return false;
    }

    void Project::setEditable( bool editable ) 
    {
    }

    bool Project::isInside( const QString & filePath ) const 
    {
        return false;
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
        return QList<QString>();
    }

    bool Project::rmdirRec( const QModelIndex & parent ) 
    {
        return false;
    }

    void Project::addPath( const QString & path ) 
    {
    }

    void Project::addPaths( const QList<QString> & paths ) 
    {
    }

    void Project::removePath( const QString & path ) 
    {
    }

    void Project::clearPaths() 
    {
    }

    QList<QString> Project::paths() const 
    {
        return QList<QString>();
    }

    void Project::close() 
    {
    }

    QModelIndex Project::root() const 
    {
        return QModelIndex();
    }

    int Project::columnCount( const QModelIndex & ) const 
    {
        return 1;
    }

    /// TODO Implement this !
    QVariant Project::data( const QModelIndex & index, int role ) const 
    {
        return QString("Error");
    }

    /// TODO Implement this !
    QVariant Project::headerData( int section, Qt::Orientation orientation, int role ) const 
    {
        return QString("Error");
    }

    /// TODO Implement this !
    bool Project::dropMimeData( const QMimeData * data,
                                Qt::DropAction action,
                                int row,
                                int column,
                                const QModelIndex & parent ) 
    {
        return false;
    }

    /// TODO Implement this !
    Qt::ItemFlags Project::flags( const QModelIndex & index ) const 
    {
        return Qt::NoItemFlags;
    }
}
