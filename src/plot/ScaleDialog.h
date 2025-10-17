
#ifndef SCALE_DIALOG_H
#define SCALE_DIALOG_H

#include "xfer/XferDataItem.h"
#include "gui/GuiButtonListener.h"

class Scale;
class ScaleDialogListener;
class GuiFieldgroup;
class GuiDataField;
class GuiEventData;
class GuiElement;
class GuiForm;

class ScaleDialog : public GuiButtonListener
{
//=============================================================================//
// Constructor / Destructor                                                    //
//=============================================================================//
public:
  ScaleDialog( ScaleDialogListener *listener
	       , const std::string &itemLabel
	       , GuiEventData *event = 0 );
  virtual ~ScaleDialog();

  typedef std::vector<XferDataItem *> XferSet;

  enum RescaleMode
  {
    KeepScales,
    Fixed,
    Expanding,
    Fitting
  };
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
  bool addItem( const std::string &label
		, XferDataItem *xferMin
		, Scale *min_scale
		, XferDataItem *xferMax
		, Scale *max_scale
		, XferDataItem *xferScaleEnable
		, XferDataItem *xferAspectRatio
		, int fieldLength = 10
		, int precision = 2 );
  void setTransaction( TransactionNumber trans ){ m_trans = trans; }
  ScaleDialogListener *getListener(){ return m_listener; }
  virtual GuiElement *getElement() = 0;
  void initialize( const std::string &, XferDataItem *xferAspectRatioType );
  virtual GuiDataField* getAspectRatioTypeCB() { return m_aspectRatioTypeCB; }
  virtual GuiForm *getForm() = 0;
//=============================================================================//
// protected member functions                                                  //
//=============================================================================//
protected:
  bool isDialogUpdated();
  virtual void attach( GuiElement *e ) = 0;

//=============================================================================//
// private Data                                                                //
//=============================================================================//
private:
  ScaleDialogListener  *m_listener;
  GuiFieldgroup        *m_fieldgroup;
  GuiDataField         *m_aspectRatioTypeCB;
  GuiButton            *m_resetButton;
  GuiButton            *m_closeButton;
  GuiEventData         *m_event;
  TransactionNumber     m_trans;
  std::vector<XferSet*> m_xferSets;
  std::string           m_itemLabel;

  DataReference        *m_drefLineStyleComboboxValue;
};

#endif
