
#if !defined(__DP_DATA_ALTER_WRITE_JSON__)
#define __DP_DATA_ALTER_WRITE_JSON__

#include "datapool/DataAlterFunctor.h"
#include "utils/Separator.h"
#include <stack>

/** Mit einem DataAlterGetItem-Objekt wird ein DataItem-Objekt im JSON-Format
    in einen Outputstream geschrieben.
    @author Copyright (C) 2015  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataAlterWriteJSON : public DataAlterFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
      @param ostr Outputstream
      @param level
      @param indent
      @param hideFlags
   */
  DataAlterWriteJSON( std::ostream &ostr,
                      int level,
                      int indent,
                      const SerializableMask flags,
                      bool omit_first_indent );

  /** Destruktor
   */
  virtual ~DataAlterWriteJSON();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion wird vor der Verarbeitung aufgerufen. Der Funktor hat die
      Möglichkeit zu prüfen, ob er die Arbeit ausführen kann.
      @param ref Referenz auf das für den Zugriff vorgesehene DataReference-Objekt.
      @return true => Die Kontrolle ist nicht zufrieden => Abbruch
  */
  virtual bool check( DataReference &ref );

  /** Für jedes DataItem auf dem Weg zum gewünschten DataElement wird diese Funktion
      aufgerufen.
      @param item Referenz auf das DataItem
      @param status Rückgabewert der vorgenommenen Mutation am DataItem-Objekt
      @param inx Referenz auf das Indexobjekt
      @param final true Das DataItem ist das physisch letzte vorhandene DataItem
             Im Datenbaum. Das gesuchte DataItem resp. DataElement ist (noch) nicht vorhanden.
      @return true -> Die Verarbeitung soll weitergehen.
   */
  virtual bool alterItem( DataItem &item, UpdateStatus &status, DataInx &inx, bool final );

  /** Diese Funktion wird nicht aufgerufen. Nur das DataItem ist von Interesse.
      @param el Referenz auf das DataElement
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataElement &el );

  /** Für jedes nicht vorhandene DataElement (Löcher) in einer DataDimension wird
      diese Funktion aufgerufen. Ein Update ist nicht möglich.
      @param inx Referenz auf die aktuellen Index-Verhältnisse
      @param num_dim Nummer der Dimension, in welcher das leere Element vorkommt.
  */
  virtual void nullElement( DataInx &inx, int num_dim );

  /** Die Funktion zeigt an, dass im Datapool kein Item als 'updated' markiert werden muss.
      @return false -> Keine 'update'-Markierungen nötig.
    */
  virtual bool updateElements() const { return false; }

  /** Die Funktion zeigt an, dass kein neues Element angelegt werden muss.
      @return false -> Kein Element anlegen
   */
  virtual bool createElements() const { return false; }

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard mit dem nächsten
      Index-Wert gestartet wird.
      @param wildcard_num Nummer der Wildcard
      @param max_elements Anzahl der vorhandenen Elemente auf dieser Wildcard-Ebene
  */
  virtual void startWithWildcard( int wildcard_num, int max_elements );

  /** Dem Funktor wird angezeigt, wenn auf einer Wildcard das letzte Element
      erreicht wurde und die Liste zu Ende ist.
      @param wildcard_num Nummer der Wildcard
  */
  virtual void endWithWildcard( int wildcard_num );

  /** Dem Funktor wird angezeigt, dass genau ein Element in der Dimension
      indiziert wird.
  */
  virtual void startOneElement();

  /** Dem Funktor wird angezeigt, dass genau ein Element in der Dimension
      bearbeitet wurde und die Dimension verlassen wird.
  */
  virtual void endOneElement();

  /** Die Funktion zeigt an, ob ohne Indexliste auf einer Ebene alle Elemente durch
      alle Dimension durchlaufen werden sollen.
      @return true -> alle Elemente durchlaufen
   */
  virtual bool setWildcardWithoutIndices() const { return true; }

  /** Die Funktion zeigt an, ob etwas in den Outputstream geschrieben wurde.
   */
  bool hasElementsWritten() const { return m_elements_written > 0; }

  /** Diese Funktion wird nach einer Verarbeitung als letzte Aktion aufgerufen.
      Sie kann als Debug-Funktion implementiert werden.
      @param result true -> Die Verarbeitung hat kein update durchgeführt
  */
  virtual void epilog( bool result );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  typedef std::stack<Separator> SepStack;

  std::ostream      &m_ostr;
  bool               m_is_scalar;
  bool               m_is_struct;
  int                m_level;
  int                m_indentation;
  bool               m_omit_first_indent;
  SerializableMask   m_serialize_flags;
  bool               m_wildcards_prev_level;
  bool               m_single_element;
  int                m_elements_written;
  bool               m_last_level_item;
  int                m_list_level;
  int                m_max_elements;
  SepStack           m_sep_stack;
};

#endif
/** \file */
