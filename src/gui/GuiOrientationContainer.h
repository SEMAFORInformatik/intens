
#if !defined(GUI_ORIENTATION_CONTAINER_INCLUDED_H)
#define GUI_ORIENTATION_CONTAINER_INCLUDED_H

/** Der GuiOrientationContainer ist ein GuiCointainer-Objekt, welches eine Liste von
    GuiElementen horizontal oder vertikal anordnet und gegebenenfalls gegeneinander ausrichtet.
*/
#include "gui/GuiElement.h"

class GuiOrientationContainer
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiOrientationContainer() {}
  virtual ~GuiOrientationContainer(){}

private:
  GuiOrientationContainer( GuiOrientationContainer &cont ); // Copy Constructor

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
/*   virtual void create( Widget parent ); */
/*   virtual void resize(); */
/*   virtual void manage(); */
/*   virtual bool destroy(); */
/*   virtual bool replace( GuiElement *old_el, GuiElement *new_el ); */

/*=============================================================================*/
/* public member functions of GuiOrientationContainer                          */
/*=============================================================================*/
  virtual GuiElement* getElement() = 0;
  virtual GuiElement::Orientation getOrientation() = 0;
/*   virtual GuiElement::ElementType Type() { return m_container.Type(); } */
/*   virtual void getSize( int &x, int &y ) { m_container.getSize( x, y ); } */
/*   virtual void unmanage()                { m_container.unmanage( ); } */
/*   virtual void update( UpdateReason r)   { m_container.update( r ); } */
/*   virtual void enable()                  { m_container.enable(); }  */
/*   virtual void disable()                 { m_container.disable(); }  */
/*   virtual Widget myWidget()              { return m_container.myWidget(); } */
/*   virtual void setTabOrder()             { m_container.setTabOrder(); } */
/*   virtual void unsetTabOrder()           { m_container.unsetTabOrder(); } */
/*   virtual void setPanedWindow( FlagStatus s ) */
/*                                          { m_container.setPanedWindow(s); } */
/*   virtual void setFrame( FlagStatus s )  { m_container.setFrame(s); } */
/*   virtual void setUseFrame()             { m_container.setUseFrame(); } */
/*   virtual bool withPanedWindow()         { return m_container.withPanedWindow(); } */
/*   virtual bool withFrame()               { return m_container.withFrame(); } */

/*   virtual void serialize( std::ostream &os ); */

  virtual void attach( GuiElement *e ) = 0; //{ m_container.attach( e ); }
/*   void front( GuiElement *e )  { m_container.front( e ); } */
/*   void setUseRuler()           { m_container.setUseRuler(); } */
/*   void deleteContainer()       { m_container.deleteContainer(); } */

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
/*   virtual void setTitle( const std::string& ); */
/*   virtual Widget myParentWidget() { return m_frame.formWidget(); } */

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
/*   void addGearSpacing( Widget w, int &height, int childs ); */
/*   void getGearMargins( Widget w, int &width, int &height ); */
/*   void alignWithRuler(); */
/*   void install( GuiElement *el ); */
/*   void draw(); */

};

#endif
