#ifndef O3PRM_TREEVIEW_H
#define O3PRM_TREEVIEW_H

#include <QObject>
#include <QModelIndex>
#include <QTreeView>

class MyController: public QObject
{
    Q_OBJECT

    public:
        MyController(QObject* parent=0);

        ~MyController();

        QTreeView* view;

    public slots:
        void clicked(const QModelIndex& index);

        void doubleClicked(const QModelIndex& index);
};

#endif
