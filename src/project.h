#ifndef PROJECT_H
#define PROJECT_H

#define EXTNAME_O3PRM_PROJECT "o3prmp"
#define EXTNAME_O3PRM_RUN "o3prmr"
#define EXTNAME_O3PRM "o3prm"


#include <QFileSystemModel>

class Project : public QFileSystemModel {
    Q_OBJECT

  public:
    Project( const QString & dir, QObject * parent = 0 );
    ~Project();

    QString name() const;
    QString dir() const;

    ///
    bool isEditable() const;
    void setEditable( bool editable );

    ///
    bool isInside( const QString & filePath ) const;
    QList<QString> files() const;
    bool rmdirRec( const QModelIndex & index );

    void addPath( const QString & path );
    void addPaths( const QList<QString> & paths );
    void removePath( const QString & path );
    void clearPaths();
    QList<QString> paths() const;

    /** Close the project. */
    void close();


    QModelIndex root() const;
    int columnCount( const QModelIndex &parent ) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual bool dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

  signals:
    /// This signal is emited when a file is moved by drag and drop in the view.
    /// For constance with fileRenamed, \a oldFilePath is the complete, absolute,
    /// filename with its path, \a newPath too.
    void fileMoved( const QString & oldFilePath, const QString & newPath );

  private:

    struct PrivateData;
    PrivateData * d;
};

#endif // PROJECT_H
