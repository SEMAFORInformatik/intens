
#include <typeinfo>
#include <sstream>
#include <fcntl.h>
#include <QMenu>
#include <QProcess>

#include "utils/utils.h"
#include "streamer/StreamManager.h"
#include "streamer/Stream.h"
#include "app/DataPoolIntens.h"
#include "app/AppData.h"
#include "app/UiManager.h"
#include "gui/GuiFactory.h"
#include "gui/GuiManager.h"
#include "gui/GuiPlotDataItem.h"
#include "plot/qt/Canvas.h"
#include "plot/ListGraph.h"
#include "plot/Gdev.h"
#include "plot/GdevPS.h"
#include "plot/GdevHPGL.h"
#include "plot/GdevSVG.h"
#include "plot/qt/GdevQt.h"
#include "gui/GuiPopupMenu.h"
// #include "gui/motif/GuiMotifPulldownMenu.h"
#include "gui/GuiMenuButton.h"
#include "gui/GuiMenuToggle.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/GuiLabel.h"
#include "gui/GuiDialog.h"
#include "gui/GuiFactory.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiButton.h"
#include "gui/GuiButton.h"
#include "gui/GuiScrolledText.h"
#include "plot/qt/QtListPlot.h"
#include "operator/InputChannelEvent.h"
#include "utils/gettext.h"

#if ( __GNUC__ > 2 )
#include "utils/fdiostream.h"
#endif


/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* Konstruktor --                                                              */
/* --------------------------------------------------------------------------- */

QtListPlot::QtListPlot( GuiElement *parent, const std::string &name, int width, int height )
  : GuiQtElement( parent, name ), ListPlot( name, width, height )
    , m_name( name )
    , m_id( 0 )
    , m_canvas( 0 )
    , m_gdev( 0 )
    , m_configDialog( 0 )
    , m_nsubdivx( 1 )
    , m_nsubdivy( 1 )
    , m_title ( "" )
    , m_width(0)
    , m_height(0)
    , m_legendStream( 0 )
    , m_legend( "" )
    , m_sameYRange( false )
    , m_allCycles( false )
    , m_ostrPrinter( 0 )
    , m_currentFileFormat( HardCopyListener::Postscript ) {
  m_canvas = new iCanvas( name, this, width, height );
#ifdef HAVE_QT
#else
  pthread_mutex_init( &m_mutex, 0 );
  pthread_cond_init( &m_printingDone, 0 );
#endif
  //  m_supportedFileFormats[HardCopyListener::HPGL]       = HardCopyListener::OWN_CONTROL;
  m_supportedFileFormats[HardCopyListener::Postscript] = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::PDF]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::PNG]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::JPEG]       = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::GIF]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::SVG]        = HardCopyListener::FILE_EXPORT;

  // Hardcopy Listener intallieren
  UImanager::Instance().addHardCopy(m_name, this);
}

/* --------------------------------------------------------------------------- */
/* Destruktor --                                                               */
/* --------------------------------------------------------------------------- */

QtListPlot::~QtListPlot() {
}

/* --------------------------------------------------------------------------- */
/* PrintType --                                                                */
/* --------------------------------------------------------------------------- */

