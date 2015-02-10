#include "prmtreemodel.h"
#include <QDebug>

using namespace std;
using namespace gum;
using namespace gum::prm;

const QString & PRMTreeModel::m_separator = ".";

PRMTreeModel::PRMTreeModel( const PRM<double> * prm,
        const o3prmr::O3prmrContext<double> * context,
        QObject *parent ) :
    QStandardItemModel( parent ), m_package( 0 ), m_block( 0 ), m_keywords( 0 ) 
{
    QStandardItem * root = invisibleRootItem();

    // Foreach >>>>>>  PRM::TYPE  <<<<<<
    const gum::Set< prm::Type<double> * > & sets = prm->types();

    for ( gum::Set<prm::Type<double> *>::iterator i = sets.begin() ; i != sets.end() ; ++i ) 
    {
        const prm::Type<double> * type = *i;

        QStandardItem * item = createChild( QString::fromStdString( type->name() ), root );
        item->setData( Type, ObjectRole );

        // Foreach labels
        const gum::DiscreteVariable & var = type->variable();

        for ( gum::Size i = 0 ; i < var.domainSize() ; i++ ) 
        {
            // Create the PRMTreeItem2 for the label (at the root)
            QStandardItem * label = createChild( QString::fromStdString( var.label( i ) ), root );
            label->setData( Label, ObjectRole );
        }
    }

    // Foreach >>>>>>  PRM::INTERFACE  <<<<<<
    foreach( prm::Interface<double> * i, prm->interfaces() )
    {

        QStandardItem * intrfce = createChild( QString::fromStdString( i->name() ).split( separator() ), root );
        intrfce->setData( Interface, ObjectRole );

        foreach( prm::Attribute<double> * a, i->attributes() )
        {
            QStandardItem * item = createChild( QString::fromStdString( a->name() ), intrfce );
            item->setData( Attribute, ObjectRole );
        }

        foreach( prm::ReferenceSlot<double> * r, i->referenceSlots() )
        {
            QStandardItem * refItem = createChild( QString::fromStdString( r->name() ), intrfce );
            refItem->setData( Refererence, ObjectRole );
            refItem->setData( QString::fromStdString( r->slotType().name() ).split( separator() ), TypeRole );
            refItem->setData( r->isArray(), IsArrayRole );
        }
    }

    // Foreach >>>>>>  PRM::CLASS  <<<<<<
    foreach( prm::Class<double> * c, prm->classes() )
    {
        QStandardItem * clss = createChild( QString::fromStdString( c->name() ).split( separator() ), root );
        clss->setData( Class, ObjectRole );

        foreach( prm::Attribute<double> * a, c->attributes() )
        {
            QStandardItem * item = createChild( QString::fromStdString( a->name() ), clss );
            item->setData( Attribute, ObjectRole );
        }

        foreach( prm::Aggregate<double> * a, c->aggregates() )
        {
            QStandardItem * item = createChild( QString::fromStdString( a->name() ), clss );
            item->setData( Aggregate, ObjectRole );
        }

        foreach( prm::ReferenceSlot<double> * r, c->referenceSlots() )
        {
            QStandardItem * refItem = createChild( QString::fromStdString( r->name() ), clss );
            refItem->setData( Refererence, ObjectRole );
            refItem->setData( QString::fromStdString( r->slotType().name() ).split( separator() ), TypeRole );
            refItem->setData( r->isArray(), IsArrayRole );
        }
    }

    // Foreach >>>>>>  PRM::SYSTEM  <<<<<<
    foreach( prm::System<double> * s, prm->systems() )
    {
        QStandardItem * systm = createChild( QString::fromStdString( s->name() ).split( separator() ), root );
        systm->setData( System, ObjectRole );

        for ( prm::System<double>::iterator i = s->begin() ; i != s->end() ; ++i ) {
            //QStandardItem * inst = createChild( QString::fromStdString(( *i )->name() ), systm );
            QStandardItem * inst = createChild( QString::fromStdString(i.val()->name() ), systm );
            inst->setData( Instance, ObjectRole );
            inst->setData( QString::fromStdString(i.val()->type().name() ).split( separator() ), TypeRole );
        }
    }

    // O3prml files don't have context
    if ( context != 0 )
    {
        foreach( const o3prmr::ImportCommand * i, context->imports() )
        {
            addAlias( QString::fromStdString( i->alias ),
                    QString::fromStdString( i->value ).split( separator() ) );
        }
    }
}

