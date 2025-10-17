
#if !defined(__DP_DATA_ELEMENT__)
#define __DP_DATA_ELEMENT__

#include "datapool/DataDebugger.h"
#include "datapool/DataContainer.h"
#include "datapool/DataAttributes.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataCompareResult.h"

#include <iostream>

class DataVector;
class DataValue;
class DataItem;
class DataDimension;
class DataAlterFunctor;
class DataReadFunctor;

/** Die Klasse DataElement ist die Basisklasse aller Datenelemente.
    Ein DataElement enthält in der Regel genau einen Wert eines Basisdatentyps
    plus den zugehörigen Attributen (siehe Klasse DataAttributes).<br>
    Die Ausnahme bildet das Struktur-Element der Klasse DataStructElement. Es
    enthält eine Liste von DataItem-Objekten mit wiederum einer beliebigen
    Anzahl von DataElement-Objekten.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataElement : public DataContainer
                  , public DataAttributes
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataElement();

  /** Destruktor
   */
  virtual ~DataElement();

protected:
  /** Der Copy-Konstruktor ist nicht implementiert.
      \param ref
      \note Der Copy-Konstruktor steht nicht zur Verfügung.
   */
  DataElement( const DataElement &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataElement");

  /** Dieser Operator weist den Wert eines anderen DataElements zu. Er versucht
      auch bei unterschiedlichen Datentypen das Beste daraus zu machen.
      \param ref Referenz auf ein DataElement
      \return false -> Die Zuweisung erfolgte ohne Zwischenfall
  */
  virtual bool operator=( const DataElement &ref ) = 0;

  /** Die Funktion teilt mit, um welchen Containertyp es sich handelt.
      \return Typ die Funktion liefert immer ContainerIsElement.
   */
  virtual ContainerType getContainerType() const {return ContainerIsElement; }

  /** Die Funktion teilt mit, um welchen Elementtyp es sich handelt.
      @return Typ des Datenelements
   */
  virtual DataDictionary::DataType getElementType() const = 0;

  /** Die Funktion liefert den this-Pointer als DataElement, falls es sich wirklich
      um ein DataElement handelt.
      @return Pointer auf das DataElement
   */
  virtual DataElement *getDataElement(){ return this; }

  /** Die Funktion liefert den this-Pointer als DataElement, falls es sich wirklich
      um ein DataElement handelt.
      @return Pointer auf das DataElement
   */
  virtual const DataElement *getDataElement() const { return this; }

  /** Die Funktion zeigt an, dass es sich um ein Objekt der Basisklasse
      DataElement handelt.
      \return true
   */
  virtual bool isDataElement() const { return true; }

  /** Die Funktion teilt mit, ob es sich um ein Struktur-Element der Klasse
      DataStructElement handelt.
      \return false => Es ist kein Struktur-Element
   */
  virtual bool isStructElement() const { return false; }

  /** Die Funktion liefert die Referenz auf das DataElement ähnlich einer
      cast-Funktion. Falls es sich nicht um ein Objekt der Klasse DataStructElement handelt,
      wird die Verarbeitung mit assert() abgebrochen.
      \return Referenz auf das DataElement
  */
  virtual const DataStructElement &getStructElement() const;

  /** Die Funktion teilt mit, ob das Datenelement einen gültigen Wert enthält.
      \return true => Der Wert ist gültig
      \note Die Funktion ist identisch mit hasValidElements().
   */
  virtual bool isValid() const;

  /** Die Funktion teilt mit, ob das Datenelement einen gültigen Wert enthält.
      \return true => Der Wert ist gültig
   */
  virtual bool hasValidElements( bool incl_itemattr ) const;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für für die Prüfung
      \return serialisierbare Elemente sind vorhanden
   */
  virtual bool hasSerializableValidElements( const SerializableMask flags ) const;

  /** Die Funktion teilt mit, ob das Datenelement schreibgeschützt (locked) ist.
      @return true -> Werte sind gelockt
   */
  virtual bool isLocked() const;

  /** Die Funktion setzt den write-Lock für dieses Element.
      @return Update-Status
   */
  DataElement::UpdateStatus setLocked();

  /** Die Funktion setzt den write-Lock für dieses Element zurück.
      @return Update-Status
   */
  DataElement::UpdateStatus setUnlocked();

  /** Die Funktion liefert die Anzahl der im Container enthaltenen Elemente.
      (Ein DataElement hat immer genau ein Element)
      @return Anzahl der Elemente
   */
  virtual int getNumberOfElements() const { return 1; }

  /** Die Funktion liefert die Anzahl der Dimensionen im Container.
      (Ein DataElement hat immer 0 Dimensionen)
      @return Anzahl Dimensionen
   */
  virtual int getNumberOfDimensions() const { return 0; }

  /** Die Funktion liefert die Anzahl der Elemente einer durch den
      Parameter bestimmten Dimension.
      @param dim_num Nummer der gewünschten Dimension
      @return Anzahl der Elemente
   */
  virtual int getDataDimensionSizeOf( int dim_num ) const { return 1; }

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Ein DataElement liefert immer 0 ider 1, abhängig von der Gültigkeit.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param incl_attr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, bool incl_attr ) const;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Es werden dabei die vorhanden Indexlisten verwendet.
      Nur dort, wo Wildcards gesetzt sind, wird die Dimensionsgrösse grösser als 1.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param index aktuelles DataInx-Objekt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, DataInx &index ) const;

  /** Die Funktion liefert die logische Länge der Dimension. Sie funktioniert
      schlanker als GetAllDataDimensionSize().
      @param incl_itemattr true -> ungültige DataItem's mit gültigen Attributen werden berücksichtigt
   */
  virtual int getValidDimensionSize( bool incl_itemattr=false ) const;

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
                                     , DataItem **pFirstCycleItem )
    {assert(false); // Eigentlich sollten wir nicht hierher kommen!!!
     return;
    }

  /* -------------------------------------------------------------------------------- */
  /* Getter und Setter für die Werte im Element                                       */
  /* -------------------------------------------------------------------------------- */

  /** Die Funktion liefert den Wert des Elements als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( int &val ) const = 0;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( int val, int rundung ) = 0;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( unsigned int val, int rundung ) = 0;

  /** Die Funktion liefert den Wert des Elements als Double.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( double &val ) const = 0;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( double val, int rundung ) = 0;

  /** Die Funktion liefert den Wert des Elements als String.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( std::string &val ) const = 0;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const std::string &val, int rundung ) = 0;

  /** Die Funktion liefert den Wert des Elements als complexe Zahl.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( dComplex &val ) const = 0;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const dComplex &val, int rundung ) = 0;

  /** Die Funktion erstellt anhand der Element-Daten ein neues DataValue-Objekt,
      setzt Werte und Attribute, und liefert den Pointer darauf.
      @return Pointer auf neues DataValue-Objekt
   */
  virtual DataValue *getDataValue() const = 0;

  /** Die Funktion setzt anhand der Daten im DataValue-Objekt die Element-Daten
      und liefert den aus dieser Operation entstanden Update-Status.
      @param d DataValue-Objekt
      @return Update-Status
   */
  virtual DataElement::UpdateStatus setDataValue( DataValue *d ) = 0;

  /** Die Funktion weist alle Werte des als Parameter erhaltenen Elements zu.
      @param el Referenz auf das Source-Element (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus assignDataElement( const DataElement &el ) = 0;

  /** Die Funktion weist alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      Wenn es sich nicht um ein Struktur-Element handelt, passiert nichts.
      @param el Referenz auf das Source-Element (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus assignCorrespondingDataItem( const DataElement &el );

  /** Die Funktion löscht alle Werte der übereinstimmenden DataItems des als Parameter
      erhaltenen Elements zu. Die Struktur-Definition muss nicht dieselbe sein.
      Wenn es sich nicht um ein Struktur-Element handelt, passiert nichts.
      @param dict Referenz auf das DataDictionary-Objekt (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus clearCorrespondingDataItem( const DataDictionary &dict );

  /** Diese Funktion löscht den Wert des Datenelements markiert ihn als ungültig. Falls
      das Element bereits ungültig war, wird dies mit dem UpdateStatus angezeigt.
      \return Status der durchgeführten Aktion.
   */
  virtual UpdateStatus clearElement();

  /** Diese Funktion löscht den Wert des Datenelements markiert ihn als ungültig. Falls
      das Element bereits ungültig war, wird dies mit dem UpdateStatus angezeigt.
      @param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      \return Status der durchgeführten Aktion.
      \note Die Funktion ist identisch mit clearElement().
   */
  virtual UpdateStatus clearContainer( bool incl_itemattr );

  /** Diese Funktion löscht den Wert des Datenelements markiert ihn als ungültig. Falls
      das Element bereits ungültig war, wird dies mit dem UpdateStatus angezeigt.
      \return Status der durchgeführten Aktion.
      \note Die Funktion ist identisch mit clearElement().
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
                       , bool isScalar ) = 0;

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
                          const SerializableMask flags ) = 0;

  /** Die Funktion schreibt das Element im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
      @param index Index in einem Dimensionsvektor
   */
  virtual void writeDP( std::ostream &ostr, int i, int index ) const;

  /** Die Funktion ruft für ein oder mehrere indizierte Datenelemente (u. U. mit
      Wildcards) den Functor auf um die gewünschte Aktion auszuführen.
      @param item Referenz auf das DataItem
      @param func Referenz auf den Functor für die Aktion
      @param inx  Referenz auf Index-Objekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataItem &item
                                , DataAlterFunctor &func
                                , DataInx &inx );

  /** Die Funktion liefert die Reference auf das indizierte DataElement. Falls
      noch nicht vohanden, wird das Element angelegt.
      @param inxlist Indexliste
      @param index aktueller Index
      @return Referenz auf das DataElement
  */
  virtual DataElement &getDataElement( DataItem &item,
                                       DataIndexList &inxlist,
                                       int index );

  /** Die Funktion liefert die Reference auf das letzte indizierte DataContainer-Objekt.
      Falls noch nicht vohanden, wird das Objekt angelegt.
      @param item Referenz auf das DataItem
      @param inxlist Indexliste
      @param index aktuelle Indexnummer
      @param sz benötigte Grösse auf dem letzten Index
      @return Referenz auf das DataContainer-Objekt
  */
  virtual DataContainer &getDataContainer(  DataItem &item, DataIndexList &inxlist, int index, int sz );

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  virtual DataCompareResult compare( const DataContainer &cont, int fall, int max_faelle ) = 0;

  /** Die Funktion setzt für das vorhandene Element die entsprechenden Bits in
      der Attribut-Bitmaske.
      @param mask Maske mit den zu setzenden Attribut-Bits
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return true -> Die Bitmaske hat sich verändert
   */
  virtual bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle );

  /** Die DataDimension wird markiert als Spezialfall für das Darstellen von Vergleichen
      zwischen Strukturen mit dem Compare-Feature.
   */
  virtual void setCompareDimension();

  /** Die Funktion zeigt an, ob es sich um eine 'Compare'-DataDimension handelt.
      ( siehe setCompareDimension() )
      @return true -> Es ist eine 'Compare'-DataDimension
   */
  virtual bool isCompareDimension() const;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Die Funktion markiert den Wert des Elements als 'gültig' und setzt den Timestamp
      für die Änderung des Werts. Die zugehörigen Daten werden dabei nicht verändert.
   */
  void setValid();

  /** Die Funktion markiert das Element als 'ungültig' und setzt den Timestamp
      für die Änderung des Werts falls das Element nicht bereits 'ungültig' war.
      Die zugehörigen Daten werden dabei nicht verändert.
      @return Status des durchgeführten Updates.
   */
  UpdateStatus setInvalid();

  /** Die Funktion initialisiert den Wert des Elements und setzt ihn 'ungueltig'.
      Die übrigen Attribute werden nicht veraendert.
      @return Update-Status
   */
  virtual UpdateStatus clearValue() = 0;

  /** Die Funktion schreibt den Wert in den Outputstream
      @param ostr Outputstream
   */
  virtual void writeDPvalue( std::ostream &ostr ) const = 0;

  /** Funktion wertet die Parameter aus, setzt die entsprechenden Attribut-Bits  und
      liefert den entsprechenden Result-Wert.
      @param left_valid true -> Der linke Wert ist gültig
      @param right_valid true -> Der rechte Wert ist gültig
      @param is_equal Die beiden gültigen Werte sind gleich
      @return CompareResult-Wert
   */
  DataCompareResult result_of_compare( bool left_valid, bool right_valid, bool is_equal );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;
};

#endif
/** \file */