HardCopyListener::FileFormat QtListPlot::getFileFormat() {
  return m_currentFileFormat;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */

const std::string &QtListPlot::MenuLabel() {
  return m_title.empty() ? m_name : m_title;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool QtListPlot::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats2::iterator it;
  bool result = false;
  for( it = m_supportedFileFormats.begin(); it != m_supportedFileFormats.end() && !result; ++it )
    if( (*it).first == fileFormat )
      result = true;
  return result;
}

/* --------------------------------------------------------------------------- */
/* isExportPrintFormat --                                                             */
/* --------------------------------------------------------------------------- */
bool QtListPlot::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::PrintType  ptype = getPrintType(fileFormat);
  if (ptype==HardCopyListener::FILE_EXPORT ||
      ptype==HardCopyListener::OWN_CONTROL   )
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPrintType --                                                             */
/* --------------------------------------------------------------------------- */
HardCopyListener::PrintType QtListPlot::getPrintType( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats2::iterator it = m_supportedFileFormats.find( fileFormat );
  if ( it == m_supportedFileFormats.end() ) {
    return HardCopyListener::NOT_SUPPORTED;
  } else
    return (*it).second;
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */
InputChannelEvent* s_event;
bool QtListPlot::write( InputChannelEvent &event ) {
  BUG(BugPlot,"QtListPlot::write");


#ifdef HAVE_QT
  m_inEvent = &event;
  m_ostrPrinter = new std::ostringstream;
  doPrintAny();
#else
  XtAppAddTimeOut( GuiManager::Instance().AppContext(), 0, doPrintAnyCallback, this );
  m_ostrPrinter = &event.getOstream();
  pthread_mutex_lock( &m_mutex );
  pthread_cond_wait( &m_printingDone, &m_mutex );
  pthread_mutex_unlock( &m_mutex );
#endif
  return true;
}
bool QtListPlot::write( const std::string &s ) {
  std::string fileName( s );
  GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
//   m_currentFileFormat = hardcopy.FileFormat();
//   switch( m_currentFileFormat ){
//   case HardCopyListener::Postscript :
//     break;
//   case HardCopyListener::ASCII :
//     return true;
//     break;
//   default :
//     ReportGen::Instance().setExitMsg( compose(_("%1: Selected file format is not supported"),getName()) );
//     return false;
//   }
  //std::cerr << "WARNING (QtListPlot) Only Postscript is supported => write Postscript!!!\n"<<std::flush;
  std::string suffix = FileUtilities::getSuffix(s);
  if (suffix == "eps" || suffix == "pdf" || suffix == "png") {  // end: .svg
    fileName += ".ps";
  }
  int fd = creat( fileName.c_str(), 0644 );
  if( !(fd > 0) )
    return false;
  m_ostrPrinter = 0;
#if ( __GNUC__ > 2 )
  m_ostrPrinter = new fdostream( fd );
#else
  m_ostrPrinter = new std::ofstream( fd );
#endif
  assert( m_ostrPrinter != 0 );

  if( myWidget() == 0 && getMyForm()){
    getMyForm()->getElement()->getQtElement()->create();
    myWidget()->hide();
  }
  update(reason_Always);

#ifdef HAVE_QT
  if (suffix == "eps" || suffix == "pdf" || suffix == "png") {  // end: .svg
    doPrintPS();

    // wir convertieren das eps zu pdf
    QString program(suffix == "pdf" ? "ps2pdf" : "convert");
    QStringList arguments = {
      QString::fromStdString(fileName),
      QString::fromStdString(s)
    };
    QProcess proc;
    proc.start(program, arguments);
    if (!proc.waitForFinished()) {
      std::ostringstream logmsg;
      logmsg << "QtListPlot named '" << getName() <<
	"', an error occurs when converting the eps file." << std::endl;
      GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
    }
  } else
    writeFile(0, s, true);  // background transparent
#else
  XtAppAddTimeOut( GuiManager::Instance().AppContext(), 0, doPrintPSCallback, this );
  pthread_mutex_lock( &m_mutex );
  pthread_cond_wait( &m_printingDone, &m_mutex );
  pthread_mutex_unlock( &m_mutex );
#endif
  return true;
}

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */

bool QtListPlot::getDefaultSettings( HardCopyListener::PaperSize &size,
				   HardCopyListener::Orientation &orient,
				   int &quantity,
				   HardCopyListener::FileFormat &fileFormat,
				   double &lm, double &rm,
				   double &tm, double &bm ) {
  size = HardCopyListener::A4;
  orient = HardCopyListener::Landscape;
  quantity = 1;
  fileFormat = HardCopyListener::Postscript;
  lm = 10;
  rm = 10;
  tm = 10;
  bm = 10;

  return true;
}

void QtListPlot::doPrintAny() {
  BUG(BugGui, "QtListPlot::doPrintAny");

  double x0, x1, y0, y1;

  GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
  double leftMargin = hardcopy.leftMargin();
  double rightMargin = hardcopy.rightMargin();
  double topMargin = hardcopy.topMargin();
  double bottomMargin = hardcopy.bottomMargin();
  int PaperSize = hardcopy.PaperSize();
  int FileFormat = hardcopy.FileFormat();
  int orientation = hardcopy.Orientation();

  switch( PaperSize ) {
  case HardCopyListener::A3:
    if( orientation == HardCopyListener::Landscape ) {
      x0 = leftMargin;
      y0 = topMargin;
      x1 = 420 - rightMargin;
      y1 = 297 - bottomMargin;
    }
    else {
      x0 = leftMargin;
      y0 = bottomMargin;
      x1 = 297 - rightMargin;
      y1 = 420 - topMargin;
    }
    break;
  case HardCopyListener::A4:
    if( orientation == HardCopyListener::Landscape ) {
      x0 = leftMargin;
      y0 = topMargin;
      x1 = 297 - rightMargin;
      y1 = 210 - bottomMargin;
    }
    else {
      x0 = leftMargin;
      y0 = bottomMargin;
      x1 = 210 - rightMargin;
      y1 = 297 - topMargin;
    }
    break;
  default:
    x0 = 0.0; //plot->darea->gdev->xmin;
    x1 = m_gdev != 0 ? m_gdev->getWidth() : 0; //plot->darea->gdev->xmax;
    y0 = 0.0; //plot->darea->gdev->ymin;
    y1 = m_gdev != 0 ? m_gdev->getHeight() : 0; //plot->darea->gdev->ymax;
  }

  Gdev::Orientation orient = Gdev::ORIENT_PORTRAIT;
  if( orientation == HardCopyListener::Landscape )
    orient = Gdev::ORIENT_LANDSCAPE;

  std::string userName = AppData::Instance().UserName();
  std::string realName = AppData::Instance().RealName();

  switch( FileFormat ){
  case HardCopyListener::Postscript :
  case HardCopyListener::PDF :
  case HardCopyListener::JPEG :
  case HardCopyListener::PNG :
  case HardCopyListener::GIF :{
    BUG_MSG( "format postscript" );
    GdevPS ps( *m_ostrPrinter, m_name, x0, y0, x1, y1, realName, userName, pages(), orient );
    repaint( ps, true );
    break;
  }
  case HardCopyListener::HPGL :{
    BUG_MSG( "format hpgl" );
    GdevHPGL hpgl( *m_ostrPrinter, m_name, x0, y0, x1, y1, realName, userName, pages(), orient );
    repaint( hpgl, true );
    break;
  }
  case HardCopyListener::SVG :{
    BUG_MSG( "format svg" );
    GdevSVG svg( *m_ostrPrinter, m_name, x0, y0, x1, y1, realName, userName, pages(), orient );
    repaint( svg, true );
    break;
  }
  default :{
    BUG_MSG( "format undefined" );
    GdevPS ps( *m_ostrPrinter, m_name, x0, y0, x1, y1, realName, userName, pages(), orient );
    repaint( ps, true );
    break;
  }
  }

#ifdef HAVE_QT
  m_inEvent->write( * dynamic_cast<std::ostringstream*>(m_ostrPrinter) );
#else
  pthread_cond_signal( &m_printingDone );
#endif
}

void QtListPlot::doPrintPS() {
  BUG(BugGui, "QtListPlot::doPrintPS");
  BUG_MSG( "format postscript" );
  Gdev::Orientation orientation = m_canvas->getOrientation();

  std::string userName = AppData::Instance().UserName();
  std::string realName = AppData::Instance().RealName();
  GdevPS ps( *m_ostrPrinter, m_name, 0, 0,
             orientation == Gdev::ORIENT_LANDSCAPE ? 297 : 210,
             orientation == Gdev::ORIENT_LANDSCAPE ? 210 : 297,
             realName, userName, pages(), orientation );
  repaint( ps, true );
  delete m_ostrPrinter;
  m_ostrPrinter = 0;
#ifdef HAVE_QT
#else
  pthread_cond_signal( &m_printingDone );
#endif
}
/* --------------------------------------------------------------------------- */
/* Type --                                                                     */
/* --------------------------------------------------------------------------- */

GuiElement::ElementType QtListPlot::Type() {
  return GuiElement::type_ListPlot;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void QtListPlot::manage() {
  if (myWidget())
    myWidget()->show();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void QtListPlot::create() { //QWidget* parent
  if( m_canvas != 0 ) {
    if (m_width && m_height)
      m_canvas->setDrawingAreaSize( m_width, m_height );
    m_canvas->create();// parent );
  }
  // Popup Menue installieren
  createPopupMenu();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */

void QtListPlot::getSize( int &width, int &height ) {
  if( m_canvas != 0 )
    m_canvas->getSize( width, height );
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void QtListPlot::update( UpdateReason reason ) {
  BUG(BugPlot,"QtListPlot::update");

  switch( reason ) {
  case reason_FieldInput:
    BUG_EXIT( "reason_FieldInput" );
    break;
  case reason_Process:
    BUG_MSG( "reason_Process" );
    if( !isDataItemUpdated( GuiManager::Instance().LastGuiUpdate()) )
      return;
    break;
  case reason_Cycle:
    BUG_MSG( "reason_Cycle" );
    break;
  case reason_Cancel:
    BUG_EXIT( "reason_Cancel" );
    break;
  case reason_Always:
    BUG_MSG( "reason_Always" );
    break;
  default:
    BUG_MSG( "unhandled reason" );

  }
  readValues();
  if( m_gdev != 0 ) m_gdev->clear();
  writeValues();
  repaint( *m_gdev );

}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */

QWidget* QtListPlot::myWidget() {
  return m_canvas == 0 ? 0 : m_canvas->myWidget();
}

/* --------------------------------------------------------------------------- */
/* getPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

GuiPopupMenu* QtListPlot::getPopupMenu() {
  return m_canvas == 0 ? 0 : m_canvas->getPopupMenu();
}

/* --------------------------------------------------------------------------- */
/* repaint --                                                                  */
/* --------------------------------------------------------------------------- */

void QtListPlot::repaint( Gdev &g, bool allPages ) {
  BUG(BugPlot,"QtListPlot::repaint");

  Gdev *bak = m_gdev;
  m_gdev = &g;
  if( m_gdev == 0 ) {
    BUG_MSG("gdev is null");
    return;
  }
  // create legend
  m_legend = StreamToString( m_legendStream );


  // Zeichnungsflaeche unterteilen und den Graphen in diesen ausgeben
  int nvelmnts = m_graphs.size();
  for( int i=0; i < nvelmnts; i++ ) {
    int nhelmnts = m_graphs[i].size();
    std::vector<ListGraph*> velmnt = m_graphs[i];
    for( int j=0; j < nhelmnts; j++ ) {
      Frame plotarea;
      if( m_legend.size() != 0 ) {
	plotarea.x0 = (j)*(1.58/nhelmnts)-0.98;
	plotarea.y0 = (nvelmnts-1-i)*(1.96/nvelmnts)-0.98;
	plotarea.x1 = (j+1)*(1.58/nhelmnts)-0.98;
	plotarea.y1 = (nvelmnts-i)*(1.96/nvelmnts)-0.98;
      }
      else {
	plotarea.x0 = (j)*(1.88/nhelmnts)-0.98;
	plotarea.y0 = (nvelmnts-1-i)*(1.96/nvelmnts)-0.98;
	plotarea.x1 = (j+1)*(1.88/nhelmnts)-0.98;
	plotarea.y1 = (nvelmnts-i)*(1.96/nvelmnts)-0.98;
      }
      // Einzelner Graph ausgeben
      if( m_graphs[i][j] != 0 )
	(m_graphs[i][j])->drawGraph( m_gdev, plotarea, nhelmnts );
    }
  }

  // Legende ausgeben
  if( m_legend.size() != 0 ) {
    Frame legendarea( 0.6, 0.0, 1.0, 1.0 );
    drawLegend( legendarea );
  }
  g.endPage();
  if( typeid(*m_gdev) != typeid(GdevQt) )
    m_gdev = bak;
}

/* --------------------------------------------------------------------------- */
/* refresh --                                                                  */
/* --------------------------------------------------------------------------- */

void QtListPlot::refresh( Gdev &gdev ) {
  BUG(BugPlot, "QtListPlot::refresh" );
  repaint( gdev );
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void QtListPlot::setTitle( const std::string &title ) {
  if( m_canvas != 0 ) m_canvas->setTitle( title );
  m_title = title;
}

/* --------------------------------------------------------------------------- */
/* QtListPlot --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string &QtListPlot::getTitle() const {
  return m_title;
}

/* --------------------------------------------------------------------------- */
/* getPlot --                                                                  */
/* --------------------------------------------------------------------------- */

QtListPlot * QtListPlot::getPlot( const std::string &name ) {
  return 0;
}

/* --------------------------------------------------------------------------- */
/* addIndexButton --                                                           */
/* --------------------------------------------------------------------------- */

bool QtListPlot::addIndexButton( const std::string& title ) {
  return false;
}

/* --------------------------------------------------------------------------- */
/* addHGraph --                                                                */
/* --------------------------------------------------------------------------- */

void QtListPlot::addHGraph() {
  m_graphs.push_back( std::vector< ListGraph * >() );
}

/* --------------------------------------------------------------------------- */
/* addGraph --                                                                 */
/* --------------------------------------------------------------------------- */

ListGraph *QtListPlot::addGraph( const std::string &name ) {
  std::ostringstream ostr;
  ostr << "@ListGraphStruct" << m_name << "_" << getId();

  ListGraph *graph = new ListGraph( name, this, ostr.str() );
  m_graphs[m_graphs.size()-1].push_back( graph );
  return graph;
}

/* --------------------------------------------------------------------------- */
/* setCaptionStream --                                                         */
/* --------------------------------------------------------------------------- */

void QtListPlot::setCaptionStream( const std::string &streamname ) {
  m_legendStream = StreamManager::Instance().getStream( streamname );
}

/* --------------------------------------------------------------------------- */
/* allCycles --                                                                */
/* --------------------------------------------------------------------------- */

void QtListPlot::allCycles( bool state ) {
  m_allCycles = state;
  if( m_gdev != 0 ) m_gdev->clear();
}

/* --------------------------------------------------------------------------- */
/* detailGrid --                                                               */
/* --------------------------------------------------------------------------- */

void QtListPlot::detailGrid( bool state ) {
  if( state ) {
    m_nsubdivx = 2;
    m_nsubdivy = 2;
  }
  else {
    m_nsubdivx = 1;
    m_nsubdivy = 1;
  }
  if( m_gdev != 0 ) m_gdev->clear();
}

/* --------------------------------------------------------------------------- */
/* sameYRange --                                                               */
/* --------------------------------------------------------------------------- */

void QtListPlot::sameYRange( bool state ) {
  m_sameYRange = state;
  if( m_gdev != 0 ) m_gdev->clear();
}


/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

void QtListPlot::createPopupMenu() {
  GuiMenuButton *button;

  // Popup Menue generieren
  GuiPopupMenu *menu = GuiFactory::Instance()->createPopupMenu(this);
  menu->setTearOff( true );

  // Redraw Button generieren und an das Popup Menue anfuegen
  m_redrawButtonListener = new RedrawButtonListener( this );
  button = GuiFactory::Instance()->createMenuButton( menu->getElement(), m_redrawButtonListener );
  menu->attach( button->getElement() );
  button->setLabel( _("Redraw") );

  // Print Button generieren und an das Popup Menue anfuegen
  if (!AppData::Instance().HeadlessWebMode()) {
    m_printButtonListener = new PrintButtonListener( this );
    button = GuiFactory::Instance()->createMenuButton( menu->getElement(), m_printButtonListener );
    menu->attach( button->getElement() );
    button->setDialogLabel( _("Print") );
  }

  // AllCycles Button generieren und an das Popup Menue anfuegen
  m_allCyclesButtonListener = new AllCyclesButtonListener( this );
  m_buttonAllCycles =GuiFactory::Instance()->createMenuToggle( menu->getElement(), m_allCyclesButtonListener );
  menu->attach( m_buttonAllCycles->getElement() );
  m_buttonAllCycles->setLabel( _("All cases") );

  // DetailGrid Button generieren und an das Popup Menue anfuegen
  m_detailGridButtonListener = new DetailGridButtonListener( this );
  m_buttonDetailGrid = GuiFactory::Instance()->createMenuToggle( menu->getElement(), m_detailGridButtonListener );
  menu->attach( m_buttonDetailGrid->getElement() );
  m_buttonDetailGrid->setLabel( _("Detail grid") );

  // SameYRange Button generieren und an das Popup Menue anfuegen
  m_sameYRangeButtonListener = new SameYRangeButtonListener( this );
  m_buttonSameYRange = GuiFactory::Instance()->createMenuToggle( menu->getElement(), m_sameYRangeButtonListener );
  menu->attach( m_buttonSameYRange->getElement() );
  m_buttonSameYRange->setLabel( _("Same Y-Range") );

  // ScaleYRange Button generieren und an das Popup Menue anfuegen
  m_scaleYRangeButtonListener = new ScaleYRangeButtonListener( this );
  button = GuiFactory::Instance()->createMenuButton( menu->getElement(), m_scaleYRangeButtonListener );
  menu->attach( button->getElement() );
  button->setDialogLabel( _("Scale Y-Range") );

  // Popup Menue installieren,  reagiert auf den 3 Mausbutton
//   menu->installPopupEvent( myWidget(), 3 );
  if (m_canvas)
    m_canvas->setPopupMenu( menu );
  menu->getElement()->create();// myWidget() );
}

/* --------------------------------------------------------------------------- */
/* buildConfigDialog --                                                        */
/* --------------------------------------------------------------------------- */

void QtListPlot::buildConfigDialog() {
  BUG(BugPlot,"QtListPlot::buildConfigDialog");

  // Werte im DP aktualisieren
  writeValues();

  GuiFieldgroup     *group = 0;
  GuiFieldgroupLine *line = 0;
  GuiLabel          *label = 0;
  GuiMenuToggle     *toggle = 0;
  GuiButtonbar *bar = 0;
  GuiButton         *button = 0;

  m_configDialog = GuiFactory::Instance()->createForm( "ListPlotConfigDialog" );
  m_configDialog->setTitle( "ConfigDialog" );
  m_configDialog->resetCycleButton();
  m_configDialog->hasCloseButton( false );
  m_configDialog->getElement()->getDialog()->setApplicationModal();

  for( int i=0; i < m_graphs.size(); i++ ) {
    for( int j=0; j < m_graphs[i].size(); j++ ) {
      if( m_graphs[i][j] != 0 )
	(m_graphs[i][j])->buildConfigDialog( m_configDialog->getElement() );
    }
  }

  // Buttonbar generieren
  bar = GuiFactory::Instance()->createButtonbar( m_configDialog->getElement() );

  // Reset Button generieren und an das Buttonbar anfuegen
  m_cDresetButtonListener = new CDresetButtonListener( this );
  button = GuiFactory::Instance() -> createButton( bar->getElement(), m_cDresetButtonListener );
  button->setLabel( _("Reset") );

  // Close Button generieren und an das Buttonbar anfuegen
  m_cDcloseButtonListener = new CDcloseButtonListener( this );
  button = GuiFactory::Instance() -> createButton( bar->getElement(), m_cDcloseButtonListener );
  button->setLabel( _("Close") );

  m_configDialog->getElement()->create();

  for( int ii=0; ii < m_graphs.size(); ii++ ) {
    for( int jj=0; jj < m_graphs[ii].size(); jj++ ) {
      if( m_graphs[ii][jj] != 0 )
	(m_graphs[ii][jj])->cDupdateDatafields();
    }
  }

  m_configDialog->getElement()->manage();
}

/* --------------------------------------------------------------------------- */
/* openConfigDialog --                                                         */
/* --------------------------------------------------------------------------- */

void QtListPlot::openConfigDialog() {
  if( m_configDialog == 0 )
    buildConfigDialog();

  m_configDialog->getElement()->manage();
}

/* --------------------------------------------------------------------------- */
/* cDresetEvent --                                                             */
/* --------------------------------------------------------------------------- */

void QtListPlot::cDresetEvent() {
  writeValues( true );
  update( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* cDcloseEvent --                                                             */
/* --------------------------------------------------------------------------- */

void QtListPlot::cDcloseEvent() {
  m_configDialog->getElement()->unmanage();
}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool QtListPlot::isDataItemUpdated( TransactionNumber trans ) {
  bool ret = false;
  int nvelmnts = m_graphs.size();
  for( int i=0; i < nvelmnts; i++ ) {
    int nhelmnts = m_graphs[i].size();
    for( int j=0; j < nhelmnts; j++ ) {
      if( m_graphs[i][j] != 0 )
	if( (m_graphs[i][j])->isDataItemUpdated(trans) )
	  ret = true;
    }
  }
  return ret;
}

/* --------------------------------------------------------------------------- */
/* writeValues --                                                              */
/* --------------------------------------------------------------------------- */

void QtListPlot::writeValues( bool reset ) {
  BUG(BugPlot,"QtListPlot::writeValues");

  // Werte in DP aktualisieren
  for( int i=0; i < m_graphs.size(); i++ ) {
    for( int j=0; j < m_graphs[i].size(); j++ ) {
      if( m_graphs[i][j] != 0 )
	(m_graphs[i][j])->writeValues( reset );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* readValues --                                                               */
/* --------------------------------------------------------------------------- */

void QtListPlot::readValues() {
  BUG(BugPlot,"QtListPlot::readValues");

  // Werte in den Graphen aktualisieren
  for( int i=0; i < m_graphs.size(); i++ ) {
    for( int j=0; j < m_graphs[i].size(); j++ ) {
      if( m_graphs[i][j] != 0 )
	(m_graphs[i][j])->readValues();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* maxYRange --                                                                */
/* --------------------------------------------------------------------------- */

int QtListPlot::maxYRange() {
  int nvelmnts = m_graphs.size();
  int n=0;
  int max=0;

  for( int i=0; i < nvelmnts; i++ ) {
    int nhelmnts = m_graphs[i].size();
    for( int j=0; j < nhelmnts; j++, n++ ) {
      if( m_graphs[i][j] != 0 ) {
	if( (m_graphs[i][j])->nyaxis() > max )
	  max = (m_graphs[i][j])->nyaxis();
      }
    }
  }
  return max;
}

/* --------------------------------------------------------------------------- */
/* drawLegend --                                                               */
/* --------------------------------------------------------------------------- */

void QtListPlot::drawLegend( const Frame &viewport ) {
  m_gdev->setViewport( viewport.x0, viewport.y0, viewport.x1, viewport.y1 );
  m_gdev->setWindow( 0,0,1,1 );
  m_gdev->setColor( 0 );
  m_gdev->setTextAttributes( 0.0045, Gdev::HLEFT, Gdev::VTOP, 0, 0 );
  m_gdev->moveTo( 0.1, 0.9 );
  m_gdev->putText( m_legend );
}

/* --------------------------------------------------------------------------- */
/* StreamToString --                                                           */
/* --------------------------------------------------------------------------- */

std::string QtListPlot::StreamToString( Stream *s ) {
  if ( s != 0 ) {
    std::ostringstream ostr;
    s->write( ostr );
    return ostr.str();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool QtListPlot::acceptIndex( const std::string &name, int inx ){
  int nvelmnts = m_graphs.size();
  for( int i=0; i < nvelmnts; i++ ) {
    int nhelmnts = m_graphs[i].size();
    for( int j=0; j < nhelmnts; j++ ) {
      if( m_graphs[i][j] != 0 ){
	if( !(m_graphs[i][j])->acceptIndex( name, inx ) ){
	  return false;
	}
      }
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void QtListPlot::setIndex( const std::string &name, int inx ){
  int nvelmnts = m_graphs.size();
  for( int i=0; i < nvelmnts; i++ ) {
    int nhelmnts = m_graphs[i].size();
    for( int j=0; j < nhelmnts; j++ ) {
      if( m_graphs[i][j] != 0 ){
	(m_graphs[i][j])->setIndex( name, inx );
      }
    }
  }
  update( reason_Always );
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void QtListPlot::serializeXML(std::ostream &os, bool recursive){
}
