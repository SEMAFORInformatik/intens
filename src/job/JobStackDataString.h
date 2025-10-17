
#if !defined(JOB_STACK_DATA_STRING_INCLUDED_H)
#define JOB_STACK_DATA_STRING_INCLUDED_H

#include "job/JobStackData.h"

class JobStackDataString : public JobStackData
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataString( const std::string &value )
    : JobStackData( true )
    , m_value( value ){
  }
  virtual ~JobStackDataString(){}

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  DataDictionary::DataType getDataType() { return DataDictionary::type_String; }
  /** Die Funktion liefert die Units des Objektes.
      @return std::string
  */
  virtual bool getUnits( std::string & units );

  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackData *clone();

  /** Diese Funktion liefert Real-Teil des Werts als double. Falls Der Wert
       ungültig oder nicht kompatibel ist, ist das Resultat der Funktion False.
      @param d Wert als double.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getRealValue( double &d );

  /** Diese Funktion liefert den Wert als int. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param i Wert als int.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getIntegerValue( int &i );

  /** Diese Funktion liefert den Wert als string. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param s Wert als string.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getStringValue( std::string &s );

  /** Diese Funktion wird verwendet um zu klären, ob das Resultat einer Expression
      True oder False, resp. ungleich Null oder Null ist.
      @return True: => Der Wert des Objekts ist gültig und nicht empty().
   */
  virtual bool isTrue();

  /** Diese Funktion schreibt den Wert in den Outputstream.
      @param ostr Stream für den Output.
  */
  virtual void print( std::ostream &ostr );

  virtual OpStatus add( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus subtract( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus multiply( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus divide( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus power( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus negate( JobEngine *eng );
  virtual OpStatus greater( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus greaterEqual( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus less( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus lessEqual( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus equal( JobEngine *eng, JobStackDataPtr &dat );
  virtual OpStatus notEqual( JobEngine *eng, JobStackDataPtr &dat );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string     m_value;

};

#endif
