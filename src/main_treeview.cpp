#include <QtGui>

#include "treeview.h"

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

    auto rooms = new QStandardItem("rooms.o3prm");
    lip6->appendRow(rooms);

    QApplication app(argc, argv);
    QTreeView view;
    view.setModel(&model);
    view.expandAll();

    MyController controller(&view);

    view.show();
    return app.exec();
}

int main( int argc, char *argv[] ) 
{
    return withQStdItem(argc, argv);
    //return treeModelTreeViewUi(argc, argv);
} 
