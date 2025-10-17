
#if !defined(GUI_QT_TABLELABELITEM_INCLUDED_H)
#define GUI_QT_TABLELABELITEM_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiTableItem.h"

class GuiQtTableLabelItem: public GuiQtElement, public GuiTableItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align )
    : GuiQtElement( parent )
    , m_label( label ){
      setAlignment( align );
  }
  virtual ~GuiQtTableLabelItem(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_TableLabelItem; }
  virtual void create() {}
  virtual void manage() {}
  virtual void getSize( int &x, int &y ) { x=0; y=0; }
  virtual void update( UpdateReason ) {}
  virtual QWidget* myWidget() { return 0; }

/*=============================================================================*/
/* public member functions of GuiField                                         */
/*=============================================================================*/
public:
  virtual void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* public member functions of GuiTableItem                                     */
/*=============================================================================*/
  virtual GuiDataField* getDataField() { return NULL; }
  virtual bool isEditable() { return false; }
  virtual GuiElement* getElement() { return this; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual TableItemType ItemType() { return item_Label; }
  virtual void setFieldLabelAttributes( int, int, int, int );
  GuiTableItem *getTableItem();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string        m_label;

};

#endif
