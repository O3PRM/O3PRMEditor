#include "projectcontroller.h"

#include <iostream>

#include "buildcontroller.h"
#include "editcontroller.h"
#include "filecontroller.h"
#include "lexers/qscilexero3prml2.h"
#include "qsciscintillaextended.h"
#include "searchcontroller.h"
#include "ui_mainwindow.h"
#include "uis/mainwindow.h"
#include "uis/newprojectdialog.h"
#include "uis/projectproperties.h"
#include "viewcontroller.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QTimer>

#include <QFileSystemModel>

namespace o3prm
{
    /**
     * Private data
     */
    class ProjectController::PrivateData
    {
        public:
            PrivateData(ProjectController* parent):
                projectProperties(0)
            {
                recentsProjects = new QMenu();
                recentsProjectsMapper = new QSignalMapper(parent);
            }
            ~PrivateData() { }
            int numberMaxOfRecentsProjects;
            QMenu * recentsProjects;
            QList<QString> recentsProjectsList;
            QSignalMapper * recentsProjectsMapper;
            ProjectProperties * projectProperties;
            QMenu * fileMenu;
            QMenu * dirMenu;
            QMenu * rootMenu;
    };

    ProjectController::ProjectController(QWidget *parent ) :
        QObject( parent ),
        __currentProj( 0 ),
        __mainWidget(parent),
        d(new PrivateData(this))
    {
        __setupRecentProjects();
        __setupContextMenus();
    }

    void ProjectController::__setupRecentProjects()
    {
        // Construct "Recent project" menu
        //mw->ui->actionRecentProject->setMenu( d->recentsProjects );
        //connect( d->recentsProjectsMapper, SIGNAL( mapped( QString ) ),
        //        this, SLOT( pc->openProject( QString ) ) );
    }

    void ProjectController::__setupContextMenus()
    {
        d->fileMenu = new QMenu();
        d->fileMenu->addAction( tr( "Re&nommer" ) )->setData( "rename" );
        d->fileMenu->addAction( tr( "&Supprimer" ) )->setData( "remove" );
        d->fileMenu->addAction( tr( "&Éxécuter" ) )->setData( "execute" );

        // QMenu * m = d->fileMenu->addMenu(tr("&Refactoring"));
        // m->addAction(tr("Re&nommer"))->setData("refact-rename");
        // m->addAction(tr("&Déplacer"))->setData("refact-move");
        // m->addAction(tr("&Supprimer"))->setData("refact-remove");

        d->dirMenu = new QMenu();
        d->dirMenu->addAction( tr( "Re&nommer" ) )->setData( "rename" );
        // Don't work for moment:
        d->dirMenu->addAction( tr( "&Supprimer" ) )->setData( "remove" ); 

        QMenu * m = d->dirMenu->addMenu( tr( "Ajouter" ) );
        m->addAction( tr( "&Package" ) )->setData( "addPackage" );
        // m->addAction(tr("&Classe"))->setData("addClass");
        // m->addAction(tr("&Système"))->setData("addSystem");
        // m->addAction(tr("&Requête"))->setData("addRequest");
        m = d->dirMenu->addMenu( tr( "&Refactoring" ) );
        // m->addAction(tr("Re&nommer"))->setData("refact-rename");
        // m->addAction(tr("&Déplacer"))->setData("refact-move");
        // m->addAction(tr("&Supprimer"))->setData("refact-remove");

        d->rootMenu = new QMenu();
        d->rootMenu->addAction( tr( "Ajouter un &package" ) )->setData( "package" );
        d->rootMenu->addAction( tr( "Ajouter un fichier" ) )->setData( "file" );
    }

    ProjectController::~ProjectController()
    {
    }

    void ProjectController::triggerInit()
    {
        // QSettings settings;
        // settings.beginGroup( "project" );

        // d->numberMaxOfRecentsProjects = settings.value( "numberMaxOfRecentsProjects",5 ).toInt();

        // // Read the last closed projects in settings
        // int size = settings.beginReadArray( "recentsProjects" );

        // for ( int i = 0; i < size ; i++ )
        // {
        //     settings.setArrayIndex( i );
        //     addToRecentsProjects( settings.value( "project" ).toString() );
        // }

        // settings.endArray();

        // // Reopen project don't close last time.
        // QString lastProject = settings.value( "last project" ).toString();

        // if ( ! lastProject.isEmpty() )
        // {
        //     openProject( lastProject );
        // }
        // else
        // {
        //     currentProj = 0;
        //     mw->ui->projectExplorator->setModel( currentProj );

        //     // Change the current directory
        //     QDir::setCurrent( QDir::homePath() );
        // }
    }

