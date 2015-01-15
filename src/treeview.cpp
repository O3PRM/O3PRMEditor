#include <iostream>

#include <QtGui>

#include "treeview.h"

MyController::MyController(QObject* parent):
    QObject(parent), view(static_cast<QTreeView*>(parent))
{
    connect(view, SIGNAL(clicked(QModelIndex)),
            this, SLOT(clicked(QModelIndex)));
    connect(view, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(doubleClicked(QModelIndex)));
}

MyController::~MyController()
{

}

void MyController::clicked(const QModelIndex& index)
{
    //auto item = static_cast<QStandardItem*>(index.internalPointer());
    std::cout << "Clicked : " << index.data().toString().toStdString() << std::endl;
    std::cout << "Row : " << index.row()  << std::endl;
    std::cout << "Column : " << index.column()  << std::endl;
    std::cout << "IsValid : " << index.isValid()  << std::endl;
}

void MyController::doubleClicked(const QModelIndex& index)
{
    auto item = static_cast<QStandardItem*>(index.internalPointer());
    std::cout << "Double clicked : " << item->text().toStdString()  << std::endl;
}
