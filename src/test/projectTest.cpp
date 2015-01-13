#include "gmock/gmock.h"

#include "models/project.h"

TEST(ProjectItem, TestPath)
{
    // Arrange
    auto dir = o3prm::ProjectItem::ItemType::Directory;
    auto file = o3prm::ProjectItem::ItemType::File;
    o3prm::ProjectItem root(dir, "root");

    auto A = new o3prm::ProjectItem(dir, "A");
    root.appendRow(A);
    auto B = new o3prm::ProjectItem(dir, "B");
    root.appendRow(B);
    auto C = new o3prm::ProjectItem(dir, "C");
    A->appendRow(C);
    auto F = new o3prm::ProjectItem(file, "F.txt");
    C->appendRow(F);
    // Act
    auto path = F->path();
    // Assert
    ASSERT_EQ("root/A/C/F.txt", path.toStdString());
}
