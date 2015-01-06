#ifndef O3PRM_TREEVIEW_H
#define O3PRM_TREEVIEW_H

#include <QMainWindow>
#include "ui_treeview.h"
#include "models/treemodel.h"

class TreeView: public QMainWindow,
                private Ui::MainWindow
{
    Q_OBJECT

    public:
        TreeView(QWidget* Parent=0);
        ~TreeView();

    protected:
        void setupActions();

    private:
        o3prm::TreeModel* __model;

};

#endif

