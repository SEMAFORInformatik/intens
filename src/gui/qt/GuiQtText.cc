
//#include <qfocusdata.h>
#include <qstringlist.h>
#include <qsettings.h>
#include <QMenu>
#include <qcursor.h>
#include <qapplication.h>
#include <QScrollBar>
#include <QStackedWidget>
#include <QDir>
#include <QStyle>
#include <QtSvg/QSvgRenderer>
#include <QClipboard>
#include <QFileDialog>
#include <QTextBrowser>
#include <QLabel>
#include <QEnterEvent>

#include "utils/utils.h"
#include "utils/gettext.h"
#include "gui/GuiPrinterDialog.h"
#include "gui/GuiFactory.h"
#include "gui/GuiForm.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtText.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQtSvgView.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "datapool/DataPoolDefinition.h"

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiQtText::GuiQtText( GuiElement *parent )
  : GuiQtDataField( parent )
  , m_textedit( 0 )
#if QT_WEBENGINE
  , m_webview( 0 )
#endif
  , m_svgView( 0 )
  , m_label( 0 )
  , m_widgetStack( 0 )
  , m_length( 10 )
  , m_lines( -2 )
  , m_scrollbars( false )
  , m_autoscroll( false )
  , m_expandable( false )
  , m_CB_Reason( reason_None )
  , m_fileFormat( HardCopyListener::Text )
  , m_print_listener( this )
  , m_save_listener( this )
  , m_save_file_listener( this )
  {
  }

GuiQtText::GuiQtText( GuiQtText &text )
  : GuiQtDataField( text )
  , m_textedit( 0 )
#if QT_WEBENGINE
  , m_webview( 0 )
