/*
   treemodel.cpp

   Provides a simple tree model to show how to create and use hierarchical
   models.
 */

#include <iostream>

#include <QtGui>

#include "models/treeitem.h"
#include "models/treemodel.h"

namespace o3prm
{

    TreeModel::TreeModel(const QString &name, QObject *parent)
        : QAbstractItemModel(parent)
    {
        rootItem = new TreeItem(name);
        setupModelData();
    }

    TreeModel::~TreeModel()
    {
        delete rootItem;
    }

    int TreeModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
        else
            return rootItem->columnCount();
    }

    QVariant TreeModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

        return QVariant(item->name());
    }

    Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return 0;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
            int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return rootItem->name();

        return QVariant();
    }

    QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
        const
        {
            if (!hasIndex(row, column, parent))
                return QModelIndex();

            TreeItem *parentItem;

            if (!parent.isValid())
                parentItem = rootItem;
            else
                parentItem = static_cast<TreeItem*>(parent.internalPointer());

            TreeItem *childItem = parentItem->child(row);
            if (childItem)
                return createIndex(row, column, childItem);
            else
                return QModelIndex();
        }

    QModelIndex TreeModel::parent(const QModelIndex &index) const
    {
        if (!index.isValid())
            return QModelIndex();

        TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
        TreeItem *parentItem = childItem->parent();

        if (parentItem == rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
    }

    int TreeModel::rowCount(const QModelIndex &parent) const
    {
        TreeItem *parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        return parentItem->childCount();
    }

    void TreeModel::setupModelData()
    {
        auto readme = new TreeItem("README.txt", rootItem);
        rootItem->addChild(readme);
        auto org = new TreeItem("org/", rootItem);
        rootItem->addChild(org);
        auto lip6 = new TreeItem("lip6/", org);
        org->addChild(lip6);
        auto printers = new TreeItem("printers.o3prm", lip6);
        lip6->addChild(printers);
    }
}
