
#if !defined(GUI_CONTAINER_INCLUDED_H)
#define GUI_CONTAINER_INCLUDED_H

#include <vector>
#include "gui/GuiElement.h"
//#include "gui/qt/GuiQtElement.h"

class QGridLayout;
class QBoxLayout;
class QLayoutItem;
class QWidget;

class GuiContainer: public std::vector<GuiElement*>
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiContainer( GuiElement *parent, GuiElement::ElementType type );
  GuiContainer( const GuiContainer &gc );
  virtual ~GuiContainer(){}

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type();
  virtual void manage();
  virtual void unmanage();
  virtual bool destroy();
  virtual bool replace( GuiElement *old_el, GuiElement *new_el );
  virtual void getSize( int &x, int &y );
  virtual void resize();
  virtual void update( GuiElement::UpdateReason );
  virtual void enable();
  virtual void disable();
  virtual void setScrollbar( GuiElement::ScrollbarType sb );
  virtual void setScrollbar( GuiElement::Orientation o, GuiElement::ScrollbarType sb );
  virtual void setPanedWindow( GuiElement::FlagStatus s );
  virtual void setFrame( GuiElement::FlagStatus s );
  virtual void setUseFrame();
  virtual bool withScrollbars() const;
  virtual bool withPanedWindow();
  virtual bool withFrame();
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual void getVisibleElement(GuiElementList& res);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void attach( GuiElement * );
  virtual void front( GuiElement * );
  virtual void setUseRuler() { m_useRuler = true; }
  virtual bool useRuler() { return m_useRuler; }
  bool doLayoutRuler(bool vertical);
  virtual void alignWithHRuler();
  virtual void alignWithVRuler();
  virtual void serializeAttrs( std::ostream &os );
  virtual void serializeAttrs(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual void serializeAttrs(in_proto::ElementList* eles, google::protobuf::RepeatedPtrField<in_proto::ElementRef>* reflist, bool onlyUpdated = false);
#endif
  bool withHorizontalScrollbar() const;
  bool withVerticalScrollbar() const;
  GuiElement::ScrollbarType getHorizontalScrollbar() const;
  GuiElement::ScrollbarType getVerticalScrollbar() const;

/*=============================================================================*/
/* private class  LayoutRuler                                                  */
/*=============================================================================*/
private:
class  LayoutRuler {
public:
  LayoutRuler(bool vertical) : m_vertical( vertical ) {}

#if HAVE_HEADLESS
private:
#elif HAVE_QT
  void addGridLayout( QGridLayout* gl );
  void addBoxLayout( QBoxLayout* bl );
  void execute();
private:
  QWidget*  getSingleWidget(QLayoutItem * item);
#endif

  bool                            m_vertical;
  std::vector<QGridLayout*>       m_gridLayouts;
  std::vector<QBoxLayout*>        m_boxLayouts;
  std::vector<int>                max_lengths;
};

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiElement::ElementType      m_element_type;
  bool                         m_useRuler;
  GuiElement                  *m_element;
  GuiElement::FlagStatus       m_with_frame;
  GuiElement::ScrollbarType    m_horizontal_scrollbar;
  GuiElement::ScrollbarType    m_vertical_scrollbar;
  GuiElement::FlagStatus       m_with_paned;
};

#endif
