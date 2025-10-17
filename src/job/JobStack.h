
#if !defined(JOB_ENGINE_STACK_H)
#define JOB_ENGINE_STACK_H

#include <stack>

#include "JobStackItem.h"

/** Die Klasse JobStack wird als Stack von Objekten der Klasse JobStackItem
    von der JobEngine verwendet.
    @author Copyright (C) 2016  SEMAFOR Informatik & Energie AG, Basel, Switzerland
*/
class JobStack
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStack();
  virtual ~JobStack();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  /** Diese Funktion stellt ein JobStackItem-Objekt auf den Stack.
      \param item Pointer auf das für den Stack bestimmte Stackitem
  */
  void push( const JobStackItemPtr &item );

  /** Diese Funktion holt den Pointer eines Stack-Items vom Stack ohne es von dort
      zu eliminieren. Der Stack wird nicht verändert! Das Item bleibt auf dem Stack.
      \attention Das Objekt darf nicht gelöscht werden
      \return Pointer auf das JobStackItem-Objekt.
  */
  const JobStackItemPtr get();

  /** Diese Funktion holt den Pointer eines Stack-Items vom Stack. Der Aufrufer
      ist verantwortlich für die weitere Zukunft dieses Objekts.
      \return Pointer auf das JobStackItem-Objekt.
  */
  const JobStackItemPtr pop();

  /** Die Funktion liefert die aktuelle Grösse des Stacks.
      \return Anzahl StackItems
  */
  int size();

  /** Die Funktion schreibt den gesamten Stackinhalt in den Outputstream.
      \param ostr Referenz auf dem Outputstream
  */
  void print( std::ostream &ostr );

/*=============================================================================*/
/* private Functions                                                           */
/*=============================================================================*/
private:
  void clear();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::stack<JobStackItemPtr> Stack;

  Stack   m_stack;
};

#endif
