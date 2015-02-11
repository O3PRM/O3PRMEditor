#ifndef O3PRM_PROJECT_H
#define O3PRM_PROJECT_H

#define EXTNAME_O3PRM_PROJECT "o3prmp"
#define EXTNAME_O3PRM_RUN "o3prmr"
#define EXTNAME_O3PRM "o3prm"

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>
#include <QtXml>

#include "models/projectitem.h"

namespace o3prm
{
    class Project : public QStandardItemModel
    {
        Q_OBJECT

        public:
            static QString itemType2String(ProjectItem::ItemType type) 
            {
                switch (type)
                {
                    case ProjectItem::ItemType::Directory: { return tr("Package"); }
                    case ProjectItem::ItemType::File:      { return tr("File"); }
                    case ProjectItem::ItemType::Project:   { return tr("Project"); }
                    case ProjectItem::ItemType::Request:   { return tr("Request"); }
                    default: { return tr("Unknown"); }
                }
            }

            static QString itemType2String(int type) 
            {
                switch (type)
                {
                    case (int)ProjectItem::ItemType::Directory: { return tr("Package"); }
                    case (int)ProjectItem::ItemType::File:      { return tr("File"); }
                    case (int)ProjectItem::ItemType::Project:   { return tr("Project"); }
                    case (int)ProjectItem::ItemType::Request:   { return tr("Request"); }
                    default: { return tr("Unknown"); }
                }
            }

            /*!
              Constructor
             */
            Project( const QString & dir, const QString& name, QObject * parent = 0 );

            static Project* load(const QString& dir, const QDomElement& dom, QObject* parent = 0);

            /*!
              Destructor
             */
            ~Project();

            /*!
              Return the project name.
             */
            QString name() const;

            /*!
              Return the project root directory.
             */
            QDir dir() const;

            ProjectItem* root();

            bool isEditable() const;

            /*!
              Save a Project in XML as a .o3prmproject file
             */
            QDomDocument asXml();

            /// Legacy methods that I still need to implement
            /*!
              Return true if \a filepath is in the project, false otherwise.
             */
            bool isInside( const QString & filePath ) const;

            /*!
              Return all o3prml and o3prmr files in the project.
              \note Compute list each time.
             */
            QList<QString> files() const;

            void addPath( const QString & path );

            void clearPaths();

            /*!
              Return paths to search for classes or systems.
             */
            QList<QString> paths() const;

            QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const;

        signals:
            /*!
              This signal is emited when a file is moved by drag and drop in the view.
              For constance with fileRenamed, \a oldFilePath is the complete, absolute,
              filename with its path, \a newPath too.
             */
            void fileMoved( const QString & oldFilePath, const QString & newPath );

        private:
            QString __dir;
            QString __name;
            ProjectItem* __root;

            QDomElement __asXml(QDomDocument& doc, ProjectItem* item);
    };

}
#endif // O3PRM_PROJECT_H
