
#if !defined(GUI_QTTEXT_INCLUDED_H)
#define GUI_QTTEXT_INCLUDED_H

#include "gui/qt/GuiQtDataField.h"

/** A multi-line input or output element of a datapool variable
*/

#include <qtextedit.h>
#include <QTextBrowser>
#if QT_WEBENGINE
#if QT_VERSION >= 0x050900
#include <QtWebEngineWidgets/QWebEngineView>
#define QWebView QWebEngineView
#else
#include <QWebView>
#endif
#endif
#include "app/HardCopyListener.h"

class QStackedWidget;
class GuiQtSvgView;
class GuiQtPopupMenu;
class QGraphicsItem;
class QTextBrowser;
class QLabel;

class GuiQtText : public GuiQtDataField , public HardCopyListener {

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtText( GuiElement *parent );
  GuiQtText( GuiQtText &text );

  virtual ~GuiQtText(){
  }

  //------------------------------------------------------------------------------
  // HardCopyListener
  //------------------------------------------------------------------------------
  virtual void print(QPrinter* print=0);
  virtual HardCopyListener *getHardCopyListener() { return this; }
  virtual bool write( InputChannelEvent& event );
  virtual bool write( const std::string &fileName );
  virtual HardCopyListener::FileFormat getFileFormat() { return m_fileFormat; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &getName(){ return GuiElement::getName(); }
  virtual const std::string &MenuLabel();
  virtual bool saveFile( GuiElement * );

  //------------------------------------------------------------------------------
  // lokale Hilfsklasse
  //------------------------------------------------------------------------------
  class MyQTextEdit : public QTextEdit {
 public:
    MyQTextEdit( GuiQtText *textField, QWidget * parent = 0, const char * name = 0 )
      : QTextEdit( parent )
      , m_popup_menu( 0 )
      , m_guiQtText( textField ) {
      setTabChangesFocus( true );
      setAutoFormatting( QTextEdit::AutoAll );
    }

    virtual ~MyQTextEdit() {}
    // virtual member functions of QTextEdit
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void contextMenuEvent ( QContextMenuEvent * e );
    virtual void paintEvent ( QPaintEvent * );

  protected:
    void focusInEvent ( QFocusEvent *e ) {
      m_guiQtText->FieldFocusIn();
      QTextEdit::focusInEvent(e);
    }
    void focusOutEvent ( QFocusEvent *e ) {
      m_guiQtText->FinalWork( GuiQtText::reason_LosingFocus );
      QTextEdit::focusOutEvent(e);
    }

  private:
    GuiQtText  *m_guiQtText;
    GuiQtPopupMenu* m_popup_menu;
  };

#if QT_WEBENGINE
#if QT_VERSION < 0x050900 || defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
  class MyQWebView : public QWebView {
#else
  class MyQWebView : public QWebEngineView {
#endif
  public:
    MyQWebView( GuiQtText *textField, QWidget * parent = 0)
#if QT_VERSION < 0x050900 || defined(Q_OS_WIN) || defined(Q_OS_CYGWIN)
      : QWebView( parent )
#else
      : QWebEngineView( parent )
#endif
      , m_guiQtText( textField ) {
    }

    virtual ~MyQWebView() {}
    // virtual member functions of QWebView
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void contextMenuEvent ( QContextMenuEvent * e );
  private:
    GuiQtText  *m_guiQtText;
  };
#endif

#if ! QT_WEBENGINE
  class MyQTextBrowser : public QTextBrowser {
  public:
    MyQTextBrowser( GuiQtText *textField, QWidget * parent = 0)
      : QTextBrowser( parent )
      , m_guiQtText( textField ) {
    }

    virtual ~MyQTextBrowser() {}
    // virtual member functions of QWebView
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void contextMenuEvent ( QContextMenuEvent * e );
  private:
    GuiQtText  *m_guiQtText;
  };
#endif

  /*=============================================================================*/
  /* public member functions of GuiElement                                       */
  /*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Text; }
  /*   virtual void create( Widget parent ); */
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void enable();
  virtual void disable();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason );
  virtual QWidget *myWidget();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtText( *this ); }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList *eles, bool onlyUpdate = false);
#endif

/*=============================================================================*/
/* public member functions of GuiField                                         */
/*=============================================================================*/
public:
  virtual bool setLength( int len );
  virtual int getLength();
  virtual bool setPrecision( int prec ); // Lines
  virtual int getPrecision() { return m_lines; }
  virtual bool setThousandSep();
  virtual GuiQtDataField *CloneForFieldgroupTable();
  virtual void setScrollbar( ScrollbarType sb );
  virtual void setAutoScroll();
  virtual void setExpandable( FlagStatus status );
  virtual void doEndOfWork( bool error, bool updateForms=true );

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
  virtual void lock();
  virtual void unlock();
  virtual void switchlock();
  virtual void edit();
  virtual void protect();

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
public:
  class PrintListener : public GuiMenuButtonListener
  {
  public:
    PrintListener( GuiQtText *text ): m_text( text ) {}
    virtual ~PrintListener() {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtText  *m_text;
  };

  class SaveListener : public GuiMenuButtonListener
  {
  public:
    SaveListener( GuiQtText *text ): m_text( text ) {}
    virtual ~SaveListener() {}
    virtual void ButtonPressed(){ m_text->saveText(); }
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtText  *m_text;
  };


  class SaveFileListener : public FileSelectListener{
  public:
    SaveFileListener( GuiQtText *text ): m_text( text ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat format
			       , const std::string dir );
  private:
    GuiQtText  *m_text;
  };

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  enum CB_Reason
  { reason_None = 0
  , reason_LosingFocus
  , reason_Activate
  };

  void setColors();

  XferParameter::InputStatus checkFormat();
  void FinalWork( CB_Reason );
  void FinalWorkOk();

  void FieldFocusIn();

  void saveText();

  Q_OBJECT
private slots:
    void slot_svgViewMousePressed(const QGraphicsItem* item, const QPointF& posScene);
    void slot_copyAvailable( bool sel );

  // -----------------------------------------------------
  // private definitions
  // -----------------------------------------------------
private:
  class Trigger : public JobStarter
  {
  public:
  Trigger(GuiQtText *textfield,  JobFunction *f)
    : JobStarter( f )
    , m_textfield(textfield)
    {}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt ){
      m_textfield->doEndOfWork(rslt != JobAction::job_Ok);
    }
  private:
    GuiQtText  *m_textfield;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  MyQTextEdit    *m_textedit;
#if QT_WEBENGINE
  MyQWebView     *m_webview;
#else
  MyQTextBrowser *m_webview;
#endif
  GuiQtSvgView   *m_svgView;
  QLabel         *m_label;
  QStackedWidget *m_widgetStack;
  PrintListener    m_print_listener;
  SaveListener     m_save_listener;
  SaveFileListener m_save_file_listener;

  int          m_length;
  int          m_lines;
  bool         m_scrollbars;
  bool         m_autoscroll;
  bool         m_expandable;
  CB_Reason    m_CB_Reason;
  HardCopyListener::FileFormat m_fileFormat;

  friend class GuiQtSvgView;
};

#endif
