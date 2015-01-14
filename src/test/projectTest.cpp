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

TEST(Project, TestSerialization)
{
    // Arrange
    o3prm::Project project("/some/path/", "MyCoolProject");
    auto root = project.root();

    auto readme = new o3prm::ProjectItem(o3prm::ProjectItem::ItemType::File, "README.txt");
    root->appendRow(readme);
    auto org = new o3prm::ProjectItem(o3prm::ProjectItem::ItemType::Directory, "org/");
    root->appendRow(org);
    auto lip6 = new o3prm::ProjectItem(o3prm::ProjectItem::ItemType::Directory, "lip6/");
    org->appendRow(lip6);

    auto printers = new o3prm::ProjectItem(o3prm::ProjectItem::ItemType::File, "printers.o3prm");
    lip6->appendRow(printers);

    // Act
    auto dom = project.asXml();

    // Assert
    auto xml_root = dom.documentElement();
    ASSERT_EQ("o3prmproject", xml_root.tagName().toStdString());
    ASSERT_TRUE(xml_root.hasAttribute("name"));
    ASSERT_EQ("MyCoolProject", xml_root.attribute("name").toStdString());

    auto root_children = xml_root.childNodes();
    ASSERT_EQ(1, root_children.size());

    auto dom_node_project = root_children.at(0);
    ASSERT_EQ(QDomNode::NodeType::ElementNode, dom_node_project.nodeType());
    auto xml_project = static_cast<QDomElement*>(&dom_node_project);
    ASSERT_EQ("project", xml_project->tagName().toStdString());
    ASSERT_EQ("MyCoolProject", xml_project->attribute("name").toStdString());

    auto project_files = xml_project->childNodes();
    ASSERT_EQ(2, project_files.size());

    auto dom_node_readme = project_files.at(0);
    ASSERT_EQ(QDomNode::NodeType::ElementNode, dom_node_readme.nodeType());
    auto xml_readme = static_cast<QDomElement*>(&dom_node_readme);
    ASSERT_TRUE(xml_readme->hasAttribute("name"));
    ASSERT_EQ("file", xml_readme->tagName().toStdString());
    ASSERT_EQ("README.txt", xml_readme->attribute("name").toStdString());

    auto dom_node_org = project_files.at(1);
    ASSERT_EQ(QDomNode::NodeType::ElementNode, dom_node_org.nodeType());
    auto xml_org = static_cast<QDomElement*>(&dom_node_org);
    ASSERT_TRUE(xml_org->hasAttribute("name"));
    ASSERT_EQ("package", xml_org->tagName().toStdString());
    ASSERT_EQ("org/", xml_org->attribute("name").toStdString());

    auto org_dir = xml_org->childNodes();
    ASSERT_EQ(1, org_dir.size());

    auto dom_node_lip6 = org_dir.at(0);
    ASSERT_EQ(QDomNode::NodeType::ElementNode, dom_node_lip6.nodeType());
    auto xml_lip6 = static_cast<QDomElement*>(&dom_node_lip6);
    ASSERT_TRUE(xml_lip6->hasAttribute("name"));
    ASSERT_EQ("package", xml_lip6->tagName().toStdString());
    ASSERT_EQ("lip6/", xml_lip6->attribute("name").toStdString());

    auto lip6_dir = xml_lip6->childNodes();
    ASSERT_EQ(1, lip6_dir.size());

    auto dom_node_printers = lip6_dir.at(0);
    ASSERT_EQ(QDomNode::NodeType::ElementNode, dom_node_printers.nodeType());
    auto xml_printers = static_cast<QDomElement*>(&dom_node_printers);
    ASSERT_TRUE(xml_printers->hasAttribute("name"));
    ASSERT_EQ("file", xml_printers->tagName().toStdString());
    ASSERT_EQ("printers.o3prm", xml_printers->attribute("name").toStdString());
}
