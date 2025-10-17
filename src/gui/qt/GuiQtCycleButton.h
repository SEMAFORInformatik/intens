
#if !defined(GUI_QT_CYCLEBUTTON_INCLUDED_H)
#define GUI_QT_CYCLEBUTTON_INCLUDED_H

#include <qobject.h>

#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtCycleDialog.h"
#include "gui/GuiCycleButton.h"

class GuiButtonListener;

class QWidget;
class QLabel;

class GuiQtCycleButton : public GuiQtElement, public GuiCycleButton
{

  Q_OBJECT

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtCycleButton( GuiElement *parent )
    : GuiQtElement( parent )
    , m_cyclewidget( 0 )
    , m_labelwidget( 0 )
    , m_width( 0 )
    , m_height( 0 )
    , m_inputBox( 0 ) {
#if defined(GUI_STANDALONE_TEST)
    m_helptext = "GuiQtCycleButton";
#endif
  }
  virtual ~GuiQtCycleButton(){
  }

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_CycleButton; }
  virtual void create();
  virtual void manage();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason );
  virtual QWidget * myWidget();
  virtual void addTabGroup();
  virtual bool hasHelptext();
  virtual bool showHelptext();
  virtual void getHelptext( std::string &text );
  virtual GuiElement *getElement(){ return this; }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  private slots:
    void cycleNew();
    void cycleDown();
    void cycleUp();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void updateSize( QWidget *w, int spacing );
  void updateLabel( int num );
  virtual std::string getLabel( int num );

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  QWidget           *m_cyclewidget;
  QLabel          *m_labelwidget;
  int              m_width;
  int              m_height;

  GuiQtCycleInputBox* m_inputBox;

};

#endif
