
#if !defined(__DP_DATA_COMPLEX_VALUE__)
#define __DP_DATA_COMPLEX_VALUE__

#include "datapool/DataDictionary.h"
#include "datapool/DataValue.h"

/** Die DataComplexValue-Objekte werden von den DataComplexElement-Objekten erstellt.
    Sie enthalten temporär Wert und Attribute für Aktionen mit Vektoren.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataComplexValue : public DataValue
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataComplexValue();

  /** Konstruktor
      @param val Wert des neuen Objekts
   */
  DataComplexValue( dComplex val );

  /** Konstruktor
      @param val Wert des neuen Objekts
      @param mask Bit-Maske mit den Attributen
   */
  DataComplexValue( dComplex val, DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataComplexValue();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, dass es sich um ein Complex-Objekt handelt.
      @return Datentyp des Objekts
   */
  virtual DataDictionary::DataType getType() const { return DataDictionary::type_Complex; }

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const dComplex val );

  /** Die Funktion liefert den Wert des Objekts als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( dComplex &val ) const;

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
  dComplex    m_Value;
};

#endif
/** \file */
