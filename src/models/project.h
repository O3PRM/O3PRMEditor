#ifndef O3PRM_PROJECT_H
#define O3PRM_PROJECT_H

#define EXTNAME_O3PRM_PROJECT "o3prmp"
#define EXTNAME_O3PRM_RUN "o3prmr"
#define EXTNAME_O3PRM "o3prm"

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>

namespace o3prm
{

    class Project : public QStandardItemModel
    {
        Q_OBJECT

        public:
            /**
             * Constructor
             */
            Project( const QString & dir, const QString& name, QObject * parent = 0 ):
                QStandardItemModel(parent), __dir(dir), __name(name)
            {

            }

            /**
             * Destructor
             */
            ~Project() { }

            /**
             * Return the project name.
             */
            QString name() const { return __name; }

            /**
             * Return the project root directory.
             */
            QDir dir() const { return QDir(__dir); }

            bool isEditable() const;
            void setEditable( bool editable );

            /**
             * Return true if \a filepath is in the project, false otherwise.
             */
            bool isInside( const QString & filePath ) const;

            QString fileName ( const QModelIndex & index ) const;

            QFileInfo fileInfo ( const QModelIndex & index ) const;

            bool isDir(const QModelIndex& index) const;

            QModelIndex mkdir ( const QModelIndex & parent, const QString & name );

            bool remove ( const QModelIndex & index ) const;

            void setNameFilters ( const QStringList & filters );

            void setNameFilterDisables ( bool enable );

            void setReadOnly ( bool enable );

            QModelIndex setRootPath ( const QString & newPath );

            QModelIndex	index ( const QString & path, int column = 0 ) const;

            QString filePath ( const QModelIndex & index ) const;

            bool rmdir ( const QModelIndex & index ) const;

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
            QString __dir;
            QString __name;
    };

}
#endif // O3PRM_PROJECT_H
