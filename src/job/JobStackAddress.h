
#if !defined(JOB_STACK_ADDRESS_INCLUDED_H)
#define JOB_STACK_ADDRESS_INCLUDED_H

#include "job/JobStackItem.h"

class JobStackAddress : public JobStackItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackAddress( int addr )
    : m_addr( addr ){
  }
  virtual ~JobStackAddress(){}

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des StackItems.
      \return Typ
  */
  virtual StackType Type() { return stck_Address; }

  /** Die Funktion liefert eine Referenz auf sich selber (cast)
      \return Referenz auf this
   */
  virtual JobStackAddress &address() { return *this; }

  int getAddress() { return m_addr; }

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr ){
    ostr << "JobStackAddress: " << m_addr << std::endl;
  }
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int   m_addr;

};

#endif
