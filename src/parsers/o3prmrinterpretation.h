#ifndef O3PRMRINTERPRETATION_H
#define O3PRMRINTERPRETATION_H

#include "abstractparser.h"

#include <vector>

namespace gum {

  namespace prm {

    namespace o3prmr {

      class O3prmrInterpreter;

      struct QueryResult;
    }
  }
}

/**
 * This class interpret a document.
 * You can parse the document by calling parse() method,
 * or a single-line command by calling parseCommand(QString command) method.
 * If \a syntaxMode is true, no calcul will be done.
 * It send a finished() signal when done.
 * When done, you can always retrieve prm and errors,
 * and also results and last command (empty if it was a file).
 */

class O3prmrInterpretation : public AbstractParser {
    Q_OBJECT

  public:
    //! Constructor. Create o3prmr interpreter.
    O3prmrInterpretation( const QsciScintillaExtended * sci, QObject * parent );
    //! Destructor. Delete o3prmr interpreter.
    ~O3prmrInterpretation();

    //! Return last command.
    QString command() const;
    //! Return result of execution.
    std::vector<gum::prm::o3prmr::QueryResult> results() const;

  public slots:
    //! \reimp
    virtual void parse( Priority priority = InheritPriority );
    //! Parse a single command
    virtual void parseCommand( const QString & command, Priority priority = InheritPriority );

  protected:
    //! \reimp
    virtual void run();

    struct PrivateData;
    PrivateData * d;
};


#endif // O3PRMRINTERPRETATION_H
