#include "qscilexero3prml2.h"

#include "models/qsciscintillaextended.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include <Qsci/qsciapis.h>

#include <QDebug>

struct QsciLexerO3prml2::PrivateData 
{
  QRegExp commentLine;
  QRegExp commentLineDoc;
  QRegExp commentBlock;
  QRegExp commentBlockDoc;
  QRegExp function;
  QRegExp symbol;
  QRegExp id;
  QRegExp number;
  QRegExp operators;
  QRegExp keywords;
  QRegExp spaces;

  QRegExp assignation;
  QRegExp declarationReference;
  QRegExp declarationAttribute;

  QStringList waitBlock;
  QStringList keywordsList;
  QStringList typesList;

  QFont normalFont;
  QFont normalBoldFont;

  QsciAPIs * api;
};

bool isOperator( QChar c ) 
{
  return c.category() == QChar::Mark_Enclosing || c.category() == QChar::Symbol_Math;
}

QsciLexerO3prml2::QsciLexerO3prml2( QObject * parent ) :
    QsciLexerCustom( parent ), d( new PrivateData ) 
{
    d->waitBlock = QString( "class interface system" ).split( " ", QString::SkipEmptyParts );
    auto keywords = "boolean class default dependson extends float implements import int "
        "interface noisyOr package system type";
    d->keywordsList = QString(keywords).split( " ", QString::SkipEmptyParts );
    d->typesList = QString( "boolean float int" ).split( " ", QString::SkipEmptyParts );

    d->commentLine = QRegExp( "^//[^/]" );
    d->commentLineDoc = QRegExp( "^///" );
    d->commentBlock = QRegExp( "^/\\*[^\\*]" );
    d->commentBlockDoc = QRegExp( "^/\\*\\*" );
    d->function = QRegExp( "^([a-zA-Z_][a-zA-Z_0-9]*)\\s*\\(" );
    d->symbol = QRegExp( "^[a-zA-Z_]\\w*" );
    d->id = QRegExp( "^[a-zA-Z_]\\w*(\\.[a-zA-Z_]\\w*)*" );
    d->number = QRegExp( "^\\d+(\\.\\d+)?" );
    d->operators = QRegExp( "^(\\=|\\+|\\-|\\{|\\}|\\[|\\]|\\.|\\;|\\,|\\:|\\(|\\)|\\*)" );
    d->keywords = QRegExp( "^(" + d->keywordsList.join( "|" ) + ")\\s" );
    d->spaces = QRegExp( "^\\s+" );

    d->assignation = QRegExp( "^(([a-zA-Z_][a-zA-Z_0-9]*)(\\.([a-zA-Z_][a-zA-Z_0-9]*))*)(\\s*)((-|\\+)?\\=)" );
    d->declarationReference = QRegExp( "^[a-zA-Z_][\\.a-zA-Z_0-9]*(\\[\\d*\\])?\\s+[a-zA-Z_][a-zA-Z_0-9]*\\s*;" );
    d->declarationAttribute = QRegExp( "^[a-zA-Z_][\\.a-zA-Z_0-9]*(\\[\\d*\\])?\\s[^\\{\\;\\=]*\\{" );

    d->normalFont = QFont( "courier", 12 );
    d->normalFont.setStyleHint( QFont::Monospace );
    d->normalBoldFont = QFont( d->normalFont );
    d->normalBoldFont.setBold( true );

    d->api = new QsciAPIs( this );
    d->api->load( ":/api/o3prml.api" );
    d->api->prepare(); 
    setAPIs( d->api );
}

QsciLexerO3prml2::~QsciLexerO3prml2() 
{
    delete d;
}

const char* QsciLexerO3prml2::language() const 
{
    return "O3prm";
}

QString QsciLexerO3prml2::description( int style ) const 
{
    switch ( style )
    {
        case Default:
        {
            return "Default style";
        }
        case CommentLine:
        {
            return "CommentLine style";
        }
        case CommentLineDoc:
        {
            return "CommentLineDoc style";
        }
        case CommentBlock:
        {
            return "CommentBlock style";
        }
        case CommentBlockDoc:
        {
            return "CommentBlockDoc style";
        }
        case Keyword:
        {
            return "Keyword style";
        }
        case Operator:
        {
            return "Operator style";
        }
        case Type:
        {
            return "Type style";
        }
        case Function:
        {
            return "Function style";
        }
        case Number:
        {
            return "Number style";
        }
        case Error:
        {
            return "Doesn't exist in O3prml language.";
        }
        default:
        {
            return "unknow style";
        }
    }
}

