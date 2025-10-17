
#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include "gui/GuiButtonListener.h"

class GuiFieldgroup;

class ConfigDialog : public GuiButtonListener
{
//=============================================================================//
// public definitions                                                          //
//=============================================================================//
public:
  enum eStyle {
    LIST,
    COMBOBOX
  };
  enum eItemStyle {
	SHOW_NONE = 0,
	SHOW_AXIS = 1,
	SHOW_LINE_COLOR = 2,
	SHOW_AXIS_LINE_COLOR = 3,
	SHOW_SYMBOL_COLOR = 4,
	SHOW_LINE_STYLE = 8,
	SHOW_SYMBOL_STYLE = 16,
	SHOW_AXIS_SYMBOL = 21, // Marker
	SHOW_PALETTE = 30,     // Indexed PlotItems
	SHOW_ALL = 31
  };

//=============================================================================//
// Constructor / Destructor                                                    //
//=============================================================================//
public:
 ConfigDialog()
   : m_style( LIST ), m_comboboxLength( 10 )
	{}
  virtual ~ConfigDialog() {}

//=============================================================================//
// public member functions of GuiForm                                          //
//=============================================================================//
public:
  virtual ConfigDialog *getConfigDialog(){ assert( false ); return 0; }

//=============================================================================//
// public member functions                                                     //
//=============================================================================//
public:
  virtual bool addItem( const std::string &label, std::vector<XferDataItem *> &xfers,
						XferDataItem* colorXfer, XferDataItem* colorSymbolXfer,
						XferDataItem* lineStyleXfer, XferDataItem* symbolStyleXfer,
            XferDataItem* symbolSizeXfer, XferDataItem* unitXfer,
						eItemStyle itemStyle,  bool new_group=false ) = 0;
  virtual bool addSeparator() = 0;
  virtual bool hasCycleMode() = 0;
  virtual bool setCycleMode(bool cycleMode,  const std::vector<int>& showCycleVector) = 0;
  virtual bool isUpdated( TransactionNumber trans ) = 0;
  virtual GuiForm *getForm() = 0;
  virtual const std::vector<std::string>& getColumnLabels() = 0;
  virtual void showSymbolColorColumn(bool visible, int columnOffset=0) = 0;
  /** recreates new fieldgroup */
  virtual void recreateFieldgroup() = 0;
  /** replace previously with 'recreateFieldgroup' created fieldgroup */
  virtual void replaceFieldgroup() = 0;
  void setStyle( eStyle style, int length ){ m_style = style;
	m_comboboxLength = length > 0 ? length : 10; }
  eStyle getStyle(){ return m_style; }

protected:
  int getComboboxLength() { return m_comboboxLength; }

//=============================================================================//
// public member functions of GuiButtonListener                                //
//=============================================================================//
public:
  virtual JobAction* getAction() { return 0; }

  //=============================================================================//
// private Data                                                                //
//=============================================================================//
private:
  // prevent copy and assignement
   ConfigDialog( const ConfigDialog &d );
   ConfigDialog & operator=(const ConfigDialog &d);

   eStyle                m_style;
   int                   m_comboboxLength;
};

#endif
