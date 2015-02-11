#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

NewProjectDialog::NewProjectDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::NewProjectDialog )
{
    ui->setupUi( this );
    ui->projectDirectory->setText( QDir::homePath() );
}

NewProjectDialog::~NewProjectDialog() 
{
    delete ui;
}

void NewProjectDialog::on_selectDirectoryButton_clicked() 
{
    QString dir = QFileDialog::getExistingDirectory( this,
            tr( "Select the project new directory" ),
            QDir::homePath() );

    if ( dir.isEmpty() )
    {
        return;
    }

    ui->projectDirectory->setText( dir );
}

void NewProjectDialog::on_buttonBox_accepted() 
{
    QString name = QDir::cleanPath( ui->projectDirectory->text() + QDir::separator() + ui->projectName->text() + QDir::separator() );

    if ( name.isEmpty() )
    {
        reject();
    }

    QDir dir( name );

    if ( dir.exists() ) 
    {
        auto msg = tr( "Directory '%1' already exists.\nIf you continue, the existing directory will be saved under '%1.bak'?").arg(name);
        int button = QMessageBox::warning( this,
                tr( "New Project" ),
                msg,
                QMessageBox::Ok,
                QMessageBox::Cancel );

        if ( button == QMessageBox::Ok )
        {
            QDir( QDir::root() ).rename( name,name+".old" );
        }
        else
        {
            return;
        }
    }

    // If can't create the directory, warn the user.
    if ( ! QDir( QDir::root() ).mkpath( name ) )
    {
        QMessageBox::warning( this,
                tr( "Creating the project directory" ),
                tr( "Warning: directory cannot be created." ));
        return;
    }

    accept();
}

QString NewProjectDialog::projectName()
{
    if ( result() )
    {
        return ui->projectName->text();
    }
    else
    {
        return QString();
    }
}

QString NewProjectDialog::projectDir() 
{
    if ( result() )
    {
        return QDir::cleanPath( ui->projectDirectory->text() +
                QDir::separator() +
                ui->projectName->text() +
                QDir::separator() );
    }
    else
    {
        return QString();
    }
}