QColor QsciLexerO3prml2::defaultColor( int style ) const 
{
    switch ( style ) 
    {
        case Default:
        case Function:
        {
            return Qt::black;
        }
        case CommentLine:
        case CommentBlock:
        {
            return Qt::darkGreen;
        }
        case CommentBlockDoc:
        case CommentLineDoc:
        {
            return Qt::blue;
        }
        case Number:
        {
            return QColor( 255,170,0 );
        }
        case Type:
        {
            return Qt::darkMagenta;
        }
        case Error:
        {
            return Qt::black;
        }
        default:
        {
            return Qt::black;
        }
    }
}

QFont  QsciLexerO3prml2::defaultFont( int style ) const 
{
    switch ( style )
    {
        case Default:
        case CommentBlock:
        case CommentLine:
        case Type:
        case Number:
        {
            return d->normalFont;
        }
        case CommentBlockDoc:
        case CommentLineDoc:
        case Keyword:
        case Operator:
        case Function:
        {
            return d->normalBoldFont;
        }
        default:
        {
            return d->normalFont;
        }
    }
}

QColor QsciLexerO3prml2::defaultPaper( int ) const 
{
    return Qt::white;
}

const char * QsciLexerO3prml2::keywords( int set ) const 
{
    return d->keywordsList.join( " " ).toUtf8().constData();
}

