
#if !defined(__DP_DATA_COMPARE_RESULT__)
#define __DP_DATA_COMPARE_RESULT__

#include "datapool/DataGlobals.h"

/** Diese Klasse beinhaltet das Resultat des Vergleichs von verschiedenen Elementen.
    @author Copyright (C) 2015 SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataCompareResult
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataCompareResult();

  /** Copy-Konstruktor
      @param rslt
   */
  DataCompareResult( const DataCompareResult &rslt );

  /** Konstruktor
      @param mask
   */
  DataCompareResult( DATAAttributeMask mask );

  /** Destruktor
   */
  virtual ~DataCompareResult();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion übernimmt die gestzten Bits aus dem Parameter-Objekt. Es werden
      keine Bits zurück auf 0 gestzt.
      @param rslt Referenz auf DataCompareResult-Objekt.
   */
  void setResult( const DataCompareResult &rslt );

  /** Die Funktion übernimmt die gestzten Bits aus der Parameter-Bitmask. Es werden
      keine Bits zurück auf 0 gestzt.
      @param mask Attribut-Bitmaske zum Einfügen.
   */
  void setResult( DATAAttributeMask mask );

  /** Die Funktion liefert eine Bitmask mit dem dem mächtigsten Bit, welches in der
      Maske gesetzt ist. Die Funktion wird verwendet um das Resultat im Datenbaum
      nach unten weiter zu geben.
      @return Attribut-Bitmask
   */
  DATAAttributeMask getResult() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /** Beschreibung der Funktion isSet
      \param mask
      \return
      \todo Die Beschreibung der Funktion isSet fehlt
   */
  bool isSet( DATAAttributeMask mask ) const;

  /** Beschreibung der Funktion isNotSet
      \param mask
      \return
      \todo Die Beschreibung der Funktion isNotSet fehlt
   */
  bool isNotSet( DATAAttributeMask mask ) const;

  /** Beschreibung der Funktion isSet
      \param mask1
      \param mask2
      \return
      \todo Die Beschreibung der Funktion isSet fehlt
   */
  bool isSet( DATAAttributeMask mask1, DATAAttributeMask mask2 ) const;

/*=============================================================================*/
/* member variables                                                            */
/*=============================================================================*/
private:
  static DATAAttributeMask  s_mask_all;
  DATAAttributeMask         m_mask;

};

#endif
/** \file */
