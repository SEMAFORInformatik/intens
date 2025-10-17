
#if !defined(__DP_DATA_ALTER_SET_INTEGER_ELEMENT__)
#define __DP_DATA_ALTER_SET_INTEGER_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterSetIntegerElement-Objekt wird einem DataElement ein
    Integer-Wert zugewiesen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetIntegerElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor ohne Parameter. Das DataElement wird gelöscht.
   */
  DataAlterSetIntegerElement();

  /** Konstruktor mit dem zu setzenden Integer-Parameter.
      @param val Neuer Wert für das DataElement.
   */
  DataAlterSetIntegerElement( int val );

  /** Destruktor
   */
  virtual ~DataAlterSetIntegerElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion wird aufgerufen zum Setzen des neuen Werts.
      @param el Referenz auf des DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setValue( DataElement &el );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int   m_value;
};

#endif
/** \file */
