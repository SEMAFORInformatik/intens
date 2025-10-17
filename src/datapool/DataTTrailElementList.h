
#if !defined(__DP_DATA_TTRAIL_ELEMENT_LIST__)
#define __DP_DATA_TTRAIL_ELEMENT_LIST__

#include <list>

#include "datapool/DataDebugger.h"

class DataTTrailElement;

/** DataTTrailElementList
    @author Copyright (C) 2009  SEMAFOR Informatik & Energie AG, Basel, Switzerland

    Dieses Objekt führt eine Liste von DataTTrailElement-Elementen in einem Stack.
    Durch die optionale Angabe einer maximalen Grösse wird dafür gesorgt, dass
    der Stack diese Grösse nicht übersteigt. Im Falle eines Overflows bei einem
    insert wird der älteste Eintrag gelöscht.
 */
class DataTTrailElementList
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DataTTrailElementList();

  /** Destruktor
      \return
      \todo Die Beschreibung der Funktion ~DataTTrailElementList fehlt
   */
  virtual ~DataTTrailElementList();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataTTrailElementList");

  /** Es wird festgelegt, wieviele Elemente höchstens in der Liste sein dürfen.
      Falls bereits Einträge bestehen, wird die Liste bei Bedarf sofort auf das
      neue Maximum zurecht gestutzt.
      @param anzahl maximale Anzahl von zugelassenen Einträgen.
                    0 bedeutet unlimitiert.
   */
  void setMaxElements( int anzahl );

  /** Das gewünschte Element wird am Ende der Liste angehängt. Falls die Liste
      zu lang wird, wird der älteste Eintrag eliminiert.
      @param el Pointer auf das einzufügende Element.
      @return true => Die Liste war vor dem Aufruf leer.
   */
  bool insertElement( DataTTrailElement *el );

  /** Das Letzte Element in der Liste wird ausgegeben und aus der Liste
      gelöscht.
      @return Pointer auf das letzte Element
   */
  DataTTrailElement *getElement();

  /** Alle Elemente in der Liste werden eliminiert und aus der Liste gelöscht.
      Die Liste ist anschliessend leer.
      @return true => Es wurden Elemente gelöscht.
   */
  bool clear();

  /** Die Funktion zeigt an, ob die Liste leer ist.
   */
  bool isEmpty() const;

  /** Die Funktion zeigt an, ob die Liste Elemente beinhaltet.
   */
  bool isntEmpty() const;

  /** Die Funktion liefert die Anzahl der in der Liste enthaltenen Elemente.
   */
  int size() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  BUG_DECLARE_COUNT;

  typedef std::list<DataTTrailElement *> ElementList;

  ElementList  m_element_list;
  int          m_max_elements;
};

#endif
/** \file */
