
#if !defined(__DP_DATA_ALTER_ASSIGN_ELEMENT__)
#define __DP_DATA_ALTER_ASSIGN_ELEMENT__

#include "datapool/DataAlterSetElement.h"

class DataElement;

/** Mit einem DataAlterAssignElement-Objekt wird einem DataElement der Inhalt eines
    anderen DataElements zugewiesen.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterAssignElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor ohne Parameter. Das DataElement wird gelöscht.
   */
  DataAlterAssignElement();

  /** Konstruktor mit dem zu setzenden DataElement-Parameter.
      @param el DataElement mit dem neuen Wert.
   */
  DataAlterAssignElement( const DataElement *el );

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterAssignElement fehlt
   */
  virtual ~DataAlterAssignElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      @param ref Refeerenz auf das DataReference-Objekt
      @return true -> Die Verarbeitung ist nicht möglich.
   */
  virtual bool check( DataReference &ref );

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
  const DataElement *m_element;
};

#endif
/** \file */