#endif
  , m_svgView( 0 )
  , m_label( 0 )
  , m_widgetStack( 0 )
  , m_length( text.m_length )
  , m_lines( text.m_lines )
  , m_scrollbars( text.m_scrollbars )
  , m_autoscroll( text.m_autoscroll )
  , m_expandable( text.m_expandable )
  , m_CB_Reason( reason_None )
  , m_fileFormat( text.m_fileFormat )
  , m_print_listener( this )
  , m_save_listener( this )
  , m_save_file_listener( this )
  {
  }

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
QWidget *GuiQtText::myWidget() {
  return m_widgetStack;
  return m_textedit;
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::create(){
  BUG_DEBUG("Begin GuiQtText::create(" << m_param->DataItem()->getFullName(false) << ")");

  QWidget *parent = getParent()->getQtElement()->myWidget();

  // create stacked widget
  m_widgetStack = new QStackedWidget(parent);
  parent = m_widgetStack;

  m_textedit = new MyQTextEdit( this, parent );
  m_widgetStack->addWidget( m_textedit );
  m_widgetStack->setCurrentIndex( 0 );

  updateWidgetProperty();

  // set font
  QFont default_font = m_textedit->font();
  BUG_DEBUG("Get Font: Family=" << default_font.family().toStdString() <<
	    "  Size=" << default_font.pointSize());
  bool available;
  m_css_name = m_param->DataItem()->getFullName(false);
  QFont font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
  if( !available ){
    BUG_DEBUG("Font for " << m_css_name << " is NOT available");
    m_css_name = m_param->DataItem()->getName();
    font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font, &available );
    if( !available ){
      BUG_DEBUG("Font for " << m_css_name << " is NOT available");
      m_css_name = "text";
      font = QtMultiFontString::getQFont( "@" + m_css_name + "@", default_font );
    }
  }

  BUG_DEBUG("Set Font: Family=" << font.family().toStdString() <<
	    "  Size=" << font.pointSize());
  m_textedit->setFont(font);

  // for stylesheet settings
  m_textedit->setObjectName( "GuiText" );

  getAttributes();

  // groesse setzen
  QSize mhs =  m_textedit->minimumSizeHint();
  int delta =  floor(0.5+1.5*m_textedit->fontMetrics().horizontalAdvance( '0' ));
  int charlen = m_textedit->fontMetrics().horizontalAdvance( '0' );
  int length = m_textedit->fontMetrics().horizontalAdvance( '0' ) * m_length + delta;//(1 + m_textedit->frameWidth() +  m_length);
  int lines  = m_textedit->fontMetrics().height()     * (1 + abs(m_lines));
  // m_textedit->setMinimumSize( length, lines );
  m_widgetStack->setMinimumSize( length, lines );
  if (m_lines < 0) { // indicate default value
    // m_textedit->setMaximumWidth( length );
    // m_textedit->setSizePolicy( QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::MinimumExpanding )  );
    m_widgetStack->setMaximumWidth( length );
    m_widgetStack->setSizePolicy( QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::MinimumExpanding )  );
  } else{
    if (m_expandable) {
      // m_textedit->setMinimumSize( length, lines );
      m_widgetStack->setMinimumSize( length, lines );
      m_widgetStack->setSizePolicy( QSizePolicy(  QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding )  );
    } else {
      // m_textedit->setSizePolicy( QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::Fixed )  );
      // m_textedit->setMaximumSize( length, lines );
      m_widgetStack->setSizePolicy( QSizePolicy(  QSizePolicy::Fixed,  QSizePolicy::Fixed )  );
      m_widgetStack->setMaximumSize( length, lines );
    }
  }

  m_textedit->setReadOnly( !isEditable() );
  //  m_textedit->setEnabled( isEditable() );

  m_textedit->setTabChangesFocus( true );

  m_param->DataItem()->setDimensionIndizes();

  QObject::connect( m_textedit,
		    SIGNAL(copyAvailable( bool )),
		    this,
		    SLOT(slot_copyAvailable( bool )) );

  setMyColors();
  setColors();
  BUG_DEBUG("End GuiQtText::create");
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */
bool GuiQtText::destroy(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::getSize( int &w, int &h ){
  w = m_textedit->width();
  h = m_textedit->height();
}

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::enable(){
  setDisabled(false);
  if( m_textedit == 0 ) return;
  getAttributes();
	m_textedit->setReadOnly( !isEditable() );
	//    m_textedit->setEnabled( true );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::disable(){
  setDisabled(true);
  if( m_textedit == 0 ) return;
  getAttributes();
  m_textedit->setReadOnly( !isEditable() );
  //m_textedit->setEnabled( false );
  setColors();
}

/* --------------------------------------------------------------------------- */
/* lock --                                                                     */
/* --------------------------------------------------------------------------- */
void GuiQtText::lock(){
  BUG_DEBUG("GuiQtText::lock");
  if( m_textedit == 0 ) return;
  getAttributes();
  if( !isLockable() ) return;
  lockValue();
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* unlock --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::unlock(){
  BUG_DEBUG("GuiQtText::unlock");
  if( m_textedit == 0 ) return;
  getAttributes();
  if( !isLockable() ) return;
  unlockValue();
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* switchlock --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtText::switchlock(){
  BUG_DEBUG("GuiQtText::switchlock");
  if( m_textedit == 0 ) return;
  getAttributes();
  if( !isLockable() ) return;
  if( isLocked() ){
    unlockValue();
  }
  else{
    lockValue();
  }
  updateForms( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* edit --                                                                     */
/* --------------------------------------------------------------------------- */
void GuiQtText::edit(){
  BUG_DEBUG("GuiQtTextfield::edit");
}

/* --------------------------------------------------------------------------- */
/* protect --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::protect(){
  BUG_DEBUG("GuiQtText::protect");
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::update( UpdateReason reason ){
  BUG_DEBUG("GuiQtText::update");

  if( m_param == 0 || m_textedit == 0 ) return;
  updateWidgetProperty();
  bool changed = getAttributes();
  switch( reason ){
  case reason_FieldInput:
  case reason_Process:
    break;
  case reason_Cycle:
  case reason_Cancel:
  case reason_Always:
    setUpdated();
    changed = true;  // to be save the color is set
    break;
  default:
    BUG_DEBUG("- Unhandled Update Reason");
    setUpdated();
    break;
  }
  // --------------------------------------------------------------
  // Zuerst wird nur der Wert geprüft.
  // --------------------------------------------------------------
  if( isUpdated() ){
    // get slider position
    int oldVertSBPos = m_textedit->verticalScrollBar()->sliderPosition();
    int oldHorzSBPos = m_textedit->horizontalScrollBar()->sliderPosition();

    std::string text;
    m_param->getFormattedValue( text );
    BUG_DEBUG("- Value is '" << text << "'");
    QString qtext( QString::fromStdString(text) );
    std::string mimeTypeData = FileUtilities::getDataMimeType(text);

#if QT_WEBENGINE
    // is html?
    if (mimeTypeData == "text/html") {
      if (!m_webview) {
        m_webview = new MyQWebView(this);
        m_widgetStack->addWidget( m_webview );
      }
      m_widgetStack->setCurrentWidget( m_webview );
#ifdef _WIN32
     QUrl url("file:///"+QDir::currentPath()+"/");
#else
     QUrl url("file://"+QDir::currentPath()+"/");
#endif
      m_webview->setHtml( qtext, url );
    } else
#else
    BUG_INFO("- Text content ["<<qtext.left(10).toStdString()<<"]");
    if (mimeTypeData == "text/html") {
      if (!m_webview) {
        m_webview = new MyQTextBrowser(this);
        m_webview->setOpenExternalLinks(true);
        m_widgetStack->addWidget( m_webview );
      }
      m_widgetStack->setCurrentWidget( m_webview );
      m_webview->setHtml(qtext);
    } else
#endif
      // is svg?
      if (mimeTypeData == "image/svg+xml") {
        if (m_svgView) {
          m_widgetStack->removeWidget( m_svgView );
          delete m_svgView;
          m_svgView = 0;
        }
        if (!m_svgView) {
          m_svgView = new GuiQtSvgView(this);
          m_widgetStack->addWidget( m_svgView );
          connect( m_svgView,
                   SIGNAL(mousePressed(const QGraphicsItem*, const QPointF&)),
                   this,
                   SLOT(slot_svgViewMousePressed(const QGraphicsItem*, const QPointF&)),
                   Qt::QueuedConnection );
        }

        QByteArray bArray(qtext.toUtf8());
        QSvgRenderer* renderer = new QSvgRenderer( bArray );
        m_svgView->setRenderer( renderer );
        QRectF vb = renderer->viewBox();
        QSize vbm = m_svgView->maximumViewportSize();
        BUG_DEBUG("ViewPort Pos: " << vb.x() << ", "  << vb.y()
                  << ", Size: " << vb.width() << ", "  << vb.height());
        BUG_DEBUG("MaxSizeViewPort: " << vbm.width() << ", "  << vbm.height());
        m_widgetStack->setCurrentWidget(m_svgView);
      } else if (mimeTypeData == "image/png") {
        QByteArray bArray(text.c_str());
        QPixmap pixmap;
        pixmap.loadFromData((const uchar*) text.c_str(), text.size());
        if (!m_label) m_label = new QLabel();
        m_widgetStack->addWidget( m_label );
        m_widgetStack->setCurrentWidget(m_label);
      } else {
        // normal text
        m_widgetStack->setCurrentIndex( 0 );
        m_textedit->setText( QString::fromStdString(text) );
      }

    if (qtext.size() == 0 && m_svgView && m_svgView->isTransformed()) {
      m_svgView->resetTransform();
    }

    if( useColorSet() ){
      changed = true;
    }

    // reset slider positions
    if( m_autoscroll ){
      int m = m_textedit->verticalScrollBar()->maximum();
      m_textedit->verticalScrollBar()->setSliderPosition(m);
    }
    else{
      m_textedit->verticalScrollBar()->setSliderPosition(oldVertSBPos);
    }
    m_textedit->horizontalScrollBar()->setSliderPosition(oldVertSBPos);
  }

  // --------------------------------------------------------------
  // Falls keine Attribute (exl. Updated) geaendert haben, haben
  // wir hier nicht mehr zu tun.
  // --------------------------------------------------------------
  if( !changed ){
    return;
  }
  // --------------------------------------------------------------
  // Nun wird Editable und TraversalOn neu gesetzt.
  // --------------------------------------------------------------
  m_textedit->setReadOnly( !isEditable() );
  //  m_textedit->setEnabled(  isEditable() );

  // --------------------------------------------------------------
  // Als letztes werden die Farben neu gesetzt.
  // --------------------------------------------------------------
  setColors();

  // (re)set font
  QFont font = m_textedit->font();
  QFont new_font = QtMultiFontString::getQFont( "@" + m_css_name + "@", font );
  if( font != new_font ){
    m_textedit->setFont( new_font );
  }
}

/* --------------------------------------------------------------------------- */
/* setLength --                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQtText::setLength( int len ){
  m_length = abs(len);
  return true;
}

/* --------------------------------------------------------------------------- */
/* getLength --                                                                */
/* --------------------------------------------------------------------------- */

int GuiQtText::getLength(){
  return m_length;
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtText::setPrecision( int prec ){
  return m_lines = prec;
}

/* --------------------------------------------------------------------------- */
/* setThousandSep --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiQtText::setThousandSep(){
  return m_param->setThousandSep();
}

/* --------------------------------------------------------------------------- */
/* setScrollbar --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtText::setScrollbar( ScrollbarType sb ){
  m_scrollbars = sb == scrollbar_OFF ? false : true;
}

/* --------------------------------------------------------------------------- */
/* setAutoScroll --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtText::setAutoScroll(){
  m_autoscroll = true;
}

/* --------------------------------------------------------------------------- */
/* setExpandable --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtText::setExpandable( FlagStatus status ){
  m_expandable = status == status_ON ? true : false;
}

/* --------------------------------------------------------------------------- */
/* CloneForFieldgroupTable --                                                  */
/* --------------------------------------------------------------------------- */
GuiQtDataField *GuiQtText::CloneForFieldgroupTable(){
  GuiQtText *txt = new GuiQtText( *this );
  return txt;
}

/* --------------------------------------------------------------------------- */
/* setColors --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtText::setColors(){
  BUG_DEBUG("Begin setColors");

  QColor background, foreground;
  int dark_fac;
  bool setColor = getTheColor( background, foreground, dark_fac );
  // svg => override background
  if (m_svgView &&
      m_widgetStack->currentIndex() == m_widgetStack->indexOf(m_svgView)) {
    background = QColor(Qt::white);
  }
  BUG_DEBUG("- Background=" << background.name().toStdString() <<
	    "  Foreground=" << foreground.name().toStdString() <<
	    "  setColor=" << setColor);

  if ( !setColor ) {
    QString styleSheet = m_textedit->styleSheet();
    if ( !styleSheet.isEmpty() ) {
      BUG_DEBUG("- Edit Style: " << styleSheet.toStdString());
      setColor = true;
    }
    else{
      BUG_DEBUG("- Edit Style: empty");
    }
    styleSheet = m_widgetStack->styleSheet();
    if ( !styleSheet.isEmpty() ) {
      BUG_DEBUG("- Stack Style: " << styleSheet.toStdString());
    }
    else{
      BUG_DEBUG("- Stack Style: empty");
    }
  }
  setColor = true;

  std::string css;
  if ( setColor ) {
    BUG_DEBUG("- set color");
    int fac = isEditable() ? 100 : dark_fac;
    css = compose("background:%1; color: %2; %3",
		  background.darker(fac).name().toStdString(),
		  foreground.darker(fac).name().toStdString(),
		  QtMultiFontString::getCss( m_css_name, 0, true ));
    BUG_DEBUG("- set Stylesheet(" << css << ")");
  }

  const std::string& attrCss = GuiDataField::getStylesheet();
  if( attrCss.size() ){
    BUG_DEBUG("- add Stylesheet(" << attrCss << ")");
  }
  if (css.size() || attrCss.size()) {
    QString sheet( QString::fromStdString(css +"\n" + attrCss) );
    if(m_textedit->styleSheet() != sheet) {
      m_textedit->setStyleSheet( sheet );
    }
    if(m_widgetStack->styleSheet() != sheet) {
      m_widgetStack->setStyleSheet( sheet );
    }
  }
  BUG_DEBUG("End setColors");
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */
XferParameter::InputStatus GuiQtText::checkFormat(){
  BUG(BugGui,"GuiQtText::checkFormat");
  return m_param->checkFormat( m_textedit->toPlainText().toStdString() );
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtText::setInputValue(){
  BUG_DEBUG("GuiQtText::setInputValue");
  if( m_param->isEmpty( m_textedit->toPlainText().toStdString() ) )
    {
      m_param->clear();
      return true;
    }
  if( m_param->setFormattedValue( m_textedit->toPlainText().toStdString() ) )
    {
      return true;
    }
  BUG_DEBUG("setInputValue failed");
  return false;
}

/* --------------------------------------------------------------------------- */
/* FinalWork --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtText::FinalWork( CB_Reason reason ){
  BUG_DEBUG("GuiQtText::FinalWork");
  if (!isEditable()) return;  // nothing to do
  switch( checkFormat() ){
  case XferParameter::status_Bad:
    printMessage( _("Conversion error."), msg_Warning );
    update( reason_Cancel );
    BUG_DEBUG("FinalWork: checkFormat failed");
    return;
  case XferParameter::status_Unchanged:
    BUG_DEBUG("FinalWork: checkFormat Unchanged");
    clearMessage();
    update( reason_Cancel );
    return;
  case XferParameter::status_Changed:
    std::string error_msg;
    m_CB_Reason = reason;
    s_TargetsAreCleared = m_param->DataItem()->StreamTargetsAreCleared(error_msg);
    if( s_TargetsAreCleared ){ // Targets sind ok
      FinalWorkOk();
      return;
    }
    confirm(error_msg);
    return;
  }
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::confirmYesButtonPressed(){
  FinalWorkOk();
}

/* --------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::confirmNoButtonPressed(){
  update( reason_Cancel );
  s_DialogIsAktive = false;
  return;
}

/* --------------------------------------------------------------------------- */
/* FinalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtText::FinalWorkOk(){
  doFinalWork();
  if( m_CB_Reason == reason_Activate ){
  }
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* FieldFocusIn --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtText::FieldFocusIn(){
  BUG_DEBUG("GuiQtText::FieldFocusIn");
  m_CB_Reason = reason_None;
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQtText::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:Textfield";
  os << " rows=\"" << m_lines << "\"";
  os << " cols=\"" << m_length << "\"";
  GuiQtDataField::serializeXML(os, recursive);
  os << "</intens:Textfield>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                        */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQtText::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdate){
  auto element = eles->add_texts();
  // popup base menu
  GuiPopupMenu* pm = getElement()->getPopupMenu();
  if (pm) {
    Json::Value jsonElem = Json::Value(Json::objectValue);
    pm->serializeProtobuf(eles, element->mutable_popup_base_menu(), onlyUpdate);
  }
  return GuiQtDataField::serializeProtobuf(element->mutable_base(), onlyUpdate);
}
#endif

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQtText::write( InputChannelEvent& event ){
  std::string wintext;
  m_param->getFormattedValue( wintext );
  if( wintext.size() ) {
  std::ostringstream ostr(wintext);
  return event.write( ostr );
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQtText::write( const std::string &filename ){
  std::string wintext;
  getValue( wintext );
  if( wintext.empty() ){
    return false;
  }

  // Convert html to pdf
  if ( dynamic_cast<QTextBrowser*>(m_widgetStack->currentWidget())  ) {
      QPrinter printer;
      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setOutputFileName(QString::fromStdString(filename));
      m_webview->print(&printer);
      return true;
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
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQtText::saveFile( GuiElement *e ){
  saveText();
  return false;
}

/* --------------------------------------------------------------------------- */
/* saveText --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtText::saveText(){
  BUG_DEBUG("GuiQtScrolledText::saveText");
  std::string formatStr = "ASCII (*.txt)";
#if QT_WEBENGINE
  if ( dynamic_cast<QWebView*>(m_widgetStack->currentWidget())  ) {
    formatStr = "Html (*.html)";
  } else
#else
  if ( dynamic_cast<QTextBrowser*>(m_widgetStack->currentWidget())  ) {
     formatStr = "PDF (*.pdf)";
  } else
#endif
    if ( dynamic_cast<QGraphicsView*>(m_widgetStack->currentWidget())  ) {
      formatStr = "Svg (*.svg)";
    }
  std::vector<HardCopyListener::FileFormat> formats;
  ReportGen::getFormats( this, formats );
  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"),MenuLabel())
      , formatStr, ""
      , &m_save_file_listener
      , DialogFileSelection::Save
      , &formats
      , DialogFileSelection::AnyFile
      , getElement()->getName());
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQtText::isFileFormatSupported( const FileFormat &fileFormat ){
  return ( m_fileFormat == fileFormat );
}

/* --------------------------------------------------------------------------- */
/* isExportPrintFormat --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiQtText::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ){
  if (fileFormat == m_fileFormat)
    return true;
  if (fileFormat == HardCopyListener::ASCII)
    return true;
  if (fileFormat == HardCopyListener::Text)
    return true;
  if (fileFormat == HardCopyListener::Postscript)
    return true;
  if (fileFormat == HardCopyListener::PDF)
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPrintType --                                                             */
/* --------------------------------------------------------------------------- */
HardCopyListener::PrintType GuiQtText::getPrintType( const HardCopyListener::FileFormat &fileFormat ){
  if (fileFormat == HardCopyListener::ASCII ||
      fileFormat == HardCopyListener::Text)
    return HardCopyListener::FILE_EXPORT; //SCRIPT;
  if (fileFormat == HardCopyListener::PDF)
    return HardCopyListener::OWN_CONTROL;
  if (fileFormat == HardCopyListener::Postscript)
    return HardCopyListener::OWN_CONTROL; //SCRIPT;
  return isFileFormatSupported(fileFormat)? HardCopyListener::SCRIPT : HardCopyListener::NOT_SUPPORTED;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */
const std::string& GuiQtText::MenuLabel() {
  static std::string s("TextField");
  return s;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */
void GuiQtText::print(QPrinter* printer) {
  if ( printer) {
    if (m_widgetStack->currentIndex() == 0 ) {
      printer->setFullPage(true);
      QString cssSave = m_textedit->styleSheet();
      std::string css = compose("QTextEdit { %1 }", QtMultiFontString::getCss("textPrint"));
      m_textedit->setStyleSheet(QString::fromStdString(css));
      BUG_DEBUG("print cssNew["<<css<<"] Old["<<cssSave.toStdString()<<"]");
      m_textedit->document()->print(printer);
      m_textedit->setStyleSheet(cssSave);
    }
#if QT_WEBENGINE
    if ( dynamic_cast<QWebView*>(m_widgetStack->currentWidget())  ) {
      m_webview->render(printer);
    }
#endif
    if ( dynamic_cast<QLabel*>(m_widgetStack->currentWidget())  ) {
      m_label->render(printer);
    }
    if ( dynamic_cast<QGraphicsView*>(m_widgetStack->currentWidget())  ) {
      // render svg
      QPainter painter;
      painter.begin(printer);
      m_svgView->render(&painter);
      painter.end();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* MyQTextEdit::enterEvent --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextEdit::enterEvent( QEnterEvent *e ){
  m_guiQtText->enterEvent( e );
  QTextEdit::enterEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQTextEdit::leaveEvent --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextEdit::leaveEvent( QEvent *e ){
  m_guiQtText->clearMessage();
  QTextEdit::leaveEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQTextEdit::paintEvent --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextEdit::paintEvent( QPaintEvent *e ){
  // 2017-11-15 amg evtl. verhindern wir so den Absturz
  try {
    QTextEdit::paintEvent( e );
  } catch( const std::exception e ) {
    std::cerr << "GuiQtSvgView::paintEven Error: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "GuiQtSvgView::paintEvent" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* MyQTextEdit::contextMenuEvent --                                            */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextEdit::contextMenuEvent( QContextMenuEvent * event ) {
  if( !AppData::Instance().GuiWithTextPopuMenu() ){
    return;
  }

  if (m_popup_menu == 0) {
    GuiQtMenuButton* button;
    QMenu *menu = createStandardContextMenu();
    QFont font = menu->font();
    menu->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
    menu->addSeparator();

    m_popup_menu = new GuiQtPopupMenu(m_guiQtText, m_guiQtText->MenuLabel(), menu);
    m_popup_menu->setTearOff( true );
    m_popup_menu->clearDefaultMenu( false );
    m_popup_menu->attach( new GuiQtSeparator( m_popup_menu->getElement() ) );

    // Save Menu
    button = new GuiQtMenuButton( m_popup_menu, &(m_guiQtText->m_save_listener) );
    m_popup_menu->attach( button );
    button->setDialogLabel( _("Save") );

    // Print Menu
    if (!AppData::Instance().HeadlessWebMode()) {
      button = new GuiQtMenuButton( m_popup_menu, &(m_guiQtText->m_print_listener) );
      m_popup_menu->attach( button  );
      button->setDialogLabel( _("Print") );
    }

    m_popup_menu->create();
  }
  m_popup_menu->popup();
}

#if QT_WEBENGINE
/* --------------------------------------------------------------------------- */
/* MyQWebView::enterEvent --                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQWebView::enterEvent( QEnterEvent *e ){
  m_guiQtText->enterEvent( e );
  QWebView::enterEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQWebView::leaveEvent --                                                   */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQWebView::leaveEvent( QEvent *e ){
  m_guiQtText->clearMessage();
  QWebView::leaveEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQWebView::createPopupMenu --                                              */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQWebView::contextMenuEvent( QContextMenuEvent * event ){
  GuiQtMenuButton* button;
  GuiQtPopupMenu* _popup_menu(0);
  QMenu *menu = findChild<QMenu *>();
  if (!menu)
    menu =  new QMenu();
  QFont font = menu->font();
  menu->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
  menu->addSeparator();

  _popup_menu = new GuiQtPopupMenu(m_guiQtText, m_guiQtText->MenuLabel(), menu);
  _popup_menu->setTearOff( true );
  _popup_menu->clearDefaultMenu( false );
  _popup_menu->attach( new GuiQtSeparator( _popup_menu->getElement() ) );

  // Save Menu
   button = new GuiQtMenuButton( _popup_menu, &(m_guiQtText->m_save_listener) );
   _popup_menu->attach( button );
   button->setDialogLabel( _("Save") );

   // Print Menu
   if (!AppData::Instance().HeadlessWebMode()) {
     button = new GuiQtMenuButton( _popup_menu, &(m_guiQtText->m_print_listener) );
     _popup_menu->attach( button  );
     button->setDialogLabel( _("Print") );
   }

   _popup_menu->create();
   _popup_menu->popup();
   delete _popup_menu;
}
#else

/* --------------------------------------------------------------------------- */
/* MyQTextBrowser::enterEvent --                                               */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextBrowser::enterEvent( QEnterEvent *e ){
  m_guiQtText->enterEvent( e );
  QTextBrowser::enterEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQTextBrowser::leaveEvent --                                               */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextBrowser::leaveEvent( QEvent *e ){
  m_guiQtText->clearMessage();
  QTextBrowser::leaveEvent( e );
}

/* --------------------------------------------------------------------------- */
/* MyQTextBrowser::createPopupMenu --                                          */
/* --------------------------------------------------------------------------- */
void GuiQtText::MyQTextBrowser::contextMenuEvent( QContextMenuEvent * event ){
  GuiQtMenuButton* button;
  GuiQtPopupMenu* _popup_menu(0);
  QMenu *menu = findChild<QMenu *>();
  if (!menu)
    menu =  new QMenu();
  QFont font = menu->font();
  menu->setFont( QtMultiFontString::getQFont( "@popup@", font ) );
  menu->addSeparator();

  _popup_menu = new GuiQtPopupMenu(m_guiQtText, m_guiQtText->MenuLabel(), menu);
  _popup_menu->setTearOff( true );
  _popup_menu->clearDefaultMenu( false );
  _popup_menu->attach( new GuiQtSeparator( _popup_menu->getElement() ) );

  // Save Menu
  button = new GuiQtMenuButton( _popup_menu, &(m_guiQtText->m_save_listener) );
  _popup_menu->attach( button );
  button->setDialogLabel( _("Save") );

  _popup_menu->create();
  _popup_menu->popup();
  delete _popup_menu;
}
#endif

/* --------------------------------------------------------------------------- */
/* PrintListener::ButtonPressed --                                             */
/* --------------------------------------------------------------------------- */
void GuiQtText::PrintListener::ButtonPressed() {
  GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
  GuiQtPrinterDialog::Instance().showDialog((HardCopyListener*)m_text, m_text, &event );
}

/* --------------------------------------------------------------------------- */
/* SaveFileListener::FileSelected --                                           */
/* --------------------------------------------------------------------------- */
void GuiQtText::SaveFileListener::FileSelected( const std::string &filename,
						const HardCopyListener::FileFormat format,
						const std::string dir ){
  // std::cout << "filename["<<filename<<"]   format["<<format<<"] dir["<<dir<<"]\n";
#if 1
  m_text->write(filename);
#else
  GuiElement *dialog = GuiElement::getMainForm()->getElement();
  ReportGen::Instance().printDocument( ReportGen::SAVE
				       , m_text
				       , dialog
				       , filename
				       , ""
				       , HardCopyListener::Unscaled
				       , HardCopyListener::Portrait
				       , 1
				       , format );
#endif
}

/* --------------------------------------------------------------------------- */
/* doEndOfWork --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtText::doEndOfWork( bool error, bool updateforms ){
  GuiQtDataField::doEndOfWork( error, updateforms);
  if (m_svgView &&  m_svgView->viewport()) {
    m_svgView->viewport()->setFocus();
  }
}

/* --------------------------------------------------------------------------- */
/* slot_svgViewMousePressed --                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtText::slot_svgViewMousePressed( const QGraphicsItem* item,
					  const QPointF& posScene){
  JobFunction *func = getFunction();
  if( func == 0 ){
    return;
  }

  GuiQtText::Trigger *trigger = new GuiQtText::Trigger(this, func);
  trigger->setReason( JobElement::cll_SelectPoint );
  trigger->setDiagramXPos( posScene.x() );
  trigger->setDiagramYPos( posScene.y() );
  trigger->setDataItem( m_param->DataItem() );
  trigger->startJob();
}

/* --------------------------------------------------------------------------- */
/* slot_copyAvailable --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQtText::slot_copyAvailable( bool selected ){
  if( !selected ){
    return;
  }
  JobFunction *func = getFunction();
  if( func == 0 ){
    return;
  }

  m_textedit->copy();
  QClipboard *clipboard = QApplication::clipboard();
  //if( !clipboard->supportsSelection() ){
  //  // Unterschied zwischen Linux und Windows interessiert uns hier nicht
  //  std::cout << "-- clipboard doesnt support selection" << std::endl;
  //  return;
  //}

  QString qtext = clipboard->text(QClipboard::Clipboard);
  std::string text = qtext.toStdString();

  QTextCursor cursor = m_textedit->textCursor();
  int pos = cursor.position();

  FinalWork( GuiQtText::reason_LosingFocus );

  GuiQtText::Trigger *trigger = new GuiQtText::Trigger(this, func);
  trigger->setReason( JobElement::cll_Select );
  trigger->setDataItem( m_param->DataItem(), text );
  trigger->setIndex( pos );
  trigger->startJob();
}
