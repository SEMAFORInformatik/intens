
#if !defined(__DP_DATA_ALTER_ERASE_ELEMENT__)
#define __DP_DATA_ALTER_ERASE_ELEMENT__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterEraseElement-Objekt wird genau ein DataElement gelöscht.
    Dieser Funktor funktioniert im Prinzip gleich wie DataAlterClearElement. Nur
    bei Struktur-Elementen werden die Daten der zugehörigen DataItems physisch
    gelöscht.
 */
class DataAlterEraseElement : public DataAlterSetElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \todo Die Beschreibung der Funktion DataAlterEraseElement fehlt
   */
  DataAlterEraseElement();

  /** Destruktor
      \todo Die Beschreibung der Funktion ~DataAlterEraseElement fehlt
   */
  virtual ~DataAlterEraseElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion wird aufgerufen mit dem gesuchten DataElement zum Löschen
      der angehängten Werte.
      @param el Referenz auf das DataElement
      @return Status des durchgeführten Updates.
    */
  virtual UpdateStatus alterData( DataElement &el );

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
