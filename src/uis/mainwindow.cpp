#include "mainwindow.h"

#include <iostream>

#include "models/qsciscintillaextended.h"
#include "controllers/newsearchcontroller.h"
#include "controllers/projectcontroller.h"
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
    ui( new Ui::MainWindow )
{
    ui->setupUi( this );

    __setupControllers();
    __setupProjectExplorer();
    __setupTabWidget();

    __setupConnections();

    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
    delete __data;
    delete ui;
}

Ui::MainWindow* MainWindow::mainwindow()
{
    return ui;
}

void MainWindow::__setupControllers()
{
    //ec = new EditController( this, this );
    sc = new o3prm::NewSearchController( this, this );
    //vc = new ViewController( this, this );
    __setupProjectController();
    //bc = new o3prm::BuildController( this, this );

    __menuContoller = new o3prm::MenuController(this);
    __editorController = new o3prm::EditorController(this);
}

void MainWindow::__setupProjectController()
{
    pc = new o3prm::ProjectController( this );
    ui->projectExplorator->setVisible( false );
    ui->projectExplorator->setDragDropMode( QAbstractItemView::NoDragDrop );
    ui->projectExplorator->setEditTriggers( QAbstractItemView::NoEditTriggers );
    //ui->actionProjectProperties->setEnabled( false );
}

void MainWindow::__setupTabWidget()
{
    ui->splitter->setStretchFactor( 1, 128 );
    ui->splitter2->setStretchFactor( 0, 5 );
    ui->splitter2->setStretchFactor( 1, 0 );
    ui->splitter2->setStretchFactor( 2, 0 );

    //vc->setCommandWidgetVisible( false );
}

void MainWindow::__setupProjectExplorer()
{
    ui->projectExplorator->resize( 200, ui->projectExplorator->height() );
}

void MainWindow::__setupConnections()
{
    // Main ui's various events
    connect( ui->actionQuit, SIGNAL( triggered() ),
            this, SLOT( close() ) );
    connect( ui->actionHelp, SIGNAL( triggered() ),
            this, SLOT( showHelp() ) );
    connect( ui->actionAbout, SIGNAL( triggered() ),
            this, SLOT( showAboutDialog() ) );

    connect( pc, SIGNAL( projectLoaded(o3prm::Project*) ),
            this, SLOT(loadProject(o3prm::Project*)));
    connect( pc, SIGNAL( projectSaved(o3prm::Project*) ),
            this, SLOT(saveProject(o3prm::Project*)));
    connect( pc, SIGNAL( projectClosed() ),
            this, SLOT(closeProject()));

    // Setting up connections of each controller
    //fc->setupConnections();
    pc->setupConnections();
    //bc->setupConnections();
    sc->setupConnections();
    __editorController->setupConnections();
    __menuContoller->setupConnections();
}

void MainWindow::saveProject(o3prm::Project* project)
{
    // TODO: Refresh buttons accordingly
}

void MainWindow::closeProject()
{
    __menuContoller->setProjectExploratorVisibility( false );

    //// Disable new specific file creation
    //ui->actionNewClass->setEnabled( false );
    //ui->actionNewModel->setEnabled( false );
    //ui->actionNewRequestFile->setEnabled( false );
    ui->actionShowProjectExplorator->setEnabled( false );
    ui->projectExplorator->hide();

    //// Disable auto syntax check
    //bc->setAutoSyntaxCheck( false );
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    //bool oldAutoSyntax = bc->isAutoSyntaxCheck();
    //bc->setAutoSyntaxCheck( false );

    if ( __editorController->quit() ) 
    {
        event->accept();
    }
    else
    {
        //bc->setAutoSyntaxCheck( oldAutoSyntax );
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
    ui->actionShowProjectExplorator->setEnabled( true );
    ui->projectExplorator->setModel( project );
    //ui->projectExplorator->setRootIndex( project->root() );
    __menuContoller->setProjectExploratorVisibility( true );
    ui->projectExplorator->expandAll();
    ui->projectExplorator->show();

    // Enable new specific file creation
    // ui->actionNewClass->setEnabled( true );
    // ui->actionNewModel->setEnabled( true );
    // ui->actionNewRequestFile->setEnabled( true );

    // Enable auto syntax check
    //bc->setAutoSyntaxCheck( true );
}

o3prm::ProjectController* MainWindow::projectController()
{
    return pc;
}

