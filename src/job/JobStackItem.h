
#if !defined(JOB_STACK_ITEM_INCLUDED_H)
#define JOB_STACK_ITEM_INCLUDED_H

#include <iostream>

#include "job/JobElement.h"
#include "utils/PointerBasis.h"

class JobStackItemPtr;
class JobStackAddress;
class JobStackReturn;


class JobStackItem : public JobElement
                   , public PointerBasis<JobStackItem>
{
  friend class JobStackItemPtr;
  friend class Pointer<JobStackItem>;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobStackItem();
protected:
  virtual ~JobStackItem();

/*=============================================================================*/
/* public Interfaces                                                           */
/*=============================================================================*/
public:
  /** Die Funktion liefert den Typ des StackItems.
      \return Typ
  */
  virtual StackType Type() = 0;

  static int getItemCount();

  virtual void print( std::ostream &ostr ) = 0;

  /** Die Funktion meldet, ob es sich um ein Pausenzeichen handelt.
      \retval false Ein normales StackItem
      \retval true  Ein Pausenzeichen
   */
  virtual bool isPause() { return false; }

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual JobStackAddress &address();
  virtual JobStackReturn &returncontrol();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static int   s_item_count;
};

inline std::ostream &operator<<( std::ostream &ostr, JobStackItem &item ){
  item.print( ostr );
  return ostr;
}


class JobStackData;

class JobStackItemPtr : public Pointer<JobStackItem>
{
public:
  JobStackItemPtr();
  JobStackItemPtr( JobStackItem *p );
  JobStackItemPtr( const JobStackItemPtr & );

public:
  JobStackAddress &address();
  JobStackReturn &returncontrol();

};

inline std::ostream &operator<<( std::ostream &ostr, const JobStackItemPtr &ptr ){
  ptr->print( ostr );
  return ostr;
}

#endif
