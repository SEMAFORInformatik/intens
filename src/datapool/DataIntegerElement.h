
#if !defined(__DP_DATA_INTEGER_ELEMENT__)
#define __DP_DATA_INTEGER_ELEMENT__

#include "datapool/DataElement.h"

class DataValue;

/** Das DataIntegerElement-Objekt  enthält einen Wert des Typs int inklusive der
    zugehörigen Attribute aus der Basisklasse DataElement.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataIntegerElement : public DataElement
{

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataIntegerElement();

  /** Destruktor
   */
  virtual ~DataIntegerElement();

protected:
  /** Copy-Konstruktor
      \param ref
      \note Der Copy-Konstruktor ist nicht implementiert
   */
  DataIntegerElement( const DataIntegerElement &ref );

  /** Zuweisungsoperator
      \param ref
      \return
      \note Der Zuweisungsoperator ist nicht implementiert
   */
  DataIntegerElement& operator=( const DataIntegerElement &ref );

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
      @param ref Referenz auf das DataElement
      @return false -> Die Zuweisung erfolgte ohne Zwischenfall
  */
  virtual bool operator=( const DataElement &ref );

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

  /* -------------------------------------------------------------------------------- */
  /* Getter und Setter für die Werte im Element                                       */
  /* -------------------------------------------------------------------------------- */

  /** Die Funktion liefert den Wert des Elements als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( int &val ) const;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( unsigned int val, int rundung );

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( int val, int rundung );

  /** Die Funktion liefert den Wert des Elements als Double.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( double &val ) const;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( double val, int rundung );

  /** Die Funktion liefert den Wert des Elements als String.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( std::string &val ) const;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const std::string &val, int rundung );

  /** Die Funktion liefert den Wert des Elements als complexe Zahl.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( dComplex &val ) const;

  /** Die Funktion weist den Wert val dem Element zu.
      @param val neuer Wert des Elements
      @param rundung wenn der Wert >= 0 ist, wird vor dem Vergeich
             mit dem alten Wert gerundet auf die gewünschte Anzahl
             Nachkommastellen.
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const dComplex &val, int rundung );

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

  /** Die Funktion weist alle Werte des als Parameter erhaltenen Elements zu.
      @param el Referenz auf das Source-Element (Quelle).
      @return Update-Status
   */
  virtual DataElement::UpdateStatus assignDataElement( const DataElement &el );

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

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  virtual DataCompareResult compare( const DataContainer &cont, int fall, int max_faelle );

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
  /** Die Funktion initialisiert den Wert des Elements und setzt ihn 'ungueltig'.
      Die übrigen Attribute werden nicht veraendert.
      @return Update-Status
   */
  virtual UpdateStatus clearValue();

  /** Die Funktion schreibt den Wert in den Outputstream
      @param ostr Outputstream
   */
  virtual void writeDPvalue( std::ostream &ostr ) const;

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  int       m_value;
};

#endif
/** \file */
