#include "uis/treeview.h"

#include "models/treemodel.h"
#include "models/treeitem.h"

TreeView::TreeView(QWidget* parent):
    QMainWindow(parent)
{
    __model = new o3prm::TreeModel(tr("NewProject"), this);
    setupUi(this);
    //treeView->setModel(__model);
}

TreeView::~TreeView()
{

}

void TreeView::setupActions()
{

}

