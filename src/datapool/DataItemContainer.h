
#if !defined(__DP_DATA_ITEM_CONTAINER__)
#define __DP_DATA_ITEM_CONTAINER__

#include <vector>
#include <iostream>

#include "datapool/DataGlobals.h"
#include "datapool/DataCompareResult.h"
#include "datapool/DataDebugger.h"
#include "datapool/DataUserAttr.h"

class DataItem;
class DataDictionary;
class DataPool;
class DataTTrail;
class DataReadFunctor;
class PythonObject;

/** Das DataItemContainer-Objekt enthält alle DataItem-Objekte eines DataStruktElements.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataItemContainer : public DataGlobals
{
  friend class DataStructElement;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param parent
      @param dict
      @param update
      \todo Die Beschreibung der Funktion fehlt
   */
  DataItemContainer( DataItem *parent, DataDictionary &dict, bool update );

  /** Copy-Konstruktor
      @param ref
      \todo Die Beschreibung der Funktion fehlt
   */
  DataItemContainer( const DataItemContainer &ref );

  /** Destruktor
   */
  virtual ~DataItemContainer();

  /** Zuweisungsopertor
      \param ref
      \return
      \todo Die Beschreibung der Funktion fehlt
   */
  DataItemContainer& operator=(const DataItemContainer &ref); // Copy-Constructor

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataItemContainer");

  /** Die Funktion liefert einen Pointer auf das gewünschte DataItem-Objekt.
      @param inx Index auf das DataItem
      @return Pointer auf das DataItem
   */
  DataItem *GetItem( int inx ) const;

  /** Die Funktion liefert einen Pointer auf das gewünschte DataItem-Objekt.
      @param name Name des gesuchten DataItems
      @return Pointer auf das DataItem oder null
   */
  DataItem *getItem( const std::string &name ) const;

  /** Die Funktion liefert einen Pointer auf das gewünschte DataItem-Objekt.
      @param dbAttr DbAttr Name des gesuchten DataItems
      @return Pointer auf das DataItem oder null
   */
  DataItem *getItemByDbAttr( const std::string &dbAttr ) const;

  /** Die Funktion liefert eine Referenz auf das gewünschte DataItem-Objekt.
      Das Objekt muss vorhanden sein.
      @param inx Index auf das DataItem
      @return Referenz auf das DataItem
   */
  DataItem &Item( int inx ) const;

  /** Die Funktion platziert den Pointer des DataItem's  am gewünschten Ort.
      @param inx Index auf die gewünschte Position des DataItem's
      @param item Pointer auf das DataItem-Objekt
   */
  void SetItem( int inx, DataItem *item );

  /** Die Funktion teilt mit, ob gültige Datenelemente vorhanden sind.
      \param incl_itemattr true -> ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      \return true => es sind gültige Datenelemente vorhanden
   */
  bool hasValidElements(  bool incl_itemattr ) const;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für für die Prüfung
      \return serialisierbare Elemente sind vorhanden
   */
  virtual bool hasSerializableValidElements( const SerializableMask flags ) const;

  /** DataDictionaryModified
      @param datapool
      @param pThisDict
      @param pDict
      @param AddOrDelete
      @param GlobalFlag
      @param FirstCycle
      @param pFirstCycleItem
   */
  void DataDictionaryModified( DataPool &datapool
                             , DataDictionary *pThisDict
                             , DataDictionary *pDict
                             , bool AddOrDelete
                             , bool GlobalFlag
                             , bool FirstCycle
                             , DataItem **pFirstCycleItem );

  /** Die Funktion löscht einen Cycle im Datapool. Der Container muss der Basiscontainer
      des DataPools sein, sonst ist das Resultat fatal.
      @param ttrail Referenz auf das TTrail-Objekt
   */
  void clearCycle( DataTTrail &ttrail );

  /** Die Funktion weist das DataItemContainer-Objekt cont zu.
      @param cont DataItemContainer-Objekt
      @return Update-Status
   */
  UpdateStatus assignContainer( const DataItemContainer &cont );

  /** Die Funktion weist alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      @param cont Referenz auf den DataItemContainer (Quelle).
      @return Update-Status
   */
  UpdateStatus assignCorrespondingDataItem( const DataItemContainer &cont );

  /** Die Funktion löscht alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      @param dict Referenz auf das DataDictionary-Objekt (Quelle).
      @return Update-Status
   */
  UpdateStatus clearCorrespondingDataItem( const DataDictionary &dict );

  /** Die Funktion schreibt das Element im XML-Format in den Output-Stream.
      @param ostr Output-Stream
      @param attrs String-Liste für die Attribute
      @param level
      @param debug
      @return true -> erfolgreich
   */
  bool writeXML( std::ostream &ostr
               , std::vector<std::string> &attrs
               , int level
               , bool debug );

  /** Die Funktion schreibt das Element im JSON-Format in den Output-Stream.
      \param ostr Output-Stream
      \param level
      \param debug
      \param flags Flags für für die Prüfung von 'serializable'
      \return true -> erfolgreich
   */
  bool writeJSON( std::ostream &ostr
                , int level
                , int indentation
                , const SerializableMask flags );

  /** Die Funktion schreibt die Items im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
   */
  void writeDP( std::ostream &ostr, int i ) const;

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  DataCompareResult compare( const DataItemContainer &cont, int  fall, int max_faelle );

  /** Die Funktion setzt für alle vorhandenen Elemente die entsprechenden Bits in
      der Attribut-Bitmaske.
      @param mask Maske mit den zu setzenden Attribut-Bits
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return true -> Die Bitmaske hat sich verändert
   */
  bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle );

  /** Die Funktion sucht das SortKey-DataItem-Ojekt und liefert den entsprechenen
      Wert im key-Parameter.
      @param key Parameter für den Wert der gefundenen SortKey's
      @return true -> SortKey vorhanden
   */
  bool getSortKey( std::string &key ) const;

  /** Die Funktion liefert eine Pythonobjekt mit allen im Container vorhandenen Daten.
      @return Pointer auf ein PyObject.
   */
  virtual void *getPython();

  /** Die Funktion schreibt alle passenden Werte des Pythonobjekts in den Container.
      @param obj Referenz auf das Pythonobjekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setPython( const PythonObject &obj );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
protected:
  /** Die Funktion liefert die Anzahl der in der Struktur enthaltenen DataItems.
      @return Anzahl DataItems.
  */
  int getNumberOfItems() const;

  /** Die Funktion sucht das zum Parameter-DataItem passende DataItem und liefert
      den Pointer darauf. Falls kein Item passt, wird ein NUll-Pointer geliefert.
      @param dest_item Dataitem-Objekt
      @return Pointer auf das entsprechende Dataitem-Objekt
      */
  const DataItem *getCorrespondingDataItem( const DataItem &dest_item ) const;

  /** getCorrespondingDictionary() wird von getCorrespondingDataItem() als Hilfsfunktion
       verwendet.
       @param src
       @param itm
       @return Pointer auf das passende DataDictionary-Objekt
   */
  DataDictionary *getCorrespondingDictionary( DataDictionary *src, DataDictionary *itm );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /** Diese Funktion löscht alle Werte des Datenelements markiert sie als ungültig. Falls
      bereits alle Elemente ungültig waren, wird dies mit dem UpdateStatus angezeigt.
      \param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      \return Status der durchgeführten Aktion.
   */
  DataItemContainer::UpdateStatus clearContainer( bool incl_itemattr );

  /** Beschreibung der Funktion clearContainerExceptSortKey
      \return
      \todo Die Beschreibung der Funktion clearContainerExceptSortKey fehlt
   */
  void clearContainerExceptSortKey();

  /** Die Funktion initialisiert alle DataItem-Objekte in dieser Struktur. Die
      Attribute bleiben erhalten. Alle Elemente der DataItem-Objekte werden
      gelöscht. Die Attribute der Elemente gehen dabei unweigerlich verloren.
      \return Status der durchgeführten Aktion.
  */
  DataItemContainer::UpdateStatus eraseContainer();

  /** Die Funktion löscht alle Dataitems in der Itemliste und setzt die
      Pointers in der Liste auf 0 (siehe deleteItem() in DataItem).
      @return Status der durchgeführten Aktion.
   */
  DataItemContainer::UpdateStatus deleteContainer();

  /** Die Funktion sorgt für die korrekte Grösse der Item-Liste.
      \param sz gewünschte Grösse
   */
  void resizeItemList( int sz );

  /** Die Funktion füllt die Itemliste mit neuen Dataitems mit Strukturvariablen,
      aber ohne Daten.
      \param dict
      \param update
      \return
      \todo Die Beschreibung der Funktion fehlt
  */
  bool buildItemContainer( DataItem *parent, DataDictionary &dict, bool update );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  typedef std::vector<DataItem *> DataItemList;

  DataItemList  m_itemlist;  // Items in diesem Container

  BUG_DECLARE_COUNT;
};

#endif // !defined(DATAITEMCONTAINER_H__INCLUDED_)
/** \file */
