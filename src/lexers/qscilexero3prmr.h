#ifndef QSCILEXERO3PRMR_H
#define QSCILEXERO3PRMR_H

#include <QObject>

#include <Qsci/qscilexerjava.h>


//! \brief The QsciLexerO3prmr class encapsulates the Scintilla O3prmr lexer.

class QsciLexerO3prmr : public QsciLexerJava {
    Q_OBJECT

  public:
    //! Construct a QsciLexerO3prmr with parent \a parent.  \a parent is
    //! typically the QsciScintilla instance.
    QsciLexerO3prmr( QObject * parent = 0 );

    //! Destroys the QsciLexerO3prmr instance.
    virtual ~QsciLexerO3prmr();

    //! Returns the name of the language.
    const char * language() const;

    //! Returns the set of keywords for the keyword set \a set recognised
    //! by the lexer as a space separated string.
    const char * keywords( int set ) const;
};

#endif // QSCILEXERO3PRMR_H