    Project * ProjectController::currentProject() const
    {
        return __currentProj;
    }

    bool ProjectController::isOpenProject() const
    {
        return __currentProj != 0;
    }

    void ProjectController::newProject()
    {
        NewProjectDialog dial( __mainWidget );

        // If user cancel, do nothing
        if ( dial.exec() )
        {
            __closeProject();

            __currentProj = new Project( dial.projectDir(), tr("NewProject"), this );
            emit projectLoaded(__currentProj);

            // connect( currentProj, SIGNAL( fileRenamed( QString,QString,QString ) ),
            //         this, SLOT( onProjectFileRenamed( QString,QString,QString ) ) );
            // connect( currentProj, SIGNAL( fileMoved( QString,QString ) ),
            //         this, SLOT( onProjectFileMoved( QString,QString ) ) );

            // d->projectProperties = new ProjectProperties( currentProj, mw );
            // mw->ui->actionProjectProperties->setEnabled( true );

            // connect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
            //         d->projectProperties, SLOT( exec() ) );

            // // We change current directory to the project directory
            // QDir::setCurrent( dial.projectDir() );

            // Create empty files
            //createNewClassFile();


            saveProjectsState();
        }
    }

    void ProjectController::__closeProject()
    {
        // Close existing projects
        // if ( currentProj )
        // {
        //     currentProj->close();
        //     currentProj->deleteLater();
        //     currentProj = 0;
        // }
    }

    void ProjectController::createNewClassFile()
    {
        //// We add a new o3prml empty file,
        //auto sci = mw->fc->newDocument( tr( "Nouveau o3prml" ), QsciScintillaExtended::O3prml );
        //sci->setLexer( QsciScintillaExtended::O3prml );

        //// give it the focus,
        //mw->ui->tabWidget->setCurrentWidget( sci );
        //sci->setFocus();

        //auto path = currentProj->dir().absoluteFilePath("classes/empty_file.o3prml");
        //if ( mw->fc->saveAsFile( sci, path ) )
        //{
        //    // and prefill it.
        //    QFileInfo info( sci->filename() );
        //    QString className = info.baseName();
        //    auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
        //    QString packageName = packageRelativePath.replace( "/","." );
        //    QString text;

        //    if ( ! packageName.isEmpty() )
        //    {
        //        text += "\npackage "+ packageName +";\n";
        //    }
        //    text += "\n//\nclass "+ className + " {\n}\n";

        //    sci->setText( text );
        //    sci->setCursorPosition( 2,6 );

        //    mw->fc->saveFile( sci );
        //}
        //else
        //{
        //    mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        //}
    }

    void ProjectController::createNewRequestFile()
    {
        //// We add a new o3prmr empty file,
        //auto sci = mw->fc->newDocument( tr( "Nouveau o3prmr" ), QsciScintillaExtended::O3prmr );
        //sci->setLexer( QsciScintillaExtended::O3prmr );

        //// give it the focus,
        //mw->ui->tabWidget->setCurrentWidget( sci );
        //sci->setFocus();

        //auto path = currentProj->dir().absoluteFilePath("requests/empty_file.o3prmr");
        //if ( mw->fc->saveAsFile( sci, path ) ) {
        //    // and prefill it.
        //    QFileInfo info( sci->filename() );
        //    QString blockName = info.baseName();
        //    auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
        //    QString packageName = packageRelativePath.replace( "/","." );
        //    QString text;

        //    if ( ! packageName.isEmpty() )
        //    {
        //        text += "\npackage "+ packageName +";\n";
        //    }
        //    text += "\n//\nrequest "+ blockName + " {\n}\n";

        //    sci->setText( text );
        //    sci->setCursorPosition( 2,8 );

        //    mw->fc->saveFile( sci );
        //}
        //else
        //{
        //    mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        //}
    }

