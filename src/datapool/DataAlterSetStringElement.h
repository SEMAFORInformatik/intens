
#if !defined(__DP_DATA_ALTER_SET_STRING_ELEMENT__)
#define __DP_DATA_ALTER_SET_STRING_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterSetStringElement-Objekt wird einem DataElement ein
    String-Wert zugewiesen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetStringElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor ohne Parameter. Das DataElement wird gelöscht.
   */
  DataAlterSetStringElement();

  /** Konstruktor mit dem zu setzenden Integer-Parameter.
      @param val Neuer Wert für das DataElement.
   */
  DataAlterSetStringElement( const std::string &val );

  /** Destruktor
   */
  virtual ~DataAlterSetStringElement();

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
  const std::string m_value;
};

#endif
/** \file */
