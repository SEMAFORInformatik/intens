
#if !defined(GUI_QTPIXMAP_INCLUDED_H)
#define GUI_QTPIXMAP_INCLUDED_H

#include "streamer/Stream.h"
#include "gui/GuiPixmap.h"
#include "gui/qt/GuiQtDataField.h"

class QStackedWidget;
class QWebView;
class GuiQtSvgView;
class QSvgWidget;
class QWidget;
class QLabel;
#include <qpixmap.h>


class GuiQtPixmap : public GuiPixmap,  public GuiQtDataField, public BasicStream
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtPixmap( GuiElement *parent );
  virtual ~GuiQtPixmap();

protected:
  GuiQtPixmap( const GuiQtPixmap &pixmap );

/*=============================================================================*/
/* public member functions of GuiPixmap                                       */
/*=============================================================================*/
public:
  virtual GuiElement *getElement() {return this;}

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );
  virtual GuiElement::ElementType Type() { return GuiElement::type_Pixmap; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void setSize( int w, int h );
  virtual void getSize( int &w, int &h, bool hint=true);
  virtual void update( UpdateReason );
  virtual QWidget *myWidget();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtPixmap( *this ); }
  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual  GuiElement::Orientation getDialogExpandPolicy();

  virtual void serializeXML(std::ostream &os, bool recursive = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void setExpandable( FlagStatus status );

/*=============================================================================*/
/* public member functions of GuiDataField                                     */
/*=============================================================================*/
public:
  // Default length in GuiQtDataField changed to 0
  // For GuiQtPixmap, it is never changed,
  // so getLength would return 0 which
  // would cause the GuiQtPixmap not to be created
  virtual int getLength(){ return 1; } // visible flag

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() {}
  virtual void confirmNoButtonPressed() {}

  /** Basisfunktion für den ChannelListener
   */
  virtual bool write( InputChannelEvent &event );
  virtual bool read( OutputChannelEvent & );
  virtual void resetContinuousUpdate(){};
  virtual bool setContinuousUpdate( bool flag ){ return false; };
  virtual int getParameters( std::ostream &os ) const { return 0; }
  virtual void getValues( StreamSource *src ) {}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  QStackedWidget *m_widgetStack;
#if QT_VERSION < 0x050900
  QWebView       *m_webview;
#else
  QSvgWidget     *m_webview;
#endif
  QLabel         *m_myLabel;

  bool        m_expandable;
  int         m_width;
  int         m_height;
};

#endif
