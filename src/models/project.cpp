#include "models/project.h"

#include <iostream>

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
        auto rootItem = this->invisibleRootItem();
        __root = new ProjectItem(ProjectItem::ItemType::Project, name);
        rootItem->appendRow(__root);
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

    ProjectItem* Project::root()
    {
        return __root;
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

    QDomElement Project::__asXml(QDomDocument& doc, ProjectItem* item) 
    {
        auto name = Project::itemType2String(item->type()).toLower();
        auto element = doc.createElement(name);
        element.setAttribute("name", item->text());
        for (int i = 0; i < item->rowCount(); ++i)
        {
            auto child = __asXml(doc, static_cast<ProjectItem*>(item->child(i)));
            element.appendChild(child);
        }
        return element;
    }

    QDomDocument Project::asXml()
    {
        QDomDocument doc(name());
        auto xml_root = doc.createElement("o3prmproject");
        xml_root.setAttribute("name", name());

        doc.appendChild(xml_root);
        xml_root.appendChild(__asXml(doc, this->root()));

        return doc;
    }


}
