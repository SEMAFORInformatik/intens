#if !defined(GUI_QT_TABLEDATAITEM_INCLUDED_H)
#define GUI_QT_TABLEDATAITEM_INCLUDED_H

#include "gui/qt/GuiQtDataField.h"
#include "gui/GuiTableItem.h"

class GuiQtTableDataItem : public GuiQtDataField, public GuiTableItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtTableDataItem( GuiElement *parent );
  virtual ~GuiQtTableDataItem(){}
private:
  GuiQtTableDataItem( const GuiQtTableDataItem &item );

public:
/*=============================================================================*/
/* public own member functions                                                 */
/*=============================================================================*/
  virtual XferParameter::InputStatus checkFormat();

/*=============================================================================*/
/* public member functions from TableItem                                      */
/*=============================================================================*/
public:
  void setDataItem( XferDataItem * );
  virtual bool isDataItemUpdated( TransactionNumber trans );
  virtual TableItemType ItemType(); // { return item_None; }

  virtual GuiTableItem *getTableItem();
  virtual GuiTableItem *getTableItem( int );
  virtual GuiTableItem *getTableItem( int, int );
  virtual void getTableItemSize( int &row, int &col );

  virtual void setFieldDataAttributes( int row, int col );
  virtual void setFieldColor( int row, int col );
  virtual std::string CellValue();
  virtual void ValidateCell(const std::string &value );

  virtual void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_TableDataItem; }
  virtual void create() {}
  virtual void manage() {}
  virtual void getSize( int &x, int &y ) { x=0; y=0; }
  virtual void update( GuiElement::UpdateReason );
  virtual QWidget* myWidget() { return 0; }
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

/*=============================================================================*/
/* public member functions of GuiTableItem                                     */
/*=============================================================================*/
  virtual GuiDataField* getDataField() { return this; }
  virtual bool isEditable() { return GuiQtDataField::isEditable(); }
  virtual GuiElement* getElement() { return this; }

/*=============================================================================*/
/* public member functions of GuiDataField                                     */
/*=============================================================================*/
public:
  virtual bool setLength( int len );
  virtual bool setPrecision( int prec );
  virtual bool setScalefactor( Scale *scale );
  virtual bool setThousandSep();
  virtual int getLength() { return m_length; }
  virtual Scale* getScalefactor();

  virtual bool setInputValue();

  enum PasteConfirmDialogMode { Activated, Disabled, Inactivated};
  static bool isPasteConfirmDialogMode(PasteConfirmDialogMode mode);
  static void setPasteConfirmDialogMode(PasteConfirmDialogMode mode);
#if HAVE_PROTOBUF
  virtual in_proto::ComboBox* serializeProtobuf();
#endif

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setTableIndex( const std::string &name, int inx );
  void FinalWork();
  void FinalWorkOk();
  void EndOfWorkFinished();

  void checkAttributes( int, int );
  GuiQtTableDataItem *cloneTableDataItem();
  void registerIndex();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  TableItemType            m_itemtype;
  int                      m_length;
  int                      m_row;
  int                      m_col;
  static PasteConfirmDialogMode s_disableConfirmDialog;
protected:
  std::string              m_value;

  friend class GuiQtTableComboBoxItem;
};

#endif
