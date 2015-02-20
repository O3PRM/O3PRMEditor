#include "projectcontroller.h"

#include <iostream>
#include <sstream>

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QStringBuilder>
#include <QTimer>

#include <QFileSystemModel>

#include "ui_mainwindow.h"

#include "controllers/settingscontroller.h"
#include "lexers/qscilexero3prml2.h"
#include "models/qsciscintillaextended.h"
#include "uis/mainwindow.h"
#include "uis/newprojectdialog.h"
#include "uis/projectproperties.h"

namespace o3prm
{
    ///////////////////////////////////////////////////////////////////////////
    //                          PUBLIC METHODS                               //
    ///////////////////////////////////////////////////////////////////////////

    ProjectController::ProjectController(MainWindow *parent ) :
        QObject( parent ),
        __currentProj( 0 ),
        __mainWidget(parent),
        __defaultDir(QDir::homePath())
    {
        // Setting up context menus
        __projectMenu = new QMenu(__mainWidget);
        __projectMenu->addAction( tr( "Add a package" ) )->setData( "package" );
        __projectMenu->addAction( tr( "Add a file" ) )->setData( "file" );
        __projectMenu->addAction( tr( "Add a request" ) )->setData( "request" );

        __packageMenu = new QMenu(__mainWidget);
        __packageMenu->addAction( tr( "Add a package" ) )->setData( "package" );
        __packageMenu->addAction( tr( "Add a file" ) )->setData( "file" );
        __packageMenu->addAction( tr( "Add a request" ) )->setData( "request" );
        __packageMenu->addAction( tr( "Rename" ) )->setData( "rename" );
        __packageMenu->addAction( tr( "Delete" ) )->setData( "delete" );

        __fileMenu = new QMenu(__mainWidget);
        __fileMenu->addAction( tr( "Rename" ) )->setData( "rename" );
        __fileMenu->addAction( tr( "Delete" ) )->setData( "delete" );

        __requestMenu = new QMenu(__mainWidget);
        __requestMenu->addAction( tr( "Execute" ) )->setData( "execute" );
        __requestMenu->addAction( tr( "Rename" ) )->setData( "rename" );
        __requestMenu->addAction( tr( "Delete" ) )->setData( "delete" );
    }

    ProjectController::~ProjectController()
    {
    }

    void ProjectController::setupConnections()
    {
        connect( __mainWidget->mainWindow()->projectExplorator, SIGNAL( clicked( QModelIndex ) ),
                this, SLOT( _onClick( QModelIndex ) ) );
        connect( __mainWidget->mainWindow()->projectExplorator, SIGNAL( doubleClicked( QModelIndex ) ),
                this, SLOT( _onDoubleClick( QModelIndex ) ) );
        connect( __mainWidget->mainWindow()->projectExplorator, SIGNAL( customContextMenuRequested( QPoint ) ),
                this, SLOT( _onCustomContextMenuRequested( QPoint ) ) );

        connect(__mainWidget->mainWindow()->actionNewProject, SIGNAL( triggered() ),
                this, SLOT( _newProject() ) );

        connect( __mainWidget->mainWindow()->actionOpenProject, SIGNAL( triggered() ),
                this, SLOT( _openProject() ) );
        connect( __mainWidget->mainWindow()->actionCloseProject, SIGNAL( triggered() ),
                this, SLOT( _closeProject() ) );

        connect(__mainWidget->settingsController(), SIGNAL( recentProjectClicked(QString) ),
                this, SLOT( _openProject(QString) ));
        connect(__mainWidget->settingsController(), SIGNAL( lastDirChanged(QString) ),
                this, SLOT( _lastDirChanged(QString) ));
    }

    Project* ProjectController::currentProject() const
    {
        return __currentProj;
    }

    bool ProjectController::hasProject() const
    {
        return __currentProj != 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    //                          PROTECTED SLOTS                              //
    ///////////////////////////////////////////////////////////////////////////

    void ProjectController::_newProject()
    {
        if (hasProject())
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

            QString path = __currentProj->dir().absoluteFilePath(__currentProj->name() + ".o3prmproject");
            emit projectLoaded(path);
        }
    }

    void ProjectController::_openProject( QString path )
    {
        if (hasProject())
        {
            __saveProject();
            __closeProject();
        }

        if ( path.isEmpty() )
        {
            path = QFileDialog::getOpenFileName ( __mainWidget,
                    tr( "Open project file" ),
                    __defaultDir,
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
                QString path = __currentProj->dir().absoluteFilePath(__currentProj->name() + ".o3prmproject");
                emit projectLoaded(path);
            }
            file.close();
        }
    }

    void ProjectController::_closeProject() 
    {
        if (hasProject())
        {
            __saveProject();
            __closeProject();
            QDir::setCurrent( QDir::homePath() );
        }

    }

