
#if !defined(__DP_DATA_ATTRIBUTES__)
#define __DP_DATA_ATTRIBUTES__

#include "datapool/DataDebugger.h"

#include <iostream>

/** Ein DataAttributes-Objekt enthält die Attribut-Bitmaske für Datencontainer.
    Diese Klasse wird allen Klassen vererbt, welche eine Bitmask verwenden
    (DataElement, DataValue, DataItem).
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAttributes
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataAttributes();

  /** Konstruktor
      \param mask
  */
  DataAttributes( DATAAttributeMask mask );

  /** Copy-Konstruktor
      \param mask
  */
  DataAttributes( const DataAttributes &attr );

  /** Destruktor
   */
  virtual ~DataAttributes();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion liefert die Attribute-Bitmaske des Containers.
      @return Attribute
   */
  DATAAttributeMask getAttributes() const;

  /** Mit dieser Funktion können Attribute gesetzt(set) oder initialisiert(reset) werden.
      @param set_mask Bit-Maske mit zu setzenden Attributen
      @param reset_mask Bit-Maske mit zurück zu setzenden Attributen
      @return true => Die Attribute haben sich verändert
   */
  bool setAttributes( DATAAttributeMask set_mask, DATAAttributeMask reset_mask );

  /** Die Funktion initialisiert die gewünschten Bits mit 1 und gibt die modifizierten
      Attribute als Resultat zurück.
   */
  static DATAAttributeMask setAttrBits( DATAAttributeMask mask, DATAAttributeMask set_mask );

  /** Die Funktion initialisiert die gewünschten Bits mit 0 und gibt die modifizierten
      Attribute als Resultat zurück.
   */
  static DATAAttributeMask resetAttrBits( DATAAttributeMask mask, DATAAttributeMask reset_mask );

  /** Die Attribute werden komplet neu gesetzt.
      @param mask Die neuen Attribute
      @return true => Die Attribute haben sich verändert
   */
  bool resetAttributes( DATAAttributeMask mask );

  /** Mit dieser Funktion können Attribute gesetzt(set) werden.
      (Vereinfachung von setAttributes() )
      @param mask Bit-Maske mit zu setzenden Attributen
      @param set true -> setzen, false -> initialisieren
      @return true => Die Attribute haben sich verändert
   */
  bool setAttribute( DATAAttributeMask mask, bool set=true );

  /** Mit dieser Funktion können Attribute initialisiert(reset) werden.
      (Vereinfachung von setAttributes() )
      @param reset_mask Bit-Maske mit zurück zu setzenden Attributen
      @return true => Die Attribute haben sich verändert
   */
  bool resetAttribute( DATAAttributeMask reset_mask );

  /** Die Funktion teilt mit, ob das Attribut, welches mit dem Parameter mask geliefert wird,
      gesetzt ist.
      @param mask Attribut
      @return true -> Das Attribut ist gesetzt
   */
  bool isAttributeSet( DATAAttributeMask mask ) const;

  /** Die Funktion schreibt die Datapool-Attribut-Werte in den Outputstream
      @param ostr Outputstream
   */
  virtual void writeDPattributes( std::ostream &ostr ) const;

  /** Die Funktion schreibt alle Attribut-Werte in den Outputstream
      @param ostr Outputstream
   */
  virtual void writeDPattributeBits( std::ostream &ostr ) const;

  static void attributeBits( DATAAttributeMask mask, std::ostream &ostr );

  /** Diese Funktion setzt das Stylesheet auf dem DataItem.
      Ändert das Stylesheet ist der Rückgabewert true.
      @param stylesheet
      @return true -> Änderung des Stylesheets
   */
  bool setStylesheet(const std::string &stylesheet);
  /** Diese Funktion gibt das Stylesheet zurück
      @return Stylesheet
   */
  const std::string& getStylesheet() const;

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  DATAAttributeMask m_attr_mask;
  std::string       m_stylesheet;
};

#endif
/** \file */
