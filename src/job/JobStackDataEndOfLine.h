
#if !defined(JOB_STACK_DATA_ENDOFLINE_INCLUDED_H)
#define JOB_STACK_DATA_ENDOFLINE_INCLUDED_H

#include "job/JobStackDataString.h"

class JobStackDataEndOfLine : public JobStackDataString
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataEndOfLine()
    : JobStackDataString( "\n" ){
  }
  virtual ~JobStackDataEndOfLine(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackDataEndOfLine *clone();

  /** Die Funktion meldet, ob es sich um ein EndOfLine handelt.
   */
  virtual bool isEndOfLine() { return true; }

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr );

};

#endif
