
#if !defined(__DP_DATA_INTEGER_VALUE__)
#define __DP_DATA_INTEGER_VALUE__

#include "datapool/DataDictionary.h"
#include "datapool/DataValue.h"

/** Die DataIntegerValue-Objekte werden von den DataIntegerElement-Objekten erstellt.
    Sie enthalten temporär Wert und Attribute für Aktionen mit Vektoren.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataIntegerValue : public DataValue
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataIntegerValue();

  /** Konstruktor
      @param val Wert des neuen Objekts
   */
  DataIntegerValue( int val );

  /** Konstruktor
      @param val Wert des neuen Objekts
      @param mask Bit-Maske mit den Attributen
   */
  DataIntegerValue( int val, DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataIntegerValue();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, dass es sich um ein Integer-Objekt handelt.
      @return Datentyp des Objekts
   */
  inline DataDictionary::DataType getType() const { return DataDictionary::type_Integer; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const int val );

  /** Die Funktion liefert den Wert des Objekts als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( int &val ) const;

  /** Die Funktion schreibt den Inhalt des Objekts in den Outputstream.
      @param ostr Outputstream
   */
  virtual void print( std::ostream &ostr ) const;

  /** Die Funktion erstellt anhand seiner Daten ein passendes leeres DataElement-Objekt.
      @return Pointer auf das neue DataElement-Objekt
   */
  virtual DataElement *createEmptyElement();

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  int    m_Value;

};

#endif
/** \file */
