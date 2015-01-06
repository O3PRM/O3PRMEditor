#include "uis/treeview.h"

#include "models/treemodel.h"
#include "models/treeitem.h"

TreeView::TreeView(QWidget* parent):
    QMainWindow(parent)
{
    setupUi(this);
    __model = new o3prm::TreeModel(tr("NewProject"), this);
    treeView->setModel(__model);
}

TreeView::~TreeView()
{
    if (__model)
        delete __model;
}

void TreeView::setupActions()
{

}

