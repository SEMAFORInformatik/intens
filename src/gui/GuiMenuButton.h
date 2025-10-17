
#if !defined(GUI_MENUBUTTON_INCLUDED_H)
#define GUI_MENUBUTTON_INCLUDED_H

#include <string>
#include "jsoncpp/json/json.h"
#if HAVE_PROTOBUF
#include "protobuf/Message.pb.h"
#endif

class GuiElement;
class GuiEventData;
class GuiButtonListener;
class JobEngine;

/** The GuiMenuButton is a Pushbutton, which can be included in a PulldownMenu.
    It delegates the input event handling to a GuiButtonListener and
    can be controlled by GuiListenerController.
 */
class GuiMenuButton
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiMenuButton(){
  }
  virtual ~GuiMenuButton(){
  }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit der Funktion setLabel() wird der Text des Labels des Buttons gesetzt.
      @param text Label-Text des Buttons
   */
  virtual bool setLabel( const std::string &text ) = 0;

  /** Mit der Funktion getLabel() wird der Text des Labels des Buttons zurückgegeben.
   */
  virtual const std::string getLabel() = 0;

  /** Mit der Funktion setDialogLabel() wird der Text des Labels des Buttons gesetzt.
      Dem Label werden drei Punkte angehängt zum Zeigen, das durch das Aktivieren
      des Buttons eine weitere Dialog-Form erscheint.
      @param text Label-Text des Buttons
   */

  virtual void setDialogLabel( const std::string &text ) = 0;
  /** Mit der Funktion setAccelerator() kann für das Aktivieren ein Shortcut
      installiert werden.
      @param acc_key Zeichenfolge des Shortcuts
      @param acc_text Text des Shortcuts
  */
  virtual void setAccelerator( const std::string &acc_key, const std::string &acc_text ) = 0;
  virtual GuiElement * getElement() = 0;
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);
protected:
  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  virtual GuiButtonListener* getButtonListener() = 0;
};

#endif
