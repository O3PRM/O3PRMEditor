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
            QMenu * packageMenu;
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
        d->rootMenu->addAction( tr( "Add a package" ) )->setData( "package" );
        d->rootMenu->addAction( tr( "Add a file" ) )->setData( "file" );

        d->packageMenu = new QMenu(__mainWidget);
        d->packageMenu->addAction( tr( "Add a package" ) )->setData( "package" );
        d->packageMenu->addAction( tr( "Add a file" ) )->setData( "file" );
        d->packageMenu->addAction( tr( "Rename" ) )->setData( "rename" );
        d->packageMenu->addAction( tr( "Delete" ) )->setData( "delete" );

        d->fileMenu = new QMenu(__mainWidget);
        d->fileMenu->addAction( tr( "Rename" ) )->setData( "rename" );
        d->fileMenu->addAction( tr( "Delete" ) )->setData( "delete" );
    }

    void ProjectController::setupConnections()
    {
        connect( __mainWidget->ui->projectExplorator, SIGNAL( clicked( QModelIndex ) ),
                this, SLOT( on_projectExplorator_clicked( QModelIndex ) ) );
        connect( __mainWidget->ui->projectExplorator, SIGNAL( doubleClicked( QModelIndex ) ),
                this, SLOT( on_projectExplorator_doubleClicked( QModelIndex ) ) );
        connect( __mainWidget->ui->projectExplorator, SIGNAL( customContextMenuRequested( QPoint ) ),
                this, SLOT( onCustomContextMenuRequested( QPoint ) ) );
        connect( __mainWidget->ui->projectExplorator->itemDelegate(), SIGNAL( closeEditor( QWidget* ) ),
                this, SLOT( onItemRenameFinished() ) );

        connect(__mainWidget->ui->actionNewFileProject, SIGNAL( triggered() ),
                this, SLOT( newProject() ) );
        connect( __mainWidget->ui->actionNewProject, SIGNAL( triggered() ),
                this, SLOT( newProject() ) );

        connect( __mainWidget->ui->actionOpenProject, SIGNAL( triggered() ),
                this, SLOT( openProject() ) );
        connect( __mainWidget->ui->actionNewClass, SIGNAL( triggered() ),
                this, SLOT( createNewClassFile() ) );
        connect( __mainWidget->ui->actionNewModel, SIGNAL( triggered() ),
                this, SLOT( createNewSystemFile() ) );
        connect( __mainWidget->ui->actionNewRequestFile, SIGNAL( triggered() ),
                this, SLOT( createNewRequestFile() ) );
        connect( __mainWidget->ui->actionCloseProject, SIGNAL( triggered() ),
                this, SLOT( closeProject() ) );

        QTimer::singleShot( 200, this, SLOT( triggerInit() ) );
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
        if (isOpenProject())
        {
            __saveProject();
            __closeProject();
        }

        NewProjectDialog dial( __mainWidget );

        // If user cancel, do nothing
        if ( dial.exec() )
        {
            __currentProj = new Project( dial.projectDir(), dial.projectName(), this );
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
            emit projectSaved(__currentProj);
            return true;
        }
        return false;
    }

    void ProjectController::__closeProject()
    {
        if ( __currentProj )
        {
            __currentProj->deleteLater();
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
        if (isOpenProject())
        {
            __saveProject();
            __closeProject();
        }

        if ( path.isEmpty() )
        {
            path = QFileDialog::getOpenFileName ( __mainWidget,
                    tr( "Open project file" ),
                    QDir::home().absolutePath(),
                    tr("O3PRM project Files (*.o3prmproject)")
                    );
        }

        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        { 
            QDomDocument doc("o3prmproject");
            if( doc.setContent( &file ) )
            {
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
            file.close();
        }
    }

    void ProjectController::closeProject() 
    {
        if (isOpenProject())
        {
            addToRecentsProjects( __currentProj->dir().absolutePath() );
            __saveProject();
            __closeProject();
            QDir::setCurrent( QDir::homePath() );
        }

    }

    void ProjectController::refactor( const QString & fromFilePath, const QString & toFilePath ) 
    {
        // qWarning() << "Project::refactor() was called but is not yet implemented.\n"
        //     "For moment, this method do nothing.";
        // return;
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
        if (isOpenProject())
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
        if (isOpenProject())
        {
            auto view = __mainWidget->mainwindow()->projectExplorator;
            auto index = view->indexAt(pos);
            if (index.isValid())
            {
                auto parent = static_cast<QStandardItem*>(index.internalPointer());
                // Only the invisible root is of type QStandardItem
                auto item = static_cast<ProjectItem*>(parent->child(index.row()));

                switch (item->type())
                {
                    case (int)ProjectItem::ItemType::Project:
                        {
                            __projectCustomContextMenu(pos, item);
                            break;
                        }
                    case (int)ProjectItem::ItemType::Directory:
                        {
                            __packageCustomContextMenu(pos, item);
                            break;
                        }
                    case (int)ProjectItem::ItemType::File:
                        {
                            __fileCustomContextMenu(pos, item);
                            break;
                        }

                    default:
                        {
                            // Don't know what to do
                            return;
                        }
                }

            }
        }
    }

    void ProjectController::__projectCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainwindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = d->rootMenu->exec(map);

        if (action and action->data().toString() == "package")
        {
            __addPackage(item);
        }
        else if (action and action->data().toString() == "file")
        {
            __addFile(item);
        }
    }

    void ProjectController::__packageCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainwindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = d->packageMenu->exec(map);

        if (action and action->data().toString() == "package")
        {
            __addPackage(item);
        }
        else if (action and action->data().toString() == "file")
        {
            __addFile(item);
        }
        else if (action and action->data().toString() == "rename")
        {
            __rename(item);
        }
        else if (action and action->data().toString() == "delete")
        {
        }

    }

    void ProjectController::__fileCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainwindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = d->fileMenu->exec(map);

        if (action and action->data().toString() == "rename")
        {
            __rename(item);
        }
        else if (action and action->data().toString() == "delete")
        {
        }
    }

    void ProjectController::__rename(ProjectItem* item)
    {
        // TODO: need to refresh editor, fot the moment does not work as intended
        auto msg = tr("Warning: close file(s) before renaming !");
        QMessageBox::warning(__mainWidget, msg, msg);
        bool ok;
        auto new_name = __askForName((ProjectItem::ItemType) item->type(), ok);
        if (ok and __validNameAndWarn(new_name))
        {
            QDir dir(__currentProj->dir());
            dir.cd(item->path());
            if (dir.rename(item->text(), new_name))
            {
                item->setText(new_name);
                __saveProject();
            }
            else
            {
                auto title = tr("Name already used!");
                auto type_name = Project::itemType2String(item->type());
                auto msg = tr("The %1 %2 is already used.").arg(type_name, new_name);
                QMessageBox::warning(__mainWidget, title, msg);
            }
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

    bool ProjectController::__validNameAndWarn(QString name)
    {
        if (name.isEmpty() or name == ".o3prm") //not boost::filesystem::portable_name(name.toStdString()))
        {
            if (name.isEmpty())
            {
                name = "Empty name";
            }
            QMessageBox::warning ( __mainWidget,
                    tr("Invalid name"),
                    tr("%1 is not a valid choice for this action, please chose another name.").arg(name));
            return false;
        }
        return true;
    }

    QString ProjectController::__askForName(ProjectItem::ItemType type, bool& ok)
    {
        ok = false;
        auto type_name = Project::itemType2String(type);
        auto title = tr("Choose a name for this new %1").arg(type_name.toLower());
        auto msg = tr("%1 name:").arg(type_name);
        auto default_value = tr("new_%1").arg(type_name);

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
        bool ok;
        auto name = __askForName(ProjectItem::ItemType::Directory, ok);
        if (ok
            and __validNameAndWarn(name)
            and not __existsAndWarn(name, parent, ProjectItem::ItemType::Directory))
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
        bool ok;
        auto name = __askForName(ProjectItem::ItemType::File, ok);

        if (ok
            and __validNameAndWarn(name)
            and not __existsAndWarn(name, parent, ProjectItem::ItemType::File))
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

    void ProjectController::onItemRenameFinished() 
    {
        //currentProj->setEditable( false );
    }

} // o3prm
