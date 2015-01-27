#include "mainwindow.h"

#include <iostream>

#include "qsciscintillaextended.h"
#include "controllers/filecontroller.h"
#include "controllers/editcontroller.h"
#include "controllers/searchcontroller.h"
#include "controllers/projectcontroller.h"
#include "controllers/viewcontroller.h"
#include "controllers/buildcontroller.h"
#include "controllers/editorcontroller.h"

#include <QFileDialog>
#include <QTextBrowser>
#include <QMessageBox>
#include <QLayout>
#include <QDebug>

struct MainWindow::PrivateData {
    QDialog * dial;
    QTextBrowser * browser;
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
    fc = new o3prm::FileController( this, this );
    ec = new EditController( this, this );
    sc = new SearchController( this, this );
    vc = new ViewController( this, this );
    __setupProjectController();
    bc = new o3prm::BuildController( this, this );

    __editorController = new o3prm::EditorController(this, this);
}

void MainWindow::__setupProjectController()
{
    pc = new o3prm::ProjectController( this );
    ui->projectExplorator->setVisible( false );
    ui->projectExplorator->setDragDropMode( QAbstractItemView::InternalMove );
    ui->projectExplorator->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui->actionProjectProperties->setEnabled( false );
}

void MainWindow::__setupTabWidget()
{
    ui->splitter->setStretchFactor( 1, 128 );
    ui->splitter2->setStretchFactor( 0, 5 );
    ui->splitter2->setStretchFactor( 1, 0 );
    ui->splitter2->setStretchFactor( 2, 0 );

    vc->setCommandWidgetVisible( false );

    ui->tabBar->addTab( tr( "Search" ) );
    ui->tabBar->addTab( tr( "Build" ) );
    ui->tabBar->addTab( tr( "Execute" ) );
    ui->tabBar->setShape( QTabBar::RoundedSouth );
}

void MainWindow::__setupProjectExplorer()
{
    ui->projectExplorator->resize( 200, ui->projectExplorator->height() );
}

void MainWindow::__setupConnections()
{
    // TabBar
    connect( ui->tabBar, SIGNAL( currentChanged( int ) ),
             ui->dockStack, SLOT( setCurrentIndex( int ) ) );
    connect( ui->dockStack, SIGNAL( currentChanged( int ) ),
             ui->tabBar, SLOT( setCurrentIndex( int ) ) );
    connect( ui->tabBar, SIGNAL( currentChanged( int ) ),
             ui->dockStack, SLOT( show() ) );

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
    fc->setupConnections();
    pc->setupConnections();
    bc->setupConnections();
    __editorController->setupConnections();
}

void MainWindow::saveProject(o3prm::Project* project)
{
    // TODO: Refresh buttons accordingly
}

void MainWindow::closeProject()
{
    vc->setProjectExploratorVisibility( false );

    //// Disable new specific file creation
    ui->actionNewClass->setEnabled( false );
    ui->actionNewModel->setEnabled( false );
    ui->actionNewRequestFile->setEnabled( false );
    ui->actionProjectExploratorVisibility->setEnabled( false );
    ui->projectExplorator->hide();

    //// Disable auto syntax check
    bc->setAutoSyntaxCheck( false );
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    bool oldAutoSyntax = bc->isAutoSyntaxCheck();
    bc->setAutoSyntaxCheck( false );

    if ( fc->quit() ) 
    {
        event->accept();
    }
    else
    {
        bc->setAutoSyntaxCheck( oldAutoSyntax );
        event->ignore();
    }
}

void MainWindow::showHelp()
{
    if (__data->dial == 0)
    {
        // Create Help dialog
        __data->dial = new QDialog( this );
        __data->dial->setWindowIcon( QIcon( "qrc:/logo" ) );
        __data->dial->setWindowTitle( tr( "O3prmEditor -- Aide" ) );
        QVBoxLayout * layout = new QVBoxLayout( __data->dial );
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
    message += tr( "Auteurs : Vincent Renaudineau, Pierre-Henri Wuillemin.\n\n" );
    message += tr( "Copyright 2010 Lip6 (Paris, France). Tous droits réservés." );
    message += tr( "Ce logiciel est sous licence GPL v3.\n\n" );
    message += tr( "Il utilise les librairies :\n" );
    message += tr( " - Qt, de Nokia;\n" );
    message += tr( " - QScintilla, de Riverbank;\n" );
    message += tr( " - aGrUM, du Lip6 (Pierre-Henri Wuillemin, Christophe Gonzales, "
            "Lionel Torti, Vincent Renaudineau).\n" );
    QMessageBox::about( this, tr( "À Propos de O3prmEditor" ), message );
}

void MainWindow::loadProject(o3prm::Project* project)
{
    // Show project arborescence
    ui->actionProjectExploratorVisibility->setEnabled( true );
    ui->projectExplorator->setModel( project );
    //ui->projectExplorator->setRootIndex( project->root() );
    //vc->setProjectExploratorVisibility( true );
    ui->projectExplorator->expandAll();
    ui->projectExplorator->show();

    // Enable new specific file creation
    ui->actionNewClass->setEnabled( true );
    ui->actionNewModel->setEnabled( true );
    ui->actionNewRequestFile->setEnabled( true );

    // Enable auto syntax check
    bc->setAutoSyntaxCheck( true );
}

o3prm::ProjectController* MainWindow::projectController()
{
    return pc;
}

