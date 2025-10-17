
#if !defined(GUI_QT_SCROLLEDTEXT_INCLUDED_H)
#define GUI_QT_SCROLLEDTEXT_INCLUDED_H

#include<QTextEdit>
#include<QEvent>

#include "gui/qt/GuiQtDataField.h"
#include "gui/GuiFactory.h"
#include "gui/GuiMenuButtonListener.h"
#include "gui/GuiScrolledText.h"
#include "app/HardCopyListener.h"
#include "gui/FileSelectListener.h"

class InputChannelEvent;
class GuiQtPopupMenu;

class GuiQtScrolledText : public GuiQtDataField
                        , public HardCopyListener
                        , public GuiScrolledText
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtScrolledText( GuiElement *parent, const std::string &name );
  virtual ~GuiQtScrolledText();

  Q_OBJECT
 signals:
  void writeTextSignal(const std::string& text);
private slots:
  void writeTextSlot(const std::string& text);

public:
  class MyQTextEdit : public QTextEdit {
  public:
    MyQTextEdit(GuiQtScrolledText *st)
      : QTextEdit()
      , m_scrolledText(st)
      , m_popup_menu( 0 ) {}
    virtual QSize minimumSizeHint() const;
    void setMinimumSizeHint(QSize msh);

  private:
    virtual void contextMenuEvent ( QContextMenuEvent* e );

    GuiQtScrolledText*  m_scrolledText;
    GuiQtPopupMenu*  m_popup_menu;

    QSize m_mhs;
  };


/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_ScrolledText; }
  virtual void create();
  virtual void manage();
  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate = false);
#endif

  virtual GuiElement *getElement() { return this; }
/*   virtual GuiQtScrolledText *getQtScrolledText() { return this; } */
  virtual HardCopyListener *getHardCopyListener() { return this; }

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed() { assert( false ); }
  virtual void confirmNoButtonPressed() { assert( false ); }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
/*   static void addActions(); */
  void setDirname( const std::string &dirname ) { m_dir = dirname; }
  virtual bool setLength( int len ) { m_length = len; return true; }
  virtual void setLines( int lines ) { m_lines = lines; }
  virtual void setWordwrap( bool wrap ) { m_wordwrap = wrap; }
  virtual bool setLabel( const std::string &label );
  virtual void setFormat( Format fmt ) { m_format = fmt; }
  virtual void setMaxLines( int maxlines ) { m_maxlines = maxlines; }
  bool createDataReference( const std::string &name );

  virtual void setOptionLength( int len ) { setLength( len ); }
  virtual int getOptionLength() { return m_length; }
  virtual void setOptionLines( int lines ) { setLines( lines ); }
  virtual int getOptionLines() { return m_lines; }
  virtual bool setOptionLabel( const std::string &label ) { return setLabel( label ); }
  virtual void setOptionWordwrap( bool wrap ) { setWordwrap( wrap ); }
  virtual void setOptionFormat( Format fmt ) { setFormat( fmt ); }
  virtual void setOptionMaxLines( int maxlines ) { setMaxLines( maxlines ); }
  virtual void setOptionPrintFilter( const std::string &filter ) { setPrintFilter( filter ); }

  /** write message to window */
  virtual void writeText( const std::string & );
  /** write message to window (threadsafe) */
  virtual void writeTextFromSubthread( const std::string & );
/*   void writeNewLine(); */
  virtual void clearText();
  void saveText();
  //  void writeFile( const std::string &filename );
  virtual GuiMenuButtonListener *getSaveListener();

  // HardCopyListener
  virtual HardCopyListener::FileFormat getFileFormat() { return m_fileFormat; }
  virtual bool isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat );
  virtual bool isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat );
  virtual PrintType getPrintType( const HardCopyListener::FileFormat &fileFormat );
  virtual const std::string &MenuLabel();
  virtual void print(QPrinter* print=0);

  virtual const std::string &getName(){ return m_name; }
  virtual bool isEmpty();
  virtual bool getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &fileFormat,
				   double &lm, double &rm,
				   double &tm, double &bm );
  virtual bool write( InputChannelEvent& event );
  virtual bool write( const std::string &fileName ); //{ writeFile(fileName); return true; }
  virtual bool saveFile( GuiElement * );

  virtual QProcess* getQProcess() { assert(false); return NULL;};

/*=============================================================================*/
/* private Definitions                                                         */
/*=============================================================================*/
private:
  class PrintListener : public GuiMenuButtonListener
  {
  public:
    PrintListener( GuiQtScrolledText *text ): m_text( text ) {}
    virtual ~PrintListener() {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtScrolledText  *m_text;
  };

  class PreviewListener : public GuiMenuButtonListener
  {
  public:
    PreviewListener( GuiQtScrolledText *text ): m_text( text ) {}
    virtual ~PreviewListener() {}
    virtual void ButtonPressed();
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtScrolledText  *m_text;
  };

  class ClearListener : public GuiMenuButtonListener
  {
  public:
    ClearListener( GuiQtScrolledText *text ): m_text( text ) {}
    virtual ~ClearListener() {}
    virtual void ButtonPressed(){ m_text->clearText(); }
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtScrolledText  *m_text;
  };

  class SaveListener : public GuiMenuButtonListener
  {
  public:
    SaveListener( GuiQtScrolledText *text ): m_text( text ) {}
    virtual ~SaveListener() {}
    virtual void ButtonPressed(){ m_text->saveText(); }
    virtual JobAction *getAction(){ return 0; }
  private:
    GuiQtScrolledText  *m_text;
  };

  class SaveFileListener : public FileSelectListener{
  public:
    SaveFileListener( GuiQtScrolledText *text ): m_text( text ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat format
			       , const std::string dir );
  private:
    GuiQtScrolledText  *m_text;
  };

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
/* private: */
  void convertFortran( std::string &text );
  int newLines( const std::string &text );
  void setText( const std::string &text );
  void appendText( const std::string &text );
  void setCharPerLine( const std::string &text );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string    m_dir;
  MyQTextEdit*   m_textwidget;
  std::string    m_label;
  std::string    m_name;
  int            m_length;
  int            m_lines;
  bool           m_wordwrap;
  Format         m_format;
  int            m_charsperline;
  int            m_numlines;
  int            m_maxlines;
  PrintListener    m_print_listener;
  PreviewListener  m_preview_listener;

  ClearListener    m_clear_listener;
  SaveListener     m_save_listener;
  SaveFileListener m_save_file_listener;
  TransactionNumber   m_lastUpdate;  // prevents to many updates
  HardCopyListener::FileFormat m_fileFormat;
};

#endif