    void ProjectController::createNewSystemFile()
    {
        //auto sci = mw->fc->newDocument( tr( "Nouveau o3prml" ), QsciScintillaExtended::O3prml );
        //sci->setLexer( QsciScintillaExtended::O3prml );

        //// give it the focus,
        //mw->ui->tabWidget->setCurrentWidget( sci );
        //sci->setFocus();

        //if ( mw->fc->saveAsFile( sci, currentProj->dir().absoluteFilePath("systems/empty_file.o3prml") ) )
        //{
        //    // and prefill it.
        //    QFileInfo info( sci->filename() );
        //    QString systemName = info.baseName();
        //    auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
        //    QString packageName = packageRelativePath.replace( "/","." );
        //    QString text;

        //    if ( ! packageName.isEmpty() )
        //    {
        //        text += "\npackage "+ packageName +";\n";
        //    }
        //    text += "\n//\nsystem "+ systemName + " {\n}\n";

        //    sci->setText( text );
        //    sci->setCursorPosition( 2,7 );

        //    mw->fc->saveFile( sci );
        //}
        //else
        //{
        //    mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        //}
    }

    void ProjectController::openProject( QString projectpath )
    {
        //if ( projectpath.isEmpty() )
        //{
        //    // Retrieve the project dir
        //    auto msg = tr( "Sélectionnez le répertoire du projet" );
        //    projectpath = QFileDialog::getExistingDirectory( mw, msg, QDir::homePath() );
        //}

        //QDir qDir( projectpath );

        //// TODO : Warning are not ?
        //if ( projectpath.isEmpty() || ! qDir.exists() )
        //{
        //    return;
        //}

        //if ( ! qDir.exists( qDir.dirName()+".o3prmp" ) ) 
        //{
        //    auto msg = tr( "Ce répertoire ne contient pas de fichier projet.\nLe créer ?" );
        //    int rep = QMessageBox::warning( mw,
        //            tr( "Attention" ),
        //            msg,
        //            QMessageBox::Cancel,
        //            QMessageBox::Ok );

        //    if ( rep == QMessageBox::Cancel )
        //    {
        //        return;
        //    }
        //}

        //if ( currentProj )
        //{
        //    closeProject();
        //}

        //currentProj = new Project( qDir.absolutePath(), "OpenedProject", this );

        //connect( currentProj, SIGNAL( fileRenamed( QString,QString,QString ) ),
        //        this, SLOT( onProjectFileRenamed( QString,QString,QString ) ) );
        //connect( currentProj, SIGNAL( fileMoved( QString,QString ) ),
        //        this, SLOT( onProjectFileMoved( QString,QString ) ) );

        //d->projectProperties = new ProjectProperties( currentProj, mw );

        //mw->ui->actionProjectProperties->setEnabled( true );

        //connect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
        //        d->projectProperties, SLOT( exec() ) );

        //// We change current directory to the project directory
        //QDir::setCurrent( currentProj->dir().absolutePath() );

        //removeOfRecentsProjects( qDir.absolutePath() );

        //// Show project arborescence
        //mw->ui->actionProjectExploratorVisibility->setEnabled( true );

        //mw->ui->projectExplorator->setModel( currentProj );

        //mw->ui->projectExplorator->setRootIndex( currentProj->root() );

        //mw->vc->setProjectExploratorVisibility( true );

        //// Enable new specific file creation
        //mw->ui->actionNewClass->setEnabled( true );

        //mw->ui->actionNewModel->setEnabled( true );

        //mw->ui->actionNewRequestFile->setEnabled( true );

        //saveProjectsState();

        //// Enable auto syntax check
        //mw->bc->setAutoSyntaxCheck( true );

        //QTimer::singleShot( 200, mw->ui->projectExplorator, SLOT( expandAll() ) );
    }

    void ProjectController::closeProject() 
    {
        //if ( currentProj ) {
        //    addToRecentsProjects( currentProj->dir().absolutePath() );

        //    // Tests because can be delete by MainWindow
        //    disconnect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
        //            d->projectProperties, SLOT( exec() ) );
        //    d->projectProperties->close();
        //    d->projectProperties->deleteLater();
        //    d->projectProperties = 0;

        //    currentProj->deleteLater();
        //    currentProj = 0;

        //    mw->ui->actionProjectProperties->setEnabled( false );
        //    QDir::setCurrent( QDir::homePath() );
        //}

        //mw->ui->projectExplorator->setModel( currentProj );
        //mw->vc->setProjectExploratorVisibility( false );

        //// Disable new specific file creation
        //mw->ui->actionNewClass->setEnabled( false );
        //mw->ui->actionNewModel->setEnabled( false );
        //mw->ui->actionNewRequestFile->setEnabled( false );
        //mw->ui->actionProjectExploratorVisibility->setEnabled( false );

        //// Disable auto syntax check
        //mw->bc->setAutoSyntaxCheck( false );

        //saveProjectsState();
    }

