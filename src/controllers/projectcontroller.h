#ifndef O3PRM_PROJECTCONTROLLER_H
#define O3PRM_PROJECTCONTROLLER_H

#include <QObject>
#include <QModelIndex>

#include "models/project.h"
#include "models/buildmodel.h"

// Cross reference, do not replace with an include directive
class MainWindow; // uis/mainwindow.h

/*
 * Réflections sur les interactions dans l'explorateur de projet (EdP) :
 * Si il y a des fautes dans un fichier, quand on fait le parsage du projet,
 * mets à jour l'icône dans l'EdP.

 * CLIC DROIT
 *   - sur un dossier -> menu avec
 *      * Renommer
 *      * Supprimer (Confirmation)
 *      * Nouveau
 *      + Package
 *      + Classe
 *      + Système
 *      + Requête
 *      * Refactoring
 *      + Renommer ("dans tout le projet")
 *      + Déplacer ("dans tout le projet")
 *      + Supprimer ("indique les références dans le projet, source d'erreurs")
 *      - sur un fichier -> menu avec
 *      * Renommer
 *      * Supprimer (confirmation)
 *      * Éxécuter (uniquement pour le fichier O3PRMR)
 *      * Refactoring
 *      + Renommer ("Renomme dans tout le projet")
 *      + Déplacer ("dans tout le projet")
 *      + Supprimer ("indique les références dans le projet, source d'erreurs")

 * DRAG AND DROP
 *      - d'un dossier -> ne renomme PAS tout (passer par refactoring).
 *      - d'un fichier -> ne renomme PAS tout (passer par refactoring).

 * Un fichier peut être renommé :
 *      1) par l'explorateur de projet;
 *      2) par sauvegarder sous.

 * Quand un fichier est renommé :
 *      On change la référence dans le fichier même (projet ou pas). -> FileController
 *      On change la référence dans les autres fichiers que si projet. -> ProjectController

 * Quand un fichier est déplacé :
 *      On change le package partout que si projet. -> ProjectController

 * Quand on fait Refactoring -> Renommer : comme renommer normal
 * Quand on fait Refactoring -> Déplacer : on comme un drag and drop ?
 * Quand on fait Refactoring -> Supprimer : comme supprimer normal + on cherche dans le projet et message.
 */
namespace o3prm
{
    class ProjectController : public QObject 
    {

        Q_OBJECT

        public:
            /**
             * Constructor
             */
            explicit ProjectController( MainWindow *parent = 0 ); 

            /**
             * Destructor
             */
            ~ProjectController();
            
            // Call this to setup connection after all other controllers
            // have been properly initalised.
            void setupConnections();

            /**
             * Return the current project
             */
            Project * currentProject() const;

            /**
             * Return true if a project is open, false otherwise.
             */
            bool isOpenProject() const;

        signals:
            /// Emitted when a project is loaded
            void projectLoaded(o3prm::Project* project);

            /// Emitted when a project is saved
            void projectSaved(o3prm::Project* project);

            /// Emitted when a project is closed
            void projectClosed();

            void fileOpened(QString path);

            /// Emitted when a file is renamed.
            void fileRenamed(QString oldPath, QString newPath);

        public slots:
            /**
             * Create a new project.
             * Open a dialog and ask the name and the directory of the project.
             */
            void newProject();

            /**
             * Open the project projectpath,
             * or open a dialog to choose the project directory if projectpath is empty.
             */
            void openProject( QString projectpath = QString() );

            /**
             * Close the current project.
             */
            void closeProject();

        protected:
            /**
             * Add the project the "recentsProjects" list.
             */
            void addToRecentsProjects( const QString & projectPath );
            /**
             * Remove the projects of the "recentsProjects" list.
             */
            void removeOfRecentsProjects( const QString & projectPath );
            void saveProjectsState();

        protected slots:
            /**
             * Switch to this file if it is open.
             */
            bool _onClick( QModelIndex index );

            /**
             * Open the file or switch to it if it is already open.
             */
            bool _onDoubleClick( QModelIndex index );

            /**
             * When files are renamed, change the document filename if it is open;
             */
            void onProjectFileRenamed( const QString & path,
                    const QString & oldName,
                    const QString & newName );
            /**
             * When files are moved, change the document filename if it is open;
             */
            void onProjectFileMoved( const QString & oldFilePath, const QString & newPath );

            /**
             * Propose a menu when users ask for it.  Users can:
             *  - remove a package or a file,
             *  - add a package or a file,
             *  - rename a package or a file.
             *  - execute a o3prmr file.
             */
            void onCustomContextMenuRequested( const QPoint & pos );

            /**
             * This slot is called when user has called "rename" in the project explorator,
             * and the editing is finished.
             * We reset the model to non-editable.
             * \see onCustomContextMenuRequested()
             */
            void onItemRenameFinished();

            void triggerInit();

        private:
            Project *__currentProj;
            BuildModel *__build;

            MainWindow *__mainWidget;

            struct PrivateData;
            PrivateData * d;

            void __setupRecentProjects();
            void __setupContextMenu();

            bool __saveProject();

            // Close the project without saving it
            void __closeProject();

            /// Shows a Dialog and ask user to type a filename.
            /// If ok is false, then the user cancelled the operation.
            QString __askForName(ProjectItem::ItemType type, bool& ok);

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
    };

} // o3prm

#endif // O3PRM_PROJECTCONTROLLER_H
