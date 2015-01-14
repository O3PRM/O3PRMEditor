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

TEST(Project, TestDeserialisation)
{
    // Arrange
    QDomDocument dom("MyCoolProject");
    auto xml_root = dom.createElement("o3prmproject");
    xml_root.setAttribute("name", "MyCoolProject");
    dom.appendChild(xml_root);

    auto xml_project = dom.createElement("project");
    xml_project.setAttribute("name", "MyCoolProject");
    xml_root.appendChild(xml_project);

    auto xml_readme = dom.createElement("file");
    xml_readme.setAttribute("name", "README.txt");
    xml_project.appendChild(xml_readme);

    auto xml_org = dom.createElement("package");
    xml_org.setAttribute("name", "org/");
    xml_project.appendChild(xml_org);

    auto xml_lip6 = dom.createElement("package");
    xml_lip6.setAttribute("name", "lip6/");
    xml_org.appendChild(xml_lip6);

    auto xml_printers = dom.createElement("file");
    xml_printers.setAttribute("name", "printers.o3prm");
    xml_lip6.appendChild(xml_printers);

    // Act
    o3prm::Project* project = o3prm::Project::load("/some/path", xml_project);

    // Assert
    auto root = project->root();
    ASSERT_EQ("MyCoolProject", root->text().toStdString());
    ASSERT_EQ((int)o3prm::ProjectItem::ItemType::Project, root->type());
    ASSERT_EQ(2, root->rowCount());

    auto readme = static_cast<o3prm::ProjectItem*>(root->child(0));
    ASSERT_EQ("README.txt", readme->text().toStdString());
    ASSERT_EQ((int)o3prm::ProjectItem::ItemType::File, readme->type());
    ASSERT_EQ(0, readme->rowCount());

    auto org = static_cast<o3prm::ProjectItem*>(root->child(1));
    ASSERT_EQ("org/", org->text().toStdString());
    ASSERT_EQ((int)o3prm::ProjectItem::ItemType::Directory, org->type());
    ASSERT_EQ(1, org->rowCount());

    auto lip6 = static_cast<o3prm::ProjectItem*>(org->child(0));
    ASSERT_EQ("lip6/", lip6->text().toStdString());
    ASSERT_EQ((int)o3prm::ProjectItem::ItemType::Directory, lip6->type());
    ASSERT_EQ(1, lip6->rowCount());

    auto printers = static_cast<o3prm::ProjectItem*>(lip6->child(0));
    ASSERT_EQ("printers.o3prm", printers->text().toStdString());
    ASSERT_EQ((int)o3prm::ProjectItem::ItemType::File, printers->type());
    ASSERT_EQ(0, printers->rowCount());

    delete project;
}
