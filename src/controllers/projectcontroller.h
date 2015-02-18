#ifndef O3PRM_PROJECTCONTROLLER_H
#define O3PRM_PROJECTCONTROLLER_H

#include <QObject>
#include <QMenu>
#include <QModelIndex>

#include "models/project.h"
#include "models/buildmodel.h"

// Cross reference, do not replace with an include directive
class MainWindow; // uis/mainwindow.h

namespace o3prm
{
    class ProjectController : public QObject 
    {

        Q_OBJECT

        public:
            /// Constructor
            explicit ProjectController( MainWindow *parent = 0 ); 

            /// Destructor
            ~ProjectController();
            
            /// Call this to setup connection after all other controllers
            /// have been properly initalised.
            void setupConnections();

            /// Return the current project
            Project* currentProject() const;

            /// Return true if a project is open, false otherwise.
            bool hasProject() const;

        signals:
            /// Emitted when a project is loaded.
            void projectLoaded(o3prm::Project* project);

            /// Emitted when a project is saved.
            void projectSaved(o3prm::Project* project);

            /// Emitted when a project is closed.
            void projectClosed();

            /// Emitted when an item is double clicked.
            void fileOpened(QString path);

            /// Emitted when a file is removed.
            void fileRemoved(QString path);

            /// Emitted when an package is removed.
            void packageRemoved(QString path);

            /// Emitted when a file is renamed.
            void fileRenamed(QString oldPath, QString newPath);

            /// Emitted when a file is renamed.
            void packageRenamed(QString oldPath, QString newPath);

            /// Emitied before inference starts
            void beforeInference();

        protected slots:
            /// Create a new project.
            /// Open a dialog and ask the name and the directory of the project.
            void _newProject();

            /// Open the project projectpath,
            /// or open a dialog to choose the project directory if projectpath is empty.
            void _openProject( QString projectpath = QString() );

            /// Close the current project.
            void _closeProject();

            /// Does nothing.
            bool _onClick( QModelIndex index );

            /// Open the file or switch to it if it is already open.
            bool _onDoubleClick( QModelIndex index );

            /// Shows the custom context menu mathcin the ProjectItem under pos
            void _onCustomContextMenuRequested( const QPoint & pos );

        private:
            /// The current opened project.
            Project* __currentProj;

            /// The app main window
            MainWindow* __mainWidget;

            /// The build model used for inference.
            BuildModel* __build;

            /// The menu shown above Request ProjectItem
            QMenu* __requestMenu;

            /// The menu shown above File ProjectItem
            QMenu* __fileMenu;

            /// The menu shown above Package ProjectItem
            QMenu* __packageMenu;

            /// The menu shown above Project ProjectItem
            QMenu* __projectMenu;

            /// Saves the project
            bool __saveProject();

            // Close the project without saving it
            void __closeProject();

            /// Shows a Dialog and ask user to type a filename.
            /// If ok is false, then the user cancelled the operation.
            QString __askForName(ProjectItem::ItemType type, bool& ok, QString defaultValue="");

            /// Returns true if name is a child of parent in the current project
            /// and warns the user the his name is unusable.
            bool __existsAndWarn(QString name, ProjectItem* parent, ProjectItem::ItemType type);

            /// Returns true if name is a valid name, i.e. could be a file name and is not empty
            bool __validNameAndWarn(QString name);

            /// Add a file to the project and the filesystem.
            void __addFile(ProjectItem* parent);

            /// Add a request to the project and the filesystem.
            void __addRequest(ProjectItem* parent);

            /// Add a directory (package) to the project and the filesystem.
            void __addPackage(ProjectItem* parent);

            /// Handles custom context menu for projects.
            void __projectCustomContextMenu(const QPoint& pos, ProjectItem* item);

            /// Handles custom context menu for packages.
            void __packageCustomContextMenu(const QPoint& pos, ProjectItem* item);

            /// Handles custom context menu for files.
            void __fileCustomContextMenu(const QPoint& pos, ProjectItem* item);

            /// Handles custom context menu for requests.
            void __requestCustomContextMenu(const QPoint& pos, ProjectItem* item);

            // Show the rename dialog and rename the corresponding ProjectItem
            // and saves the project
            void __rename(ProjectItem* item);
            
            // Returns the default package declaration for item.
            QString __defaultPackage(ProjectItem* item);

            /// Executes the given item if its a Request.
            void __execute(ProjectItem* item);

            void __delete(ProjectItem* item);

            bool __removeDir(const QString & dirName);
    };

} // o3prm

#endif // O3PRM_PROJECTCONTROLLER_H
