#ifndef QSCILEXERO3PRML2_H
#define QSCILEXERO3PRML2_H

#include <Qsci/qscilexercustom.h>

class QsciLexerO3prml2 : public QsciLexerCustom {
    Q_OBJECT

  public:
    enum {
      Default,
      CommentLine,
      CommentLineDoc,
      CommentBlock,
      CommentBlockDoc,
      Keyword,
      Operator,
      Type,
      Function,
      Number,
      Error
    };

    static bool isComment( int style ) {
      return style == CommentLine || style == CommentLineDoc || style == CommentBlock || style == CommentBlockDoc;
    }

    QsciLexerO3prml2( QObject * parent = 0 );
    ~QsciLexerO3prml2();

    virtual const char *language() const;
    virtual QString description( int ) const;
    virtual QColor defaultColor( int ) const;
    virtual QFont  defaultFont( int ) const;
    virtual QColor defaultPaper( int ) const;
    virtual const char *keywords( int set ) const;

    virtual void styleText( int start, int end );

  protected:
    void addToApi( const QString & entry );

  private:

    struct PrivateData;
    PrivateData * d;
};

#endif // QSCILEXERO3PRML2_H