    void ProjectController::refactor( const QString & fromFilePath, const QString & toFilePath ) 
    {
        qWarning() << "Project::refactor() was called but is not yet implemented.\n"
            "For moment, this method do nothing.";
        return;
    } 

    void ProjectController::addToRecentsProjects( const QString & projetPath )
    {
        //if ( projetPath.isEmpty() || d->recentsProjectsList.contains( projetPath ) )
        //{
        //    return;
        //}

        //d->recentsProjectsList.append( projetPath );

        //QAction * action = new QAction( QDir( projetPath ).dirName(), mw );

        //action->setData( projetPath );

        //connect( action, SIGNAL( triggered() ), d->recentsProjectsMapper, SLOT( map() ) );

        //d->recentsProjectsMapper->setMapping( action, projetPath );

        //if ( d->recentsProjects->actions().isEmpty() )
        //{
        //    d->recentsProjects->addAction( action );
        //}
        //else
        //{
        //    d->recentsProjects->insertAction( d->recentsProjects->actions().first(), action );
        //}

        ////
        //if ( d->recentsProjects->actions().size() > d->numberMaxOfRecentsProjects )
        //{
        //    d->recentsProjects->removeAction( d->recentsProjects->actions().last() );
        //}
    }

    void ProjectController::removeOfRecentsProjects( const QString & projetPath ) 
    {
        //if ( projetPath.isEmpty() || ! d->recentsProjectsList.contains( projetPath ) )
        //{
        //    return;
        //}

        //d->recentsProjectsList.removeOne( projetPath );

        //QAction * a = qobject_cast<QAction *>( d->recentsProjectsMapper->mapping( projetPath ) );

        //if ( a != 0 ) 
        //{
        //    d->recentsProjectsMapper->removeMappings( a );
        //    d->recentsProjects->removeAction( a );
        //}
    }

    void ProjectController::saveProjectsState() 
    {
        //QSettings settings;
        //settings.beginGroup( "project" );

        //if ( currentProj ) 
        //{
        //    settings.setValue( "last project", currentProj->dir().absolutePath() );
        //    currentProj->close();
        //}
        //else
        //{
        //    settings.setValue( "last project", "" );
        //}

        //// Save the last closed projects in settings
        //int size = d->recentsProjects->actions().size();

        //settings.beginWriteArray( "recentsProjects",size );

        //for ( int i = 0 ; i < size ; i++ ) 
        //{
        //    settings.setArrayIndex( i );
        //    settings.setValue( "project",d->recentsProjects->actions().at( i )->data() );
        //}

        //settings.endArray();
    }

    bool ProjectController::on_projectExplorator_clicked( QModelIndex index ) 
    {
        //QString filename = index.data( QFileSystemModel::FilePathRole ).toString();

        //if ( mw->fc->isOpenFile( filename ) )
        //{
        //    mw->ui->tabWidget->setCurrentWidget( mw->fc->fileToDocument( filename ) );
        //}
        //else
        //{
        //    return false;
        //}

        //return true;

        return false;
    }

    bool ProjectController::on_projectExplorator_doubleClicked( QModelIndex index ) 
    {
        // // Return false if index is a dir
        // if ( currentProj->isDir( index ) )
        // {
        //     return false;
        // }
        // else // Else open the file
        // {
        //     return mw->fc->openFile( index.data( QFileSystemModel::FilePathRole ).toString() );
        // }

        return false;
    }

    void ProjectController::onProjectFileRenamed( const QString & path,
            const QString & oldName,
            const QString & newName ) 
    {
         //QsciScintillaExtended * sci = mw->fc->fileToDocument( path + "/" + oldName );

         //if ( sci != 0 )
         //{
         //    sci->setFilename( path + "/" + newName );
         //}
    }

    void ProjectController::onProjectFileMoved( const QString & oldFilePath, const QString & newFilePath ) 
    {
        //QsciScintillaExtended * sci = mw->fc->fileToDocument( oldFilePath );

        //if ( sci != 0 ) 
        //{
        //    sci->setFilename( newFilePath );
        //}
    }

