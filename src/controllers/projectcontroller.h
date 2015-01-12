#ifndef O3PRM_PROJECTCONTROLLER_H
#define O3PRM_PROJECTCONTROLLER_H

#include <QObject>
#include <QModelIndex>

#include "models/project.h"

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
            explicit ProjectController( MainWindow * mw, QObject *parent = 0 );
            /**
             * Destructor
             */
            ~ProjectController();

            /**
             * Return the current project
             */
            Project * currentProject() const;

            /**
             * Return true if a project is open, false otherwise.
             */
            bool isOpenProject() const;

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

            /**
             * Create an empty o3prml file with a class architecture,
             * and ask the filename.
             */
            void createNewClassFile();
            /**
             * Create an empty o3prmr file with a request architecture,
             * and ask the filename.
             */
            void createNewRequestFile();
            /**
             * Create an empty o3prml file with a system architecture,
             * and ask the filename.
             */
            void createNewSystemFile();

            /**
             * This method change a filename, a package in all file in the project.
             * It change package in the file when it is moved,
             * class or system name when it is renamed,
             * and imports in others files in both cases.

             * If \a fromFilePath is a file, it can be :
             *     - renamed, if only filename change in \a toFilePath;
             *     - moved, if only path change in \a toFilePath;
             *     - moved and renamed, if both path and filename change;
             *     - removed, if \a toFilePath is an empty string;

             * If \a fromFilePath is a directory, it can be :
             *     - renamed, if only dirname change in \a toFilePath;
             *     - moved, if only path change in \a toFilePath;
             *     - moved and renamed, if both path and dirname change;
             *     - removed, if \a toFilePath is an empty string;

             * This method do nothing if fromFilePath is not a valid file in the project.

             * Les imports dans les autres fichiers peuvent être
             * relatifs (ex : dans le même répertoire juste le
             * nom de la classe).

             * Peut être RELATIFS, que si c'est le MÊME package
             * ou un SUR dossier.
             * Sinon forcément ABSOLU par rapport au projet.

             * CLASSPATHS ? Il n'y a rien à changer dans le
             * classPath car si on fait appelle à lui c'est
             * qu'il ne fait pas appelle à nous.

             * WARNING : "import dir.*;" pas géré.
             * Pas de changement dans ces fichiers.
             * 
             * WARNING : Suppose qu'un fichier n'importe pas
             * plusieurs fichiers ayant le même nom.
             */
            void refactor( const QString & fromFilePath, const QString & toFilePath );

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
            bool on_projectExplorator_clicked( QModelIndex index );
            /**
             * Open the file or switch to it if it is already open.
             */
            bool on_projectExplorator_doubleClicked( QModelIndex index );
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
            MainWindow * mw;
            Project * currentProj;

            struct PrivateData;
            PrivateData * d;
    };

} // o3prm

#endif // O3PRM_PROJECTCONTROLLER_H
