
#if !defined(__DP_DATA_ALTER_SET_STYLESHEET__)
#define __DP_DATA_ALTER_SET_STYLESHEET__

#include "datapool/DataAlterSetElement.h"

/** Mit einem DataAlterSetStylesheet-Objekt wird den referenzierten DataElements
    der gewünschte Stylesheet gesetzt. Es sind Wildcards an beliebiger Stelle möglich.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterSetStylesheet : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      \param t Stylesheet für die referenzierten DataElements
      \param status Art des Stylesheet-Updates
   */
  DataAlterSetStylesheet(const std::string& stylesheet);

  /** Destruktor
   */
  virtual ~DataAlterSetStylesheet();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion prüft, ob die gewünschte Bearbeitung durchgeführt werden kann.
      Wildcards sind erlaubt. Nur ohne Wildcards wird bei Bedarf ein neues
      DataElement erstellt.
      @param ref Referenz auf das DataReference-Objekt
      @return true -> Die Verarbeitung ist nicht möglich.
   */
  virtual bool check( DataReference &ref );

  /** Für jedes DataItem auf dem Weg zum gewünschten DataElement wird diese Funktion
      aufgerufen.
      \param item Referenz auf das DataItem
      \param status Rückgabewert der vorgenommenen Mutation am DataItem-Objekt
      \param inx Referenz auf das Indexobjekt
      \param final true Das DataItem ist das physisch letzte vorhandene DataItem
             Im Datenbaum. Das gesuchte DataItem resp. DataElement ist (noch) nicht vorhanden.
      \retval false Die Verarbeitung wird abgebrochen.
      \retval true  Die Verarbeitung soll weitergehen.
   */
  virtual bool alterItem( DataItem &item, UpdateStatus &status, DataInx &inx, bool final );

  /** Diese Funktion setzt anhand des im Konstruktor gesetzten Status den gewünschten
      Timestamp des DataElements.
      \param el Referenz auf das DataElement
      \return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Die Funktion zeigt an, dass im Datapool die betroffenen DataItem. als 'updated'
      markiert werden müssen.
      \return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return true; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      \return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return m_create; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  const std::string  m_stylesheet;
  bool               m_create;
};

#endif
/** \file
    Die Datei enthält die Definition der Klasse <b>DataAlterSetStylesheet</b>.
*/
