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
    ASSERT_TRUE(false);
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

TEST(TreeItemTests, AddChild)
{
    // Arrange
    o3prm::TreeItem parent(QString("MyRootItem"));
    auto child = new o3prm::TreeItem("MyChild", &parent);
    // Act
    parent.addChild(child);
    // Assert
    ASSERT_EQ(parent.childCount(), 1);
    ASSERT_EQ(child, parent.child(0));
}

TEST(TreeItemTests, AddChildren)
{
    // Arrange
    o3prm::TreeItem rootItem("RootItem");
    auto readme = new o3prm::TreeItem("README.txt", &rootItem);
    auto org = new o3prm::TreeItem("org/", &rootItem);
    auto lip6 = new o3prm::TreeItem("lip6/", org);
    auto printers = new o3prm::TreeItem("printers.o3prm", lip6);
    // Act
    rootItem.addChild(readme);
    rootItem.addChild(org);
    org->addChild(lip6);
    lip6->addChild(printers);
    // Assert
    ASSERT_EQ(2, rootItem.childCount());
    ASSERT_EQ(1, org->childCount());
    ASSERT_EQ(1, lip6->childCount());
    ASSERT_EQ(0, printers->childCount());
}
