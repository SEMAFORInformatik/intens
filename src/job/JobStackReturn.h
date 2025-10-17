
#if !defined(JOB_STACK_RETURNCONTROL_INCLUDED_H)
#define JOB_STACK_RETURNCONTROL_INCLUDED_H

#include "job/JobStackItem.h"
#include <QTime>

class JobStackReturn : public JobStackItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackReturn(JobCodeStream *code, int addr, const std::string &name )
    : m_code( code )
    , m_addr( addr )
    , m_name( name )
    , m_startTime(QTime::currentTime())
    {
  }
  virtual ~JobStackReturn(){}

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  virtual StackType Type() { return stck_ReturnControl; }

  /** Die Funktion liefert eine Referenz auf sich selber (cast)
      \return Referenz auf this
   */
  virtual JobStackReturn &returncontrol() { return *this; }

  OpStatus getCodeStream( JobCodeStream *&code, int &addr, std::string &name ){
    code = m_code;
    addr = m_addr;
    name = m_name;
    if( m_code == 0 ) return op_EndOfJob;
    return op_Ok;
  }

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr ){
    ostr << "JobStackReturn: " << m_addr << std::endl;
  }
  const QTime& getStartTime() { return m_startTime; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  JobCodeStream    *m_code;
  int               m_addr;
  std::string       m_name;
  QTime             m_startTime;

};

#endif
