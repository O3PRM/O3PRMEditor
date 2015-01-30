#include "models/projectitem.h"

namespace o3prm
{
    ProjectItem::ProjectItem(ItemType type):
        QStandardItem(), __type(type)
    {
    }

    ProjectItem::ProjectItem(ItemType type, const QString &text):
        QStandardItem(text), __type(type)
    {
    }

    ProjectItem::ProjectItem(ItemType type, const QIcon &icon, const QString &text):
        QStandardItem(icon, text), __type(type)
    {
    }

    ProjectItem::ProjectItem(ItemType type, int rows, int columns):
        QStandardItem(rows, columns), __type(type)
    {
    }

    int ProjectItem::type() const 
    {
        return (int)__type;
    }

    int ProjectItem::itemType() const 
    {
        return __type;
    }

    void ProjectItem::setType(int type)
    {
        __type = (ItemType) type;
    }

    QString ProjectItem::path() const
    {
        QString path = text();
        int type_value = (int)ItemType::Directory;
        for (auto iter = parent(); iter != 0 and iter->type() == type_value; iter = iter->parent())
        {
            path = iter->text() + "/" + path;
        }
        return path;
    }

    ProjectItem* ProjectItem::child(int row, int column) const
    {
        return static_cast<ProjectItem*>(QStandardItem::child(row, column));
    }

    bool ProjectItem::isInPackage() const 
    {
        return parent()->type() == ItemType::Directory;
    }

    QString ProjectItem::package() const
    {
        if (isInPackage())
        {
            return static_cast<ProjectItem*>(parent())->path().replace('/', '.');
        }
        return QString();
    }

} // o3prm

