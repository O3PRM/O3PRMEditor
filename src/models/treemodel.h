#ifndef O3PRM_TREEMODEL_H
#define O3PRM_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace o3prm {

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        TreeModel(const QString &data, QObject *parent = 0);
        ~TreeModel();

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

    signals:
        /// This signal is emited when a file is moved by drag and drop in the view.
        /// For constance with fileRenamed, \a oldFilePath is the complete, absolute,
        /// filename with its path, \a newPath too.
        void fileMoved( const QString & oldFilePath, const QString & newPath );
        void directoryLoaded(const QString& path);
        void fileRenamed(const QString& path, const QString& oldName, const QString& newName);
        void rootPathChanged(const QString& newPath);

    private:
        void setupModelData();

        TreeItem *rootItem;
};

}

#endif
