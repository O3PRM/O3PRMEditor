#include "qscilexero3prml.h"

#include <Qsci/qsciapis.h>
#include <QRegExp>
#include <QDebug>

QsciLexerO3prml::QsciLexerO3prml( QObject * parent ) : QsciLexerJava( parent ) 
{
    QFont font( "courier" );
    font.setPointSize( 12 );
    font.setStyleHint( QFont::Monospace );
    setFont( font,QsciLexerJava::Default );
    setFont( font,QsciLexerJava::Comment );
    setFont( font,QsciLexerJava::CommentLine );
    setFont( font,QsciLexerJava::Number );
    setFont( font,QsciLexerJava::Operator );
    setFont( font,QsciLexerJava::Identifier );
    setFont( font,QsciLexerJava::GlobalClass );
    font.setBold( true );
    setFont( font,QsciLexerJava::CommentDoc );
    setFont( font,QsciLexerJava::CommentLineDoc );
    setFont( font,QsciLexerJava::Keyword );
    setFont( font,QsciLexerJava::Operator );

    setColor( Qt::blue, QsciLexerJava::CommentDoc );
    setColor( Qt::blue, QsciLexerJava::CommentLineDoc );
    setColor( Qt::darkGreen, QsciLexerJava::CommentLine );
    setColor( Qt::darkGreen, QsciLexerJava::Comment );
    setColor( QColor( 255,170,0 ), QsciLexerJava::Number );

    QsciAPIs *api = new QsciAPIs( this );
    api->load( ":/api/o3prml.api" );
    api->prepare(); 
    setAPIs( api ); 
}

QsciLexerO3prml::~QsciLexerO3prml() { }

const char * QsciLexerO3prml::language() const 
{
    return "O3prm";
}

const char * QsciLexerO3prml::keywords( int set ) const 
{
    if ( set != 1 )
    {
        return 0;
    }

    auto keywords = "boolean class default dependson extends float implements "
        "import int interface noisyOr package system type";
    return keywords;
}
