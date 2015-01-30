#ifndef O3PRM_PROJECT_ITEM_H
#define O3PRM_PROJECT_ITEM_H

#include <QStandardItem>

namespace o3prm
{
    class ProjectItem: public QStandardItem
    {
        public :
            enum ItemType {
                Directory = 1000,
                File = 1001,
                Project = 1002,
                Request = 1003
            };

            /// Values used by ProjectItem must be above or equal to 1000.
            static int minItemTypeInt() { return (int)ItemType::Directory; }

            ProjectItem(ItemType type);

            ProjectItem(ItemType type, const QString &text);

            ProjectItem(ItemType type, const QIcon &icon, const QString &text);

            ProjectItem(ItemType type, int rows, int columns = 1);

            int type() const;

            int itemType() const;

            void setType(int type);

            // Returns the path to parent using / as separators.
            QString path() const;

            ProjectItem* child(int row, int column=0) const;

            bool isInPackage() const;

            QString package() const;

        private:
            ItemType __type;
    };

} // o3prm

#endif // O3PRM_PROJECT_ITEM_H

