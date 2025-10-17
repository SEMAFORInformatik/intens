
#if !defined(__DP_DATA_ALTER_CLEAR_ELEMENT__)
#define __DP_DATA_ALTER_CLEAR_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterClearElement-Objekt wird genau ein DataElement ungültig
    gemacht.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterClearElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \todo Die Beschreibung der Funktion DataAlterClearElement fehlt
   */
  DataAlterClearElement();

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterClearElement fehlt
   */
  virtual ~DataAlterClearElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion zeigt an, dass nie ein neues Element angelegt werden muss,
      da nicht vorhandene Elemente nicht gelöscht werden müssen.
      @return false -> Neues Element nie anlegen
   */
  virtual bool createElements() const { return false; }

  /** Die Funktion zeigt an, ob führende Indizes mit Wert 0 in einer Indexliste
      ignoriert werden sollen.
   */
  virtual bool ignoreLeadingZeroIndices() const { return true; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion sollte nicht aufgerufen werden, da die Arbeit von alterData()
      bereits erledigt wird.
      @param el Referenz auf des DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setValue( DataElement &el );

};

#endif
/** \file */