    bool ProjectController::_onClick( QModelIndex index ) 
    {
        return false;
    }

    bool ProjectController::_onDoubleClick( QModelIndex index ) 
    {
        if (hasProject())
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
                case ProjectItem::ItemType::Request:
                    {
                        QDir dir(__currentProj->dir());
                        auto path = dir.absoluteFilePath(item->path());
                        emit fileOpened(path);
                        return true;
                    }
                default:
                    {
                        return false;
                    }
            }
        }
        return false;
    }

    void ProjectController::_onCustomContextMenuRequested( const QPoint & pos ) 
    {
        if (hasProject())
        {
            auto view = __mainWidget->mainWindow()->projectExplorator;
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
                    case (int)ProjectItem::ItemType::Request:
                        {   
                            __requestCustomContextMenu(pos, item);
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

    ///////////////////////////////////////////////////////////////////////////
    //                          PRIVATE METHODS                              //
    ///////////////////////////////////////////////////////////////////////////

    void ProjectController::__closeProject()
    {
        if ( __currentProj )
        {
            __currentProj->deleteLater();
            __currentProj = 0;
            emit projectClosed();
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

    void ProjectController::__projectCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainWindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = __projectMenu->exec(map);

        if (action and action->data().toString() == "package")
        {
            __addPackage(item);
        }
        else if (action and action->data().toString() == "file")
        {
            __addFile(item);
        }
        else if (action and action->data().toString() == "request")
        {
            __addRequest(item);
        }
    }

    void ProjectController::__packageCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainWindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = __packageMenu->exec(map);

        if (action and action->data().toString() == "package")
        {
            __addPackage(item);
        }
        else if (action and action->data().toString() == "file")
        {
            __addFile(item);
        }
        else if (action and action->data().toString() == "request")
        {
            __addRequest(item);
        }
        else if (action and action->data().toString() == "rename")
        {
            __rename(item);
        }
        else if (action and action->data().toString() == "delete")
        {
            __delete(item);
        }

    }

    void ProjectController::__fileCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainWindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = __fileMenu->exec(map);

        if (action and action->data().toString() == "rename")
        {
            __rename(item);
        }
        else if (action and action->data().toString() == "delete")
        {
            __delete(item);
        }
    }

    void ProjectController::__requestCustomContextMenu(const QPoint& pos, ProjectItem* item)
    {
        auto view = __mainWidget->mainWindow()->projectExplorator;
        auto map = view->mapToGlobal( pos ) ;
        QAction * action = __requestMenu->exec(map);

        if (action and action->data().toString() == "execute")
        {
            __execute(item);
        }
        else if (action and action->data().toString() == "rename")
        {
            __rename(item);
        }
        else if (action and action->data().toString() == "delete")
        {
            __delete(item);
        }
    }

    void ProjectController::__rename(ProjectItem* item)
    {
        bool ok;
        auto old_name = item->text();
        auto new_name = __askForName((ProjectItem::ItemType) item->type(), ok, item->text());

        // Moving to the item's location
        auto dir(__currentProj->dir());
        auto parent = static_cast<ProjectItem*>(item->parent());
        if (not parent->path().isEmpty())
        {
            dir.cd(parent->path());
        }

        if (ok and __validNameAndWarn(new_name))
        {
            if (dir.rename(item->text(), new_name))
            {
                item->setText(new_name);
                switch (item->type())
                {
                    case (int) ProjectItem::ItemType::File:
                    case (int) ProjectItem::ItemType::Request:
                        {
                            emit fileRenamed(dir.absoluteFilePath(old_name), dir.absoluteFilePath(item->text()));
                            break;
                        }
                    case (int) ProjectItem::ItemType::Directory:
                        {
                            emit packageRenamed(dir.absoluteFilePath(old_name), dir.absoluteFilePath(item->text()));
                            break;
                        }
                    default:
                        {
                            // do nothing
                            break;
                        }
                }
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
        //Should use BOOST !
        // if (not boost::filesystem::portable_name(name.toStdString())
        if (name.isEmpty() or name == ".o3prm") 
        {
            if (name.isEmpty())
            {
                name = "Empty name";
            }
            QMessageBox::warning ( __mainWidget,
                    tr("Invalid name"),
                    tr("%1 is not a valid choice, please chose another name.").arg(name));
            return false;
        }
        else if (name.contains(" "))
        {
            QMessageBox::warning ( __mainWidget,
                    tr("Do not use whitespace in name"),
                    tr("%1 is not a valid choice, please chose another name.").arg(name));
            return false;
        }
        return true;
    }

    QString ProjectController::__askForName(ProjectItem::ItemType type, bool& ok, QString defaultValue)
    {
        ok = false;
        auto type_name = Project::itemType2String(type);
        auto title = tr("Choose a name for this new %1").arg(type_name.toLower());
        auto msg = tr("%1 name:").arg(type_name);
        if (defaultValue.isEmpty())
        {
            defaultValue = tr("new_%1").arg(type_name);
        }

        auto name = QInputDialog::getText(__mainWidget,
                title, msg,
                QLineEdit::Normal, defaultValue,
                &ok);

        if (type == ProjectItem::ItemType::File and not name.endsWith(".o3prm"))
        {
            name = name + ".o3prm";
        }
        else if (type == ProjectItem::ItemType::Request and not name.endsWith(".o3prmr"))
        {
            name = name + ".o3prmr";
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
                out << __defaultPackage(file) << '\n';
            }

            __saveProject();
        }
    }

    void ProjectController::__addRequest(ProjectItem* parent)
    {
        bool ok;
        auto name = __askForName(ProjectItem::ItemType::Request, ok);

        if (ok
                and __validNameAndWarn(name)
                and not __existsAndWarn(name, parent, ProjectItem::ItemType::Request))
        {
            auto file = new ProjectItem(ProjectItem::ItemType::Request, name);
            parent->appendRow(file);

            QDir dir(__currentProj->dir());
            dir.cd(parent->path());

            auto file_path = dir.absoluteFilePath(file->text());
            QFile data(file_path);

            if (data.open(QFile::WriteOnly | QFile::Truncate)) 
            {
                QTextStream out(&data);
                out << __defaultPackage(file) << '\n';
            }

            __saveProject();
        }
    }

    QString ProjectController::__defaultPackage(ProjectItem* item)
    {
        if (item->isInPackage())
        {
            QString s = "package %1;";
            return s.arg(item->package());
        }
        return QString();
    }

    void ProjectController::__execute(ProjectItem* item)
    {
        emit beforeInference();
        __build = new BuildModel(__currentProj, this);
        auto interpreter = __build->build(item);
        std::stringstream strBuff;
        auto errors = interpreter->errorsContainer();
        if (errors.count() > 0)
        {
            strBuff << "Errors and warnings: ";
            for (int i = 0; i < errors.count(); ++i)
            {
                auto parse_error = errors.error(i);
                strBuff << std::endl << parse_error.msg;
            }
            strBuff << std::endl << std::endl;
        }

        if (interpreter->results().size())
        {
            strBuff << "Results: ";
            for (size_t i = 0; i < interpreter->results().size(); ++i)
            {
                auto result = interpreter->results()[i];
                strBuff << std::endl << "    " << result.command << ": ";
                for (size_t j = 0; j < result.values.size(); ++j)
                {
                    auto value = result.values[j];
                    strBuff << "\n" << "        " << value.label << ": " << value.p;
                }
            }
        }

        QMessageBox msgBox;
        msgBox.setText(QString::fromStdString(strBuff.str()));
        msgBox.exec();
    }

    void ProjectController::__delete(ProjectItem* item)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Delete %1?").arg(item->text()));
        auto item_type = Project::itemType2String(item->type());
        msgBox.setInformativeText(tr("Do you want to delete %1 %2?").arg(item_type, item->text()));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Ok)
        {
            auto dir = QDir(__currentProj->dir());
            switch (item->type())
            {
                case (int)ProjectItem::ItemType::File:
                case (int)ProjectItem::ItemType::Request:
                    {
                        auto path = dir.absoluteFilePath(item->path());
                        emit fileRemoved(path);
                        dir.remove(path);
                        break;
                    }
                case (int)ProjectItem::ItemType::Directory:
                    {
                        if (dir.exists(item->path()))
                        {
                            dir.cd(item->path());
                            __removeDir(dir.absolutePath());
                            emit packageRemoved(dir.absolutePath());
                        }
                        break;
                    }
                default:
                    {
                        // Do nothing
                        break;
                    }
            }
            item->parent()->removeRow(item->row());
        }
        __saveProject();
    }

    bool ProjectController::__removeDir(const QString & dirName)
    {
        bool result = true;
        QDir dir(dirName);

        if (dir.exists(dirName))
        {
            auto flags = QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files;
            Q_FOREACH(QFileInfo info, dir.entryInfoList(flags, QDir::DirsFirst))
            {
                if (info.isDir())
                {
                    result = __removeDir(info.absoluteFilePath());
                }
                else
                {
                    result = QFile::remove(info.absoluteFilePath());
                }

                if (!result)
                {
                    return result;
                }
            }
            result = dir.rmdir(dirName);
        }
        return result;
    }

    void ProjectController::_lastDirChanged(QString path)
    {
        __defaultDir = path;
    }

} // o3prm
