
#if !defined(GUI_QT_TABLE_INCLUDED_H)
#define GUI_QT_TABLE_INCLUDED_H

#include "gui/GuiContainer.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/GuiDataField.h"
#include "gui/GuiTable.h"


class GuiTableSize;
class GuiTableLine;
class GuiTableItem;
class GuiTableDataItem;
class GuiQtPopupMenu;
class GuiQtMenuButton;
class GuiElement;
class GuiQtTableViewBase;
class QtStandardItemModel;
class QStandardItem;
class QModelIndex;


class GuiQtTable : public GuiQtElement, public GuiContainer, public GuiTable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtTable( GuiElement *parent, const std::string name="" );
  GuiQtTable( const GuiQtTable& );
  virtual ~GuiQtTable();

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Table; }
  virtual void create();
  virtual void manage();
  virtual void enable();
  virtual void disable();
  bool isEditable();
  virtual void update( UpdateReason );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false) {
    return GuiTable::hasChanged(trans, xfer, show);
  }
  virtual QWidget* myWidget();
  void addTabGroup() {}
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;

  virtual GuiElement * getElement() { return this; }
  virtual GuiTable    *getTable(){ return this; }
  virtual GuiPopupMenu *getPopupMenu();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiTable::variantMethod(method, jsonArgs, eng);
  }

  // GuiIndexListener
  virtual bool acceptIndex( const std::string &name, int inx );
  virtual void setIndex( const std::string &name, int inx );

  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  virtual GuiElement::Orientation getDialogExpandPolicy();
  virtual Qt::Orientations getExpandPolicy();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  GuiPopupMenu* PopupMenu(int x, int y, GuiElement::Orientation orientation);
  void updateItem( int row, int col, GuiTableItem* item );
  virtual void deleteSelectedData( bool );
  virtual void clearSelection();
  virtual bool getSelectedItems(std::vector<int>& row_idxs, std::vector<int>& column_idxs);
  virtual bool selectRow(int row, bool recursive=true);
  virtual bool selectRows(std::vector<int> rows, bool recursive=true);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  class TableActionFunctor;

  void setFieldAttributes();
  void setFieldTableAttributes();
  void setFieldHorizontalAttributes();
  void setFieldVerticalAttributes();
  void setActionFunctions();
  bool setFieldLength( GuiTableItem *item, int row, int col, GuiTableLine* line );
  void setFieldPicklists();
  void setFieldTablePicklists();
  void setFieldHorizontalPicklists();
  void setFieldVerticalPicklists();

///nop  void setFieldEditable( int row, int col );

  void addTitles();
  void createTableHorizontal();
  void createTableVertical();
  void createTableMatrix();

  void setValues();
  //???  void print_type( int );
  void setWidgetSize(SizeType size_type);
  void updateUnitFields();

  Q_OBJECT
private slots:
  void slotItemChanged(QStandardItem * item);
  void slotCommitData(QWidget* widget);

public slots:
  void rowSelect( const QModelIndex& index );
  void rowUnselect( const QModelIndex& index );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
  friend class SpreadSheetDelegate;
  friend class QtHeaderView;
private:
  QWidget              *m_frame;
  GuiQtTableViewBase   *m_tablewidget;
  QtStandardItemModel  *m_model;

  std::vector<GuiQtTable*> m_clonedTable;
  const GuiQtTable*  m_cloneTable;
};

#endif
