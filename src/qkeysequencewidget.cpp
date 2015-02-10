#include "qkeysequencewidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include <QApplication>
#include <QStyle>

struct QKeySequenceWidget::PrivateData 
{
    MyLineEdit * lineEdit;
    QPushButton * clearButton;
    QPushButton * okButton;
};

void QKeySequenceWidget::initParameters() 
{
    setMinimumWidth( 100 );
    setFixedHeight( 25 );

    d = new PrivateData;

    d->lineEdit = new MyLineEdit( this );
    auto cl_icon = QIcon(qApp->style()->standardIcon(QStyle::SP_DialogResetButton).pixmap(64, 64));
    d->clearButton = new QPushButton( cl_icon, "", this );
    d->clearButton->setFixedSize( QSize( 25, 25 ) );
    auto ok_icon = QIcon(qApp->style()->standardIcon(QStyle::SP_DialogOkButton).pixmap(64, 64));
    d->okButton = new QPushButton( ok_icon, "", this );
    d->okButton->setFixedSize( QSize( 25, 25 ) );

    QHBoxLayout * box = new QHBoxLayout( this );
    box->setContentsMargins( 0, 0, 0, 0 );
    box->setSpacing( 3 );
    box->addWidget( d->lineEdit );
    box->addWidget( d->clearButton );
    box->addWidget( d->okButton );

    connect( d->clearButton, SIGNAL( clicked() ),
            this, SLOT( clear() ) );
    connect( d->clearButton, SIGNAL( released() ),
            d->lineEdit, SLOT( setFocus() ) );
    connect( d->lineEdit, SIGNAL( editingFinished() ),
            this, SLOT( onLineEditionFinished() ) );
}

QKeySequenceWidget::QKeySequenceWidget( QWidget * parent ) :
    QWidget( parent ) 
{
    initParameters();
}

QKeySequenceWidget::QKeySequenceWidget( const QKeySequence & sequence, QWidget * parent ) :
    QWidget( parent )
{
    initParameters();
    d->lineEdit->setKeySequence( sequence );
}

QSize QKeySequenceWidget::sizeHint() const 
{
    return QSize( 200, 30 );
}

QKeySequence QKeySequenceWidget::keySequence() const 
{
    return d->lineEdit->keySequence();
}

void QKeySequenceWidget::setKeySequence( const QKeySequence & keySequence ) 
{
    d->lineEdit->setKeySequence( keySequence );
}

void QKeySequenceWidget::setKeySequence( const QString & keySequence ) 
{
    setKeySequence( QKeySequence( keySequence ) );
}

void QKeySequenceWidget::clear() 
{
    d->lineEdit->clear();
}

void QKeySequenceWidget::setFocus() 
{
    d->lineEdit->setFocus();
    d->lineEdit->selectAll();
}

void QKeySequenceWidget::onLineEditionFinished() 
{
    emit shortcutChanged( keySequence() );
}

QDebug operator<<( QDebug dbg, const QKeySequenceWidget & object ) 
{
    return dbg.nospace() << "QKeySequenceWidget("
        << &object
        << ") : "
        << object.keySequence().toString();
}