    void ProjectController::onCustomContextMenuRequested( const QPoint & pos ) 
    {
        auto view = ((MainWindow*)__mainWidget)->ui->projectExplorator;
        auto index = view->indexAt(pos);
        auto item = static_cast<QStandardItem*>(index.internalPointer());

        auto map = view->mapToGlobal( pos ) ;
        QAction * a = d->rootMenu->exec(map);

        if ( a == 0 )
        {
            return;
        }
        auto parent = item->hasChildren()?item->child(0):item;
        QString path = "";
        for (auto iter = parent; iter != 0; iter = iter->parent())
        {
            path = iter->text() + "/" + path;
        }
        QDir dir(__currentProj->dir());
        if ( a->data().toString() == "package" ) 
        {
            auto package = new ProjectItem(ProjectItem::ItemType::Directory, "new_package/");
            parent->appendRow(package);
            dir.mkpath(QString(path));
        }
        else if ( a->data().toString() == "file" )
        {
            auto file = new ProjectItem(ProjectItem::ItemType::File, "new_file.o3prm");
            parent->appendRow(file);
            dir.cd(path);
            auto file_path = dir.absoluteFilePath(file->text());
            QFile data(file_path);
            if (data.open(QFile::WriteOnly | QFile::Truncate)) 
            {
                QTextStream out(&data);
                path = path.replace('/', '.').trimmed();
                while (path.size() > 0 and path.endsWith('.'))
                {
                    path.truncate(path.size() - 1);
                }
                out << "namespace " << path << ';' << '\n';
            }

        }
    }

//    void ProjectController::onCustomContextMenuRequested( const QPoint & pos ) 
//    {
//        auto view = ((MainWindow*)__mainWidget)->ui->projectExplorator;
//        auto selection = view->selectionModel()->selectedIndexes();
//        if (selection.count() == 0)
//        {
//            std::cout << "nothing" << std::endl;
//            return;
//        }
//        std::cout << "Selection count: " << selection.count() << std::endl;
//        auto first = selection[0];
//
//        auto item = static_cast<QStandardItem*>(first.internalPointer());
//
//        std::cout << "Item: " << item << std::endl;
//        std::cout << "Type: " << item->type() << std::endl;
//        std::cout << "Has children " << item->hasChildren() << std::endl;
//
//        // If it's a dir
//        // if ( item->type() == ProjectItem::ItemType::Directory ) 
//        // {
//        auto map = view->viewport()->mapToGlobal( pos ) ;
//        QAction * a = d->rootMenu->exec(map);
//
//        if ( a == 0 )
//        {
//            return;
//        }
//
//        if ( a->data().toString() == "package" ) 
//        {
//            auto package = new ProjectItem(ProjectItem::ItemType::Directory, "new_package");
//            if (item->hasChildren())
//                item->child(0)->appendRow(package);
//            else
//                item->appendRow(package);
//            // QModelIndex newPackage = currentProj->mkdir( currentProj->root(),"new_package" );
//            // // setEditable is set to false when editing is finished. See onItemRenameFinished()
//            // currentProj->setEditable( true ); 
//            // mw->ui->projectExplorator->edit( newPackage );
//        }
//        else if ( a->data().toString() == "execute" ) 
//        {
//            // qDebug() << "execute project !";
//        }
//    }

    //void ProjectController::onCustomContextMenuRequested( const QPoint & pos ) 
    //{
    //    auto view = ((MainWindow*)__mainWidget)->ui->projectExplorator;
    //    auto index = view->indexAt( pos );
    //    auto item = static_cast<QStandardItem*>(index.internalPointer());

    //    std::cout << "Item: " << item << std::endl;
    //    std::cout << "Type: " << item->type() << std::endl;

    //    // If it's a dir
    //    // if ( item->type() == ProjectItem::ItemType::Directory ) 
    //    // {
    //        auto map = view->viewport()->mapToGlobal( pos ) ;
    //        QAction * a = d->rootMenu->exec(map);

    //        if ( a == 0 )
    //        {
    //            return;
    //        }

