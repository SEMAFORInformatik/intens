
#if !defined(__DP_DATA_REAL_VALUE__)
#define __DP_DATA_REAL_VALUE__

#include "datapool/DataDictionary.h"
#include "datapool/DataValue.h"

/** Die DataRealValue-Objekte werden von den DataRealElement-Objekten erstellt.
    Sie enthalten temporär Wert und Attribute für Aktionen mit Vektoren.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataRealValue : public DataValue
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataRealValue();

  /** Konstruktor
      @param val Wert des neuen Objekts
   */
  DataRealValue( double val );

  /** Konstruktor
      @param val Wert des neuen Objekts
      @param mask Bit-Maske mit den Attributen
   */
  DataRealValue( double val, DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataRealValue();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, dass es sich um ein Real-Objekt handelt.
      @return Datentyp des Objekts
   */
  inline DataDictionary::DataType getType() const { return DataDictionary::type_Real; }

  /** Die Funktion liefert den Wert des Objekts als Real.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( double &val ) const;

  /** Die Funktion liefert den Wert des Objekts als String.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( std::string &val ) const;

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const double val );

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
  double   m_Value;
};

#endif
/** \file */
