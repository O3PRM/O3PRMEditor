#include <QtGui>
#include "models/treemodel.h"

int main( int argc, char *argv[] ) 
{
    // QApplication a( argc, argv );
    // TreeView w;
    // w.show();

    QApplication app(argc, argv);
    o3prm::TreeModel model("NewProject");

    QTreeView view;
    view.setModel(&model);
    view.show();

    return app.exec();
} 
