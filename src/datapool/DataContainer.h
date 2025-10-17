
#if !defined(__DP_DATA_CONTAINER__)
#define __DP_DATA_CONTAINER__

#include <vector>

#include "datapool/DataPool.h"
#include "datapool/DataGlobals.h"
#include "datapool/DataCompareResult.h"
#include "datapool/DataUserAttr.h"

class DataItem;
class DataElement;
class DataDimension;
class DataPool;
class DataAccess;
class DataDictionary;
class DataVector;
class DataAlterFunctor;
class DataInx;
class DataIndexList;
class PythonObject;

/** Der DataContainer ist die Basisklasse für DataElement oder DataDimension.
    Diese Objekte enthalten die Werte eines DataItem-Objekts.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataContainer : public DataGlobals
{
/*=============================================================================*/
/* Daten-Definitionen                                                          */
/*=============================================================================*/
public:
  /** Liste von DataContainer-Objekten im DataDimension-Objekt.
   */
  typedef std::vector<DataContainer *> ContainerList;

  /** Liste für die Ausgabe der Grösse von Mehrdimensionalen Dimensionen.
   */
  typedef std::vector<int>   SizeList;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataContainer();

  /** Destruktor
   */
  virtual ~DataContainer();

protected:
  /** Copy-Konstruktor
      \param ref
      \note Der Copy-Konstruktor ist nicht implementiert
   */
  DataContainer( const DataContainer &ref );

  /** Zuweisungsoperator
      \param ref
      \return
      \note Der Zuweisungsoperator ist nicht implementiert
   */
  DataContainer& operator=( const DataContainer &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, um welchen Containertyp es sich handelt.
      @return Datentyp
   */
  virtual ContainerType getContainerType() const = 0;

  /** Die Funktion liefert den this-Pointer als DataElement, falls es sich wirklich
      um ein DataElement-Objekt handelt.
      @return Pointer auf das DataElement-Objekt
   */
  virtual DataElement *getDataElement(){ return 0; }

  /** Die Funktion liefert den this-Pointer als DataElement, falls es sich wirklich
      um ein DataElement-Objekt handelt.
      @return Pointer auf das DataElement-Objekt
   */
  virtual const DataElement *getDataElement() const { return 0; }

  /** Die Funktion liefert den this-Pointer als DataDimension, falls es sich wirklich
      um ein DataDimension-Objekt handelt.
      @return Pointer auf das DataDimension-Objekt
   */
  virtual DataDimension *getDataDimension(){ return 0; }

  /** Die Funktion liefert den this-Pointer als DataDimension, falls es sich wirklich
      um ein DataDimension-Objekt handelt.
      @return Pointer auf das DataDimension-Objekt
   */
  virtual const DataDimension *getDataDimension() const { return 0; }

  /** Beschreibung der Funktion isDataElement
      \return
      \todo Die Beschreibung der Funktion isDataElement fehlt
   */
  virtual bool isDataElement() const { return false; }

  /** Beschreibung der Funktion isDataDimension
      \return
      \todo Die Beschreibung der Funktion isDataDimension fehlt
   */
  virtual bool isDataDimension() const { return false; }

  /** Die Funktion liefert die Anzahl der im Container enthaltenen Elemente,
      unabhängig davon, ob sie gültig sind oder nicht. Ein DataElement
      liefert immer eine 1.
      @return Anzahl der Elemente
   */
  virtual int getNumberOfElements() const = 0;

  /** Die Funktion liefert die Anzahl der Dimensionen im Container.
      @return Anzahl Dimensionen
   */
  virtual int getNumberOfDimensions() const = 0;

  /** Die Funktion liefert die Anzahl der Elemente einer durch den
      Parameter bestimmten Dimension.
      @param dim_num Nummer der gewünschten Dimension
      @return Anzahl der Elemente
   */
  virtual int getDataDimensionSizeOf( int dim_num ) const = 0;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param incl_attr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( SizeList &dimsize, int dim, bool incl_attr ) const = 0;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Es werden dabei die vorhanden Indexlisten verwendet.
      Nur dort, wo Wildcards gesetzt sind, wird die Dimensionsgrösse grösser als 1.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param index aktuelles DataInx-Objekt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, DataInx &index ) const = 0;

  /** Die Funktion liefert die logische grösse der Dimension. Sie funktioniert
      schlanker als GetAllDataDimensionSize().
      @param incl_attr true -> ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return logische grösse der Dimension
   */
  virtual int getValidDimensionSize( bool incl_itemattr=false ) const = 0;

  /** Die Funktion fügt eine neue Dimension im Datenbaum ein.
      @param item DataItem der betroffenen Container.
      @param reserve physische Grösse als Reserve für weitere Einträge
      @return Pointer auf die neu eingefügte DataDimension
   */
   DataContainer *addNewDimension( DataItem &item, int reserve=0 );

  /** Diese Funktion erstellt eine Kopie des Containers und liefert einen
      Pointer darauf.
      @return Pointer auf den neu erstellten Container
   */
   virtual DataContainer *clone() const = 0;

  /** Diese Funktion überträgt die dynamischen Modifikationen des DataDictionary in den
      Datapool.
      @param datapool
      @param pDict
      @param AddOrDelete
      @param GlobalFlag
      @param FirstCycle
      @param pFirstCycleItem
   */
  virtual void DataDictionaryModified( DataPool &datapool
                                     , DataDictionary *pDict
                                     , bool AddOrDelete
                                     , bool GlobalFlag
                                     , bool FirstCycle
                                     , DataItem **pFirstCycleItem ) = 0;

  /** Die Funktion teilt mit, ob gültige Datenelemente vorhanden sind.
      \return true => es sind gültige Datenelemente vorhanden
   */
  virtual bool hasValidElements( bool incl_itemattr ) const = 0;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für die Prüfung von 'serializable'
      \return serialisierbare Elemente sind vorhanden
   */
  virtual bool hasSerializableValidElements( const SerializableMask flags ) const = 0;

  /** Diese Funktion markiert den Container als Updated. Der Daten-Timestamp
      des Containers wird neu gesetzt. Gleichzeitig wird auch der Timestamp
      des zugehoerigen DataItem's gesetzt.
      @param item zugehoeriges Item
   */
  void markDataContainerDataUpdated( DataItem &item );

  /** Diese Funktion markiert ein Objekt als Updated. Der Werte-Timestamp des
      Conainers wird neu gesetzt. Gleichzeitig wird auch der Timestamp
      des zugehoerigen DataItem's gesetzt.
      @param item zugehoeriges Item
   */
   void markDataContainerValueUpdated( DataItem &item );

  /** Diese Funktion markiert den Container als 'updated', indem der aktuelle
      Daten-Timestamp als Update-Timestamp gesetzt wird.
      @param status Status des durchgeführten Updates
   */
   void markDataContainerUpdated( UpdateStatus status );

  /** Diese Funktion markiert den Container als 'updated', indem der aktuelle
      Daten-Timestamp als Update-Timestamp gesetzt wird.
      @param yes false => Eine Markierung wird rückgängig gemacht
   */
   void markDataContainerDataUpdated( bool yes=true );

  /** Diese Funktion markiert den Container als 'updated', indem der aktuelle
      Werte-Timestamp als Update-Timestamp gesetzt wird.
      @param yes false => Eine Markierung wird rückgängig gemacht
   */
   void markDataContainerValueUpdated( bool yes=true );

  /** Die Funktion zeigt an, ob der Container bezüglich der als Parameter übergebenen
      Transaktionsnummer veraendert worden ist.
      @param t Transaktionsnumer, gegen die geprüft werden soll.
  */
   bool isDataContainerDataUpdated( TransactionNumber t ) const;

  /** Die Funktion zeigt an, ob Werte des Containers bezüglich der als Parameter
      übergebenen Transaktionsnummer veraendert worden ist.
      @param t Transaktionsnumer, gegen die geprüft werden soll.
  */
   bool isDataContainerValueUpdated( TransactionNumber t ) const;

   /** Die Funktion liefert die TransactionNumber des DataContainers.
    */
   TransactionNumber getDataContainerDataUpdated() const;

   /** Die Funktion liefert die TransactionNumber für den Werte des DataContainers.
    */
   TransactionNumber getDataContainerValueUpdated() const;

   /** Die Funktion setzt die TransactionNumber des DataContainers wegen einer
       Änderung der Daten.
       @param t Zu setzende Transaktionsnumer
    */
   void setDataContainerDataUpdated( TransactionNumber t );

   /** Die Funktion setzt die TransactionNumber des DataContainers wegen einer
       Änderung der Werte.
       @param t Zu setzende Transaktionsnumer
    */
   void setDataContainerValueUpdated( TransactionNumber t );

  /** Die Funktion löscht alle Werte im Struktur-Baum. Alle vorhanden DataItem's
      und ihre Attribute bleiben erhalten.
      @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      @return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus clearContainer( bool incl_itemattr ) = 0;

  /** Die Funktion löscht alle Werte im Struktur-Baum ausser das eventuell vorhandene
      SortKey-DataItem. Die Funktion wird nur vom Compare-Feature verwendet beim
      erstellen der Vergleichsstruktur.
      Alle vorhanden DataItem's und ihre Attribute bleiben erhalten.
  */
  virtual void clearContainerExceptSortKey(){}

  /** Die Funktion löscht alle Werte, Strukturen und DataItem's im Struktur-Baum.
      Die Attribute der DataItem's gehen dabei verloren.
      @return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus eraseContainer() = 0;

  /** Die Funktion erstellt eine exakte Kopie des Containers und liefert den
      Pointer darauf.
      @param datapool Referenz auf den DataPool
      @return Pointer auf neuen Container
  */
  virtual DataContainer *copyContainer( DataPool &datapool ) const = 0;

  /** Die Funktion schreibt den Inhalt des Containers im XML-Format
      in den Outputstream ostr.
      @param ostr
      @param attrs
      @param is_structitem
      @param level
      @param debug
      @param isScalar
   */
  virtual bool writeXML( std::ostream &ostr
                       , std::vector<std::string> &attrs
                       , bool is_structitem
                       , int level
                       , bool debug
                       , bool isScalar ) = 0;

  /** Die Funktion schreibt den Inhalt des Containers im JSON-Format
      in den Outputstream ostr.
      \param ostr
      \param is_structitem
      \param level
      \param indentation
      \param isScalar
      \param scale
      \param flags Flags für die Prüfung von 'serializable'
   */
  virtual bool writeJSON( std::ostream &ostr,
                          bool is_structitem,
                          int level,
                          int indentation,
                          bool isScalar,
                          double scale,
                          const SerializableMask flags ) = 0;

  /** Die Funktion schreibt die Elemente des Containers im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
      @param index Index in einem Dimensionsvektor
   */
  virtual void writeDP( std::ostream &ostr, int i, int index ) const = 0;

  /** Die Funktion ruft für ein oder mehrere indizierte Datenelemente (mit
      Wildcards) den Functor auf um die gewünschten Aktionen auszuführen.
      @param item Referenz auf das DataItem
      @param func Referenz auf den Functor für die Aktion
      @param inx  Referenz auf Index-Objekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataItem &item
                                , DataAlterFunctor &func
                                , DataInx &inx ) = 0;

  /** Die Funktion liefert die Reference auf das indizierte DataElement. Falls
      noch nicht vohanden, wird das Element angelegt.
      @param item Referenz auf das DataItem
      @param inxlist Indexliste
      @param index aktuelle Indexnummer
      @return Referenz auf das DataElement
  */
  virtual DataElement &getDataElement( DataItem &item,
                                       DataIndexList &inxlist,
                                       int index ) = 0;

  /** Die Funktion liefert die Reference auf das letzte indizierte DataContainer-Objekt.
      Falls noch nicht vohanden, wird das Objekt angelegt.
      @param item Referenz auf das DataItem
      @param inxlist Indexliste
      @param index aktuelle Indexnummer
      @param sz benötigte Grösse auf dem letzten Index
      @return Referenz auf das DataContainer-Objekt
  */
  virtual DataContainer &getDataContainer( DataItem &item,
                                           DataIndexList &inxlist,
                                           int index,
                                           int sz ) = 0;

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  virtual DataCompareResult compare( const DataContainer &cont, int fall, int max_faelle ) = 0;

  /** Die Funktion setzt für alle vorhandenen Elemente in der Struktur die entsprechenden
      Bits in der Attribut-Bitmaske.
      @param mask Maske mit den zu setzenden Attribut-Bits
      @return true -> Die Bitmaske hat sich verändert
   */
  virtual bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle ) = 0;

  /** Die DataDimension wird markiert als Spezialfall für das Darstellen von Vergleichen
      zwischen Strukturen mit dem Compare-Feature.
   */
  virtual void setCompareDimension() = 0;

  /** Die Funktion zeigt an, ob es sich um eine 'Compare'-DataDimension handelt.
      ( siehe setCompareDimension() )
      @return true -> Es ist eine 'Compare'-DataDimension
   */
  virtual bool isCompareDimension() const = 0;

  /** Die Funktion liefert eine Pythonobjekt mit allen im Container vorhandenen Daten.
      @return Pointer auf ein PyObject.
   */
  virtual void *getPython() = 0;

  /** Die Funktion schreibt alle passenden Werte des Pythonobjekts in den Container.
      @param obj Pointer auf das Pythonobjekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setPython( const PythonObject &obj ) = 0;

  virtual UpdateStatus setPython( const PythonObject &obj, DataItem &item, int dims );

/*=============================================================================*/
/* member variables                                                            */
/*=============================================================================*/
private:
  TransactionNumber m_DataLastUpdated;   // Timestamp des letzten Updates der Daten inkl. Attribute.
  TransactionNumber m_ValueLastUpdated;  // Timestamp des letzten Updates der Werte ohne Attribute.

};

#endif
/** \file */
