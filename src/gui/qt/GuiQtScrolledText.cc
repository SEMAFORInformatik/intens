#include <fstream>
#include <sys/stat.h>

#include <sstream>

#include<qmessagebox.h>
#include<QMenu>
#include <QContextMenuEvent>
#include <QCursor>
#include <QPrintDialog>
#include <QFileDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QApplication>
#if QT_VERSION > 0x050600
#include <QScreen>
#else
#include <QDesktopWidget>
#endif

#include "app/DataPoolIntens.h"
#include "app/AppData.h"
#include "app/ReportGen.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/GuiForm.h"
#include "gui/GuiFactory.h"
#include "utils/gettext.h"
#include "utils/Debugger.h"

INIT_LOGGER();

/*******************************************************************************/
/* static variables                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiQtScrolledText::GuiQtScrolledText( GuiElement *parent, const std::string &name )
  : GuiQtDataField( parent, name )
    , m_textwidget( 0 )
    , m_label( name )
    , m_name ( name )
    , m_length( 40 )
    , m_lines( 5 )
    , m_wordwrap( false )
    , m_format( format_None )
    , m_charsperline( 0 )
    , m_numlines( 0 )
    , m_maxlines( 100 )
    , m_print_listener( this )
    , m_preview_listener( this )
    , m_clear_listener( this )
    , m_save_listener( this )
    , m_save_file_listener( this )
    , m_lastUpdate( 0 )
    , m_fileFormat( HardCopyListener::Text ){
  m_maxlines=AppData::Instance().MaxLines();
}

GuiQtScrolledText:: ~GuiQtScrolledText() {}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledText::create(){
  BUG(BugGui,"GuiQtScrolledText::create");
  m_textwidget = new MyQTextEdit( this );

  // set font
  QFont font = m_textwidget->font();
  m_textwidget->setFont( QtMultiFontString::getQFont( "@text@", font ) );

  // further settings
  m_textwidget->setReadOnly( true );
  if (m_wordwrap)
    m_textwidget-> setLineWrapMode ( QTextEdit::WidgetWidth);
  else
    m_textwidget-> setLineWrapMode ( QTextEdit::NoWrap );

  // groesse setzen
  int length = m_textwidget->fontMetrics().horizontalAdvance( 'W' ) * (1 + m_textwidget->frameWidth() +  m_length);
  int lines  = m_textwidget->fontMetrics().height()     * (1 + m_lines);
  m_textwidget->setMinimumSizeHint(QSize(length, lines));

  // connect
  qRegisterMetaType<std::string>("std::string");
  connect(this, SIGNAL(writeTextSignal(const std::string&)),
   		  this, SLOT(writeTextSlot(const std::string&)),
   		  Qt::QueuedConnection);
}

/* --------------------------------------------------------------------------- */
/* setLabel --                                                                 */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledText::setLabel( const std::string &label ) {
  m_save_listener.setLabel( label );
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::getSize( int &w, int &h ){
  int height = 0;
  int width  = 0;
  if( m_textwidget == 0 ){
    w = h = 0;
    return;
  }
  QSize  hs   =  m_textwidget->sizeHint();
  QSize  mhs   =  m_textwidget->minimumSizeHint();
  h = m_textwidget->height();
  w = m_textwidget->width();
  h = hs.height() < h ? h : hs.height();
  w = hs.width()  < w ? w : hs.width();
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* GuiQtScrolledText::myWidget() {
  return m_textwidget;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::manage(){
  if (m_textwidget)
    m_textwidget->show();
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::update( UpdateReason reason ){
  BUG(BugGui,"GuiQtScrolledText::update");
  if( m_param == 0 ){
    return;
  }
  switch( reason ){
  case reason_FieldInput:
  case reason_Cancel:
    return;
  case reason_Process:
    if( !m_param->isUpdated( m_lastUpdate+1, true ) ){
      return;
    }
    break;
  case reason_Cycle:
  case reason_Always:
    setUpdated();
    break;
  default:
    BUG_MSG("Unhandled Update Reason");
    break;
  }

  std::string str;
  m_param->getFormattedValue( str );
  m_numlines = newLines( str );

  if( m_format == format_Fortran ){
    setCharPerLine( str );
  }
  setText( str );
}

/* --------------------------------------------------------------------------- */
/* convertFortran --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::convertFortran( std::string &text ){
  int i = 0;
  int len = (int)text.size();

  for( i=0; i < len; i++ ){
    if( m_charsperline == 0 ){
      switch( text[i] ){
      case '1':
	text[i] = '\f';
	break;
      case '0':
	text[i] = '\n';
	break;
      default:
	break;
      }
      m_charsperline++;
    }
    else {
      if( text[i]=='\n' )
	m_charsperline = 0;
      else
	m_charsperline++;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setCharPerLine --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::setCharPerLine( const std::string &text ){
  int len = (int)text.size();
  m_charsperline = 0;
  for( int i=len-1; i>= 0; --i ){
    if( text[i] == '\n' ){
      return;
    }
    m_charsperline++;
  }
}

/* --------------------------------------------------------------------------- */
/* newLines --                                                                 */
/* --------------------------------------------------------------------------- */

int GuiQtScrolledText::newLines( const std::string &text ){
  int len = (int)text.size();
  if (len == 0) return 0;
  int lines = 0;
  if( len > 0 ) {
    ++lines;
    std::string::size_type found=-1;
    while ((found=text.find('\n', found+1)) !=std::string::npos)
      ++lines;
  }
  return (text[len-1] == '\n') ? lines-1 : lines;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::setText( const std::string &text ){
  if( m_textwidget != 0 ){
    QString s = QString::fromStdString(text);
    while (s.size() && QChar(s.right(1).at(0)).isSpace()) {
      s=s.remove(s.size()-1,1);
    }
    m_textwidget->clear();
    m_textwidget->append( s );
  }
}

/* --------------------------------------------------------------------------- */
/* appendText --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::appendText( const std::string &text ){
  if( m_textwidget != 0 ){
    QString s = QString::fromStdString(text);
    while (s.size() && QChar(s.right(1).at(0)).isSpace()) {
      s=s.remove(s.size()-1,1);
    }
    m_textwidget->append( s );
  }
}

/* --------------------------------------------------------------------------- */
/* writeTextSlot --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::writeTextSlot( const std::string &str ){
  writeText( str );
}

/* --------------------------------------------------------------------------- */
/* writeText --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::writeText( const std::string &str ){
  std::string text = str;
  BUG_DEBUG(getName() << ":: Text: " << str);

  if( m_format == format_Fortran ){
    convertFortran( text );
  }

  // Ein ScrolledText ohne DataPool-Anschluss
  if( m_param == 0 ){
    appendText( text );
    return;
  }

  std::string wintext;
  getValue( wintext );
//   // add a std::endl if datapool string ends without one
//   if (wintext[wintext.size()-1] != '\n' && text[0] != '\n')
//     wintext.push_back('\n');
  wintext += text;

   if( m_maxlines > 0 ){
     m_numlines = newLines( wintext );
     if( m_numlines > m_maxlines ){

       // Overflow. Es hat zuviele Linien
       size_t len = wintext.size();
       int lines = m_numlines - m_maxlines * 0.8;

       std::string::size_type found=-1;
       while ((found=wintext.find('\n', found+1)) !=std::string::npos) {
	 --lines;
	 if( lines <= 0 ){
	   text = wintext.substr( found+1, len );
	   setText( text );
	   setValue( text );
	   m_lastUpdate = DataPoolIntens::CurrentTransaction();
	   return;
	 }
       }
     }
  }

  appendText( text );
  setValue( wintext );
  m_lastUpdate = DataPoolIntens::CurrentTransaction();
}

/* --------------------------------------------------------------------------- */
/* writeTextFromSubthread --                                                   */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::writeTextFromSubthread( const std::string &str ){
  emit writeTextSignal(str);
}

/* --------------------------------------------------------------------------- */
/* clearText --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::clearText(){
   if( m_textwidget != 0 ){
     m_textwidget->clear();
   }
  m_numlines       = 0;

  if( m_param != 0 ){
    m_param->clear();
  }
}

/* --------------------------------------------------------------------------- */
/* getSaveListener --                                                          */
/* --------------------------------------------------------------------------- */

GuiMenuButtonListener *GuiQtScrolledText::getSaveListener(){
  if( !m_label.empty() ){
    m_save_listener.setLabel( MenuLabel() );
  }
  return &m_save_listener;
}

/* --------------------------------------------------------------------------- */
/* saveText --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::saveText(){
  BUG(BugGui,"GuiQtScrolledText::saveText");
  std::vector<HardCopyListener::FileFormat> formats;
  ReportGen::getFormats( this, formats );
  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"),MenuLabel())
      , "ASCII (*.txt)", m_dir
      , &m_save_file_listener
      , DialogFileSelection::Save
      , &formats
      , DialogFileSelection::AnyFile
      , getElement()->getName());
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::write( const std::string &filename ){
  std::string wintext;
  getValue( wintext );
  if( wintext.empty() ){
    return false;
  }

  std::ofstream fs( filename.c_str() );
  if( !fs.is_open() ){
    return false;
  }

  size_t len = wintext.size();
  for( size_t i = 0; i < len; i++ ){
    fs << wintext[i];
  }
  fs.close();
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::write( InputChannelEvent& event ){
  std::string wintext;
  m_param->getFormattedValue( wintext );
  if( wintext.size() ) {
  std::ostringstream ostr(wintext);
  return event.write( ostr );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::saveFile( GuiElement *e ){
  saveText();
  return true;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledText::isFileFormatSupported( const FileFormat &fileFormat ){
  return ( m_fileFormat == fileFormat );
}

/* --------------------------------------------------------------------------- */
/* isExportPrintFormat --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtScrolledText::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ){
  if (fileFormat == m_fileFormat)
    return true;
  if (fileFormat == HardCopyListener::PDF)
    return true;
  if (fileFormat == HardCopyListener::Postscript)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPrintType --                                                             */
/* --------------------------------------------------------------------------- */
HardCopyListener::PrintType GuiQtScrolledText::getPrintType( const HardCopyListener::FileFormat &fileFormat ){
  if (fileFormat == HardCopyListener::ASCII)
    return HardCopyListener::FILE_EXPORT; //SCRIPT;
  if (fileFormat == HardCopyListener::PDF)
    return HardCopyListener::OWN_CONTROL;
  if (fileFormat == HardCopyListener::Postscript)
    return HardCopyListener::OWN_CONTROL; //SCRIPT;
  return isFileFormatSupported(fileFormat)? HardCopyListener::SCRIPT : HardCopyListener::NOT_SUPPORTED;
}

/* --------------------------------------------------------------------------- */
/* isEmpty --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::isEmpty(){
  std::string wintext;
  m_param->getFormattedValue( wintext );
  return wintext.empty();
}

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::getDefaultSettings( HardCopyListener::PaperSize &size,
					  HardCopyListener::Orientation &orient,
					  int &quantity,
					  HardCopyListener::FileFormat &fileFormat,
					  double &lm, double &rm,
 					  double &tm, double &bm ) {
  size = HardCopyListener::A4;
  orient = HardCopyListener::Portrait;
  quantity = 1;
  fileFormat = HardCopyListener::Postscript;
  lm = 10;
  rm = 10;
  tm = 10;
  bm = 10;
  return true;
}
/* --------------------------------------------------------------------------- */
/* createDataReference --                                                      */
/* --------------------------------------------------------------------------- */

bool GuiQtScrolledText::createDataReference( const std::string &name ){
  DataPool &datapool = DataPoolIntens::getDataPool();
  DataDictionary *dict = 0;
  std::string data_name = "@GuiScrolledText_" + name;
  dict = datapool.AddToDictionary( "", data_name, DataDictionary::type_String );
  if( dict == 0 ){
    return false;
  }
  // ignore this object in  datapool transactions
  // to keep message output
  dict->setItemWithoutTTrail();

  DataReference *ref = 0;
  ref = DataPoolIntens::getDataReference( data_name );
  if( ref == 0 ){
    return false;
  }
  XferDataItem *item = new XferDataItem( ref );
  return installDataItem( item );
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

void GuiQtScrolledText::SaveFileListener::FileSelected( const std::string &filename
							, const HardCopyListener::FileFormat format
							, const std::string dir ){
  m_text->write(filename);
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtScrolledText::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:ScrolledText";
  GuiQtDataField::serializeXML( os, recursive );
  os << "</intens:ScrolledText>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtScrolledText::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate){
  return GuiQtDataField::serializeProtobuf(eles->add_data_fields(), onlyUpdate);
}
#endif

/* --------------------------------------------------------------------------- */
/* MyQTextEdit::contextMenuEvent --                                            */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::MyQTextEdit::contextMenuEvent ( QContextMenuEvent * e ) {
  if (!m_popup_menu) {
    QMenu *menu = createStandardContextMenu(); // default menu
    m_popup_menu = new GuiQtPopupMenu(m_scrolledText, m_scrolledText->MenuLabel(), menu);
    GuiQtMenuButton *button;
    button = new GuiQtMenuButton( m_popup_menu, NULL);
    m_popup_menu->attach( button  );
    button->setDialogLabel( m_scrolledText->MenuLabel() );
    m_popup_menu->setTearOff( true );

    m_popup_menu->attach( new GuiQtSeparator( m_popup_menu->getElement() ) );

    button = new GuiQtMenuButton( m_popup_menu, &(m_scrolledText->m_save_listener) );
    m_popup_menu->attach( button );
    button->setDialogLabel( _("Save") );

    if (!AppData::Instance().HeadlessWebMode()) {
      button = new GuiQtMenuButton( m_popup_menu, &(m_scrolledText->m_print_listener) );
      m_popup_menu->attach( button );
      button->setDialogLabel( _("Print") );
    }

    button = new GuiQtMenuButton( m_popup_menu, &(m_scrolledText->m_preview_listener) );
    m_scrolledText->m_preview_listener.disallow();
    m_popup_menu->attach( button );
    button->setDialogLabel( _("Preview") );

    button = new GuiQtMenuButton( m_popup_menu, &(m_scrolledText->m_clear_listener) );
    m_popup_menu->attach( button );
    button->setLabel( _("Clear") );

    m_popup_menu->create();
  }
  m_popup_menu->popup();
}

/* --------------------------------------------------------------------------- */
/* setMinimumSizeHint --                                                       */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::MyQTextEdit::setMinimumSizeHint(QSize msh) {
  m_mhs = msh;
}

/* --------------------------------------------------------------------------- */
/* minimumSizeHint --                                                          */
/* --------------------------------------------------------------------------- */

QSize GuiQtScrolledText::MyQTextEdit::minimumSizeHint() const {
  QSize minhs;
  QSize windowSize = QTextEdit::window()->size();
#if QT_VERSION > 0x050600
  QRect drect = QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect drect = QApplication::desktop()->availableGeometry();
#endif

  if ( m_mhs.isValid() ) {
    bool hsb = false, vsb=false;
    if (drect.width() >= windowSize.width()) {
      minhs.setWidth(m_mhs.width());
    } else {
      minhs.setWidth( drect.width() - abs(m_mhs.width()-windowSize.width()) ) ;
    }
    if (drect.height() >= windowSize.height()) {
      minhs.setHeight( m_mhs.height() );
    } else {
      minhs.setHeight( drect.height() - abs(m_mhs.height()-windowSize.height()-0) );
    }
  }
  return minhs;
}

/* --------------------------------------------------------------------------- */
/* PrintListener --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::PrintListener:: ButtonPressed() {

  GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
  GuiFactory::Instance()->createPrinterDialog()->showDialog( m_text, m_text, &event );
}


/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                    */
/* --------------------------------------------------------------------------- */

const std::string& GuiQtScrolledText::MenuLabel() {
  if (m_label == "STD_WINDOW")
    m_label = _("Standard Window");
  if (m_label == "LOG_WINDOW")
    m_label = _("Log Window");
  return m_label;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::print(QPrinter* printer) {

  if ( printer) {
    m_textwidget->document()->print(printer);
  }
}

/* --------------------------------------------------------------------------- */
/* PreviewListener::ButtonPressed --                                           */
/* --------------------------------------------------------------------------- */

void GuiQtScrolledText::PreviewListener::ButtonPressed(){
  GuiFactory::Instance()->createPrinterDialog()->preview( m_text, m_text );
}
