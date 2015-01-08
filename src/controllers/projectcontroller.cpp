#include "projectcontroller.h"

#include "buildcontroller.h"
#include "editcontroller.h"
#include "filecontroller.h"
#include "lexers/qscilexero3prml2.h"
#include "models/treemodel.h"
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
    struct ProjectController::PrivateData
    {
        int numberMaxOfRecentsProjects;
        QMenu * recentsProjects;
        QList<QString> recentsProjectsList;
        QSignalMapper * recentsProjectsMapper;
        ProjectProperties * projectProperties;
        QMenu * fileMenu;
        QMenu * dirMenu;
        QMenu * rootMenu;
    };

    ProjectController::ProjectController( MainWindow * mw, QObject *parent ) :
        QObject( parent ), mw( mw ), currentProj( 0 ), d( new PrivateData )
    {
        mw->ui->projectExplorator->setVisible( false );
        mw->ui->projectExplorator->setDragDropMode( QAbstractItemView::InternalMove );

        d->projectProperties = 0;
        mw->ui->actionProjectProperties->setEnabled( false );

        d->recentsProjects = new QMenu( mw );
        d->recentsProjectsMapper = new QSignalMapper( mw );

        // Construct "Recent project" menu
        mw->ui->actionRecentProject->setMenu( d->recentsProjects );
        connect( d->recentsProjectsMapper, SIGNAL( mapped( QString ) ),
                this, SLOT( openProject( QString ) ) );

        d->fileMenu = new QMenu( mw->ui->projectExplorator );
        d->fileMenu->addAction( tr( "Re&nommer" ) )->setData( "rename" );
        d->fileMenu->addAction( tr( "&Supprimer" ) )->setData( "remove" );
        d->fileMenu->addAction( tr( "&Éxécuter" ) )->setData( "execute" );
        // QMenu * m = d->fileMenu->addMenu(tr("&Refactoring"));
        // m->addAction(tr("Re&nommer"))->setData("refact-rename");
        // m->addAction(tr("&Déplacer"))->setData("refact-move");
        // m->addAction(tr("&Supprimer"))->setData("refact-remove");

        d->dirMenu = new QMenu( mw->ui->projectExplorator );
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

        d->rootMenu = new QMenu( mw->ui->projectExplorator );
        d->rootMenu->addAction( tr( "Ajouter un &package" ) )->setData( "package" );
        d->rootMenu->addAction( tr( "Éxecuter" ) )->setData( "execute" );

        connect( mw->ui->projectExplorator, SIGNAL( clicked( QModelIndex ) ),
                this, SLOT( on_projectExplorator_clicked( QModelIndex ) ) );
        connect( mw->ui->projectExplorator, SIGNAL( doubleClicked( QModelIndex ) ),
                this, SLOT( on_projectExplorator_doubleClicked( QModelIndex ) ) );
        connect( mw->ui->projectExplorator, SIGNAL( customContextMenuRequested( QPoint ) ),
                this, SLOT( onCustomContextMenuRequested( QPoint ) ) );
        connect( mw->ui->projectExplorator->itemDelegate(), SIGNAL( closeEditor( QWidget* ) ),
                this, SLOT( onItemRenameFinished() ) );

        connect( mw->ui->actionNewProject, SIGNAL( triggered() ),
                this, SLOT( newProject() ) );
        connect( mw->ui->actionOpenProject, SIGNAL( triggered() ),
                this, SLOT( openProject() ) );
        connect( mw->ui->actionNewClass, SIGNAL( triggered() ),
                this, SLOT( createNewClassFile() ) );
        connect( mw->ui->actionNewModel, SIGNAL( triggered() ),
                this, SLOT( createNewSystemFile() ) );
        connect( mw->ui->actionNewRequestFile, SIGNAL( triggered() ),
                this, SLOT( createNewRequestFile() ) );
        connect( mw->ui->actionCloseProject, SIGNAL( triggered() ),
                this, SLOT( closeProject() ) );

        QTimer::singleShot( 200, this, SLOT( triggerInit() ) );
    }

    ProjectController::~ProjectController()
    {
        delete d;
    }

    void ProjectController::triggerInit()
    {
        QSettings settings;
        settings.beginGroup( "project" );

        d->numberMaxOfRecentsProjects = settings.value( "numberMaxOfRecentsProjects",5 ).toInt();

        // Read the last closed projects in settings
        int size = settings.beginReadArray( "recentsProjects" );

        for ( int i = 0; i < size ; i++ )
        {
            settings.setArrayIndex( i );
            addToRecentsProjects( settings.value( "project" ).toString() );
        }

        settings.endArray();

        // Reopen project don't close last time.
        QString lastProject = settings.value( "last project" ).toString();

        if ( ! lastProject.isEmpty() )
        {
            openProject( lastProject );
        }
        else
        {
            currentProj = 0;
            mw->ui->projectExplorator->setModel( currentProj );

            // Change the current directory
            QDir::setCurrent( QDir::homePath() );
        }
    }

    Project * ProjectController::currentProject() const
    {
        return currentProj;
    }

    bool ProjectController::isOpenProject() const
    {
        return currentProj != 0;
    }

    void ProjectController::newProject()
    {
        NewProjectDialog dial( mw );

        // If user cancel, do nothing
        if ( ! dial.exec() )
        {
            return;
        }

        if ( currentProj )
        {
            currentProj->close();
            currentProj->deleteLater();
            currentProj = 0;
        }

        currentProj = new Project( dial.projectDir(),this );

        connect( currentProj, SIGNAL( fileRenamed( QString,QString,QString ) ),
                this, SLOT( onProjectFileRenamed( QString,QString,QString ) ) );
        connect( currentProj, SIGNAL( fileMoved( QString,QString ) ),
                this, SLOT( onProjectFileMoved( QString,QString ) ) );

        d->projectProperties = new ProjectProperties( currentProj, mw );
        mw->ui->actionProjectProperties->setEnabled( true );
        connect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
                d->projectProperties, SLOT( exec() ) );

        // We change current directory to the project directory
        QDir::setCurrent( dial.projectDir() );

        // Create empty files
        //createNewClassFile();

        // Show project arborescence
        mw->ui->actionProjectExploratorVisibility->setEnabled( true );
        mw->ui->projectExplorator->setModel( currentProj );
        mw->ui->projectExplorator->setRootIndex( currentProj->root() );
        mw->vc->setProjectExploratorVisibility( true );
        mw->ui->projectExplorator->expandAll();

        // Enable new specific file creation
        mw->ui->actionNewClass->setEnabled( true );
        mw->ui->actionNewModel->setEnabled( true );
        mw->ui->actionNewRequestFile->setEnabled( true );

        // Enable auto syntax check
        mw->bc->setAutoSyntaxCheck( true );

        saveProjectsState();
    }

    void ProjectController::createNewClassFile()
    {
        // We add a new o3prml empty file,
        auto sci = mw->fc->newDocument( tr( "Nouveau o3prml" ), QsciScintillaExtended::O3prml );
        sci->setLexer( QsciScintillaExtended::O3prml );

        // give it the focus,
        mw->ui->tabWidget->setCurrentWidget( sci );
        sci->setFocus();

        auto path = currentProj->dir() + tr( "/classes/empty_file.o3prml" );
        if ( mw->fc->saveAsFile( sci, path ) )
        {
            // and prefill it.
            QFileInfo info( sci->filename() );
            QString className = info.baseName();
            auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
            QString packageName = packageRelativePath.replace( "/","." );
            QString text;

            if ( ! packageName.isEmpty() )
            {
                text += "\npackage "+ packageName +";\n";
            }
            text += "\n//\nclass "+ className + " {\n}\n";

            sci->setText( text );
            sci->setCursorPosition( 2,6 );

            mw->fc->saveFile( sci );
        }
        else
        {
            mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        }
    }

    void ProjectController::createNewRequestFile()
    {
        // We add a new o3prmr empty file,
        auto sci = mw->fc->newDocument( tr( "Nouveau o3prmr" ), QsciScintillaExtended::O3prmr );
        sci->setLexer( QsciScintillaExtended::O3prmr );

        // give it the focus,
        mw->ui->tabWidget->setCurrentWidget( sci );
        sci->setFocus();

        auto path = currentProj->dir() + tr( "/requests/empty_file.o3prmr" );
        if ( mw->fc->saveAsFile( sci, path ) ) {
            // and prefill it.
            QFileInfo info( sci->filename() );
            QString blockName = info.baseName();
            auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
            QString packageName = packageRelativePath.replace( "/","." );
            QString text;

            if ( ! packageName.isEmpty() )
            {
                text += "\npackage "+ packageName +";\n";
            }
            text += "\n//\nrequest "+ blockName + " {\n}\n";

            sci->setText( text );
            sci->setCursorPosition( 2,8 );

            mw->fc->saveFile( sci );
        }
        else
        {
            mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        }
    }

    void ProjectController::createNewSystemFile()
    {
        auto sci = mw->fc->newDocument( tr( "Nouveau o3prml" ), QsciScintillaExtended::O3prml );
        sci->setLexer( QsciScintillaExtended::O3prml );

        // give it the focus,
        mw->ui->tabWidget->setCurrentWidget( sci );
        sci->setFocus();

        if ( mw->fc->saveAsFile( sci, currentProj->dir() + tr( "/systems/empty_file.o3prml" ) ) ) {
            // and prefill it.
            QFileInfo info( sci->filename() );
            QString systemName = info.baseName();
            auto packageRelativePath = QDir( currentProj->dir() ).relativeFilePath( info.path() );
            QString packageName = packageRelativePath.replace( "/","." );
            QString text;

            if ( ! packageName.isEmpty() )
            {
                text += "\npackage "+ packageName +";\n";
            }
            text += "\n//\nsystem "+ systemName + " {\n}\n";

            sci->setText( text );
            sci->setCursorPosition( 2,7 );

            mw->fc->saveFile( sci );
        }
        else
        {
            mw->fc->closeFile( mw->ui->tabWidget->indexOf( sci ) );
        }
    }

    void ProjectController::openProject( QString projectpath )
    {
        if ( projectpath.isEmpty() )
        {
            // Retrieve the project dir
            auto msg = tr( "Sélectionnez le répertoire du projet" );
            projectpath = QFileDialog::getExistingDirectory( mw, msg, QDir::homePath() );
        }

        QDir qDir( projectpath );

        // TODO : Warning are not ?
        if ( projectpath.isEmpty() || ! qDir.exists() )
        {
            return;
        }

        if ( ! qDir.exists( qDir.dirName()+".o3prmp" ) ) 
        {
            auto msg = tr( "Ce répertoire ne contient pas de fichier projet.\nLe créer ?" );
            int rep = QMessageBox::warning( mw,
                    tr( "Attention" ),
                    msg,
                    QMessageBox::Cancel,
                    QMessageBox::Ok );

            if ( rep == QMessageBox::Cancel )
            {
                return;
            }
        }

        if ( currentProj )
        {
            closeProject();
        }

        currentProj = new Project( qDir.absolutePath(),this );

        connect( currentProj, SIGNAL( fileRenamed( QString,QString,QString ) ),
                this, SLOT( onProjectFileRenamed( QString,QString,QString ) ) );
        connect( currentProj, SIGNAL( fileMoved( QString,QString ) ),
                this, SLOT( onProjectFileMoved( QString,QString ) ) );

        d->projectProperties = new ProjectProperties( currentProj, mw );

        mw->ui->actionProjectProperties->setEnabled( true );

        connect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
                d->projectProperties, SLOT( exec() ) );

        // We change current directory to the project directory
        QDir::setCurrent( currentProj->dir() );

        removeOfRecentsProjects( qDir.absolutePath() );

        // Show project arborescence
        mw->ui->actionProjectExploratorVisibility->setEnabled( true );

        mw->ui->projectExplorator->setModel( currentProj );

        mw->ui->projectExplorator->setRootIndex( currentProj->root() );

        mw->vc->setProjectExploratorVisibility( true );

        // Enable new specific file creation
        mw->ui->actionNewClass->setEnabled( true );

        mw->ui->actionNewModel->setEnabled( true );

        mw->ui->actionNewRequestFile->setEnabled( true );

        saveProjectsState();

        // Enable auto syntax check
        mw->bc->setAutoSyntaxCheck( true );

        QTimer::singleShot( 200, mw->ui->projectExplorator, SLOT( expandAll() ) );
    }

    void ProjectController::closeProject() 
    {
        if ( currentProj ) {
            addToRecentsProjects( currentProj->dir() );

            // Tests because can be delete by MainWindow
            disconnect( mw->ui->actionProjectProperties, SIGNAL( triggered() ),
                    d->projectProperties, SLOT( exec() ) );
            d->projectProperties->close();
            d->projectProperties->deleteLater();
            d->projectProperties = 0;

            currentProj->deleteLater();
            currentProj = 0;

            mw->ui->actionProjectProperties->setEnabled( false );
            QDir::setCurrent( QDir::homePath() );
        }

        mw->ui->projectExplorator->setModel( currentProj );
        mw->vc->setProjectExploratorVisibility( false );

        // Disable new specific file creation
        mw->ui->actionNewClass->setEnabled( false );
        mw->ui->actionNewModel->setEnabled( false );
        mw->ui->actionNewRequestFile->setEnabled( false );
        mw->ui->actionProjectExploratorVisibility->setEnabled( false );

        // Disable auto syntax check
        mw->bc->setAutoSyntaxCheck( false );

        saveProjectsState();
    }

    void ProjectController::refactor( const QString & fromFilePath, const QString & toFilePath ) 
    {
        qWarning() << "Project::refactor() was called but is not yet implemented.\n"
            "For moment, this method do nothing.";
        return;
    } 

    void ProjectController::addToRecentsProjects( const QString & projetPath )
    {
        if ( projetPath.isEmpty() || d->recentsProjectsList.contains( projetPath ) )
        {
            return;
        }

        d->recentsProjectsList.append( projetPath );

        QAction * action = new QAction( QDir( projetPath ).dirName(), mw );

        action->setData( projetPath );

        connect( action, SIGNAL( triggered() ), d->recentsProjectsMapper, SLOT( map() ) );

        d->recentsProjectsMapper->setMapping( action, projetPath );

        if ( d->recentsProjects->actions().isEmpty() )
        {
            d->recentsProjects->addAction( action );
        }
        else
        {
            d->recentsProjects->insertAction( d->recentsProjects->actions().first(), action );
        }

        //
        if ( d->recentsProjects->actions().size() > d->numberMaxOfRecentsProjects )
        {
            d->recentsProjects->removeAction( d->recentsProjects->actions().last() );
        }
    }

    void ProjectController::removeOfRecentsProjects( const QString & projetPath ) 
    {
        if ( projetPath.isEmpty() || ! d->recentsProjectsList.contains( projetPath ) )
        {
            return;
        }

        d->recentsProjectsList.removeOne( projetPath );

        QAction * a = qobject_cast<QAction *>( d->recentsProjectsMapper->mapping( projetPath ) );

        if ( a != 0 ) 
        {
            d->recentsProjectsMapper->removeMappings( a );
            d->recentsProjects->removeAction( a );
        }

    }

    void ProjectController::saveProjectsState() 
    {
        QSettings settings;
        settings.beginGroup( "project" );

        if ( currentProj ) 
        {
            settings.setValue( "last project",currentProj->dir() );
            currentProj->close();
        }
        else
        {
            settings.setValue( "last project","" );
        }

        // Save the last closed projects in settings
        int size = d->recentsProjects->actions().size();

        settings.beginWriteArray( "recentsProjects",size );

        for ( int i = 0 ; i < size ; i++ ) 
        {
            settings.setArrayIndex( i );
            settings.setValue( "project",d->recentsProjects->actions().at( i )->data() );
        }

        settings.endArray();
    }

    bool ProjectController::on_projectExplorator_clicked( QModelIndex index ) 
    {
        QString filename = index.data( QFileSystemModel::FilePathRole ).toString();

        if ( mw->fc->isOpenFile( filename ) )
        {
            mw->ui->tabWidget->setCurrentWidget( mw->fc->fileToDocument( filename ) );
        }
        else
        {
            return false;
        }

        return true;
    }

    bool ProjectController::on_projectExplorator_doubleClicked( QModelIndex index ) 
    {
        // Return false if index is a dir
        if ( currentProj->isDir( index ) )
        {
            return false;
        }
        else // Else open the file
        {
            return mw->fc->openFile( index.data( QFileSystemModel::FilePathRole ).toString() );
        }
    }

    void ProjectController::onProjectFileRenamed( const QString & path,
            const QString & oldName,
            const QString & newName ) 
    {
        QsciScintillaExtended * sci = mw->fc->fileToDocument( path + "/" + oldName );

        if ( sci != 0 )
        {
            sci->setFilename( path + "/" + newName );
        }
    }

    void ProjectController::onProjectFileMoved( const QString & oldFilePath, const QString & newFilePath ) 
    {
        QsciScintillaExtended * sci = mw->fc->fileToDocument( oldFilePath );

        if ( sci != 0 ) 
        {
            sci->setFilename( newFilePath );
        }
    }

    void ProjectController::onCustomContextMenuRequested( const QPoint & pos ) 
    {
        QModelIndex index = mw->ui->projectExplorator->indexAt( pos );

        // If it's a dir
        if ( ! index.isValid() ) 
        {
            QAction * a = d->rootMenu->exec( mw->ui->projectExplorator->viewport()->mapToGlobal( pos ) );

            if ( a == 0 )
            {
                return;
            }

            if ( a->data().toString() == "package" ) 
            {
                QModelIndex newPackage = currentProj->mkdir( currentProj->root(),"new_package" );
                // setEditable is set to false when editing is finished. See onItemRenameFinished()
                currentProj->setEditable( true ); 
                mw->ui->projectExplorator->edit( newPackage );
            }
            else if ( a->data().toString() == "execute" ) 
            {
                qDebug() << "execute project !";
            }
        }
        else if ( currentProj->isDir( index ) ) 
        {
            QAction * a = d->dirMenu->exec( mw->ui->projectExplorator->viewport()->mapToGlobal( pos ) );

            if ( a == 0 )
            {
                return;
            }

            if ( a->data().toString() == "rename" ) 
            {
                // setEditable is set to false when editing is finished. See onItemRenameFinished()
                currentProj->setEditable( true );
                mw->ui->projectExplorator->edit( index );
            }
            else if ( a->data().toString() == "remove" ) 
            {
                auto msg = tr( "Voulez-vous vraiment supprimer définitivement le package"
                        "%1 ainsi que tout son contenu ?" );
                msg = msg.arg( currentProj->fileName( index ) );
                int ret = QMessageBox::question( mw,
                        tr( "Supprimer le package" ),
                        msg,
                        QMessageBox::Ok,
                        QMessageBox::Cancel );

                if ( ret != QMessageBox::Ok )
                {
                    return;
                }

                currentProj->rmdirRec( index );
            }
            else if ( a->data().toString() == "addPackage" ) 
            {
                QModelIndex newPackage = currentProj->mkdir( index,"new_package" );
                // setEditable is set to false when editing is finished. See onItemRenameFinished()
                currentProj->setEditable( true ); 
                mw->ui->projectExplorator->edit( newPackage );
            }
            else if ( a->data().toString() == "addClass" ) 
            {
                //createNewClassFile();
            }
            else if ( a->data().toString() == "addSystem" ) 
            {
                //createNewSystemFile();
            }
            else if ( a->data().toString() == "addRequest" ) 
            {
                //createNewRequestFile();
            }
        }
        else
        {
            // Hide or show execute action (show only with o3prmr files).
            auto filter = index.data().toString().endsWith( ".o3prmr",Qt::CaseInsensitive );
            d->fileMenu->actions().last()->setVisible( filter );

            QAction * a = d->fileMenu->exec( mw->ui->projectExplorator->viewport()->mapToGlobal( pos ) );

            if ( a == 0 )
            {
                return;
            }

            if ( a->data().toString() == "rename" ) 
            {
                // setEditable is set to false when editing is finished. See onItemRenameFinished()
                currentProj->setEditable( true ); 
                mw->ui->projectExplorator->edit( index );
            }
            else if ( a->data().toString() == "remove" ) 
            {
                auto msg = tr( "Voulez-vous vraiment supprimer définitivement le fichier\n%1 ?" );
                msg = msg.arg( currentProj->fileName( index ) );
                int ret = QMessageBox::question( mw,
                        tr( "Supprimer le fichier" ),
                        msg,
                        QMessageBox::Ok,
                        QMessageBox::Cancel );

                if ( ret != QMessageBox::Ok )
                {
                    return;
                }

                currentProj->remove( index );
            }
            else if ( a->data().toString() == "execute" and
                    currentProj->fileInfo( index ).suffix() == "o3prmr" ) 
            {
                mw->bc->execute( index.data( QFileSystemModel::FilePathRole ).toString() );
            }
        }
    }

    void ProjectController::onItemRenameFinished() 
    {
        currentProj->setEditable( false );
    }

} // o3prm
