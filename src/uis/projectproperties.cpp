#include "projectproperties.h"

#include <QDebug>
#include <QFileDialog>

namespace o3prm 
{
    ProjectProperties::ProjectProperties( Project * p, QWidget *parent ) :
        QDialog( parent ),
        ui( new Ui::ProjectProperties ),
        p( p ),
        parent( parent )
    {
        ui->setupUi( this );

        if ( p )
        {
            ui->listWidget->addItems( p->paths() );
            ui->listWidget->item( 0 )->setFlags( Qt::NoItemFlags );
        }

        connect( ui->addButton, SIGNAL( clicked() ),
                this, SLOT( onAddButtonClicked() ) );
        connect( ui->delButton, SIGNAL( clicked() ),
                this, SLOT( onDelButtonClicked() ) );
    }

    ProjectProperties::~ProjectProperties() 
    {
        delete ui;
    }

    void ProjectProperties::onAddButtonClicked() 
    {
        auto msg = tr( "Add a class directory" );
        auto dirPath = QFileDialog::getExistingDirectory( parent, msg, QDir::homePath() );
        ui->listWidget->addItem( dirPath );
    }

    void ProjectProperties::onDelButtonClicked()
    {
        auto row = ui->listWidget->currentRow();
        delete ui->listWidget->takeItem( row );
    }

    void ProjectProperties::accept()
    {
        if ( p )
        {
            p->clearPaths();
            auto items = ui->listWidget->findItems( "", Qt::MatchContains );
            foreach( QListWidgetItem * item, items )
            {
                p->addPath( item->text() );
            }
        }
        QDialog::accept();
    }

    void ProjectProperties::reject() 
    {
        ui->listWidget->clear();

        if ( p )
        {
            ui->listWidget->addItems( p->paths() );
            ui->listWidget->item( 0 )->setFlags( Qt::NoItemFlags );
        }

        QDialog::reject();
    }
}
