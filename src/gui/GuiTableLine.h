
#if !defined(GUI_TABLELINE_INCLUDED_H)
#define GUI_TABLELINE_INCLUDED_H

#include "gui/GuiElement.h"
#include "gui/GuiTableItem.h"
#include "gui/GuiIndexListener.h"

class GuiTable;
class GuiTableDataItem;
class GuiTableLabelItem;
class XferDataItem;

class GuiTableLine : public GuiIndexListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiTableLine(GuiTable* table)
    : m_label_alignment( GuiElement::align_Left ), m_table(table), m_visible(true), m_visibility_updated(false) {
  }
  virtual ~GuiTableLine(){
  }
  void setParent( GuiElement *parent ) { /*m_parent = parent;*/ }
  GuiTable*  getGuiTable() { return m_table; }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  void setIndex( const std::string &name, int inx );
  bool acceptIndex( const std::string &name, int inx );

  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

  virtual void serializeXML(std::ostream &os, bool recursive);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setLabelAlignment( GuiElement::Alignment );
  virtual bool setLabel( const std::string & );
  virtual bool setTooltip( const std::string & );

  GuiElement::Alignment LabelAlignment() { return m_label_alignment; }
  std::string &Label();
  std::string &Tooltip();
  void LineSize( int &, int & );

  void syncroniseTableItemLength();
  int getMaxLength();
  void setMaxLength( int len );
  int getLength( int n );
  void setLength( int len, int n );
  Scale *getScalefactor( int col );

  GuiTableItem *addTableDataItem( XferDataItem *dataitem );
  GuiTableItem *addTableLabelItem( const std::string &, GuiElement::Alignment );
  XferDataItem *getDataItem( int col );
  GuiTableItem *getTableItem( int );
  GuiTableItem *getTableItem( int, int );
  bool check();
  GuiTableItem::TableItemType ItemType();
  bool isDataItemUpdated( TransactionNumber trans );
  void setVisible(bool visible);
  bool isVisible() { return m_visible; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<GuiTableItem *> GuiTableItemList;

  std::string         m_label;
  GuiElement::Alignment           m_label_alignment;
  std::string         m_tooltip;
  GuiTableItemList    m_tableitems;
  GuiTable*           m_table;
  bool                m_visible;
  bool                m_visibility_updated;
};

#endif
