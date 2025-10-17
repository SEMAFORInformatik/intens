
#if !defined(GUI_FIELDGROUPLINE_INCLUDED_H)
#define GUI_FIELDGROUPLINE_INCLUDED_H

#include "GuiElement.h"

class GuiElement;
class GuiRuler;
class GuiFieldgroup;

class GuiFieldgroupLine
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiFieldgroupLine(GuiElement *parent);
  virtual ~GuiFieldgroupLine();
protected:
  GuiFieldgroupLine(const GuiFieldgroupLine &line);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual GuiElement* getElement() = 0;
  virtual void attach( GuiElement * );
  virtual GuiElement* getLastElement() = 0;
  void setArrowbar() { m_arrowbar = true; }
  bool isArrowbar() { return m_arrowbar; }
  void setTableIndexRange( int minIndex, int maxIndex );
  void setTableSizeVisibleLines( int tablesize );
  int getTableSizeVisibleLines() { return m_tablesize_visible_lines; }
  virtual bool cloneable() = 0;
  virtual GuiElement *clone() = 0;
  GuiDataField *getActionDataField();
  GuiDataField *getTableDataField( int no, int size );

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, in_proto::FieldGroup *element,  bool onlyUpdated = false);
#endif

  const GuiElementList& getElementList() { return m_elements; }

 /*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  virtual bool attachTableElement( GuiFieldgroup *fg, GuiElement *el );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
protected:
  GuiElementList   m_elements;
private:
  bool             m_arrowbar;
  int              m_tableposition;
  int              m_tablesize_visible_lines;
};

#endif
