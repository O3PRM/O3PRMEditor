#include <QtGui/QApplication>
#include <QTextCodec>
#include <QDebug>

#include <agrum/core/utils.h>

#include <iostream>

#include "uis/mainwindow.h"

/*!
    The app is composed of a main window and different controller classes.

    In uis/ you will find the different user interfaces used by the application,
    the most notable one is defined in uis/mainwindow.h.
    
    In constrollers/ you will find the controllers which handles events emited
    by the different uis or by controllers themselves. 

    In models/ you will find the models displayed by the uis.

    In lexers/ and pareser/ you will find QScintilla extensions for the O3PRM language.

    Finally, rsrc/ contains ressources used by the application.
 */
int main( int argc, char *argv[] ) try
{
    QApplication a( argc, argv );
    QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
    QCoreApplication::setOrganizationName( "Lip6" );
    QCoreApplication::setApplicationName( "O3prmEditor" );

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
catch ( const gum::Exception & err ) 
{
    std::cerr << err.errorContent() << std::endl;
    return 1;
}
catch ( const std::string & err ) 
{
    std::cerr << err << std::endl;
    return 2;
}
catch ( const QString & err ) 
{
    std::cerr << err.data() << std::endl;
    return 3;
}

