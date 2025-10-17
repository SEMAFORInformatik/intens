
#ifndef QT_CYCLES_DIALOG_H
#define QT_CYCLES_DIALOG_H

#include "gui/qt/GuiQtForm.h"
#include "plot/CyclesDialog.h"

class QtCyclesDialog : public CyclesDialog
		    , public GuiQtForm {
//=============================================================================//
// Constructor / Destructor                                                    //
//=============================================================================//
public:
  QtCyclesDialog( GuiElement *parent
		, CyclesDialogListener *listener
		 , const std::string &title
		 , GuiEventData *event = 0 );
  virtual ~QtCyclesDialog();

//=============================================================================//
// public member functions of GuiForm                                          //
//=============================================================================//
public:
  virtual void create();
  virtual void update( UpdateReason );
  virtual void manage();

//=============================================================================//
// public member functions                                                     //
//=============================================================================//
public:
  //  void getValues( std::vector<int> &v, int newValue );
  //  void setValues( std::vector<int> &v, int newValue );
  virtual GuiForm *getForm(){ return this; }

protected:
  virtual GuiElement *getElement(){ return this; }
  virtual void attach( GuiElement *e ){
    GuiQtForm::attach(e);
  }

};

#endif
