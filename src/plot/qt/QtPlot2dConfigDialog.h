#ifndef QT_PLOT2D_CONFIG_DIALOG_H
#define QT_PLOT2D_CONFIG_DIALOG_H

#include "plot/ConfigDialog.h"
#include "gui/qt/GuiQtForm.h"

class GuiQtDataField;
class GuiRadioButton;
class GuiQtLabel;
class GuiQtFieldgroupLine;
class GuiQtOrientationContainer;

class QtPlot2dConfigDialog  : public GuiQtForm, public ConfigDialog{
  //=============================================================================//
  // private definitions
  //=============================================================================//
 private:
  //=============================================================================//
  // Constructor / Destructor
  //=============================================================================//
 public:
  QtPlot2dConfigDialog( ConfigDialogListener *listener
		      , GuiElement *parent
		      , const std::string &name
		      , const std::string &title
		      , const std::vector<std::string>& labels
		      , bool cyclePlotMode
		      , GuiEventData *event = 0 );
  virtual ~QtPlot2dConfigDialog();

 private:
  // prevent copy and assignement
  QtPlot2dConfigDialog();
  QtPlot2dConfigDialog( const QtPlot2dConfigDialog &d );
  QtPlot2dConfigDialog & operator=(const QtPlot2dConfigDialog &d);

  //=============================================================================//
  // public member functions
  //=============================================================================//
  public :
    DataReference *getDrefXComboboxValue(){ return m_drefXComboboxValue; }
    DataReference *getDrefLineColor(){ return m_drefLineColor; }
    DataReference *getDrefSymbolColor(){ return m_drefSymbolColor; }
    DataReference *getDrefLineStyleComboboxValue(){ return m_drefLineStyleComboboxValue; }
    DataReference *getDrefSymbolStyleComboboxValue(){ return m_drefSymbolStyleComboboxValue; }
    DataReference *getDrefSymbolSizeComboboxValue(){ return m_drefSymbolSizeComboboxValue; }
    DataReference *getDrefUnitComboboxValue(){ return m_drefUnitComboboxValue; }
	virtual bool hasCycleMode() {  return m_cycleMode; }
	virtual bool setCycleMode(bool cycleMode, const std::vector<int>& showCycleVector);
	void recreateFieldgroup();
	void replaceFieldgroup();

  //=============================================================================//
  // public member functions of ConfigDialog
  //=============================================================================//
 public:
  virtual bool isUpdated( TransactionNumber trans );
  virtual bool addItem( const std::string &label, std::vector<XferDataItem *> &xfers,
						XferDataItem* colorXfer, XferDataItem* symbolColorXfer,
						XferDataItem* lineStyleXfer, XferDataItem* symbolStyleXfer,
            XferDataItem* symbolSizeXfer, XferDataItem* unitXfer,
						eItemStyle itemStyle, bool new_group=false );
  virtual bool addSeparator();
  virtual void createDataset();
  virtual void showSymbolColorColumn(bool visible, int columnOffset=0);
  virtual GuiForm *getForm(){ return this; }
  const std::vector<std::string>& getColumnLabels() { return m_columnLabels; }

//=============================================================================//
// public member functions of GuiButtonListener                                //
//=============================================================================//
public:
  virtual void ButtonPressed( GuiEventData *event );
  /* virtual JobAction* getAction() {} */

  //=============================================================================//
  // public member functions of GuiElement                                       //
  //=============================================================================//
 public:
  virtual void create();
  virtual void manage();
  virtual void update( GuiElement::UpdateReason reason );

  //=============================================================================//
  // private member functions
  //=============================================================================//
 private:
  GuiFieldgroup *getFieldgroup(){ return m_fieldgroup; }
  GuiFieldgroup *createFieldgroup();
  void createLocalData( const std::string &name );
  const std::string &getName() const { return m_name; } ///???

  //=============================================================================//
  // private Data
  //=============================================================================//
 private:
  GuiButton            *m_resetButton;
  GuiButton            *m_closeButton;

  XferDataItem              *m_combosXfer;
  GuiDataField              *m_xDataField;
  int                        m_numItems;
  std::vector<XferDataItem*> m_xComboItems;
  DataSet                   *m_dataset[4];
  DataReference             *m_drefXComboboxValue;
  DataReference             *m_drefLineColor;
  DataReference             *m_drefSymbolColor;
  DataReference             *m_drefLineStyleComboboxValue;
  DataReference             *m_drefSymbolStyleComboboxValue;
  DataReference             *m_drefSymbolSizeComboboxValue;
  DataReference             *m_drefUnitComboboxValue;
  GuiRadioButton*            m_previousButton;
  GuiElement                *m_parent;
  std::string           m_name;
  GuiFieldgroup        *m_fieldgroup;
  GuiFieldgroup        *m_fieldgroupOld;
  bool                  m_cycleMode;
  std::vector<int>      m_showCycleVector;
  bool                  m_recreate;
  bool                  m_created;
  GuiEventData         *m_event;
  ConfigDialogListener *m_listener;
  GuiQtFieldgroupLine  *m_currentLine;
  GuiQtOrientationContainer* m_vertCont;
  static const std::string s_datasetName[4];
  std::vector<std::string> m_columnLabels;
  std::vector<GuiQtFieldgroupLine*> m_fgLines;
};

#endif
