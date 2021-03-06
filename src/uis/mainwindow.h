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
    class EditorController;
    class NewSearchController;
    class MenuController;
    class SettingsController;
}

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

        Ui::MainWindow* mainWindow();

        o3prm::NewSearchController* searchController();
        o3prm::ProjectController* projectController();
        o3prm::SettingsController* settingsController();
        o3prm::MenuController* menuController();
        o3prm::EditorController* editorController();

    signals:
        void started();
        void closing();

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

        Ui::MainWindow *__ui;

        o3prm::NewSearchController *__search;
        o3prm::ProjectController *__project;
        o3prm::SettingsController *__settings;
        o3prm::MenuController *__menu;
        o3prm::EditorController* __editor;

        void __setupControllers();
        void __setupProjectController();
        void __setupProjectExplorer();
        void __setupTabWidget();
        void __setupConnections();
};

#endif // MAINWINDOW_H
