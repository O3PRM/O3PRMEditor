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

    Project* Project::load(const QString& dir, const QDomElement& project_node, QObject* parent)
    {
        auto name = project_node.attribute("name");
        auto project = new Project(dir, name, parent);
        auto root = project->root();

        QList<QPair<ProjectItem*, QDomNode>> list;
        for (int i = 0; i < project_node.childNodes().count(); ++i)
        {
            auto pair = qMakePair(root, project_node.childNodes().at(i));
            list.append(pair);
        }

        while (list.size())
        {
            auto pair = list.first();
            list.removeFirst();

            ProjectItem *item = 0;
            auto node = static_cast<QDomElement*>(&(pair.second));
            auto text = node->attribute("name");

            if (node->tagName() == itemType2String(ProjectItem::ItemType::Directory).toLower())
            {
                item = new ProjectItem(ProjectItem::ItemType::Directory, text); 
            }
            else if (node->tagName() == itemType2String(ProjectItem::ItemType::File).toLower())
            {
                item = new ProjectItem(ProjectItem::ItemType::File, text);
            }
            else if (node->tagName() == itemType2String(ProjectItem::ItemType::Request).toLower())
            {
                item = new ProjectItem(ProjectItem::ItemType::Request, text);
            }

            pair.first->appendRow(item);

            for (int i = 0; i < node->childNodes().count(); ++i)
            {
                list.append(qMakePair(item, node->childNodes().at(i)));
            }
        }

        return project;
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
        QDomDocument doc(name().replace(" ", "_"));
        auto xml_root = doc.createElement("o3prmproject");
        xml_root.setAttribute("name", name());

        doc.appendChild(xml_root);
        xml_root.appendChild(__asXml(doc, this->root()));

        return doc;
    }

    bool Project::isEditable() const
    {
        return false;
    }

    QVariant Project::data(const QModelIndex& index, int role) const
    {
        if (role == Qt::DecorationRole)
        {
            auto parent = reinterpret_cast<QStandardItem*>(index.internalPointer());
            auto item = static_cast<ProjectItem*>(parent->child(index.row()));
            switch (item->type())
            {
                case (int)ProjectItem::ItemType::Directory:
                    {
                        if (dir().exists(item->path()))
                        {
                            return QVariant(QIcon(":/icons/icons/folder.png"));
                        }
                        else
                        {
                            return QVariant(QIcon(":/icons/icons/warning2.png"));
                        }
                    }
                case (int)ProjectItem::ItemType::File:
                case (int)ProjectItem::ItemType::Request:
                    {
                        if (dir().exists(item->path()))
                        {
                            return QVariant(QIcon(":/icons/icons/new.png"));
                        }
                        else
                        {
                            return QVariant(QIcon(":/icons/icons/warning2.png"));
                        }
                    }
                case (int)ProjectItem::ItemType::Project:
                    {
                        return QVariant(QIcon(":/icons/icons/transpO3PRM.png"));
                    }
                default:
                    {
                        // Do nothing
                    }
            }
        }
        return QStandardItemModel::data(index, role);
    }
}
