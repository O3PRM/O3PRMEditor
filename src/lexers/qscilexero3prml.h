#ifndef QSCILEXERO3PRML_H
#define QSCILEXERO3PRML_H

#include <QObject>

#include <Qsci/qscilexerjava.h>


//! \brief The QsciLexerO3prml class encapsulates the Scintilla O3prml lexer.

class QsciLexerO3prml : public QsciLexerJava {
    Q_OBJECT

  public:
    //! Construct a QsciLexerO3prml with parent \a parent.  \a parent is
    //! typically the QsciScintilla instance.
    QsciLexerO3prml( QObject * parent = 0 );

    //! Destroys the QsciLexerO3prml instance.
    virtual ~QsciLexerO3prml();

    //! Returns the name of the language.
    const char * language() const;

    //! Returns the set of keywords for the keyword set \a set recognised
    //! by the lexer as a space separated string.
    const char * keywords( int set ) const;
};

#endif // QSCILEXERO3PRML_H