PRMTreeModel::~PRMTreeModel()
{

}

/// Add keywords to model.
void PRMTreeModel::setKeywords( const QStringList & keywords )
{
    if ( m_keywords != 0 )
    {
        removeSameChildren( m_keywords, invisibleRootItem() );
        delete m_keywords;
        m_keywords = 0;
    }

    if ( ! keywords.isEmpty() )
    {
        m_keywords = new QStandardItem( "keywords" );
        foreach( const QString & key, keywords )
        {
            QStandardItem * keyItem = new QStandardItem( key );
            keyItem->setData( key, LocalDataRole );
            keyItem->setData( Unknow, ObjectRole );
            m_keywords->appendRow( keyItem );
        }

        appendSameChildren( m_keywords, invisibleRootItem() );
    }
}

void PRMTreeModel::setCurrentPackage( const QStringList & package ) 
{
    if ( m_package != 0 ) 
    {
        removeSameChildren( m_package, invisibleRootItem() );
        m_package = 0;
    }

    if ( ! package.isEmpty() ) 
    {
        m_package = getChild( package );

        if ( m_package != 0 )
        {
            appendSameChildren( m_package, invisibleRootItem() );
        }
        else
        {
            qWarning() << "Warning : in PRMTreeModel::setCurrentPackage("
                << package.join( separator() )
                << ") : package not found.";
        }
    }
}

void PRMTreeModel::setCurrentBlock( const QString & block ) 
{
    if ( m_block != 0 ) 
    {
        removeSameChildren( m_block, invisibleRootItem() );
        m_block = 0;
    }

    if ( ! block.isEmpty() && m_package != 0 ) 
    {
        m_block = getChild( block, m_package );

        if ( m_block != 0 )
        {
            appendSameChildren( m_block, invisibleRootItem() );
        }
        else
        {
            qWarning() << "Warning : in PRMTreeModel::setCurrentBlock("
                << block
                << ") : block not found in package"
                << m_package->data( Qt::DisplayRole ).toString();
        }
    }
    else if ( m_package == 0 )
    {
        qWarning() << "Warning : in PRMTreeModel::setCurrentBlock("
            << block
            << ") : package must be set previously.";
    }
}

void PRMTreeModel::addAlias( const QString & alias, const QStringList & to ) 
{
    if ( alias.isEmpty() || to.isEmpty() )
    {
        return;
    }

    QStandardItem * aliasItem = 0;

    foreach( QStandardItem * item, m_aliases )
    {
        if ( item->data( LocalDataRole ).toString() == alias ) 
        {
            aliasItem = item;
            break;
        }
    }

    if ( aliasItem != 0 )
    {
        if ( aliasItem->data( TypeRole ).toStringList() != to ) 
        {
            // If default it already exists but does not reference the same item
            if ( alias == "default" )
            {
                removeSameChildren( aliasItem, invisibleRootItem() );
            }

            aliasItem->setData( to, TypeRole );
            aliasItem->removeRows( 0, aliasItem->rowCount() - 1 );

            if ( alias == "default" )
            {
                fetchMore( aliasItem->index() );
                appendSameChildren( aliasItem, invisibleRootItem() );
            }
        }
    }
    else
    {
        //  aliasItem = new QStandardItem(alias);
        aliasItem = createChild( alias, invisibleRootItem() );
        aliasItem->setData( alias, LocalDataRole );
        aliasItem->setData( to, TypeRole );
        aliasItem->setData( Alias, ObjectRole );
        m_aliases << aliasItem;

        if ( alias == "default" )
        {
            fetchMore( aliasItem->index() );
            appendSameChildren( aliasItem, invisibleRootItem() );
            takeRow( aliasItem->row() );
        }
    }
}

bool PRMTreeModel::canFetchMore( const QModelIndex & parent ) const 
{
    QStandardItem * item = itemFromIndex( parent );

    if ( item == 0 || item->rowCount() > 0 )
    {
        return false;
    }

    const QStringList & typeList = item->data( TypeRole ).toStringList();

    if ( typeList.isEmpty() )
    {
        return false;
    }

    QStandardItem * type = getChild( typeList );

    if ( type == 0 )
    {
        return false;
    }

    return type->rowCount() > 0;
}

