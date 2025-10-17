
#if !defined(GUI_QT_FORM_INCLUDED_H)
#define GUI_QT_FORM_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtDialog.h"
#include "gui/GuiContainer.h"
#include "gui/GuiButtonListener.h"
#include "gui/GuiToggleListener.h"
#include "gui/GuiForm.h"

#include "job/JobStarter.h"

#include <QDialog>

class JobFunction;

class GuiButtonbar;
class QBoxLayout;
class QSplitter;
class QWidget;
class QScrollArea;

/** Alle Dialoge werden mit der Klasse GuiForm erstellt. Verschiedene Optionen
    sorgen dafür, dass ein Dialog mit den gewünschten Eigenschaften daherkommt.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtForm.h,v 1.35 2008/07/08 07:21:50 amg Exp $
*/
class GuiQtForm
: public GuiQtElement, // Container
  public GuiForm,
  public GuiQtDialog
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtForm( GuiElement *parent, bool useSettings, const std::string &name="");
  virtual ~GuiQtForm();

/*=============================================================================*/
/* public class Definitions                                                    */
/*=============================================================================*/
  //==============
  // QtDialog
  //==============
  class QtDialog : public QDialog {
  public:
    QtDialog(GuiQtForm *form, QWidget * parent = 0);

  private:
    virtual bool event( QEvent *event );
    virtual void moveEvent ( QMoveEvent * event );
    virtual void showEvent ( QShowEvent * event );
    virtual void timerEvent ( QTimerEvent * event );
#if defined Q_OS_WIN || defined Q_OS_CYGWIN
    void resizeEvent ( QResizeEvent * event );
  public:
	void setHintSize( int w, int h );
#endif
  protected:
    // to catch esc key event
    virtual void reject();
    void closeEvent ( QCloseEvent * e );

  public:
    void startTimerSetMaximumSize();
    void setMaximumSize(bool force=false);
  private:
    GuiQtForm *m_form;
    QSize m_hintSize;
    QSize m_hintSize_resize;
    QString m_screenNumber;
    int   m_timerId;
  };

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiQtForm *form, JobFunction *f )
      : JobStarter( f )
      , m_form( form ){
    }
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult jobResult );
  private:
    GuiQtForm *m_form;
  };

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type();
  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual bool isShown() { return GuiForm::isShown(); }
  virtual bool destroy();
  virtual void resize();
  virtual void update( UpdateReason );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual void getSize( int &x, int &y ); /*eigentlich im Container */
  virtual void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);
  virtual void enable();
  virtual void disable();
  virtual bool replace( GuiElement *old_el, GuiElement *new_el );
  virtual void waitCursor( bool wait, GuiDialog *installer ){
    GuiForm::waitCursor( wait, installer );
  }
  virtual void printMessage( const std::string &, MessageType, time_t delay = 2 );
  virtual void clearMessage();
  /** returns true if created */

  virtual bool isCreated() const;
  virtual void writeSettings();

  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &, int );

  virtual GuiElement  *getElement() { return this; }
  virtual GuiForm     *getForm()    { return this; }
  virtual GuiDialog   *getDialog()  { return this; }
  virtual GuiQtDialog *getQtDialog(){ return this; }
  virtual GuiQtForm   *getQtForm()  { return this; }
  virtual GuiElement::Orientation getDialogExpandPolicy();
  virtual void setMaximumSize(bool force=false);
  void startTimerSetMaximumSize();
  virtual bool isIconic();
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng) {
    return GuiForm::variantMethod(method, jsonArgs, eng);
  }

/*=============================================================================*/
/* public member functions (GuiContainer)                                      */
/*=============================================================================*/
public:
  virtual void attach( GuiElement *e ) { m_elements.push_back(e); }
  virtual void front( GuiElement *e )  { m_elements.insert(m_elements.begin(), e); }
  virtual void setScrollbar( ScrollbarType sb ) { m_elements.setScrollbar( sb ); }

