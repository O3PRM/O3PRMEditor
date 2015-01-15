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
#include <QFileDialog>
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

    ProjectController::ProjectController(MainWindow *parent ) :
        QObject( parent ),
        __currentProj( 0 ),
        __mainWidget(parent),
        d(new PrivateData(this))
    {
        __setupRecentProjects();
        __setupContextMenu();
    }

    void ProjectController::__setupRecentProjects()
    {
        // Construct "Recent project" menu
        //mw->ui->actionRecentProject->setMenu( d->recentsProjects );
        //connect( d->recentsProjectsMapper, SIGNAL( mapped( QString ) ),
        //        this, SLOT( pc->openProject( QString ) ) );
    }

    void ProjectController::__setupContextMenu()
    {
        d->rootMenu = new QMenu(__mainWidget);
        //d->rootMenu->addAction( tr( "Ajouter un &package" ) )->setData( "package" );
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
            __saveProject();

            emit projectLoaded(__currentProj);
        }
    }

    bool ProjectController::__saveProject()
    {
        QString name = "%1.o3prmproject";
        name = name.arg(__currentProj->name());

        QDir dir(__currentProj->dir());
        QFile file(dir.absoluteFilePath(name));

        if( file.open( QIODevice::WriteOnly | QIODevice::Truncate) )
        {
            auto dom = __currentProj->asXml();

            QTextStream ts( &file );
            ts << dom.toString();

            file.close();
            return true;
        }
        return false;
    }

    void ProjectController::__closeProject()
    {
        if ( __currentProj )
        {
            delete __currentProj;
            __currentProj = 0;
            emit projectClosed();
        }
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

    void ProjectController::openProject( QString path )
    {
        __closeProject();

        if ( path.isEmpty() )
        {
            path = QFileDialog::getOpenFileName ( __mainWidget,
                    tr( "Open project file" ),
                    QDir::home().absolutePath(),
                    tr("O3PRM project Files (*.o3prmproject)")
                    );
        }

        QFile file(path);

        if ( path.isEmpty() or not file.exists() )
        {
            return;
        }

        if( !file.open( QIODevice::ReadOnly ) )
        {
            return;
        }

        QDomDocument doc("o3prmproject");
        if( !doc.setContent( &file ) )
        {
            file.close();
            return;
        }
        file.close();

        QFileInfo info(file);
        auto tag = Project::itemType2String(ProjectItem::ItemType::Project).toLower();
        auto projects = doc.elementsByTagName(tag);

        if (not projects.size())
        {
            return;
        }

        auto node = projects.at(0);
        auto elt = static_cast<QDomElement*>(&node);
        __currentProj = Project::load(info.absoluteDir().absolutePath(), *elt, this);

        emit projectLoaded(__currentProj);
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
        auto parent = static_cast<ProjectItem*>(index.internalPointer());
        auto item = parent->child(index.row());
        switch (item->type())
        {
            case ProjectItem::ItemType::Directory:
                {
                    return false;
                }
            case ProjectItem::ItemType::File:
                {
                    QDir dir(__currentProj->dir());
                    auto path = dir.absoluteFilePath(item->path());
                    std::cout << path.toStdString() << std::endl;
                    return __mainWidget->fc->openFile(path);
                }
            default:
                {
                    return false;
                }
        }
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
        auto view = __mainWidget->mainwindow()->projectExplorator;
        auto index = view->indexAt(pos);
        auto item = static_cast<QStandardItem*>(index.internalPointer());

        auto map = view->mapToGlobal( pos ) ;
        QAction * a = d->rootMenu->exec(map);

        if ( a == 0 ) return;

        ProjectItem* parent = 0;
        if (item->type() >= ProjectItem::minItemTypeInt())
        {
            parent = static_cast<ProjectItem*>(item);
        }
        else
        {
            parent = __currentProj->root();
        }

        if ( a->data().toString() == "package" ) 
        {
            __addPackage(parent);
        }
        else if ( a->data().toString() == "file" )
        {
            __addFile(parent);
        }
    }

    bool ProjectController::__existsAndWarn(QString name, ProjectItem* parent, ProjectItem::ItemType type)
    {
        QDir dir(__currentProj->dir());
        dir.cd(parent->path());
        if (dir.exists(name))
        {
            auto type_name = Project::itemType2String(type).toLower();
            auto title = tr("Name already used!");
            auto msg = tr("The %1 %2 is already used in package %3").arg(type_name, name, parent->text());
            QMessageBox::warning(__mainWidget, title, msg);
            return true;
        }
        return false;
    }

    QString ProjectController::__askForName(ProjectItem::ItemType type)
    {
        auto type_name = Project::itemType2String(type);
        auto title = tr("Choose a name for this new %1").arg(type_name.toLower());
        auto msg = tr("%1 name:").arg(type_name);
        auto default_value = tr("new_%1").arg(type_name);

        bool ok;
        auto name = QInputDialog::getText(__mainWidget,
                title, msg,
                QLineEdit::Normal, default_value,
                &ok);

        if (type == ProjectItem::ItemType::File and not name.endsWith(".o3prm"))
        {
            name = name + ".o3prm";
        }
        return name;
    }

    void ProjectController::__addPackage(ProjectItem* parent)
    {
        auto name = __askForName(ProjectItem::ItemType::Directory);
        if (not __existsAndWarn(name, parent, ProjectItem::ItemType::Directory))
        {
            QDir dir(__currentProj->dir());
            dir.cd(parent->path());
            dir.mkdir(name);
            auto package = new ProjectItem(ProjectItem::ItemType::Directory, name);
            parent->appendRow(package);

            __saveProject();
        }
    }

    void ProjectController::__addFile(ProjectItem* parent)
    {
        auto name = __askForName(ProjectItem::ItemType::File);
        if (not __existsAndWarn(name, parent, ProjectItem::ItemType::File))
        {
            auto file = new ProjectItem(ProjectItem::ItemType::File, name);
            parent->appendRow(file);
            QDir dir(__currentProj->dir());
            dir.cd(parent->path());
            auto file_path = dir.absoluteFilePath(file->text());
            QFile data(file_path);
            if (data.open(QFile::WriteOnly | QFile::Truncate)) 
            {
                QTextStream out(&data);
                auto ns = file->path().replace('/', '.').trimmed();
                while (ns.size() > 0 and ns.endsWith('.'))
                {
                    ns.truncate(ns.size() - 1);
                }
                out << "namespace " << ns << ';' << '\n';
            }
            __saveProject();
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
