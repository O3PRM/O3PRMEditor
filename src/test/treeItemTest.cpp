#include <QString>

#include "agrum/core/set.h"

#include "gmock/gmock.h"

#include "models/treeitem.h"

TEST(TreeItemTests, ConstructorDefault)
{
    // Arrange
    QString name("MyItem");
    // Act
    o3prm::TreeItem item(name);
    // Assert
    ASSERT_EQ(item.name(), name);
}

TEST(TreeItemTests, ConstructorWithParent)
{
    // Arrange
    o3prm::TreeItem parent(QString("MyRootItem"));
    // Act
    auto child = new o3prm::TreeItem(QString("MyChildItem"), &parent);
    // Arrange
    ASSERT_EQ(&parent, child->parent());
}

