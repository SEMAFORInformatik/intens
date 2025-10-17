
#if !defined(GUI_QT_ORIENTATION_CONTAINER_INCLUDED_H)
#define GUI_QT_ORIENTATION_CONTAINER_INCLUDED_H

#include "gui/GuiOrientationContainer.h"
#include "gui/GuiContainer.h"
#include "gui/qt/GuiQtElement.h"

#include <QScrollArea>
#include <QSplitter>
#include <QGroupBox>

/** Der GuiQtOrientationContainer ist ein GuiCointainer-Objekt, welches eine Liste von
    GuiElementen horizontal  anordnet und gegebenenfalls gegeneinander ausrichtet.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtOrientationContainer.h,v 1.18 2006/08/03 12:30:22 amg Exp $
*/
class GuiQtOrientationContainer : public GuiOrientationContainer, public GuiQtElement
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtOrientationContainer( Orientation orientation, GuiElement *parent );
  virtual ~GuiQtOrientationContainer(){}
  void setVertical( bool b ) { m_vertical=b;}
  Qt::Orientation getQtOrientation();
  GuiElement::Orientation getOrientation() { return m_orientation; }

private:
  GuiQtOrientationContainer( const GuiQtOrientationContainer &cont ); // Copy Constructor

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual void create();
  virtual void resize();
  virtual void manage();
  virtual bool destroy();
  virtual bool replace( GuiElement *old_el, GuiElement *new_el );
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;
  virtual GuiElement *getElement() { return this; }
  virtual void setTitle( const std::string& t ) { m_title=t; };
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

/*=============================================================================*/
/* public member functions of GuiContainer                                     */
/*=============================================================================*/
  virtual GuiElement::ElementType Type() { return m_container.Type(); }
  virtual void getSize( int &x, int &y ); // { m_container.getSize( x, y ); }
  virtual void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);
  virtual void unmanage()                { m_container.unmanage( ); }
  virtual void update( UpdateReason r)   { m_container.update( r ); }
  virtual void enable()                  { m_container.enable(); }
  virtual void disable()                 { m_container.disable(); }
  virtual QWidget* myWidget(); //              { return m_container.myWidget(); }
  virtual void setScrollbar( ScrollbarType sb );
  virtual void setScrollbar( Orientation o, ScrollbarType sb );
  virtual void setPanedWindow( FlagStatus s )
                                         { m_container.setPanedWindow(s); }
  virtual void setFrame( FlagStatus s )  { m_container.setFrame(s); }
  virtual void setUseFrame()             { m_container.setUseFrame(); }
  virtual bool withScrollbars()          { return m_container.withScrollbars(); }
  virtual bool withPanedWindow()         { return m_container.withPanedWindow(); }
  virtual bool withFrame()               { return m_container.withFrame(); }
  virtual bool setPixmap( const std::string &pm , bool withLabel=false ) { m_pixmap = pm; return true; }
  const std::string getPixmap() { return m_pixmap; }

  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
  virtual bool serializeProtobuf(in_proto::ElementList* eles, in_proto::Folder::Page* page, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);

  void setUseRuler()           { m_container.setUseRuler(); }
  void attach( GuiElement *e )           { m_container.front( e ); }
  void front( GuiElement *e )            { m_container.front( e ); }
  int countElements()            { return m_container.size(); }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  GuiContainer &container(){ return m_container; }
  Qt::ScrollBarPolicy getHorizontalScrollbarPolicy() const;
  Qt::ScrollBarPolicy getVerticalScrollbarPolicy() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void install( GuiElement *el );

/*=============================================================================*/
/* Private classes                                                             */
/*=============================================================================*/
class MyQSplitter : public QSplitter {
public:
  MyQSplitter(Qt::Orientation, GuiQtElement *e );
  virtual ~MyQSplitter();
  virtual void enterEvent ( QEnterEvent *e );
  virtual void leaveEvent ( QEvent *e );
private:
  GuiQtElement *m_element;
};

class MyQScrollArea : public QScrollArea {
public:
  MyQScrollArea(GuiQtElement *e );
  virtual ~MyQScrollArea();
  virtual void enterEvent ( QEnterEvent *e );
  virtual void leaveEvent ( QEvent *e );
private:
  GuiQtElement *m_element;
};

class MyQGroupBox : public QGroupBox {
public:
  MyQGroupBox(GuiQtElement *e );
  virtual ~MyQGroupBox();
  virtual void enterEvent ( QEnterEvent *e );
  virtual void leaveEvent ( QEvent *e );
private:
  GuiQtElement *m_element;
};

class MyQWidget : public QWidget {
public:
  MyQWidget(GuiQtElement *e );
  virtual ~MyQWidget();
  virtual void enterEvent ( QEnterEvent *e );
  virtual void leaveEvent ( QEvent *e );
private:
  GuiQtElement *m_element;
};

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiContainer   m_container;
  QWidget         *m_frame;
  QWidget         *m_scrollview;
  QWidget         *m_splitter;
  std::string      m_pixmap;
  bool             m_vertical;
  Orientation      m_orientation;
  std::string      m_title;
  std::vector<GuiElement*> m_clonedList;
};

#endif
