
#if !defined(GUI_QTFIELDGROUPLINE_INCLUDED_H)
#define GUI_QTFIELDGROUPLINE_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiFieldgroupLine.h"

class GuiRuler;
class GuiFieldgroup;
class GuiQtDataField;

class GuiQtFieldgroupLine : public GuiFieldgroupLine, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFieldgroupLine( GuiElement *parent );
  virtual ~GuiQtFieldgroupLine();
private:
  GuiQtFieldgroupLine(const GuiQtFieldgroupLine &line );

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_FieldgroupLine; }
  virtual void create();
  virtual void manage();
  virtual bool destroy();
  virtual bool replace( GuiElement *old_el, GuiElement *new_el );
  virtual void getSize( int &x, int &y ); // { x=y=0; }
  virtual QWidget* myWidget() { return  m_widget; }
  virtual void update( UpdateReason );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual void enable();
  virtual void disable();
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &, int );
  //  GuiQtDataField *getActionDataField();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtFieldgroupLine( *this ); }
  virtual BasicStream *streamableObject();

  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, in_proto::FieldGroup *element, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  GuiElement* getElement() { return this; }
  GuiFieldgroupLine* getFieldgroupLine() { return this; }
  void attach(GuiElement *e) { GuiFieldgroupLine::attach(e); }
  GuiElement* getLastElement();
  ///  GuiQtDataField *getTableDataField( int no, int size );
  /** is hidden line (for fieldgroups with tablesize) */
  bool isHiddenLine();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /* bool attachTableElement( GuiFieldgroup *fg, GuiElement *el ); */

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QWidget         *m_widget;
  friend class GuiQtFieldgroup;
  friend class GuiQtContainer;
};

#endif