void PRMTreeModel::fetchMore( const QModelIndex & parent ) 
{
    QStandardItem * item = itemFromIndex( parent );

    if ( item == 0 ) 
    {
        qWarning() << "Warning : in PRMTreeModel::fetchMore : item == 0.";
        return;
    }

    auto typeItem = getChild( item->data( TypeRole ).toStringList(), invisibleRootItem() );

    if ( typeItem == 0 ) 
    {
        qWarning() << "Warning : in PRMTreeModel::fetchMore : typeItem == 0.";
        return;
    }

    appendSameChildren( typeItem, item );
}

void PRMTreeModel::update( QSharedPointer<PRMTreeModel> model,
        QsciScintillaExtended * currentDocument ) 
{
    // >>>>>>>>>>>>>>>>>>>>>>   ALIASES   <<<<<<<<<<<<<<<<<<<<<<<<<

    QList<QStandardItem* > aliasesTemp = m_aliases;

    // Pour chaque alias du nouveau modèle
    foreach( QStandardItem * modelAliasItem, model->m_aliases )
    {
        const QString & modelAlias = modelAliasItem->data( LocalDataRole ).toString();
        bool aliasFound = false;
        // We check if it already exists in this model
        foreach( QStandardItem * thisAlias, aliasesTemp ) 
        {
            // if it already exists
            if ( modelAlias == thisAlias->data( LocalDataRole ).toString() ) {
                aliasFound = true;
                // We update it
                addAlias( modelAlias, modelAliasItem->data( TypeRole ).toStringList() );
                aliasesTemp.removeOne( thisAlias );
                break;
            }
        }

        // If it doesn't exists, we create it
        if ( ! aliasFound )
        {
            addAlias( modelAlias, modelAliasItem->data( TypeRole ).toStringList() );
        }
    }

    // We delete all remaining elements
    foreach( QStandardItem * aliasItem, aliasesTemp )
    {
        if ( aliasItem->data( LocalDataRole ).toString() == "default" )
        {
            removeSameChildren( aliasItem, invisibleRootItem() );
        }
        else
        {
            invisibleRootItem()->removeRow( aliasItem->row() );
        }

        m_aliases.removeOne( aliasItem );
    }

    // >>>>>>>>>>>>>>>>>>>>>>   CURRENT PACKAGE   <<<<<<<<<<<<<<<<<<<<<<<<<
    if ( ! currentDocument->package().isEmpty() )
    {
        setCurrentPackage( currentDocument->package().split( separator() ) );
    }
    else
    {
        setCurrentPackage( QStringList() );
    }

    // >>>>>>>>>>>>>>>>>>>>>>   CURRENT BLOCK   <<<<<<<<<<<<<<<<<<<<<<<<<
    if ( m_package == 0 )
    {
        return;
    }

    setCurrentBlock( currentDocument->block().second );

    // >>>>>>>>>>>>>>>>>>>>>>   BLOCK MEMBERS   <<<<<<<<<<<<<<<<<<<<<<<<<
    // If block haven't been found, can't update it.
    if ( m_block == 0 )
    {
        return;
    }

    auto modelBlock = model->getChild( m_block->data( Qt::DisplayRole ).toString().split( separator() ) );

    if ( modelBlock == 0 )
    {
        return;
    }

    // Remove all children foreach references that have fetchedMore
    auto items = m_block->data( Qt::DisplayRole ).toString().split( separator() );
    auto itemsList = findItems( items, TypeRole );

    foreach( QStandardItem * item, itemsList )
    {
        item->removeRows( 0, item->rowCount() );
    }

    removeSameChildren( m_block, invisibleRootItem() );
    removeSameChildren( m_block, m_block );

    appendSameChildren( modelBlock, m_block );
    appendSameChildren( m_block, invisibleRootItem() );
}

QStandardItem * PRMTreeModel::createChild( const QStringList & datas, QStandardItem * parent ) 
{
    if ( datas.isEmpty() )
    {
        return parent;
    }

    QString previousPath;

    if ( parent == 0 )
    {
        parent = invisibleRootItem();
    }
    else if ( parent != invisibleRootItem() )
    {
        previousPath = parent->data( Qt::DisplayRole ).toString() + separator();
    }

    QStandardItem * item = getChild( datas.first(), parent );

    if ( item == 0 ) 
    {
        item = createChild( datas.first(), parent );
        item->setData( datas.first(), LocalDataRole );
    }

    if ( datas.size() > 1 ) 
    {
        item->setData( Package, ObjectRole );
        return createChild( datas.mid( 1 ), item );
    }
    else
    {
        return item;
    }
}

