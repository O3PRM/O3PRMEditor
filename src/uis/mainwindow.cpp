#include "mainwindow.h"

#include <iostream>

#include "qsciscintillaextended.h"

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

void MainWindow::__setupControllers()
{
    fc = new FileController( this, this );
    ec = new EditController( this, this );
    sc = new SearchController( this, this );
    vc = new ViewController( this, this );
    __setupProjectController();
    bc = new BuildController( this, this );
}

void MainWindow::__setupProjectController()
{
    pc = new o3prm::ProjectController( this );
    ui->projectExplorator->setVisible( false );
    ui->projectExplorator->setDragDropMode( QAbstractItemView::InternalMove );
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

    __setupProjectControllerConnections();
}

void MainWindow::__setupProjectControllerConnections()
{
    connect( ui->projectExplorator, SIGNAL( clicked( QModelIndex ) ),
             pc, SLOT( on_projectExplorator_clicked( QModelIndex ) ) );
    connect( ui->projectExplorator, SIGNAL( doubleClicked( QModelIndex ) ),
             pc, SLOT( on_projectExplorator_doubleClicked( QModelIndex ) ) );
    connect( ui->projectExplorator, SIGNAL( customContextMenuRequested( QPoint ) ),
             pc, SLOT( onCustomContextMenuRequested( QPoint ) ) );
    connect( ui->projectExplorator->itemDelegate(), SIGNAL( closeEditor( QWidget* ) ),
             pc, SLOT( onItemRenameFinished() ) );
    
    connect( ui->actionNewProject, SIGNAL( triggered() ),
             pc, SLOT( newProject() ) );

    connect( pc, SIGNAL( projectLoaded(o3prm::Project*) ),
             this, SLOT(loadProject(o3prm::Project*)));

    connect( ui->actionOpenProject, SIGNAL( triggered() ),
             pc, SLOT( openProject() ) );
    connect( ui->actionNewClass, SIGNAL( triggered() ),
             pc, SLOT( createNewClassFile() ) );
    connect( ui->actionNewModel, SIGNAL( triggered() ),
             pc, SLOT( createNewSystemFile() ) );
    connect( ui->actionNewRequestFile, SIGNAL( triggered() ),
             pc, SLOT( createNewRequestFile() ) );
    connect( ui->actionCloseProject, SIGNAL( triggered() ),
             pc, SLOT( closeProject() ) );

    QTimer::singleShot( 200, pc, SLOT( triggerInit() ) );
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

