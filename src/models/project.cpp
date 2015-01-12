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

    Project::Project( const QString & dir, const QString& name, QObject * parent ) :
        QStandardItemModel(parent), __dir(dir), __name(name)
    {
        this->setHorizontalHeaderItem(0, new QStandardItem(name));
        auto rootItem = this->invisibleRootItem();

        auto project_dir = new QStandardItem(name);
        rootItem->appendRow(project_dir);
    }

    Project::~Project()
    {
    }

    QString Project::name() const 
    {
        return __name;
    }

    QDir Project::dir() const 
    {
        return QDir(__dir);
    }

    bool Project::isInside( const QString & filePath ) const 
    {
        return false;
    }

    QList<QString> Project::files() const 
    {
        return QList<QString>();
    }

    void Project::addPath( const QString & path ) 
    {
    }

    void Project::clearPaths() 
    {
    }

    QList<QString> Project::paths() const 
    {
        return QList<QString>();
    }

}
