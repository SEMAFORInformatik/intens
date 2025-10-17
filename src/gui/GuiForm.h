
#if !defined(GUI_FORM_INCLUDED_H)
#define GUI_FORM_INCLUDED_H

#include <iostream>

#include "app/HelpManager.h"
#include "gui/GuiListenerController.h"
#include "gui/GuiElement.h"

class GuiElement;
class GuiButtonbar;
class GuiButtonListener;
class GuiToggleListener;
class GuiDialog;
class GuiListenerController;
class JobFunction;

/** Alle Dialoge werden mit der Klasse GuiForm erstellt. Verschiedene Optionen
    sorgen dafür, dass ein Dialog mit den gewünschten Eigenschaften daherkommt.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiForm.h,v 1.33 2005/11/25 13:22:58 amg Exp $
*/
class GuiForm : public GuiListenerController{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiForm()
    : m_hasCloseButton(true)
    , m_useSettings(true)
    , m_with_cyclebutton(true)
    , m_is_popped_up( false )
    , m_main( false ) {
  }
  virtual ~GuiForm(){
  }

  enum DialogExpandPolicy
  { expand_Default = 0
  , expand_AtMapTime
  };
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type();
  void waitCursor( bool wait, GuiDialog *installer );
  void setTitle( const std::string & );
  std::string getTitle();
  virtual void setShown(bool value);
  virtual bool setMain();
  virtual bool isMain() { return m_main; }
  virtual void setHelpkey( HelpManager::HelpKey *helpkey ) = 0;
  virtual void attachMenu( GuiElement *menu ) = 0;
  virtual GuiButtonbar *getButtonbar() = 0;
  virtual void createButtonbar() = 0;
  bool hasCyclebutton(){ return m_with_cyclebutton; }
  void resetCycleButton(){ m_with_cyclebutton=false; }
  virtual void resetStandardForm() = 0;
  virtual void setButtonsPerLine( int b ) = 0;
  virtual void setOptionScrollbar( GuiElement::ScrollbarType sb ) = 0;
  virtual void setOptionPanedWindow( GuiElement::FlagStatus s ) = 0;
  virtual void setOptionUseRuler() = 0;
  virtual void setDialogExpandPolicy(DialogExpandPolicy policy) = 0;
  virtual void setJobFunction( JobFunction *func ) = 0;
  virtual GuiElement *getElement() = 0;
  virtual GuiDialog *getDialog() = 0;
  /** returns true if created
   */
  virtual bool isCreated() const = 0;

  // GuiListenController methods
  GuiListenerController *getListenerController() { return this; }
  virtual GuiButtonListener *getFormButtonListener() { return getButtonListener(); }
  virtual GuiToggleListener *getFormToggleListener() { return getToggleListener(); }
  virtual void setFormMenuInstalled() { setMenuInstalled(); }
  virtual void ButtonPressed() = 0;

  void serializeXML(std::ostream &os, bool recursive = false);
  void serializeJson(Json::Value& jsonObj, bool recursive=false);
#if HAVE_PROTOBUF
  void serializeProtobuf(in_proto::ElementList *eles, bool recursive=false);
  virtual void serializeAttrs(in_proto::ElementList *eles, google::protobuf::RepeatedPtrField<in_proto::ElementRef>* reflist, bool onlyUpdate = false)=0;
#endif
  virtual void serializeAttrs( std::ostream &os )=0;
  virtual void serializeAttrs(Json::Value& obj, bool recursive = false)=0;

  static GuiForm* currentForm(){
    if( s_managedForms.empty() )
      return 0;
    return s_managedForms.back();
  }

  void hasCloseButton( bool cb ){
    m_hasCloseButton=cb;
  }
  void useSettings( bool us ){
    m_useSettings=us;
  }
  bool isShown()  { return m_is_popped_up; }
  virtual bool isIconic()  = 0;
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,JobEngine *eng);

/*=============================================================================*/
/* public class Definitions                                                    */
/*=============================================================================*/
public:
  class GuiFormButtonListener : public GuiButtonListener
  {
  public:
    GuiFormButtonListener( GuiForm *form ): m_form( form ){}
    virtual void ButtonPressed() { m_form->ButtonPressed(); }
    JobAction* getAction(){ return 0; }
    GuiForm* getForm() { return m_form; }
  private:
    GuiForm  *m_form;
  };

protected:
  bool hasCloseButton(){
    return m_hasCloseButton;
  }
  bool useSettings(){
    return m_useSettings;
  }
  void registerIndex();
  void unregisterIndex();

protected:
  static std::vector<GuiForm*> s_managedForms;
private:
  bool             m_main;
  bool             m_hasCloseButton;
  bool             m_with_cyclebutton;
  bool             m_is_popped_up;
  bool             m_useSettings;
};

#endif
