
#if !defined(__DP_DATA_ELEMENT_MAP__)
#define __DP_DATA_ELEMENT_MAP__

#include <map>

#include "datapool/DataDebugger.h"
#include "datapool/DataGlobals.h"
#include "datapool/DataCompareResult.h"

class DataDimension;
class DataElement;

/** Das DataElementMap-Objekt ist eine nach einem SortKey sortierten Liste
    von Pointern auf DataElement-Objekte. Die DataElement-Objekte müssen eine Struktur
    sein und einen SortKey enthalten. Nach der Verarbeitung werden keine Elemente
    gelöscht.
 */
class DataElementMap : public DataGlobals
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataElementMap();

  /** Destruktor
   */
  virtual ~DataElementMap();

protected:
  /** Copy-Konstruktor
      \param v
      \note Der Copy-Konstruktor ist nicht implementiert
   */
  DataElementMap(const DataElementMap &v );

public:
  /// Sortierte Liste für den Vergleich mit SortKey
  typedef std::map<std::string, DataElement *> ElementMap;

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataElementMap");

  /** Die Funktion fügt das übergebene DataElement in die interne Liste ein.
      @param el Pointer auf das DataElement
   */
  bool insert( DataElement *el );

  /** Zwei sortierte DataElement-Listen werden miteinander verglichen.
      @param m Referenz auf das DataElementMap-Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult
   */
  DataCompareResult compare( DataElementMap &m, int fall, int max_faelle );

  /** Die in der internen Liste verbliebenen DataElement-Objekte werden kopiert
      und der DataDimension hinten angehängt.
      @param dim Referenz auf die DataDimension
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return true -> Es wurden Objekte angehängt
   */
  bool append( DataDimension &dim, int fall, int max_faelle );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

  /** Beschreibung der Funktion get
      \param key
      \return
      \todo Die Beschreibung der Funktion get fehlt
   */
  ElementMap::iterator get( const std::string &key );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;


  ElementMap  m_el_map;
};

#endif
/** \file */
