#include "properties.h"

#include "mainwindow.h"
#include "ui_properties.h"
#include "ui_mainwindow.h"

#include "qsciscintillaextended.h"
#include "controllers/filecontroller.h"
#include "controllers/editorcontroller.h"

#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

struct Properties::PrivateData 
{
    QHash<QTreeWidgetItem*, QAction*> hash;
    QHash<QTreeWidgetItem*, QList<QTreeWidgetItem*> > conflicts;
    QTreeWidgetItem * edited;
};

Properties::Properties( MainWindow * mainw, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::Properties ),
    mw( mainw ) 
{
    ui->setupUi( this );
    d = new PrivateData;
    d->edited = 0;

    ui->listIcon->addItem( tr( "Editor" ) );
    ui->listIcon->addItem( tr( "Keyboard shortcuts" ) );
    ui->listIcon->addItem( tr( "Metadata" ) );

    foreach( QAction * a, mw->ui->menuBar->actions() ) 
    {
        ui->shortcutsTree->addTopLevelItem( addShortcuts( a ) );
    }

    ui->shortcutsTree->expandAll();

    QSettings settings;
    settings.beginGroup( "preferences" );

    settings.beginGroup( "editor" );
    ui->tabWidthSpinBox->setValue( settings.value( "tabWidth" ).toInt() );
    ui->indentWidthSpinBox->setValue( settings.value( "indentWidth" ).toInt() );
    ui->autoIndentCheckBox->setChecked( settings.value( "autoIndent",true ).toBool() );
    ui->indentUseTabsCheckBox->setChecked( settings.value( "indentUseTabs",true ).toBool() );
    ui->tabIndentsCheckBox->setChecked( settings.value( "tabIndents",true ).toBool() );
    auto value = settings.value( "backspaceUnindents",true ).toBool();
    ui->backspaceUnindentsCheckBox->setChecked(value);
    settings.endGroup();

    settings.beginGroup( "metadata" );
    ui->auteurPlainText->setPlainText( settings.value( "autor" ).toString() );
    ui->licencePlainText->setPlainText( settings.value( "licence" ).toString() );
    ui->dateCheck->setChecked( settings.value( "updateDateChecked",true ).toBool() );
    settings.endGroup();

    settings.beginGroup( "shortcuts" );
    foreach( QTreeWidgetItem * item, d->hash.keys() )
    {
        auto item_value = item->data( 3,Qt::DisplayRole ).toString();
        QString shortcut = settings.value( item_value ).toString();

        if ( ! shortcut.isEmpty() )
        {
            item->setData( 1, Qt::DisplayRole, shortcut );
            item->setData( 2, Qt::DisplayRole, shortcut );
            d->hash[item]->setShortcut( shortcut );
        }
    }

    settings.endGroup();
}

Properties::~Properties() 
{
    delete d;
    delete ui;
}

void Properties::on_shortcutsTree_itemSelectionChanged()
{
    if ( ui->shortcutsTree->selectedItems().isEmpty() ) 
    {
        d->edited = 0;
        return;
    }

    QTreeWidgetItem * item = ui->shortcutsTree->selectedItems().first();
    ui->keySequenceWidget->setKeySequence( item->data( 1, Qt::DisplayRole ).toString() );
}

void Properties::on_shortcutsTree_itemActivated( QTreeWidgetItem * item, int ) 
{
    ui->keySequenceWidget->setFocus();
    d->edited = item;
}

void Properties::on_keySequenceWidget_shortcutChanged( const QKeySequence & key ) 
{
    Q_ASSERT( d->edited != 0 );

    // We remove all items in conflict
    if ( d->conflicts.contains( d->edited ) ) 
    {
        foreach( QTreeWidgetItem * item, d->conflicts[d->edited] ) 
        {
            d->conflicts[item].removeOne( d->edited );
            if ( d->conflicts[item].isEmpty() )
            {
                d->conflicts.remove( item );
            }
        }

        d->conflicts.remove( d->edited );
    }

    // We change its value
    d->edited->setData( 1, Qt::DisplayRole, key.toString() );
    d->hash[d->edited]->setShortcut( key );

    foreach( QTreeWidgetItem * item, d->hash.keys() ) 
    {
        // We check if shortcuts are different
        // It they are not
        if ( item != d->edited and
             item->data( 1, Qt::DisplayRole ) == d->edited->data( 1, Qt::DisplayRole ) )
        {
            // On les ajoute à la hash conflit
            d->conflicts[d->edited] << item;
            d->conflicts[item] << d->edited;
        }

        // If there is a conflict
        if ( d->conflicts.contains( item ) )
        {
            item->setTextColor( 1, Qt::red );
        }
        else
        {
            item->setTextColor( 1, Qt::black );
        }
    }

    if ( d->conflicts.contains( d->edited ) )
    {
        d->edited->setTextColor( 1, Qt::red );
    }
}

