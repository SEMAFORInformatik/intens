/*
 * SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if !defined(__DP_DATA_STRING_VALUE__)
#define __DP_DATA_STRING_VALUE__

#include <string>
#include "datapool/DataDictionary.h"
#include "datapool/DataValue.h"

class DataStringValue : public DataValue
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataStringValue();

  /** Konstruktor
      @param val Wert des neuen Objekts
   */
  DataStringValue( const std::string &val );

  /** Konstruktor
      @param val Wert des neuen Objekts
      @param mask Bit-Maske mit den Attributen
   */
  DataStringValue( const std::string &val, DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataStringValue();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, dass es sich um ein String-Objekt handelt.
      @return Datentyp des Objekts
   */
  inline DataDictionary::DataType getType() const { return DataDictionary::type_String; }

  /** Die Funktion liefert den Wert des Objekts als Integer.
      @param val Rückgabewert
      @return true -> der Wert im Paramter ist gültig
   */
  virtual bool getValue( std::string &val ) const;

  /** Die Funktion weist den Wert val dem Objekt zu.
      @param val neuer Wert des Elements
      @return true Die Aktion war erfolgreich.
   */
  virtual bool setValue( const std::string &val );

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
  std::string    m_Value;

};

#endif
/** \file */