QStandardItem * PRMTreeModel::createChild( const QString & data, QStandardItem * parent ) 
{
    QString previousPath;

    if ( parent == 0 )
    {
        parent = invisibleRootItem();
    }
    else if ( parent != invisibleRootItem() )
    {
        previousPath = parent->data( Qt::DisplayRole ).toString() + separator();
    }

    // If a child with this name already exist, return it.
    QStandardItem * previousExistingChild = getChild( data, parent ) ;

    if ( previousExistingChild != 0 )
    {
        return previousExistingChild;
    }

    QStandardItem * item = new QStandardItem( previousPath + data );

    item->setData( data, LocalDataRole );

    parent->appendRow( item );

    return item;
}

QStandardItem * PRMTreeModel::getChild( const QString & data, QStandardItem * parent ) const 
{
    if ( parent == 0 )
    {
        parent = invisibleRootItem();
    }

    for ( int i = parent->rowCount() - 1 ; i >= 0 ; i-- )
    {
        QStandardItem * child = parent->child( i );

        if ( child->data( LocalDataRole ).toString() == data )
        {
            return child;
        }
    }

    return 0;
}

QStandardItem * PRMTreeModel::getChild( const QStringList & path, QStandardItem * parent ) const 
{
    if ( parent == 0 )
    {
        parent = invisibleRootItem();
    }

    if ( path.isEmpty() )
    {
        return parent;
    }

    for ( int i = parent->rowCount() - 1 ; i >= 0 ; i-- )
    {
        QStandardItem * child = parent->child( i );

        if ( child->data( LocalDataRole ).toString() == path.first() )
        {
            return getChild( path.mid( 1 ), child );
        }
    }

    return 0;
}

void PRMTreeModel::removeSameChildren( const QStandardItem * from , QStandardItem * in ) 
{
    beginRemoveRows( in->index(),0,in->rowCount()-1 );
    blockSignals( true );

    for ( int i = from->rowCount() - 1 ; i >= 0 ; i-- )
    {
        for ( int j = in->rowCount() - 1 ; j >= 0 ; j-- )
        {
            auto in_child = in->child( j )->data( LocalDataRole ).toString();
            auto from_child = from->child( i )->data( LocalDataRole ).toString();
            if ( in_child == from_child)
            {
                in->removeRow( j );
                break;
            }
        }
    }

    blockSignals( false );

    endRemoveRows();
}

void PRMTreeModel::appendSameChildren( const QStandardItem * from , QStandardItem * to ) 
{
    QString toPath;

    if ( to != invisibleRootItem() )
    {
        toPath = to->data( Qt::DisplayRole ).toString() + separator();
    }

    beginInsertRows( to->index(),to->rowCount(),to->rowCount() + from->rowCount()-1 );

    blockSignals( true );

    for ( int i = from->rowCount() - 1 ; i >= 0 ; i-- )
    {
        QStandardItem * child = from->child( i );
        const QString & data = child->data( LocalDataRole ).toString();

        QStandardItem * item = createChild( data, to );
        item->setData( child->data( ObjectRole ), ObjectRole );
        item->setData( child->data( TypeRole ), TypeRole );
        item->setData( child->data( IsArrayRole ), IsArrayRole );

        // For fetchMore
        if ( item->data( TypeRole ).toStringList().isEmpty() )
        {
            auto items = child->data( Qt::DisplayRole ).toString().split( separator() );
            item->setData( items, TypeRole );
        }
    }

    blockSignals( false );

    endInsertRows();
}

QList<QStandardItem *> PRMTreeModel::findItems( const QVariant & data, int role ) const 
{
    QList<QStandardItem *> results;
    QList<QStandardItem *> openList;
    openList << invisibleRootItem();

    while ( ! openList.isEmpty() ) 
    {
        QStandardItem * current = openList.takeFirst();

        if ( current == 0 )
        {
            continue;
        }

        if ( current->data( role ) == data )
        {
            results << current;
        }

        for ( int i = current->rowCount() - 1 ; i >= 0 ; i-- )
        {
            openList.append( current->child( i ) );
        }
    }

    return results;
}

