
#if !defined(JOB_STACK_DATA_STRUCTURE_INCLUDED_H)
#define JOB_STACK_DATA_STRUCTURE_INCLUDED_H

#include "job/JobStackData.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataReference.h"
#include "xfer/XferDataItem.h"

class JobStackDataStructure : public JobStackData
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackDataStructure( XferDataItem& data );
  JobStackDataStructure( DataReference* data );
  virtual ~JobStackDataStructure();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des Werts im Objekt.
      @return DataType (siehe DataDictionary.h)
  */
  DataDictionary::DataType getDataType() { return DataDictionary::type_StructVariable; }

  /** Diese Funktion erstellt eine Kopie des Objekts.
      @return Pointer auf neues Objekt.
  */
  virtual JobStackDataStructure *clone();

  /** Diese Funktion liefert den Pointer der DataReference des Objects, falls
      sie auf einen Wert derselben StructDefinition zeigt wie d.
      @param dat Resultat als Pointer auf DataReference
      @param d DataReference zum Vergleichen
      @return True: => Die gewünschte DataReference ist gültig.
  */
  virtual bool getStructureValue( DataReference * &dat, DataReference *d );

  /** Diese Funktion liefert den Wert als string. Falls Der Wert ungültig ist oder
      nicht kompatibel ist, ist das Resultat der Funktion False.
      @param s Wert als string.
      @return True: => Der gewünschte Wert ist gültig.
   */
  virtual bool getStringValue( std::string &s );

  /** Die Funktion liefert einen Pointer auf das DataReference-Objekt.
      @return Pointer auf DataReference
  */
  virtual DataReference *getDataReference();

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
  XferDataItem m_data;

};

#endif
