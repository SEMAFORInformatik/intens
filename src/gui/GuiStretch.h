
#if !defined(GUI_STRETCH_INCLUDED_H)
#define GUI_STRETCH_INCLUDED_H

#include "gui/GuiElement.h"

class GuiStretch : public GuiElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiStretch(int fac)
    : GuiElement( 0 )
    , m_horzFactor( fac )
    , m_vertFactor( fac ){
  }
  GuiStretch(int hfac, int vfac)
    : GuiElement( 0 )
    , m_horzFactor( hfac )
    , m_vertFactor( vfac ){
  }
  virtual ~GuiStretch(){
  }
protected:
  GuiStretch( const GuiStretch &v )
    : GuiElement( v )
    , m_horzFactor( v.m_horzFactor )
    , m_vertFactor( v.m_vertFactor ){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Stretch; }
  virtual void create( /*Widget parent*/ ) {}
  virtual void manage() {}
  virtual bool destroy() { return true; }
  virtual void getSize(int &x, int &y, bool hint=true) {x=0; y=0;}
  virtual void update( UpdateReason ) {}
  virtual bool cloneable() { return true; }
  virtual bool isShown() { return false; }
  virtual GuiElement *clone() { return new GuiStretch( *this ); }

  virtual void serializeXML( std::ostream &os, bool recursive = false){
	  os << "<intens:Stretch/>" << std::endl;
  }
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false){
    if(onlyUpdated){
      return false;
    }
	  jsonObj["type"] = "Stretch";
	  jsonObj["horizontal_factor"] = m_horzFactor;
	  jsonObj["vertical_factor"] = m_vertFactor;
    return true;
  }
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false) {
    if(onlyUpdated){
      return false;
    }
    auto element = eles->add_stretchs();
    element->set_id(getElementIntId());
    element->set_horizontal_factor(m_horzFactor);
    element->set_vertical_factor(m_vertFactor);
    return true;
  }
#endif

  virtual GuiMotifElement *getMotifElement() { return 0; }
  virtual GuiQtElement    *getQtElement()    { return 0; }
/*=============================================================================*/
/* public member functions of GuiStretch                                          */
/*=============================================================================*/
public:
  void getStretchFactor(int& hfac, int& vfac) { vfac = m_vertFactor; hfac = m_horzFactor; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int   m_horzFactor;
  int   m_vertFactor;
};

#endif
