
#if !defined(GUI_TABLE_ACTION_FUNCTOR_INCLUDED_H)
#define GUI_TABLE_ACTION_FUNCTOR_INCLUDED_H

#include "job/JobElement.h"

class GuiDataField;

class GuiTableActionFunctor
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiTableActionFunctor( JobElement::CallReason reason, int idx )
    : m_reason( reason ), m_index( idx )  {}
  virtual ~GuiTableActionFunctor() {}

/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  virtual GuiTableActionFunctor *clone() { return 0; }
  virtual bool operator() ( GuiDataField &field ) = 0;
  JobElement::CallReason getReason() { return m_reason; }
  int                    getIndex()  { return m_index;  }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  JobElement::CallReason   m_reason;
  int                      m_index;
};

#endif
