/*
   treeitem.cpp

   A container for items of data supplied by the simple tree model.
 */

#include <QStringList>

#include "models/treeitem.h"

namespace o3prm
{

    TreeItem::TreeItem(const QString &data, TreeItem *parent):
        itemData(data), parentItem(parent)
    {
    }

    TreeItem::~TreeItem()
    {
        qDeleteAll(childItems);
    }

    void TreeItem::addChild(TreeItem *item)
    {
        childItems.append(item);
    }

    TreeItem *TreeItem::child(int row)
    {
        return childItems.value(row);
    }

    int TreeItem::childCount() const
    {
        return childItems.count();
    }

    int TreeItem::columnCount() const
    {
        return 1;
    }

    QString TreeItem::name() const
    {
        return itemData;
    }

    TreeItem *TreeItem::parent()
    {
        return parentItem;
    }

    int TreeItem::row() const
    {
        if (parentItem)
            return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

        return 0;
    }

}
