#include "mainwindow.h"

#include <iostream>

#include "models/qsciscintillaextended.h"
#include "controllers/newsearchcontroller.h"
#include "controllers/projectcontroller.h"
#include "controllers/settingscontroller.h"
#include "controllers/menucontroller.h"
#include "controllers/editorcontroller.h"

#include <QFileDialog>
#include <QTextBrowser>
#include <QMessageBox>
#include <QLayout>
#include <QDebug>
#include <QStatusBar>

struct MainWindow::PrivateData
{
    QDialog * dial;
    QTextBrowser * browser;
    PrivateData():
        dial(0), browser(0)
    { }

};

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    __data( new PrivateData ),
    __ui( new Ui::MainWindow )
{
    __ui->setupUi( this );

    __setupControllers();
    __setupProjectExplorer();
    __setupTabWidget();

    __setupConnections();

    statusBar()->showMessage(tr("Ready"));
    emit started();
}

MainWindow::~MainWindow()
{
    delete __data;
    delete __ui;
}

Ui::MainWindow* MainWindow::mainWindow()
{
    return __ui;
}

o3prm::NewSearchController* MainWindow::searchController()
{
    return __search; 
}

o3prm::ProjectController* MainWindow::projectController() 
{
    return __project; 
}

o3prm::SettingsController* MainWindow::settingsController()
{
    return __settings; 
}

o3prm::MenuController* MainWindow::menuController()
{
    return __menu; 
}

o3prm::EditorController* MainWindow::editorController()
{
    return __editor; 
}

void MainWindow::__setupControllers()
{
    __search = new o3prm::NewSearchController( this, this );
    __setupProjectController();
    __settings = new o3prm::SettingsController(this, this);
    __menu = new o3prm::MenuController(this);
    __editor = new o3prm::EditorController(this);
}

void MainWindow::__setupProjectController()
{
    __project = new o3prm::ProjectController( this );
    __ui->projectExplorator->setVisible( false );
    __ui->projectExplorator->setDragDropMode( QAbstractItemView::NoDragDrop );
    __ui->projectExplorator->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void MainWindow::__setupTabWidget()
{
    __ui->splitter->setStretchFactor( 1, 128 );
    __ui->splitter2->setStretchFactor( 0, 5 );
    __ui->splitter2->setStretchFactor( 1, 0 );
    __ui->splitter2->setStretchFactor( 2, 0 );
}

void MainWindow::__setupProjectExplorer()
{
    __ui->projectExplorator->resize( 200, __ui->projectExplorator->height() );
}

void MainWindow::__setupConnections()
{
    // Main __ui's various events
    connect( __ui->actionQuit, SIGNAL( triggered() ),
            this, SLOT( close() ) );
    connect( __ui->actionHelp, SIGNAL( triggered() ),
            this, SLOT( showHelp() ) );
    connect( __ui->actionAbout, SIGNAL( triggered() ),
            this, SLOT( showAboutDialog() ) );

    connect( __project, SIGNAL( projectLoaded(o3prm::Project*) ),
            this, SLOT(loadProject(o3prm::Project*)));
    connect( __project, SIGNAL( projectSaved(o3prm::Project*) ),
            this, SLOT(saveProject(o3prm::Project*)));
    connect( __project, SIGNAL( projectClosed() ),
            this, SLOT(closeProject()));

    // Setting up connections of each controller
    __search->setupConnections();
    __project->setupConnections();
    __settings->setupConnections();
    __editor->setupConnections();
    __menu->setupConnections();
}

void MainWindow::saveProject(o3prm::Project* project)
{
    // TODO: Refresh buttons accordingly
}

void MainWindow::closeProject()
{
    __menu->setProjectExploratorVisibility( false );

    //// Disable new specific file creation
    __ui->actionShowProjectExplorator->setEnabled( false );
    __ui->projectExplorator->hide();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    emit closing();
    if ( __editor->quit() ) 
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::showHelp()
{
    if (__data->dial == 0)
    {
        // Create Help dialog
        __data->dial = new QDialog( this );
        __data->dial->setWindowIcon( QIcon( ":/icons/logo" ) );
        __data->dial->setWindowTitle( tr( "O3prmEditor -- Help" ) );
        auto layout = new QVBoxLayout( __data->dial );
        layout->setSpacing( 0 );
        layout->setContentsMargins( 0,0,0,0 );
        __data->browser = new QTextBrowser( __data->dial );
        __data->browser->setSource( QUrl( "qrc:/doc/index.html" ) );
        __data->browser->setOpenExternalLinks( true );
        layout->addWidget( __data->browser );
        __data->dial->setLayout( layout );
        __data->dial->resize( 1024 + 5, 768 + 5 );
    }
    __data->dial->open();
}

void MainWindow::showAboutDialog() 
{
    QString message;
    message += tr( "O3prmEditor version 1.0b4\n\n" );
    message += tr( "Authors : Vincent Renaudineau, Pierre-Henri Wuillemin, Lionel Torti.\n\n" );
    message += tr( "Copyright 2010 Lip6 (Paris, France). All right reserved." );
    message += tr( "This software is under GPL v3 licence.\n\n" );
    message += tr( "It uses the following libraries:\n" );
    message += tr( " - Qt;\n" );
    message += tr( " - QScintilla;\n" );
    message += tr( " - aGrUM.\n" );
    QMessageBox::about( this, tr( "About O3prmEditor" ), message );
}

void MainWindow::loadProject(o3prm::Project* project)
{
    // Show project arborescence
    __ui->actionShowProjectExplorator->setEnabled( true );
    __ui->projectExplorator->setModel( project );
    __menu->setProjectExploratorVisibility( true );
    __ui->projectExplorator->expandAll();
    __ui->projectExplorator->show();
}

