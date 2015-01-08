#ifndef O3PRM_PROJECT_H
#define O3PRM_PROJECT_H

#define EXTNAME_O3PRM_PROJECT "o3prmp"
#define EXTNAME_O3PRM_RUN "o3prmr"
#define EXTNAME_O3PRM "o3prm"


#include <QFileSystemModel>
namespace o3prm
{

    class Project : public QFileSystemModel
    {
        Q_OBJECT

        public:
            /**
             * Constructor
             */
            Project( const QString & dir, QObject * parent = 0 );
            /**
             * Destructor
             */
            ~Project();

            /**
             * Return the project name.
             */
            QString name() const;
            /**
             * Return the project root directory.
             */
            QString dir() const;

            bool isEditable() const;
            void setEditable( bool editable );

            /**
             * Return true if \a filepath is in the project, false otherwise.
             */
            bool isInside( const QString & filePath ) const;
            /**
             * Return all o3prml and o3prmr files in the project.
             * \note Compute list each time.
             */
            QList<QString> files() const;
            bool rmdirRec( const QModelIndex & index );

            void addPath( const QString & path );
            void addPaths( const QList<QString> & paths );
            void removePath( const QString & path );
            void clearPaths();
            /**
             * Return paths to search for classes or systems.
             */
            QList<QString> paths() const;

            /**
             * Close project.
             * Save project's properties in project o3prmp file.
             */
            void close();

            QModelIndex root() const;
            int columnCount( const QModelIndex &parent ) const;
            virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
            QVariant headerData( int section,
                    Qt::Orientation orientation, 
                    int role = Qt::DisplayRole ) const;
            /**
             * \reimp Remove Qt::ItemIsEditable of default flags
             */
            virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
            virtual bool dropMimeData( const QMimeData * data,
                    Qt::DropAction action,
                    int row,
                    int column,
                    const QModelIndex & parent );

        signals:
            /// This signal is emited when a file is moved by drag and drop in the view.
            /// For constance with fileRenamed, \a oldFilePath is the complete, absolute,
            /// filename with its path, \a newPath too.
            void fileMoved( const QString & oldFilePath, const QString & newPath );

        private:
            struct PrivateData;
            PrivateData * d;
    };

}
#endif // O3PRM_PROJECT_H
