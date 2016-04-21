#ifndef O3PRMLINTERPRETATION_H
#define O3PRMLINTERPRETATION_H

#include <agrum/PRM/o3prm/O3prmReader.h>
#include "abstractparser.h"

/*!
  This class read a o3prm file to check its syntax.
  It send a finished() signal when done.
  When done, you can retrieve the O3prmReader.
 */
class O3prmlInterpretation : public AbstractParser 
{
    Q_OBJECT

    gum::prm::o3prm::O3prmReader<double> * m_reader;
    mutable bool prmChanged;

  public:
    O3prmlInterpretation( const QsciScintillaExtended * sci, QObject * parent );
    ~O3prmlInterpretation();

  protected:
    void run();
};

#endif // O3PRMLINTERPRETATION_H
