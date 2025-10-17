
#if !defined(GUI_QT_TABLE_COMBOBOX_ITEM_INCLUDED_H)
#define GUI_QT_TABLE_COMBOBOX_ITEM_INCLUDED_H

#include "gui/qt/GuiQtTableDataItem.h"

#include <QtCore/QStringList>

class GuiQtTableComboBoxItem : public GuiQtTableDataItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtTableComboBoxItem( GuiElement *parent );
  GuiQtTableComboBoxItem( const GuiQtTableComboBoxItem &item );

  virtual ~GuiQtTableComboBoxItem();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  GuiQtTableComboBoxItem *cloneComboBoxItem();
  bool setSetName( const std::string &name );

  virtual bool isDataItemUpdated( TransactionNumber trans );
  virtual void setColors();
  virtual void setFieldPicklist( int row, int col );

  virtual std::string CellValue();
  QStringList getCellItems();
  static void getTrimmedCellItems(QStringList& slist);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated);
#if HAVE_PROTOBUF
  in_proto::ComboBox* serializeProtobuf();
#endif

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual XferParameter::InputStatus checkFormat();
  TransactionNumber getPickList_timestamp() { return m_pickList_timestamp; }
  void setPickList_timestamp(TransactionNumber trans) { m_pickList_timestamp = trans; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void createPickList();
  void deletePickList();
  void fill( int );
  bool convertInput( std::string &value );
  bool convertOutput( std::string &value );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  DataReference   *m_set;
  DataReference   *m_set_input;
  DataReference   *m_set_output;
  DataReference   *m_set_strings;
  bool             m_use_input;
  std::vector<QStringList>      m_picklist;
  bool             m_noInvalidEntry;
  TransactionNumber m_pickList_timestamp;
};

#endif
