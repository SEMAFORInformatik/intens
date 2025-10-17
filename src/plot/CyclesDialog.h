
#ifndef CYCLE_DIALOG_H
#define CYCLE_DIALOG_H

#include "xfer/XferDataItem.h"
#include "gui/GuiButtonListener.h"

class Cycle;
class CyclesDialogListener;
class GuiFieldgroup;
class GuiEventData;
class GuiElement;
class GuiForm;

class CyclesDialog : public GuiButtonListener
{
//=============================================================================//
// Constructor / Destructor                                                    //
//=============================================================================//
public:
  CyclesDialog( CyclesDialogListener *listener
	       , GuiEventData *event = 0 );
  virtual ~CyclesDialog();

  typedef std::vector<XferDataItem *> XferSet;

//=============================================================================//
// public member functions of GuiForm                                          //
//=============================================================================//
public:
  void create();

//=============================================================================//
// public member functions of GuiButtonListener                                //
//=============================================================================//
public:
  JobAction* getAction(){ return 0; }
  virtual void ButtonPressed( GuiEventData *event );

//=============================================================================//
// public member functions                                                     //
//=============================================================================//
public:
  bool addItem( const std::string &label, const std::string &name,  XferDataItem *xfer );
  void setTransaction( TransactionNumber trans ){ m_trans = trans; }
  void getValues( std::vector<int> &v, int newValue );
  void setValues( std::vector<int> &v, int newValue );
  void closeEvent();
  CyclesDialogListener *getListener(){ return m_listener; }
  virtual GuiElement *getElement() = 0;
  void initialize( const std::string & );

//=============================================================================//
// protected member functions                                                  //
//=============================================================================//
// protected:
  bool isDialogUpdated();

//=============================================================================//
// private Data                                                                //
//=============================================================================//
private:
  CyclesDialogListener *m_listener;
  GuiFieldgroup        *m_fieldgroup;
  GuiButton            *m_closeButton;
  std::vector<XferDataItem *> m_xfers;
  GuiEventData         *m_event;
  TransactionNumber     m_trans;
};

#endif
