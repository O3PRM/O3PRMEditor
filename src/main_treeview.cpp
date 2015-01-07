#include <QtGui>
#include "uis/treeview.h"
#include "models/treemodel.h"

int treeModelTreeViewUi(int argc, char *argv[])
{
    QApplication app( argc, argv );
    TreeView w;
    w.show();
    return app.exec();
}

int treeModelInWidget(int argc, char *argv[])
{
    QApplication app(argc, argv);
    o3prm::TreeModel model("NewProject");

    QTreeView view;
    view.setModel(&model);
    view.show();

    return app.exec();
}


int withQStdItem(int argc, char *argv[])
{
    QStandardItemModel model;
    model.setHorizontalHeaderItem(0, new QStandardItem("NewProject"));
    auto rootItem = model.invisibleRootItem();

    auto readme = new QStandardItem("README.txt");
    auto org = new QStandardItem("org/");

    QList<QStandardItem*> first_row;
    rootItem->appendRow(readme);
    rootItem->appendRow(org);

    auto lip6 = new QStandardItem("lip6/");
    org->appendRow(lip6);

    auto printers = new QStandardItem("printers.o3prm");
    lip6->appendRow(printers);

    QApplication app(argc, argv);
    QTreeView view;
    view.setModel(&model);
    view.expandAll();
    view.show();

    return app.exec();
}

int main( int argc, char *argv[] ) 
{
    return withQStdItem(argc, argv);
    //return treeModelTreeViewUi(argc, argv);
} 
