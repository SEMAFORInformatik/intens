
#if !defined(__DP_DATA_STRUCT_ELEMENT__)
#define __DP_DATA_STRUCT_ELEMENT__

#include "datapool/DataElement.h"
#include "datapool/DataItemContainer.h"


class DataDictionary;
class DataDimension;
class DataAlterFunctor;
class DataDiff;

/** Das DataStructElement-Objekt ist der DatenContainer für eine Struktur.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataStructElement : public DataElement
{

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param dict DataDictionary der Struktur
      @param update true -> die erstellten DataItems in der Struktur werden mit 'FullUpdated' markiert.
   */
  DataStructElement( DataItem *parent, DataDictionary &dict, bool update );

  /** Destruktor
   */
  virtual ~DataStructElement();

protected:
  /** Copy-Konstruktor
      \param ref
      \note Copy-Konstruktor steht nicht zur Verfügung
   */
  DataStructElement( const DataStructElement &ref );

  /** Zuweisungsoperator
      \param ref
      \return
      \note Der Zuweisungsoperatorsteht nicht zur Verfügung
   */
  DataStructElement& operator=( const DataStructElement &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion gibt den Typ des Elements zurueck.
      @return Typ des Elements
  */
  virtual DataDictionary::DataType getElementType() const;

  /** Dieser Operator weist den Wert eines anderen DataElements zu. Er versucht
      auch bei unterschiedlichen Datentypen das Beste daraus zu machen.
      @param ref Referenz auf DataElement
      @return false -> Die Zuweisung erfolgte ohne Zwischenfall
  */
  virtual bool operator=( const DataElement &ref );

  /** Die Funktion teilt mit, ob es sich um ein Struktur-Element der Klasse
      DataStructElement handelt.
      \return true => Es ist ein Struktur-Element
   */
  virtual bool isStructElement() const { return true; }

  /** Die Funktion liefert die Referenz auf das DataElement ähnlich einer
      cast-Funktion.
      @return Referenz auf das DataStructElement-Objekt (this)
  */
  virtual const DataStructElement &getStructElement() const;

  /** Diese Funktion erstellt eine Kopie des Containers und liefert einen
      Pointer darauf.
      @return Pointer auf den neu erstellten Container
  */
  virtual DataContainer *clone() const;

  /** Die Funktion erstellt eine exakte Kopie des Containers und liefert den
      Pointer darauf.
      @param datapool Referenz auf den DataPool
      @return Pointer auf neuen Container
  */
  virtual DataContainer *copyContainer( DataPool &datapool ) const;

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
                                     , DataItem **pFirstCycleItem );

  /** Die Funktion liefert die Referenz auf des entsprechende DataItem.
      Das DataItem muss vorhanden sein.
      @param index Index
      @return Referenz auf das DataItem.
  */
  DataItem &item( int index ) const;

  /** Die Funktion liefert den Pointer auf des entsprechende DataItem.
      Das DataItem muss nicht vorhanden sein.
      @param index Index
      @return Pointer auf das DataItem.
  */
  DataItem *getItem( int index ) const;

  /** Die Funktion liefert den Pointer auf des entsprechende DataItem.
      Das DataItem muss nicht vorhanden sein.
      @param name Name des DataItems
      @return Pointer auf das DataItem.
  */
  DataItem *getItem( const std::string &name ) const;

  /** Die Funktion liefert den Pointer auf des entsprechende DataItem.
      Das DataItem muss nicht vorhanden sein.
      @param dbAttr DbAttr Name des DataItems
      @return Pointer auf das DataItem.
  */
  DataItem *getItemByDbAttr( const std::string &dbAttr ) const;

   /** Die Funktion liefert den Pointer auf den DataItem-Container
       der Datenstruktur.
       @return Pointer auf das DataItemContainer-Objekt.
    */
   DataItemContainer *getItemContainer() { return &m_Value; }

  /* -------------------------------------------------------------------------------- */
  /* Getter und Setter für die Werte im Element                                       */
  /* In einer Struktur kann man keine Werte lesen oder setzen.                        */
  /* -------------------------------------------------------------------------------- */

  /** Die Funktion macht nichts. @param val @return false */
  virtual bool getValue( int &val ) const { return false; }
  /** Die Funktion macht nichts. @param val  @param rundung @return false */
  virtual bool setValue( int val, int rundung ) { return false; }
  /** Die Funktion macht nichts. @param val  @param rundung @return false */
  virtual bool setValue( unsigned int val, int rundung ) { return false; }
  /** Die Funktion macht nichts. @param val @return false */
  virtual bool getValue( double &val ) const { return false; }
  /** Die Funktion macht nichts. @param val  @param rundung @return false */
  virtual bool setValue( double val, int rundung ) { return false; }
  /** Die Funktion macht nichts. @param val @return false */
  virtual bool getValue( std::string &val ) const { return false; }
  /** Die Funktion macht nichts. @param val @param rundung @return false */
  virtual bool setValue( const std::string &val, int rundung ) { return false; }
  /** Die Funktion macht nichts. @param val @return false */
  virtual bool getValue( dComplex &val ) const { return false; }
  /** Die Funktion macht nichts. @param val @param rundung @return false */
  virtual bool setValue( const dComplex &val, int rundung ) { return false; }

  /** Die Funktion erstellt anhand der Element-Daten ein neues DataValue-Objekt,
      setzt Werte und Attribute, und liefert den Pointer darauf.
      @return Pointer auf neues DataValue-Objekt
   */
  virtual DataValue *getDataValue() const;

  /** Die Funktion setzt anhand der Daten im DataValue-Objekt die Element-Daten
      und liefert den aus dieser Operation entstanden Update-Status.
      @param d DataValue-Objekt
      @return Update-Status
   */
  virtual DataElement::UpdateStatus setDataValue( DataValue *d );

  /** Die Funktion teilt mit, ob das Datenelement gültige Werte enthält.<br>
      Das valid-Bit wird nicht abgefragt.
      \return true => es hat mindestens ein gültiges Datenelement in der Struktur.
      \note Die Funktion ist identisch mit hasValidElements().
   */
  virtual bool isValid() const;

  /** Die Funktion teilt mit, ob das Datenelement gültige Werte enthält.<br>
      Das valid-Bit wird nicht abgefragt.
      \return true => es hat mindestens ein gültiges Datenelement in der Struktur.
      \note Die Funktion ist identisch mit isValid().
   */
  virtual bool hasValidElements( bool incl_itemattr ) const;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für für die Prüfung
      \return serialisierbare Elemente sind vorhanden
   */
  virtual bool hasSerializableValidElements( const SerializableMask flags ) const;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Ein DataElement liefert immer 0 ider 1, abhängig von der Gültigkeit.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param incl_attr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, bool incl_attr ) const;

  /** Die Funktion liefert die logische Länge der Dimension. Sie funktioniert
      schlanker als GetAllDataDimensionSize().
      @param incl_itemattr true -> ungültige DataItem's mit gültigen Attributen werden berücksichtigt
   */
  virtual int getValidDimensionSize( bool incl_itemattr=false ) const;

  /** Die Funktion weist alle Werte des als Parameter erhaltenen Elements zu.
      @param el Referenz auf das Source-Element (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus assignDataElement( const DataElement &el );

  /** Die Funktion weist alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      @param el Referenz auf das Source-Element (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus assignCorrespondingDataItem( const DataElement &el );

  /** Die Funktion löscht alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      @param dict Referenz auf das DataDictionary-Objekt (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus clearCorrespondingDataItem( const DataDictionary &dict );

  /** Diese Funktion löscht alle Werte des Datenelements markiert sie als ungültig. Falls
      bereits alle Elemente ungültig waren, wird dies mit dem UpdateStatus angezeigt.
      \return Status der durchgeführten Aktion.
      \note Die Funktion ist identisch mit clearContainer(false).
   */
  virtual UpdateStatus clearElement();

  /** Diese Funktion löscht alle Werte des Datenelements markiert sie als ungültig. Falls
      bereits alle Elemente ungültig waren, wird dies mit dem UpdateStatus angezeigt.
      \param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      \return Status der durchgeführten Aktion.
      \note Die Funktion ist identisch mit clearElement().
   */
  virtual UpdateStatus clearContainer( bool incl_itemattr );

  /** Die Funktion löscht alle Werte im Struktur-Baum ausser das eventuell vorhandene
      SortKey-DataItem. Die Funktion wird nur vom Compare-Feature verwendet beim
      erstellen der Vergleichsstruktur.
      Alle vorhanden DataItem's und ihre Attribute bleiben erhalten.
  */
  virtual void clearContainerExceptSortKey();

  /** Die Funktion initialisiert alle DataItem-Objekte in dieser Struktur. Die
      Attribute bleiben erhalten. Alle Elemente der DataItem-Objekte werden
      gelöscht. Die Attribute der Elemente gehen dabei unweigerlich verloren.
      \return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus eraseContainer();

  /** Die Funktion schreibt das Element im XML-Format in den Output-Stream.
      @param ostr Output-Stream
      @param attrs String-Liste für die Attribute
      @param is_structitem
      @param level
      @param debug
      @param isScalar
      @return true -> erfolgreich
   */
  virtual bool writeXML( std::ostream &ostr
                       , std::vector<std::string> &attrs
                       , bool is_structitem
                       , int level
                       , bool debug
                       , bool isScalar );

  /** Die Funktion schreibt das Element im JSON-Format in den Output-Stream.
      @param ostr Output-Stream
      @param is_structitem
      @param level
      @param indentation
      @param isScalar
      @param scale
      @param flags Flags für für die Prüfung von 'serializable'
      @return true -> erfolgreich
   */
  virtual bool writeJSON( std::ostream &ostr,
                          bool is_structitem,
                          int level,
                          int indentation,
                          bool isScalar,
                          double scale,
                          const SerializableMask flags );

  /** Die Funktion schreibt das Element im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
      @param index Index in einem Dimensionsvektor
   */
  virtual void writeDP( std::ostream &ostr, int i, int index ) const;

  /** Die Funktion ruft für ein oder mehrere indizierte Datenelemente (mit
      Wildcards) den Functor auf um die gewünschten Aktionen auszuführen.
      @param item Referenz auf das DataItem
      @param func Referenz auf den Functor für die Aktion
      @param inx  Referenz auf Index-Objekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataItem &item
                                , DataAlterFunctor &func
                                , DataInx &inx );

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  virtual DataCompareResult compare( const DataContainer &cont, int fall, int max_faelle );

  /** Die Funktion setzt für alle vorhandenen Elemente in der Struktur die entsprechenden
      Bits in der Attribut-Bitmaske.
      @param mask Maske mit den zu setzenden Attribut-Bits
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return true -> Die Bitmaske hat sich verändert
   */
  virtual bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle );

  /** Die Funktion liefert den Inhalt des ev. vorhanden SortKey-DataItems in der
      Struktur.
      @param key Die Funktion füllt den Parameter
      @return true -> Ein Key ist vorhanden
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
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Die Funktion initialisiert alle Werte des Elements
      (siehe clearContainer() ).
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus clearValue();

  /** Die Funktion schreibt den Wert in den Outputstream. Das Strukturelement
      hat keinen eigenen Wert.
      @param ostr Outputstream
   */
  virtual void writeDPvalue( std::ostream &ostr ) const{}

  /** Die Funktion setzt die Compare-Flags neu. Die nicht mehr gültigen Flags
      werden dabei gelöscht.
      @param mask Bitmaske mit dem neuen Flag
   */
  void setCompareAttribute( DATAAttributeMask mask );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  DataItemContainer  m_Value;      // Noch nicht am Ziel
  DataDictionary    &m_dict;       // Dictionary-Pointer. Diese Variable ist nicht absolut
                                   // notwendig, macht mir aber das Leben leichter.
};

#endif // !defined(__DP_DATA_STRUCT_ELEMENT__)
/** \file */
