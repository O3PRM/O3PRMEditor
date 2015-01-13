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

    class ProjectItem: public QStandardItem
    {
        public :
            enum ItemType {
                Directory = 1000,
                File = 1001
            };

            ProjectItem(ItemType type):
                QStandardItem(), __type(type)
            {

            }
            
            ProjectItem(ItemType type, const QString &text):
                QStandardItem(text), __type(type)
            {
            }

            ProjectItem(ItemType type, const QIcon &icon, const QString &text):
                QStandardItem(icon, text), __type(type)
            {
            }

            explicit ProjectItem(ItemType type, int rows, int columns = 1):
                QStandardItem(rows, columns), __type(type)
            {
            }

            int type() const 
            {
                return (int)__type;
            }

            void setType(int type)
            {
                __type = (ItemType) type;
            }

        private:
            ItemType __type;
    };

    class Project : public QStandardItemModel
    {
        Q_OBJECT

        public:
            /**
             * Constructor
             */
            Project( const QString & dir, const QString& name, QObject * parent = 0 );

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
            QDir dir() const;

            /**
             * Return true if \a filepath is in the project, false otherwise.
             */
            bool isInside( const QString & filePath ) const;

            /**
             * Return all o3prml and o3prmr files in the project.
             * \note Compute list each time.
             */
            QList<QString> files() const;

            void addPath( const QString & path );

            void clearPaths();

            /**
             * Return paths to search for classes or systems.
             */
            QList<QString> paths() const;

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
