
#if !defined(__DP_DATA_ALTER_SET_REAL_ELEMENT__)
#define __DP_DATA_ALTER_SET_REAL_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterSetRealElement-Objekt wird einem DataElement ein
    Real-Wert zugewiesen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetRealElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor ohne Parameter. Das DataElement wird gelöscht.
   */
  DataAlterSetRealElement();

  /** Konstruktor mit dem zu setzenden Real-Parameter.
      @param val Neuer Wert für das DataElement.
   */
  DataAlterSetRealElement( double val );

  /** Destruktor
   */
  virtual ~DataAlterSetRealElement();

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
  double   m_value;
};

#endif
/** \file */
