#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "ui_mainwindow.h"
#include "models/project.h"

// #include "controllers/filecontroller.h"
// #include "controllers/editcontroller.h"
// #include "controllers/searchcontroller.h"
// #include "controllers/projectcontroller.h"
// #include "controllers/viewcontroller.h"
// #include "controllers/buildcontroller.h"

namespace o3prm
{
    class ProjectController;   
    class FileController;
    class BuildController;
    class EditorController;
    class NewSearchController;
}

class ViewController;
class EditController;

/*!
  The program main window.
  This class include and load the ui file, which describes all widgets and actions.
  Actions are handle in controllers, that are separated in different files.

  This class only handles the quit action.
 */
class MainWindow : public QMainWindow 
{
    Q_OBJECT

    public:
        explicit MainWindow( QWidget * parent = 0 );
        ~MainWindow();
        Ui::MainWindow* mainwindow();

        o3prm::ProjectController* projectController();
        ViewController* viewController() { return vc; }
        o3prm::EditorController* editorController() { return __editorController; }
        Ui::MainWindow* mainWindow() { return ui; }

    protected:
        void closeEvent( QCloseEvent * event );

    protected slots:
        void showHelp();
        void showAboutDialog();
        void loadProject(o3prm::Project* project);
        void saveProject(o3prm::Project* project);
        void closeProject();

    private:
        struct PrivateData;
        PrivateData *__data;

        Ui::MainWindow *ui;

        //o3prm::FileController *fc;
        EditController *ec;
        o3prm::NewSearchController *sc;
        ViewController *vc;
        o3prm::ProjectController *pc;
        o3prm::BuildController *bc;

        o3prm::EditorController* __editorController;

        friend class o3prm::FileController;
        friend class EditController;
        friend class ViewController;
        friend class o3prm::ProjectController;
        friend class o3prm::BuildController;
        friend class Properties;

        void __setupControllers();
        void __setupProjectController();
        void __setupTabWidget();
        void __setupProjectExplorer();

        void __setupConnections();
};

#endif // MAINWINDOW_H