void QsciLexerO3prml2::styleText( int start, int end ) 
{
    if ( !editor() || end <= start )
    {
        return;
    }

    int utf8Len = end - start;
    char *chars = ( char * ) malloc( utf8Len + 1 );

    if ( chars == NULL )
    {
        return;
    }

    editor()->SendScintilla( QsciScintilla::SCI_GETTEXTRANGE, start, end, chars );
    const QString source = QString::fromUtf8( chars );
    free( chars );

    int style, previousStyle, lineNb = 0, level = QsciScintilla::SC_FOLDLEVELBASE;
    bool isWaitingBlock = false, isWaitingPackage = false;
    bool startInComment = false, newLineJustMatch = false, startBlock = false;

    QString line;

    // If we don't start at the beggining of the document,
    // we retrieve previous context.
    if ( start != 0 )
    {
        previousStyle = editor()->SendScintilla( QsciScintilla::SCI_GETSTYLEAT, start-1 );
        startInComment = (( previousStyle == CommentBlock ) or
                ( previousStyle == CommentBlockDoc ) );

        // We get line number corresponding to start position.
        // style get index but it is unneeded
        editor()->lineIndexFromPosition( start,&lineNb,&style ); 

        // We get the previous level
        // start != 0 => lineNb != 0
        level = editor()->SendScintilla( QsciScintilla::SCI_GETFOLDLEVEL, lineNb - 1 ); 

        // If previous line is a start of block comment
        if ( level & QsciScintilla::SC_FOLDLEVELHEADERFLAG )
        { 
            // Remove header flag
            level -= QsciScintilla::SC_FOLDLEVELHEADERFLAG; 
            level++;
        }
    }

    // Text is encoded as UTF8 in Scintiall, thus 1 char does not necessarily equal 1 character.
    // Since QString hides encoding, we need to keep track of positions in both the QString and 
    // the char (in UTF8).
    for ( int i = 0, len = 0, iUtf8 = 0, lenUtf8 = 0 ;
            i < source.size() ;
            i += len, iUtf8 += lenUtf8 )
    {
        len = source.indexOf( "\n",i ) - i + 1;

        if ( len < 1 )
        {
            line = source.mid( i, source.length() );
        }
        else
        {
            line = source.mid( i, len );
        }

        // Space or unprintable char
        if ( ! startInComment && line.contains( d->spaces ) )
        {
            // match \n
            if ( d->spaces.cap( 0 ).contains( "\n" ) )
            { 

                if ( ! startBlock )
                {
                    editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
                }

                //lineNb++;
                startBlock = false;
            }

            newLineJustMatch = true;
            len = d->spaces.cap( 0 ).length();
            style = Default;

            
        }
        // Comment block
        else if ( startInComment )
        { 
            style = previousStyle;
            len = line.indexOf( "*/" ) + 2;

            if ( len < 2 ) 
            {
                len = line.length();
                editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
                //lineNb++;
            }
            else
            {
                level--;
                editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
                startInComment = false;
            }

        }
        // Comment line
        else if ( line.contains( d->commentLine ) )
        {
            style = CommentLine;
            len = line.length();
            editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );

            
        } 
        // CommentLineDoc
        else if ( line.contains( d->commentLineDoc ) )
        { 
            style = CommentLineDoc;
            len = line.length();
            editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );

            
        } 
        // CommentBlock
        else if ( line.contains( d->commentBlock ) )
        {
            style = CommentBlock;
            len = line.indexOf( "*/" ) + 2;

            if ( len < 2 )
            {
                len = line.length(); // With \n

                if ( ! startBlock )
                {
                    editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL,
                            lineNb,
                            level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );
                }

                level++;
                startInComment = true;
            }
            
        } 
        // CommentBlockDoc
        else if ( line.contains( d->commentBlockDoc ) )
        { 
            style = CommentBlockDoc;
            len = line.indexOf( "*/" ) + 2;

            if ( len < 2 )
            {
                len = line.length(); // With \n
                editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL,
                        lineNb,
                        level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );
                level++;
                startInComment = true;
            }

            
        }
        // Number: #^\d+(\.\d+)?#
        else if ( line.contains( d->number ) )
        {
            
            style = Number;
            len = d->number.cap( 0 ).length();

            
        } 
        // Operator: #^|\=|\+\=|\-\=|\{|\}|\[|\]|\.|\;|\,|\(|\)#
        else if ( line.contains( d->operators ) ) 
        { 
            style = Operator;
            len = d->operators.cap( 0 ).length();

            if ( d->operators.cap( 0 ) == "{" ) 
            {
                if ( ! startBlock ) 
                {
                    editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL,
                            lineNb,
                            level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );
                }

                level++;
                startBlock = true;
            }
            else if ( d->operators.cap( 0 ) == "}" )
            {
                level--;
                // If { and } are on the same line
                startBlock = false;
            }
            
        } 
        // Keyword: #^(type|class|import|package|...)\s#
        else if ( line.contains( d->keywords ) )
        { 
            style = Keyword;
            len = d->keywords.cap( 0 ).length();

            if ( d->keywords.cap( 1 ) == "package" ) 
            {
                isWaitingPackage = true;
                isWaitingBlock = false;
            }
            else if ( d->waitBlock.contains( d->keywords.cap( 1 ) ) ) 
            {
                isWaitingBlock = true;
                isWaitingPackage = false;
            }
            else 
            {
                isWaitingBlock = false;
                isWaitingPackage = false;
            }
            
        } 
        // Symbol: #^[a-zA-Z_]([a-zA-Z0-9_])*#
        else if ( line.contains( d->id ) ) 
        { 
            len = d->id.cap( 0 ).length();
            style = Default;

            if ( isWaitingPackage ) 
            {
                style = Type;
                editor()->markerAdd( lineNb,QsciScintillaExtended::Package );
            }
            else if ( isWaitingBlock ) 
            {
                style = Type;
                editor()->markerAdd( lineNb,QsciScintillaExtended::Block );
            }

            isWaitingPackage = false;
            isWaitingBlock = false;
        }
        else
        { 
            style = Error;
            len = line.length();
        }

        if ( newLineJustMatch )
        {
            newLineJustMatch = false;
        }

        if ( style != Keyword ) 
        {
            isWaitingBlock = false;
            isWaitingPackage = false;
        }

        // If we reach end of line, increment compter
        if ( len == line.length() )
        {
            lineNb++;
        }

        // len was calculated in QString
        // we must translate it in Utf8 bytes length.
        lenUtf8 = line.mid( 0, len ).toUtf8().length();

        Q_ASSERT( lenUtf8 > 0 );
        Q_ASSERT( start+iUtf8+lenUtf8 <= end );

        startStyling( start+iUtf8 );
        setStyling( lenUtf8, style );
        previousStyle = style;
    }

    // Usefull for the EOF, if there is no carriage return
    editor()->SendScintilla( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
}

void QsciLexerO3prml2::addToApi( const QString & entry ) 
{
    d->api->add( entry );
}
