#ifndef O3PRM_TREEITEM_H
#define O3PRM_TREEITEM_H

#include <QList>
#include <QVariant>

namespace o3prm
{
    class TreeItem
    {
        public:
            TreeItem(const QString& name, TreeItem *parent = 0);
            ~TreeItem();

            void addChild(TreeItem *child);

            TreeItem *child(int row);
            int childCount() const;
            int columnCount() const;
            QString name() const;
            int row() const;
            TreeItem *parent();

        private:
            QList<TreeItem*> childItems;
            QString itemData;
            TreeItem *parentItem;
    };

}
#endif