/*=============================================================================*/
/* public member functions (GuiDialog)                                         */
/*=============================================================================*/
public:
  virtual QDialog* getDialogWidget() { return m_dialog; }
  /** Default Verhalten des Dialog überschreiben */
  virtual void setDialogExpandPolicy(DialogExpandPolicy policy);
  virtual std::string getDialogName();
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual QWidget* myWidget();

  void setTitle( const std::string &title ) { GuiForm::setTitle(title); }
  std::string getTitle() { return GuiForm::getTitle(); }
  void setHelpkey( HelpManager::HelpKey *helpkey );
  void attachMenu( GuiElement *menu );
  virtual GuiButtonbar *getButtonbar();
  GuiQtButtonbar *getQtButtonbar();
  void createButtonbar();

  void resetStandardForm() { m_standard_form = false; }
  void setButtonsPerLine( int b ) { m_buttons_per_line = b; }
  void setOptionScrollbar( GuiElement::ScrollbarType sb ) { setScrollbar( sb ); }
  void setOptionPanedWindow( GuiElement::FlagStatus s ) { setPanedWindow( s ); }
  void setOptionUseRuler() { setUseRuler(); }
  virtual void ButtonPressed();
  void ToggleStatusChanged( bool state );

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);

  void setJobFunction( JobFunction *func ){ m_jobFunction = func; }
  void runJobFunction( JobElement::CallReason reason );

  void doEndOfWork( bool error, JobElement::CallReason reason );
  virtual void serializeAttrs( std::ostream &os ){
    m_elements.serializeAttrs( os );
  }
  virtual void serializeAttrs(Json::Value& obj, bool onlyUpdated = false){
    m_elements.serializeAttrs(obj, onlyUpdated);
  }
#if HAVE_PROTOBUF
  virtual void serializeAttrs(in_proto::ElementList *eles, google::protobuf::RepeatedPtrField<in_proto::ElementRef>* reflist, bool onlyUpdated = false){
    m_elements.serializeAttrs(eles, reflist, onlyUpdated);
  }
#endif
  void setSizeProperty();
  bool isStandardForm(){ return m_standard_form; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion erstellt einen neuen GuiButtonListener.
      @return Pointer auf neuen GuiButtonListener.
  */
  virtual GuiButtonListener *createButtonListener();

  /** Diese Funktion erstellt einen neuen GuiToggleListener.
      @return Pointer auf neuen GuiToggleListener.
  */
  virtual GuiToggleListener *createToggleListener();

  /** Diese Funktion wird bei einem DISALLOW aufgerufen. Falls die Form im
      Moment gemanaged ist. wird sie sofort geschlossen.
  */
/*   virtual void isNotAllowed(); */

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class GuiFormCloseListener : public GuiButtonListener
  {
  public:
    GuiFormCloseListener( GuiQtForm *form ): m_form( form ){}
    virtual void ButtonPressed() { m_form->unmanage(); }
    JobAction* getAction(){ return 0; }
  private:
    GuiQtForm  *m_form;
  };

/*
public:
  class GuiFormButtonListener : public GuiButtonListener
  {
  public:
    GuiFormButtonListener( GuiQtForm *form ): m_form( form ){}
    virtual void ButtonPressed() { m_form->ButtonPressed(); }
    JobAction* getAction(){ return 0; }
    GuiQtForm* getForm() { return m_form; }
  private:
    GuiQtForm  *m_form;
  };

private:
*/
  class GuiFormToggleListener : public GuiToggleListener
  {
  public:
    GuiFormToggleListener( GuiQtForm *form ): m_form( form ){}
    virtual void ToggleStatusChanged( bool state ) { m_form->ToggleStatusChanged( state ); }
    JobAction* getAction(){ return 0; }
  private:
    GuiQtForm  *m_form;
  };

  class GuiFormHelpListener : public GuiButtonListener
  {
  public:
    GuiFormHelpListener( HelpManager::HelpKey *key ): m_helpkey( key ){}
    virtual void ButtonPressed() { m_helpkey->show(); }
    JobAction* getAction(){ return 0; }
  private:
    HelpManager::HelpKey *m_helpkey;
  };

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void installCloseButton( GuiQtButtonbar * );
  void setIconPixmap();
  void unsetRunningMode();
  bool setRunningMode();
  void unmanageForm();
  void _manageForm();

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
/*   Widget           m_shell; */
  bool             m_with_cyclebutton;
  int              m_buttons_per_line;
  GuiFormHelpListener  *m_helplistener;
  DialogExpandPolicy    m_expandPolicy;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/

 private:
  bool             m_standard_form;
  QDialog*         m_dialog;
  QWidget*         m_widgetInner;
  QBoxLayout*      m_layoutOuter;
  QBoxLayout*      m_layoutInner;
  JobFunction     *m_jobFunction;
  int              m_jobRunningKey;
  GuiContainer   m_elements;
  int              m_width;
  int              m_height;
  QScrollArea*     m_scrollView;
  bool             m_firstShown;
};

#endif