    //        if ( a->data().toString() == "package" ) 
    //        {
    //            auto package = new ProjectItem(ProjectItem::ItemType::Directory, "new_package");
    //            item->appendRow(package);
    //            // QModelIndex newPackage = currentProj->mkdir( currentProj->root(),"new_package" );
    //            // // setEditable is set to false when editing is finished. See onItemRenameFinished()
    //            // currentProj->setEditable( true ); 
    //            // mw->ui->projectExplorator->edit( newPackage );
    //        }
    //        else if ( a->data().toString() == "execute" ) 
    //        {
    //            // qDebug() << "execute project !";
    //        }
    //    //}
    //    //else if ( currentProj->isDir( index ) ) 
    //    //{
    //    //    QAction * a = d->dirMenu->exec( mw->ui->projectExplorator->viewport()->mapToGlobal( pos ) );

    //    //    if ( a == 0 )
    //    //    {
    //    //        return;
    //    //    }

    //    //    if ( a->data().toString() == "rename" ) 
    //    //    {
    //    //        // setEditable is set to false when editing is finished. See onItemRenameFinished()
    //    //        currentProj->setEditable( true );
    //    //        mw->ui->projectExplorator->edit( index );
    //    //    }
    //    //    else if ( a->data().toString() == "remove" ) 
    //    //    {
    //    //        auto msg = tr( "Voulez-vous vraiment supprimer définitivement le package"
    //    //                "%1 ainsi que tout son contenu ?" );
    //    //        msg = msg.arg( currentProj->fileName( index ) );
    //    //        int ret = QMessageBox::question( mw,
    //    //                tr( "Supprimer le package" ),
    //    //                msg,
    //    //                QMessageBox::Ok,
    //    //                QMessageBox::Cancel );

    //    //        if ( ret != QMessageBox::Ok )
    //    //        {
    //    //            return;
    //    //        }

    //    //        currentProj->rmdirRec( index );
    //    //    }
    //    //    else if ( a->data().toString() == "addPackage" ) 
    //    //    {
    //    //        QModelIndex newPackage = currentProj->mkdir( index,"new_package" );
    //    //        // setEditable is set to false when editing is finished. See onItemRenameFinished()
    //    //        currentProj->setEditable( true ); 
    //    //        mw->ui->projectExplorator->edit( newPackage );
    //    //    }
    //    //    else if ( a->data().toString() == "addClass" ) 
    //    //    {
    //    //        //createNewClassFile();
    //    //    }
    //    //    else if ( a->data().toString() == "addSystem" ) 
    //    //    {
    //    //        //createNewSystemFile();
    //    //    }
    //    //    else if ( a->data().toString() == "addRequest" ) 
    //    //    {
    //    //        //createNewRequestFile();
    //    //    }
    //    //}
    //    //else
    //    //{
    //    //    // Hide or show execute action (show only with o3prmr files).
    //    //    auto filter = index.data().toString().endsWith( ".o3prmr",Qt::CaseInsensitive );
    //    //    d->fileMenu->actions().last()->setVisible( filter );

    //    //    QAction * a = d->fileMenu->exec( mw->ui->projectExplorator->viewport()->mapToGlobal( pos ) );

    //    //    if ( a == 0 )
    //    //    {
    //    //        return;
    //    //    }

    //    //    if ( a->data().toString() == "rename" ) 
    //    //    {
    //    //        // setEditable is set to false when editing is finished. See onItemRenameFinished()
    //    //        currentProj->setEditable( true ); 
    //    //        mw->ui->projectExplorator->edit( index );
    //    //    }
    //    //    else if ( a->data().toString() == "remove" ) 
    //    //    {
    //    //        auto msg = tr( "Voulez-vous vraiment supprimer définitivement le fichier\n%1 ?" );
    //    //        msg = msg.arg( currentProj->fileName( index ) );
    //    //        int ret = QMessageBox::question( mw,
    //    //                tr( "Supprimer le fichier" ),
    //    //                msg,
    //    //                QMessageBox::Ok,
    //    //                QMessageBox::Cancel );

    //    //        if ( ret != QMessageBox::Ok )
    //    //        {
    //    //            return;
    //    //        }

    //    //        currentProj->remove( index );
    //    //    }
    //    //    else if ( a->data().toString() == "execute" and
    //    //            currentProj->fileInfo( index ).suffix() == "o3prmr" ) 
    //    //    {
    //    //        mw->bc->execute( index.data( QFileSystemModel::FilePathRole ).toString() );
    //    //    }
    //    //}
    //}

    void ProjectController::onItemRenameFinished() 
    {
        //currentProj->setEditable( false );
    }

} // o3prm
