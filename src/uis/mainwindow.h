#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "ui_mainwindow.h"

#include "controllers/filecontroller.h"
#include "controllers/editcontroller.h"
#include "controllers/searchcontroller.h"
#include "controllers/viewcontroller.h"
#include "controllers/projectcontroller.h"
#include "controllers/buildcontroller.h"

/**
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

    protected:
        void closeEvent( QCloseEvent * event );

    protected slots:
        void showHelp();
        void showAboutDialog();

    private:
        struct PrivateData;
        PrivateData *__data;

        Ui::MainWindow *ui;

        FileController *fc;
        EditController *ec;
        SearchController *sc;
        ViewController *vc;
        o3prm::ProjectController *pc;
        BuildController *bc;

        friend class FileController;
        friend class EditController;
        friend class SearchController;
        friend class ViewController;
        //friend class o3prm::ProjectController;
        friend class BuildController;
        friend class Properties;

        void __setupControllers();
        void __setupProjectController();
        void __setupTabWidget();
        void __setupProjectExplorer();

        void __setupConnections();
        void __setupProjectControllerConnections();
};

#endif // MAINWINDOW_H
