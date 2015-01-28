#include "editcontroller.h"

#include <iostream>

#include "uis/mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"
#include "prmcompleter.h"

#include <Qsci/qscilexer.h>
#include <QDebug>
#include <QShortcut>
//

struct EditController::PrivateData {
    PRMCompleter * completer;
    QSharedPointer<PRMTreeModel> prmModel;
};

// Constructor
EditController::EditController( MainWindow * mw, QObject * parent ) :
    QObject( parent ),
    mw( mw ),
    d( new PrivateData ),
    pr( new Properties( mw,mw ) )
{
    d->completer = new PRMCompleter( this );
    connect(mw->ui->actionEditPreferences, SIGNAL( triggered() ),
            this, SLOT( editPreferences() ) );

    // Must be start after project triggerInit
    QTimer::singleShot( 500, this, SLOT( triggerInit() ) );
}

// Destructor
EditController::~EditController() {
    delete d;
}

void EditController::triggerInit() {
    //connect( mw->bc, SIGNAL( projectModelChanged() ), this, SLOT( onProjectModelChanged() ) );
}

QCompleter * EditController::completer() const {
    return d->completer;
}

/**
 */
void EditController::editPreferences() {
    pr->exec();
}

void EditController::onProjectModelChanged() 
{
    // QSharedPointer<PRMTreeModel> newPRMModel = mw->bc->projectModel();

    // // If doesn't changed, don't changed completer.

    // if ( newPRMModel.isNull() || d->prmModel == newPRMModel || ! mw->fc->hasCurrentDocument() )
    //     return;

    // QSharedPointer<PRMTreeModel> oldPRMModel = d->prmModel;

    // d->prmModel.clear();

    // d->prmModel = newPRMModel;

    // auto currentdoc = mw->fc->currentDocument();
    // auto lexer = currentdoc->lexer();
    // if (lexer)
    // {
    //     auto keywords = lexer->keywords( 1 );

    //     auto keys = QString(keywords).split(QChar(' '));

    //     d->prmModel->setKeywords(keys);

    //     //d->prmModel->setKeywords( QString( mw->fc->currentDocument()->lexer()->keywords( 1 ) ).split( QChar( ' ' ) ) );

    //     d->prmModel->sort( 0 );

    //     d->completer->setModel( d->prmModel.data() );

    //     d->completer->setModelSorting( QCompleter::CaseSensitivelySortedModel );

    //     emit completerChanged();
    // }
}
