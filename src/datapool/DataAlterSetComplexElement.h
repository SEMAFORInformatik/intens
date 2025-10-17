
#if !defined(__DP_DATA_ALTER_SET_COMPLEX_ELEMENT__)
#define __DP_DATA_ALTER_SET_COMPLEX_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterSetComplexElement-Objekt wird einem DataElement eine
    Complexe Zahl als Wert zugewiesen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetComplexElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor ohne Parameter. Das DataElement wird gelöscht.
   */
  DataAlterSetComplexElement();

  /** Konstruktor mit dem zu setzenden Integer-Parameter.
      @param val Neuer Wert für das DataElement.
   */
  DataAlterSetComplexElement( dComplex val );

  /** Destruktor
   */
  virtual ~DataAlterSetComplexElement();

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
  dComplex   m_value;
};

#endif
/** \file */
