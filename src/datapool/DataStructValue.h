
#if !defined(__DP_DATA_STRUCT_VALUE__)
#define __DP_DATA_STRUCT_VALUE__

#include "datapool/DataDictionary.h"
#include "datapool/DataValue.h"

/** Die DataStructValue-Objekte werden von den DataStructElement-Objekten erstellt.
    Sie enthalten temporär den Pointer auf das entsprechende DataStructElement-Objekt
    inkl. Attribute für Aktionen mit Vektoren.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataStructValue : public DataValue
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataStructValue();

  /** Konstruktor
      @param el Pointer auf das DataStructElement-Objekt
      \todo Die Beschreibung fehlt
   */
  DataStructValue( const DataStructElement *el );

  /** Konstruktor
      @param el Pointer auf das DataStructElement-Objekt
      @param mask Bit-Maske mit den Attributen
      \todo Die Beschreibung fehlt
   */
  DataStructValue( const DataStructElement *el, DATAAttributeMask mask );

  /** Destruktor.
      \note ACHTUNG: Er löscht das vorhandene DataElement NICHT.
   */
  virtual ~DataStructValue();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion teilt mit, dass es sich um ein Struktur-Objekt handelt.
      @return Datentyp des Objekts
   */
  virtual DataDictionary::DataType getType() const { return DataDictionary::type_StructVariable; }

  /** Die Funktion liefert die Referenz auf das DataStructValue-Objekt, ähnlich einer
      cast-Funktion.
      @return Referenz auf das DataStructValue
  */
  virtual const DataStructValue &getStructValue() const;

  /** Die Funktion liefert den Pointer auf das DataStructElement-Objekt.
      @return Pointer auf das Struktur-Element
   */
  const DataStructElement *getStructElement() const;

  /** Die Funktion schreibt den Inhalt des Objekts in den Outputstream.
      @param ostr Outputstream
   */
  virtual void print( std::ostream &ostr ) const;

  /** Die Funktion erstellt KEIN leeres DataElement-Objekt.
      @return Null-Pointer
   */
  virtual DataElement *createEmptyElement();

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  const DataStructElement   *m_struct_element;
};

#endif
/** \file */
