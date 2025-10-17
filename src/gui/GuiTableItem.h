
#if !defined(GUI_TABLEITEM_INCLUDED_H)
#define GUI_TABLEITEM_INCLUDED_H
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiDataField;
class GuiElement;
class XferDataItem;
class Scale;

class GuiTableItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiTableItem(){}
  GuiTableItem( const GuiTableItem &item ){}
  virtual ~GuiTableItem(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  enum TableItemType
  { item_None = 0
  , item_Label
  , item_Value
  , item_Array
  , item_Matrix
  };

  virtual TableItemType  ItemType() { return item_None; }
  virtual GuiDataField*  getDataField() = 0;
  virtual bool isEditable() = 0;
  virtual GuiElement*    getElement() = 0;
  virtual void setDataItem( XferDataItem * ) { };
  virtual bool isDataItemUpdated( TransactionNumber trans ) { return false; }

  virtual GuiTableItem *getTableItem() = 0;
  virtual GuiTableItem *getTableItem( int ) { return 0; }
  virtual GuiTableItem *getTableItem( int, int ) { return 0; }
  virtual void getTableItemSize( int &row, int &col ) { row=1; col=1; }

  virtual int getMaxLength() { return 0; }
  virtual void setMaxLength( int ){}

  virtual Scale* getScalefactor() { return 0; }

  virtual void setFieldPicklist( int row, int col ) {}
  virtual void setFieldDataAttributes( int row, int col ) {}

  virtual void setFieldLength( int row, int col ) {}
  virtual void setFieldLabelAttributes( int, int, int, int ) {}

  virtual std::string CellValue() { return ""; }
  virtual void ValidateCell(const std::string &value ) {}
#if HAVE_PROTOBUF
  virtual in_proto::ComboBox* serializeProtobuf() {return nullptr;}
#endif

  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false) { return false; }
};

#endif
