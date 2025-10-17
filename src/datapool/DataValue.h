
#if !defined(__DP_DATA_VALUE__)
#define __DP_DATA_VALUE__

#include <string>

#include "datapool/DataDebugger.h"
#include "datapool/DataPool.h"
#include "datapool/DataDictionary.h"
#include "utils/PointerBasis.h"

class DataElement;
class DataStructValue;

/** Die DataValue-Objekte werden von den DataElement-Objekten erstellt.
    Sie enthalten temporär Wert und Attribute für Aktionen mit Vektoren.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataValue : public PointerBasis<DataValue>
{
/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  /// Smart-Pointer-Definition
  typedef Pointer<DataValue> Ptr;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataValue(){}

  /** Konstruktor
      \param mask Bit-Maske mit den Attributen
   */
  DataValue( DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataValue();

protected:
  /** Der Copy-Konstruktor ist nicht implementiert
      \param ref
   */
  DataValue(const DataValue &ref); /* not implemented */

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataValue");

  /** Die Funktion zeigt an, ob der Wert des Elements gültig ist.
      @return true -> der Wert ist gültig
   */
  bool isValid() const;

  /** Die Funktion zeigt an, ob der Wert des Elements ungültig ist.
      @return true -> der Wert ist ungültig
   */
  bool isInvalid() const;

  /** Die Funktion markiert den Wert im Objekt als gültig.
   */
  void setValid();

  /** Die Funktion markiert den Wert im Objekt als gültig.
   */
  void setInvalid();

  /** Die Funktion zeigt an, ob der Wert des Elements 'locked' ist.
      @return true -> der Wert ist 'locked'
   */
  bool isLocked() const;

  /** Die Funktion markiert den Wert im Objekt als 'locked'.
   */
  void setLocked();

  /** Die Funktion markiert den Wert im Objekt als 'unlocked'.
   */
  void setUnlocked();

  /** Die Funktion teilt mit, um welchen Elementtyp es sich handelt.
      @return Datentyp des Objekts
   */
  virtual DataDictionary::DataType getType() const = 0;

  /** Die Funktion liefert den Wert des Objekts als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual inline bool getValue( int &val ) const { return false; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual inline bool setValue( const int val ) { return false; }

  /** Die Funktion liefert den Wert des Objekts als Real.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual inline bool getValue( double &val ) const { return false; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual inline bool setValue( const double val ) { return false; }

  /** Die Funktion liefert den Wert des Objekts als String.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual inline bool getValue( std::string &val ) const { return false; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual inline bool setValue( const std::string &val ) { return false; }

  /** Die Funktion liefert den Wert des Objekts als Complexe Zahl.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual inline bool getValue( dComplex &val ) const { return false; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual inline bool setValue( const dComplex &val ) { return false; }

  /** Die Funktion liefert die Referenz auf das DataStructValue-Objekt, ähnlich einer
      cast-Funktion. Falls es sich nicht um ein DataStructValue handelt, wird die
      Verarbeitung abgebrochen assert().
      @return Referenz auf das DataStructValue
  */
  virtual const DataStructValue &getStructValue() const;

  /** Die Funktion liefert den Inhalt der Attribut-Bit-Makse.
      @return Bit-Maske
   */
  DATAAttributeMask getAttributes() const;

  /** Die Funktion schreibt den Inhalt des Objekts in den Outputstream.
      @param ostr Outputstream
   */
  virtual void print( std::ostream &ostr ) const = 0;

  /** Die Funktion erstellt anhand seiner Daten ein passendes leeres DataElement-Objekt.
      @return Pointer auf das neue DataElement-Objekt
   */
  virtual DataElement *createEmptyElement() = 0;

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Mit dieser Funktion können Attribute gesetzt(set) oder initialisiert(reset) werden.
      @param set_mask Bit-Maske mit zu setzenden Attributen
      @param reset_mask Bit-Maske mit zurück zu setzenden Attributen
   */
  void setAttributes( DATAAttributeMask set_mask, DATAAttributeMask reset_mask );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  DATAAttributeMask m_attr_mask;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
