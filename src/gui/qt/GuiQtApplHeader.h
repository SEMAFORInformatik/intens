
#if !defined(GUI_QT_APPLHEADER_INCLUDED_H)
#define GUI_QT_APPLHEADER_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiApplHeader.h"

/* #include <qhbox.h> */
/* class QHBox; */


/** Dieses Objekt wird als erstes Element in der Mainform platziert. Es dient
    ausschliesslich als Dekoration. Es besteht aus zwei Labels für die Ueberschrift.
    Links und rechts davon ist jeweils ein Label zum Platzieren einer Pixmap
    vorhanden. Der Inhalt dieser Labels wird im Resourcefile gesetzt.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtApplHeader.h,v 1.5 2006/03/08 14:07:52 amg Exp $
 */
class GuiQtApplHeader : public GuiApplHeader, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtApplHeader( GuiElement *parent )
    : GuiQtElement( parent )
    , m_myWidget( 0 ){
/*     , m_width( 0 ) */
/*     , m_height( 0 ){ */
  }
  virtual ~GuiQtApplHeader(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  /** Beschreibung siehe GuiElement.
   */
  virtual GuiElement::ElementType Type() { return GuiElement::type_Header; }

  /** Beschreibung siehe GuiElement.
  */
/*   virtual void create( Widget parent ); */
  virtual void create();

  /** Beschreibung siehe GuiElement.
  */
  virtual void manage();

  /** Beschreibung siehe GuiElement.
  */
  virtual void unmanage();

  /** Beschreibung siehe GuiElement.
  */
  virtual void map();

  /** Beschreibung siehe GuiElement.
  */
  virtual void unmap();

  /** Beschreibung siehe GuiElement.
   */
  virtual void getSize( int &x, int &y );

  /** Beschreibung siehe GuiElement.
   */
  virtual void update( UpdateReason ) {}

  /** Beschreibung siehe GuiElement.
   */
  virtual QWidget *myWidget() { return m_myWidget; }
  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();
  /** Beschreibung siehe GuiElement.
   */

  virtual const std::string getTitle() { return m_title; }
  virtual const std::string getSubTitle() { return m_subtitle; }
  virtual void serializeXML(std::ostream &os, bool recursive);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  /** Beschreibung siehe GuiElement.
   */
  virtual GuiElement * getElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Beschreibung siehe auch GuiToggleListener. Die Funktion ruft je nach
      Zustand des Parameters pressed die Funktion manage() (true) oder
      unmanage() auf.
      @param pressed True => der ToggleButton ist gedrückt.
   */
  virtual void ToggleStatusChanged( bool pressed );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
/*   Widget          *m_formwidget; */
/*   int              m_width; */
/*   int              m_height; */
  std::string       m_title;
  std::string       m_subtitle;
  QWidget          *m_myWidget;

};

#endif
