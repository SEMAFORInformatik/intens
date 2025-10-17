
#if !defined(__DP_DATAITEM__)
#define __DP_DATAITEM__

#include <map>

#include "datapool/DataDebugger.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataPool.h"
#include "datapool/DataContainer.h"
#include "datapool/DataGlobals.h"
#include "datapool/DataAttributes.h"
#include "datapool/DataCompareResult.h"

class DataContainer;
class DataElement;
class DataDimension;
class DataItemContainer;
class DataVector;
class DataPool;
class DataItemAttr;
class DataAlterFunctor;
class DataInx;
class Separator;

/** Datatem

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataItem : public DataGlobals
               , public DataAttributes
{
  friend class DataPool;
  friend class DataDimension;
  friend class DataItemContainer;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param parent Parent-DataItem in einer Struktur
      @param dict Pointer auf das passende DataDictionary-Objekt
      @param update das neue DataItem wird als 'FullUpdated' markiert.
   */
  DataItem( DataItem *parent, DataDictionary *dict, bool update );

  /** Destruktor
   */
  virtual ~DataItem();

private:
  /** Copy-Konstruktor
      \param ref
      \note Der Copy-Konstruktor ist nicht implementiert
   */
  DataItem( const DataItem &ref );

  /** Zuweisungsoperator
      \param ref
      \return
      \note Der Zuweisungsoperator ist nicht implementiert
   */
  DataItem& operator=( const DataItem &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataItem");

  /** Die Funktion liefert einen Pointer auf das dem DataItem entsprechende
      DataDictionary-Objekt.
      @return Pointer auf das DataDictionary-Objekt
   */
  DataDictionary *getDictionary() const;

  /** Die Funktion liefert eine Referenz auf das dem DataItem entsprechende
      DataDictionary-Objekt. Das Dictionary-Objekt muss vorhanden sein.
      @return Referenz auf das Dictionary-Objekt
   */
  DataDictionary &dictionary() const;

  /** Die Funktion liefert eine Referenz auf das DataPool-Objekt.
      @return Referenz auf den DataPool.
   */
  DataPool &datapool() const;

  /** Die Funktion liefert den Namen des Items.
      @return String mit dem Namen
   */
  std::string getName() const;

  /** Die Funktion liefert den DbAttr Namen des Items.
      @return String mit dem DbAttr Namen
   */
  std::string getDbAttr() const;

  /** Die Funktion liefert den DbUnit Namen des Items.
      @return String mit dem DbUnit Namen
   */
  std::string getDbUnit() const;

  /** Die Funtion liefert den Typ des DataItems.
      @return Datentyp
   */
  DataDictionary::DataType getDataType() const;

  /** Die Funktion liefert einen Pointer auf das DataUser-Attribute-Objekt
      im DataDictionary des DataItems.
      @return Pointer auf das DataUserAttr-Objekt
  */
  const DataUserAttr *getDataUserAttr() const;

  /** Die Funktion teilt mit, ab es sich um ein Item mit genau einem Wert handelt.
      @return true -> DataItem ist als skalares Item markiert.
   */
  bool isScalar() const;

  /** Die Funktion liefert einen Pointer auf das DataItem-Attribute-Objekt.
      Falls dieses noch nicht vorhanden ist, wird es auf Wunsch sofort erstellt.
      @param createAttrIfNotThere true -> es wird ein Objekt erstellt, falls nötig
      @return Pointer auf das DataItemAttr-Objekt
   */
  DataItemAttr *getDataItemAttr( bool createAttrIfNotThere = false );

  /** Die Funktion teilt mit, ob ein Item-Attribute-Objekt vorhanden ist.
      @return true -> Es ist ein DataItemAttr-Objekt vorhanden
   */
  bool hasItemAttr() const;

  /** Die Funktion liefert eine Referenz auf das DataItem-Attribute-Objekt.
      Falls dieses noch nicht vorhanden ist, wird das Programm beendet.
      @return Referenz auf das DataItemAttr-Objekt
   */
  const DataItemAttr &dataItemAttr() const;

  /** Die Funktion liefert eine Referenz auf das DataItem-Attribute-Objekt.
      Falls dieses noch nicht vorhanden ist, wird das Programm beendet.
      @return Referenz auf das DataItemAttr-Objekt
   */
  DataItemAttr &dataItemAttr();

   /** Die Funktion zeigt an, ob gültige Attribute im DataItem-Attribute-Objekt
       vorhanden sind.
      @return true => Attribute sind vorhanden
   */
  bool hasValidDataItemAttr() const;

  /** Die Funktion löscht Attribute im DataItem-Attribute-Objekt.
      Falls dieses nicht vorhanden ist, geschieht nichts.
      @return true => es wurde etwas gelöscht
   */
  bool clearDataItemAttr();

  /** Dies Funktion loescht ein DataItem wenn der Referenz-Count 0 ist. Der Count
      wird um 1 reduziert. Dies ist NUR bei globalen DataItems der Fall.
      Der Destruktor ist private. Das Löschen eines DataItems ist nur mit dieser
      Funktion möglich.
   */
  static DataItem *deleteItem( DataItem * pItem );

  /** Die Funktion initialisiert das DataItem neu und liefert den Pointer zurück.
      Bei einem 'Global' oder 'Protected' Item geschieht nichts.
      @return Pointer auf das DataItem-Objekt
   */
  static DataItem *eraseItem( DataItem * pItem );

  /** Die Funktion dupliziert das DataItem bedingungslos und übergibt den
      den Pointer auf das Duplikat. Diese Funktion wird vom TTrail benötigt.
   */
  DataItem *duplicateItem();

  /** Die Funktion erstellt einen Clone eines DataItems mit all seinen
      angehaengten Items.
      @return Pointer auf das neu erstellte DataItem-Objekt.
   */
  DataItem *cloneItem();

  /** Diese Funktion übernimmt alle Daten aus dem übergebenen DataItem item und
      überlässt diesem die eigenen Daten. Die Funktion wird vom TTrail benötigt
      um bei abgebrochenen Transaktionen oder bei einem Undo oder Redo den alten
      Zustand des DataItem's wieder herzustellen.
      @param item Referenz auf das DataItem mit den zu übernehmenden Daten
   */
  void restoreItem( DataItem &item );

  /** Diese Funktion überträgt die dynamischen Modifikationen des DataDictionary in den
      Datapool.
      @param pDict
      @param AddOrDelete
      @param GlobalFlag
      @param FirstCycle
      @param pFirstCycleItem
   */
  void DataDictionaryModified( DataDictionary *pDict
                             , bool AddOrDelete
                             , bool GlobalFlag
                             , bool FirstCycle
                             , DataItem **pFirstCycleItem );

  /** Die Funktion liefert die Anzahl der physisch vorhandenen Dimensionen.
      @return Anzahl Dimensionen
  */
  int getNumberOfDimensions() const;

  /** Die Funktion liefert die Anzahl der Elemente einer durch den
      Parameter bestimmten Dimension.
      @param dim_num Nummer der gewünschten Dimension
      @return Anzahl der Elemente
   */
  int getDataDimensionSizeOf( int dim_num ) const;

  /** Die Funktion gibt die Anzahl Dimension und ihre logischen Grössen zurück.
      @param dimsize logische Grössen der einzelnen Dimensionen
      @return Anzahl Dimensionen in dimsize.
      @param incl_itemattr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
   */
  int getAllDataDimensionSize( DataContainer::SizeList &dimsize, bool incl_itemattr=false ) const;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Es werden dabei die vorhanden Indexlisten verwendet.
      Nur dort, wo Wildcards gesetzt sind, wird die Dimensionsgrösse grösser als 1.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param index aktuelles DataInx-Objekt
      @return Anzahl Dimensionen in dimsize.
   */
  int getAllDataDimensionSize_PreInx( DataContainer::SizeList &dimsize, DataInx &index ) const;

  /** Diese Funktion markiert ein Item als 'updated' indem der aktuelle Timestamp
      als Update-Timestamp gesetzt wird.
      @param status Status des durchgeführten Updates
   */
  void markDataItemUpdated( UpdateStatus status );

  /** Die Funktion initialisiert die Timestamps mit 0.
      Die Funktion wird nur nach einem ClearCycle() eingesetzt.
   */
  void clearDataItemUpdated();

  /** Diese Funktion markiert ein Item als 'updated' indem der aktuelle Timestamp
      als Update-Timestamp gesetzt wird.
      @param yes false => Eine Markierung wird rückgängig gemacht
   */
  void markDataItemDataUpdated( bool yes=true );

  /** Diese Funktion markiert ein Item als 'updated' indem der aktuelle Timestamp
      als Update-Timestamp gesetzt wird.
      @param yes false => Eine Markierung wird rückgängig gemacht
   */
  void markDataItemValueUpdated( bool yes=true );

  /** Die Funktion zeigt an, ob das Item bezüglich der als Parameter übergebenen
      Transaktionsnummer veraendert worden ist.
      @param t Transaktionsnumer, gegen die geprüft werden soll.
  */
  bool isDataItemDataUpdated( TransactionNumber t ) const;

  /** Die Funktion zeigt an, ob Werte des Items bezüglich der als Parameter
      übergebenen Transaktionsnummer veraendert worden ist.
      @param t Transaktionsnumer, gegen die geprüft werden soll.
  */
  bool isDataItemValueUpdated( TransactionNumber t ) const;

  /** Die Funktion prueft, ob ein DataItem relativ zu einer gegebenen Transaktionsnummer
      durch ein erase() oder assign() veraendert worden ist.
   */
  bool isDataItemFullUpdated( TransactionNumber t ) const;

  /** Die Funktion prueft, ob das DataItem relativ zu einer gegebenen Transaktionsnummer
      auf der Benutzeroberfläche (GUI) einen Update erhalten muss. (siehe auch
      getDataItemGuiUpdated() ).
      \note Für das Bestimmen der Notwendigkeit eines GUI-Updates diese Funktion verwenden.
      \retval false Keine Veränderung.
      \retval true  Ein GUI-Update ist notwendig.
   */
  bool isDataItemGuiUpdated( TransactionNumber t ) const;

  /** Die Funktion setzt die TransactionNumber des Dataitems wegen Änderungen an den Daten.
      @param t Transactionsnummer
   */
  void setDataItemDataUpdated( TransactionNumber t );

  /** Die Funktion setzt die TransactionNumber des Dataitems wegen Änderungen an den Werten.
      @param t Transactionsnummer
   */
  void setDataItemValueUpdated( TransactionNumber t );

  /** Die Funktion setzt die TransactionNumber des Dataitems wegen einem clear() der Elemente.
      @param t Transactionsnummer
   */
  void setDataItemFullUpdated( TransactionNumber t );

  /** Die Funktion liefert die TransactionNumber der letzten Datenänderung des Dataitems.
      @return letzte Daten-Transaktionsnummer des Items
   */
  TransactionNumber getDataItemDataUpdated() const;

  /** Die Funktion liefert die TransactionNumber der letzten Wertänderung des Dataitems.
      @return letzte Werte-Transaktionsnummer des Items
   */
  TransactionNumber getDataItemValueUpdated() const;

  /** Die Funktion liefert die TransactionNumber des letzten erase() oder
      assign() des Dataitems.
   */
  TransactionNumber getDataItemFullUpdated() const;

  /** Die Funktion liefert die TransactionNumber der letzten Datenänderung des Dataitems
      welcher für die Benutzeroberfläche (GUI) relevant ist. Es wird dabei zusätzlich
      der Timestamp des letzten Rollbacks im TTrail berücksichtigt.
      \note Für das Bestimmen der Notwendigkeit eines GUI-Updates diese Funktion verwenden.
      \return Transaktionsnummer der letzten GUI-relevanten Änderung.
   */
  TransactionNumber getDataItemGuiUpdated() const;

  /** Die Funktion setzt den Rollback-Timestamp.
   */
  void markDataItemRollback();

   /** Diese Funktion liefert den Timestamp des letzten TTrail Rollbacks
       auf diesem Item.
   */
  TransactionNumber getLastRollbackTimestamp() const;

  /** Die Funktion macht ein "deep-Copy" eines DataItems. Die
      gesamte Dimensionsmatrix wird dabei kopiert.
      @param source Das zu kopierende DataItem
      @return Status der durchgeführten Aktion.
   */
  UpdateStatus assignDataItem( const DataItem *source );

  /** Diese Funktion schreibt das DataItem mit allen Werten in XML-Format
      in den ostream.
   */
  bool writeXML( std::ostream &ostr
               , std::vector<std::string> &attrs
               , int level
               , bool debug );

  /** Diese Funktion schreibt das DataItem mit allen Werten in JSON-Format
      den ostream.
      \param ostr
      \param level
      \param indentation
      \param sep
      \param flags Flags für für die Prüfung von 'serializable'
      \return true -> erfolgreich
   */
  bool writeJSON( std::ostream &ostr,
                  int level,
                  int indentation,
                  Separator &sep,
                  const SerializableMask flags );

  /** Die Funktion schreibt das Element im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
   */
  virtual void writeDP( std::ostream &ostr, int i ) const;

  /** Die Funktion teilt mit, ob gültige Datenelemente vorhanden sind.
      \return true => es hat gültige Datenelemente
      \deprecated in Zukunft nur noch hasValidElements() verwenden.
   */
  bool isValid() const;

  /** Die Funktion teilt mit, ob gültige Datenelemente vorhanden sind.
      \param incl_itemattr true -> ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      \return true => es hat gültige Datenelemente
   */
  bool hasValidElements( bool incl_itemattr ) const;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für für die Prüfung
      \return serialisierbare Elemente sind vorhanden
   */
  bool hasSerializableValidElements( const SerializableMask flags ) const;

  /** Die Funktion liefert die Attribute des Dataitems. Wenn Ein Rollback oder FullUpdate
      nach dem Timestamp t stattgefunden hat, wir in der Bitmask das DataUpdated-Bit gesetzt
      \param t Timestamp
      \return Bitmask mit Attributen
   */
  DATAAttributeMask getAttributes( TransactionNumber t );

  /** Die Funktion liefert die Attribute des Dataitems.
      \return Bitmask mit Attributen
   */
  DATAAttributeMask getAttributes();

  /** Die Funktion löscht alle Werte im Struktur-Baum. Alle vorhanden DataItem's
      und ihre Attribute bleiben erhalten.
      @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      @return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus clearContainer( bool incl_itemattr=false );

  /** Die Funktion löscht alle Werte, Strukturen und DataItem's im Struktur-Baum.
      Die Attribute der DataItem's gehen dabei verloren.
      @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      @return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus eraseContainer( bool incl_itemattr=false );

  /** Die Funktion ruft für ein oder mehrere indizierte Datenelemente (mit
      Wildcards) den Functor auf um die gewünschten Aktionen auszuführen.
      @param func Referenz auf den Functor für die Aktion
      @param inx  Referenz auf Index-Objekt
      @return Status des durchgeführten Updates.
   */
  UpdateStatus alterData( DataAlterFunctor &func, DataInx &inx );

  /** Die Funktion liefert die Reference auf das indizierte DataElement. Falls
      noch nicht vohanden, wird das Element angelegt.
      @param inxlist Indexliste
      @return Referenz auf das DataElement
  */
  DataElement &getDataElement( DataIndexList &inxlist );

  /** Die Funktion liefert die Reference auf das letzte indizierte DataContainer-Objekt.
      Falls noch nicht vohanden, wird das Objekt angelegt.
      @param inxlist Indexliste
      @param sz benötigte Grösse auf dem letzten Index
      @return Referenz auf das DataContainer-Objekt
  */
  DataContainer &getDataContainer( DataIndexList &inxlist, int sz );

  /** Das Resultat-DataItem wird für die folgenden Vergleiche vorbereitet.
      @param el_left Mit dem ersten (linken) Element wird das Result-Item aufgebaut.
   */
  void prepareCompareResult( const DataElement &el_left );

  /** Die Funktion vergleicht die Werte zweier DataElement-Objekte und manipuliert
      bei Bedarf die Daten des eigenen Objekts.
      @param el_right Reference auf das rechte zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  DataCompareResult compare( const DataElement &el_right, int fall, int max_faelle );

  /** Die Funktion vergleicht die Werte zweier DataItem-Objekte und manipuliert
      bei Bedarf die Daten des eigenen Objekts.
      @param item Reference auf das rechte zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @return DataCompareResult-Objekt
   */
  DataCompareResult compare( const DataItem &item, int fall, int max_faelle );

  /** Beschreibung der Funktion setCompareAttributesOfAllElements
      \param mask
      \param fall
      \param max_faelle
      \return
      \todo Die Beschreibung der Funktion setCompareAttributesOfAllElements fehlt
   */
  bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle );

  /** Beschreibung der Funktion isSortKey
      \return
      \todo Die Beschreibung der Funktion isSortKey fehlt
   */
  bool isSortKey() const;

  /** Beschreibung der Funktion getSortKey
      \param key
      \return
      \todo Die Beschreibung der Funktion getSortKey fehlt
   */
  bool getSortKey( std::string &key ) const;

  /** Die Funktion liefert eine Pythonobjekt mit allen im Container vorhandenen Daten.
      @return Pointer auf ein PyObject.
   */
  void *getPython();

  /** Die Funktion schreibt alle passenden Werte des Pythonobjekts in den Container.
      @param obj Referenz auf das Pythonobjekt
      @return Status des durchgeführten Updates.
   */
  UpdateStatus setPython( const PythonObject &obj );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Die Funktion liefert eine Referenz auf den Datencontainer. Dies kann eine
      DataDimension oder ein einzelnes DataElement sein. Es muss ein Container vorhanden sein.
      @return Referenz auf das DataContainer-Objekt
  */
  DataContainer &container();

  /** Die Funktion liefert eine Referenz auf den Datencontainer. Dies kann eine
      DataDimension oder ein einzelnes DataElement sein. Es muss ein Container vorhanden sein.
      @return Referenz auf das DataContainer-Objekt
  */
  const DataContainer &container() const;

  /** Die Funktion liefert mit einem cast eine Referenz auf den Datencontainer
      als DataElement. Es muss ein Container vorhanden sein und es muss ein
      DataElement sein.
      @return Referenz auf das DataElement-Objekt
  */
  DataElement &element();

  /** Die Funktion liefert mit einem cast eine Referenz auf den Datencontainer
      als DataDimension. Es muss ein Container vorhanden sein und es muss ein
      DataDimension sein.
      @return Referenz auf das DataDimension-Objekt
  */
  DataDimension &dimension();

  /** Die Funktion ersetzt den alten Container durch einen neuen Container.
      Der alte Container wird auf Wunsch (die Regel) gelöscht.
      @param c Pointer auf den neuen DataContainer
      @param delete_old true => Der alte Container wird gelöscht
  */
  void setContainer( DataContainer *c, bool delete_old=true );

  /** Die Funktion teilt mit, ob Datencontainer vorhanden sind.
      @return true => Es sind Container vorhanden
   */
  bool hasContainer() const;

  /** Die Funktion löscht den Datencontainer und damit alle enthaltenen
      Daten über alle Dimensionen und Strukturen.
      @return Status der durchgeführten Aktion.
  */
  UpdateStatus deleteContainer();

  /** Die Funktion passt den DataContainer des DataItems demjenigen des Parameters in der
      Grösse und Ausdehnung an. Der DataContainer wird jedoch nicht verkleinert.
      @param item Referenz auf das massgebende DataItem
   */
  void resizeContainer( const DataItem &item );

  /** Die Funktion erzeugt ein neues DataElement mit allen assoziierten Elementen (Strukturen).
      Das bedeutet, dass alle Elemente rekursiv erstellt werden, jedoch ausschliesslich das
      Element mit dem Index 0.
      @param update true -> neu erstellte Container werden als 'updated' markiert.
      @return Pointer auf das neu erstellte DataContainer-Objekt
  */
  DataContainer *newContainer( bool update=true );

  /** Die Funktion installiert ein neues DataItemAttr.
   */
  void createItemAttr();

  /** Die Funktion sorgt dafür, dass mindestens die gewünschte Anzahl
      Dimensionen vorhanden sind.
      @param dims Anzahl der benötigten Dimensionen
   */
  void createDimensions( int dims );

  /** Diese Funktion markiert ein Item als Full-Updated. Das heisst, dass bei einer
      Mutation die Ausdehnung der Dimensionen reduziert wurde. Dies ist z.B. beim
      Löschen aller Datenelemente oder bei einem Assign der Fall.
   */
  void markDataItemFullUpdated();

  /** Beschreibung der Funktion setCompareAttribute
      \param mask
      \return
      \todo Die Beschreibung der Funktion setCompareAttribute fehlt
   */
  void setCompareAttribute( DATAAttributeMask mask );

  /** Beschreibung der Funktion getSortKey
      \param cont
      \param key
      \return
      \todo Die Beschreibung der Funktion getSortKey fehlt
   */
  bool getSortKey( const DataContainer *cont, std::string &key ) const;

  /** Beschreibung der Funktion complete_head
      \param dimsize
      \param wildcards
      \param val
      \return
      \todo Die Beschreibung der Funktion complete_head fehlt
   */
  void complete_head( DataContainer::SizeList &dimsize, int wildcards, int val ) const;

  ///
  int getPythonDimensions( const PythonObject &obj );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int              m_ReferenceCounter;  // Anzahl "deletefaehige" Referenzen auf dieses Item.
                                        // Eine derartige Referenz entsteht durch den cycle-
                                        // switch. Bei globalen Items wird dieses naemlich
                                        // NICHT dupliziert.
  DataDictionary   &m_dict;             // Dictionary-Eintrag des Items
  DataContainer    *m_container;        // Pointer auf DataContainer
  TransactionNumber m_DataLastUpdated;  // Timestamp des letzten Updates der Daten inkl. Attribute.
  TransactionNumber m_ValueLastUpdated; // Timestamp des letzten Updates der Werte ohne Attribute.

  // --------------------------------------------------------------------------
  // Nach Mutationen, bei denen ein DataItem komplett umgestellt wird wie bei
  // einem erase oder assign, weiss der Datapool bei Elementen, die physisch
  // nicht mehr vorhanden sind, nicht mehr, wann sie verändert wurden. In einem
  // solchen Fall wird mit dieser TransactionNumber verglichen.
  // --------------------------------------------------------------------------
  TransactionNumber m_FullUpdated;

  // --------------------------------------------------------------------------
  // Wir merken uns immer, wann auf diesem Item der letzte TTrail-Rollback
  // durchgeführt wurde. Das Gui hat unter Umständen keine Möglichkeit, seine
  // Inhalte mit dem Datapool abzustimmen.
  // --------------------------------------------------------------------------
  TransactionNumber m_LastRollback;

  DataItemAttr     *m_pDataItemAttr;    // Pointer auf DataItemAttr

  BUG_DECLARE_COUNT;
};

#endif // !defined(DATAITEM_H__INCLUDED_)
/** \file */
