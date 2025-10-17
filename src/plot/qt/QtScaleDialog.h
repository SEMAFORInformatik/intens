
#ifndef QT_SCALE_DIALOG_H
#define QT_SCALE_DIALOG_H

#include "gui/qt/GuiQtForm.h"
#include "plot/ScaleDialog.h"

class QtScaleDialog : public ScaleDialog
		    , public GuiQtForm {
//=============================================================================//
// Constructor / Destructor                                                    //
//=============================================================================//
public:
  QtScaleDialog( GuiElement *parent
		, ScaleDialogListener *listener
		 , const std::string &title
		 , const std::string &itemLabel
		 , GuiEventData *event = 0 );
  virtual ~QtScaleDialog();

//=============================================================================//
// public member functions of GuiForm                                          //
//=============================================================================//
public:
  virtual void create();
  virtual void update( UpdateReason );
  virtual void manage();

public:
  virtual GuiForm *getForm(){ return this; }

protected:
  virtual GuiElement *getElement(){ return this; }
  virtual void attach( GuiElement *e ){
    GuiQtForm::attach(e);
  }

};

#endif