QTreeWidgetItem * Properties::addShortcuts( QAction * a, const QString & parent )
{

    QString name = a->text().remove( "&" );
    QString completeName = parent + name;

    if ( a->menu() != 0 ) 
    {
        auto menu = new QTreeWidgetItem( QStringList() << name << "" << "" << completeName );
        menu->setFlags( menu->flags() ^ Qt::ItemIsSelectable );
        foreach( QAction * action, a->menu()->actions() ) 
        {
            if ( ! action->isSeparator() and
                    !( action == mw->ui->actionRecentFiles ) and
                    !( action == mw->ui->actionRecentProject ) )
            {
                menu->addChild( addShortcuts( action, completeName+"/" ) );
            }
        }

        return menu;
    }
    else
    {
        auto list = QStringList() << name << a->shortcut() << a->shortcut() << completeName;
        auto item = new QTreeWidgetItem(list);
        d->hash[item] = a;
        return item;
    }
}

void Properties::accept() 
{
    auto msg = tr( "There are unresolved conflicts. Do you want to preoceed ?" );
    // If there is no exception or user agree.
    if ( not d->conflicts.isEmpty() and
         QMessageBox::question( this,
                tr( "Warning" ),
                msg,
                QMessageBox::Cancel | QMessageBox::Ok ) != QMessageBox::Ok )
    {
        return;
    }

    QSettings settings;

    settings.beginGroup( "preferences" );
    settings.beginGroup( "editor" );
    settings.setValue( "tabWidth", ui->tabWidthSpinBox->value() );
    settings.setValue( "indentWidth", ui->indentWidthSpinBox->value() );
    settings.setValue( "autoIndent", ui->autoIndentCheckBox->isChecked() );
    settings.setValue( "indentUseTabs", ui->indentUseTabsCheckBox->isChecked() );
    settings.setValue( "tabIndents", ui->tabIndentsCheckBox->isChecked() );
    settings.setValue( "backspaceUnindents", ui->backspaceUnindentsCheckBox->isChecked() );
    settings.endGroup();

    foreach( QsciScintillaExtended * sci, mw->editorController()->openDocuments() ) 
    {
        sci->setTabWidth( ui->tabWidthSpinBox->value() );
        sci->setIndentationWidth( ui->indentWidthSpinBox->value() );
        sci->setAutoIndent( ui->autoIndentCheckBox->isChecked() );
        sci->setTabIndents( ui->tabIndentsCheckBox->isChecked() );
        sci->setBackspaceUnindents( ui->backspaceUnindentsCheckBox->isChecked() );
        sci->setIndentationsUseTabs( ui->indentUseTabsCheckBox->isChecked() );
    }

    settings.beginGroup( "metadata" );
    settings.setValue( "autor", ui->auteurPlainText->toPlainText() );
    settings.setValue( "licence", ui->licencePlainText->toPlainText() );
    settings.setValue( "updateDateChecked", ui->dateCheck->isChecked() );
    settings.endGroup();

    settings.beginGroup( "shortcuts" );
    foreach( QTreeWidgetItem * item, d->hash.keys() ) 
    {
        auto value = item->data( 3, Qt::DisplayRole ).toString();
        auto role = item->data( 1, Qt::DisplayRole );
        settings.setValue( value, role );
        item->setData( 2, Qt::DisplayRole, item->data( 1, Qt::DisplayRole ) );
    }

    settings.endGroup();

    QDialog::accept();
}

void Properties::reject()
{
    foreach( QTreeWidgetItem * item, d->hash.keys() )
    {
        auto value = item->data( 2, Qt::DisplayRole ).toString() ;
        d->hash[item]->setShortcut(value);
    }

    QDialog::reject();
}

