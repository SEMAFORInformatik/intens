
#include <math.h>
#include <limits>
#include <QStackedWidget>
#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_interval.h>
#include <qwt_plot_renderer.h>

#include <QtSvg/QSvgGenerator>
#include <QTemporaryFile>
#include <QLabel>

#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/StringUtils.h"
#include "gui/qt/GuiQtScrolledText.h"

#include "app/UiManager.h"


#include "operator/InputChannelEvent.h"

#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtPopupMenu.h"
#include "gui/qt/GuiQtPulldownMenu.h"
#include "gui/qt/GuiQtMenuButton.h"
#include "gui/qt/GuiQtMenuToggle.h"
#include "gui/qt/GuiQtButtonbar.h"
#include "gui/qt/GuiQtButton.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/qt/GuiQtManager.h"
#if HAVE_QGRAPHS
#include "gui/qt/GuiQtSurfaceGraph.h"
#include "gui/qt/GuiQtBarGraph.h"
#include "gui/qt/GuiQt3dData.h"
#else
#include "gui/qt/GuiQwtContourPlot.h"
#include "gui/qt/GuiQwtContourPlotData.h"
#endif
#include "gui/GuiFieldgroup.h"
#include "gui/qt/GuiQtFieldgroupLine.h"
#include "gui/qt/GuiQtLabel.h"
#include "gui/qt/GuiQtToggle.h"
#include "gui/GuiPlotDataItem.h"
#include "gui/qt/GuiQt3dPlot.h"
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQwtScaleDraw.h"
#include "gui/qt/GuiQwtScaleEngine.h"
#include "gui/qt/GuiQwtBasePlot.h"
#include "gui/GuiFactory.h"
typedef std::map<std::string, GuiPlotDataItem*> DataItemType;

INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
#if !HAVE_QGRAPHS
QwtLinearColorMap* GuiQt3dPlot::createColorMap() {
  QwtLinearColorMap* colormap;
  int num = std::max( m_valueList.size(), m_colorList.size() );
  // read from settings
  if (num > 1) {
    colormap = new QwtLinearColorMap(QColor(m_colorList[0].c_str()), QColor(m_colorList[num-1].c_str()));
    for (int i=1; i< num-1; ++i)
      colormap->addColorStop(m_valueList[i], QColor(m_colorList[i].c_str()));
  } else {
    // standard colors
    colormap = new QwtLinearColorMap(Qt::darkCyan, Qt::red);
    colormap->addColorStop(0.1, Qt::cyan);
    colormap->addColorStop(0.6, Qt::green);
    colormap->addColorStop(0.95,  Qt::yellow);
  }

  return colormap;
}
#endif

/* --------------------------------------------------------------------------- */
/* Constructor --                                                              */
/* --------------------------------------------------------------------------- */

GuiQt3dPlot::GuiQt3dPlot( GuiElement *parent, const std::string& name )
  : GuiQtElement( parent, name )
  , m_contourWidget( 0 )
#if HAVE_QGRAPHS
  , m_barWidget( 0 )
#endif
  , m_widgetStack( 0 )
  , m_name( name )
  , m_keyShiftPressed( false )
  , m_showText( true )
  , m_redrawButtonListener( this )
  , m_printButtonListener( this )
  , m_resetScaleButtonListener( this )
  , m_resetRotationButtonListener( this )
  , m_styleBarsButtonListener( this )
  , m_styleSurfaceButtonListener( this )
  , m_styleContourButtonListener( this )
  , m_drawHiddenLinesButtonListener( this )
  , m_perspectiveButtonListener( this )
  , m_deleteTextButtonListener( this )
  , m_showTextButtonListener( this )
  , m_drawMeshButtonListener( this )
  , m_drawShadedButtonListener( this )
  , m_drawContoursButtonListener( this )
  , m_drawZonesButtonListener( this )
  , m_configButtonListener( this )
  , m_drefStruct( 0 )
  , m_drefCameraRotationX( 0 )
  , m_drefCameraRotationY( 0 )
  , m_drefDistnMethod( 0 )
  , m_drefDistnTable( 0 )
  , m_configDialog( 0 )
  , m_cDresetButtonListener( 0 )
  , m_cDcloseButtonListener( 0 )
  , m_fieldNumDistnLevels( 0 )
  , m_fieldDistnTable( 0 )
  , m_lineDistnTable( 0 )
  , m_popupMenu( 0 )
  , m_PopupMenuCreated( false )
  , m_currentFileFormat( HardCopyListener::Postscript ) {
  m_showAnnotationLabels[0] = false;
  m_showAnnotationLabels[1] = false;

  // Hardcopy Listener intallieren
  UImanager::Instance().addHardCopy( m_name, this );
  init();
  createDataReference();
}

GuiQt3dPlot::GuiQt3dPlot( const GuiQt3dPlot &plot)
  : GuiQtElement(plot)
  , Gui3dPlot(plot)
  , m_contourWidget( 0 )
#if HAVE_QGRAPHS
  , m_barWidget( 0 )
#endif
  , m_widgetStack( 0 )
  , m_name( plot.m_name )
  , m_keyShiftPressed( plot.m_keyShiftPressed )
  , m_showText(  plot.m_showText )
  , m_redrawButtonListener( this )
  , m_printButtonListener( this )
  , m_resetScaleButtonListener( this )
  , m_resetRotationButtonListener( this )
  , m_styleBarsButtonListener( this )
  , m_styleSurfaceButtonListener( this )
  , m_styleContourButtonListener( this )
  , m_drawHiddenLinesButtonListener( this )
  , m_perspectiveButtonListener( this )
  , m_deleteTextButtonListener( this )
  , m_showTextButtonListener( this )
  , m_drawMeshButtonListener( this )
  , m_drawShadedButtonListener( this )
  , m_drawContoursButtonListener( this )
  , m_drawZonesButtonListener( this )
  , m_configButtonListener( this )
  , m_drefStruct( 0 )
  , m_drefCameraRotationX( 0 )
  , m_drefCameraRotationY( 0 )
  , m_drefDistnMethod( 0 )
  , m_drefDistnTable( 0 )
  , m_configDialog( 0 )
  , m_cDresetButtonListener( 0 )
  , m_cDcloseButtonListener( 0 )
  , m_fieldNumDistnLevels( 0 )
  , m_fieldDistnTable( 0 )
  , m_lineDistnTable( 0 )
  , m_popupMenu( 0 )
  , m_PopupMenuCreated( false )
  , m_currentFileFormat( HardCopyListener::Postscript )
  , m_withAnnotationOption( plot.m_withAnnotationOption ) {
  m_showAnnotationLabels[0] = plot.m_showAnnotationLabels[0];
  m_showAnnotationLabels[1] = plot.m_showAnnotationLabels[1];

  DataItemType::const_iterator iter;
  for( iter=plot.m_dataitems.begin(); iter != plot.m_dataitems.end(); ++iter ) {
    putDataItem( (*iter).first, (*iter).second);
  }

  init();
}

void GuiQt3dPlot::withAnnotationOption( GuiPlotDataItem* plotAxis, bool state ) {
  m_withAnnotationOption[plotAxis] = state;
}

bool GuiQt3dPlot::hasAnnotationOption(bool bXAxis) {
  GuiPlotDataItem *plotAxis = bXAxis ? getPlotDataItem("XAXIS") : getPlotDataItem("YAXIS");
  WithAnnotationMap::iterator it = m_withAnnotationOption.find(plotAxis);
  return (it != m_withAnnotationOption.end()) ? m_withAnnotationOption[plotAxis] : false;
}

void GuiQt3dPlot::showAnnotationLabels( bool bXAxis, bool status ) {
  m_showAnnotationLabels[(bXAxis?0:1)] = status;
}
bool GuiQt3dPlot::isShownAnnotationLabels( bool bXAxis ) {
  return m_showAnnotationLabels[(bXAxis?0:1)];
}


void GuiQt3dPlot::init () {
  //  createDataReference();

  m_supportedFileFormats[HardCopyListener::Postscript] = HardCopyListener::OWN_CONTROL;
  m_supportedFileFormats[HardCopyListener::PDF]        = HardCopyListener::OWN_CONTROL;
  m_supportedFileFormats[HardCopyListener::JPEG] = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::PNG]  = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::GIF]  = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::SVG]        = HardCopyListener::FILE_EXPORT;

  m_supportedFileFormats[HardCopyListener::BMP]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::PPM]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::TIFF]       = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::XBM]        = HardCopyListener::FILE_EXPORT;
  m_supportedFileFormats[HardCopyListener::XPM]        = HardCopyListener::FILE_EXPORT;
}

/* --------------------------------------------------------------------------- */
/* Destructor --                                                               */
/* --------------------------------------------------------------------------- */

GuiQt3dPlot::~GuiQt3dPlot(){
}

/* --------------------------------------------------------------------------- */
/* setAxesData --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setAxesData() {

  // Datenitems fuer x-, y- und z-Achse zuordnen
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");

  // set axis scales
  if (xaxis->getMinRange() < xaxis->getMaxRange()) {
    m_data->setRangeXUser(xaxis->getMinRange(), xaxis->getMaxRange());
  } else m_data->resetXUser();
  if (yaxis->getMinRange() < yaxis->getMaxRange()) {
    m_data->setRangeYUser(yaxis->getMinRange(), yaxis->getMaxRange());
  } else m_data->resetYUser();
  if (zaxis->getMinRange() < zaxis->getMaxRange()) {
    m_data->setRangeZUser(zaxis->getMinRange(), zaxis->getMaxRange());
  } else m_data->resetZUser();

  // set axis titles
#if HAVE_QGRAPHS
  auto plot = dynamic_cast<GuiQt3dBasePlot*>(m_widgetStack->currentWidget());
#endif
  if( xaxis != 0 && xaxis->getLabel().size() > 0 ){
#if HAVE_QGRAPHS
    plot->axisWidget(xBottom)->setTitle(QString::fromStdString(xaxis->getLabel()));
#else
    if (isSurfaceType()){
      m_contourWidget->axisWidget(xBottom)->setTitle(QString::fromStdString(xaxis->getLabel()));
    }
#endif
  }
  if( yaxis != 0 && yaxis->getLabel().size() > 0 ){
#if HAVE_QGRAPHS
    plot->axisWidget(yLeft)->setTitle(QString::fromStdString(yaxis->getLabel()));
#else
    if (isSurfaceType()){
      m_contourWidget->axisWidget(yLeft)->setTitle(QString::fromStdString(yaxis->getLabel()));
    }
#endif
  }
  if( zaxis != 0 && zaxis->getLabel().size() > 0 ){
#if HAVE_QGRAPHS
      plot->axisWidget(GuiElement::zAxis)->setTitle( QString::fromStdString(zaxis->getLabel()) );
#endif
  }
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::create(){

  BUG(BugGui,"GuiQt3dPlot::create");

  int width, height;
  getPlotSize(width, height);
  readSettings();

  // create data object
  m_widgetStack = new QStackedWidget();

#if HAVE_QGRAPHS
  m_data = new GuiQt3dData(m_dataitems);
  BUG_DEBUG("Create PlotStyle: "<<m_plotStyle.getStyle());
  if (isSurfaceType()) {
    assert(!m_contourWidget);
    m_contourWidget = new GuiQtSurfaceGraph(this);
    m_widgetStack->addWidget(m_contourWidget);
  }else if (isBarType()){
    assert(!m_barWidget);
    m_barWidget = new GuiQtBarGraph(this);
    m_widgetStack->addWidget(m_barWidget);
  }
#else
  assert(!m_contourWidget);
  m_data = new GuiQwtContourPlotData(m_dataitems);
  m_contourWidget = new GuiQwtContourPlot(this, createColorMap(), m_data);

  // x annotation
  if ( m_showAnnotationLabels[0]) {
    m_contourWidget->setAxisScaleDraw(QwtPlot::xBottom,  new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real, 'g', 10,-1 ));
    m_contourWidget->setAxisScaleEngine(QwtPlot::xBottom,  new GuiQwtLinearScaleEngine );
  }
  // y annotation
  if ( m_showAnnotationLabels[1]) {
    m_contourWidget->setAxisScaleDraw(QwtPlot::yLeft,  new GuiQwtScaleDraw( GuiQwtScaleDraw::type_real, 'g', 10,-1 ));
    m_contourWidget->setAxisScaleEngine(QwtPlot::yLeft,  new GuiQwtLinearScaleEngine );
  }
  m_widgetStack->addWidget(m_contourWidget);
#endif

  if (!isInitialSize()) {  // SIZE is set!
    QSize hs = m_widgetStack->sizeHint();
    QSize hsNew = hs.expandedTo(QSize(width, height));
    width = hsNew.width();
    height = hsNew.height();
  }
  ////widgetStack->setMinimumSize(QSize(width, height));
  m_widgetStack->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                           QSizePolicy::MinimumExpanding));

  setAxesData();

  // Popup Menue installieren
  createPopupMenu();

  // PlotSyle Ressourcen und Togglebuttons setzen (Initialisieren)
  setDrawButtons();
  setStyleButtons();
  drawZones(m_plotStyle.getFlagDrawZones());  // to make readSetting flags working
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::manage(){
  int width=300, height =400;
  myWidget()->setVisible( getVisibleFlag() );  // maybe function hide this GuiElement

  getPlotSize(width, height);
}

/* --------------------------------------------------------------------------- */
/* createPopupMenu --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::createPopupMenu() {

  // Popup Menue generieren
  m_popupMenu = new GuiQtPopupMenu( this );
  m_popupMenu->setTearOff( true );
  GuiQtMenuButton *button = 0;

  // Redraw Button generieren und an das Popup Menue anfuegen
  button = new GuiQtMenuButton( m_popupMenu, &m_redrawButtonListener );
  m_popupMenu->attach( button );
  button->setLabel( _("Redraw") );

  // Print Button generieren und an das Popup Menue anfuegen
  if (!AppData::Instance().HeadlessWebMode()) {
    button = new GuiQtMenuButton( m_popupMenu, &m_printButtonListener );
    m_popupMenu->attach( button );
    button->setDialogLabel( _("Print") );
  }
#if 1
  // ResetScale Button generieren und an das Popup Menue anfuegen
  button = new GuiQtMenuButton( m_popupMenu, &m_resetScaleButtonListener );
  m_popupMenu->attach( button );
  button->setLabel( _("Reset size") );

  // ResetRotation Button generieren und an das Popup Menue anfuegen
  button = new GuiQtMenuButton( m_popupMenu, &m_resetRotationButtonListener );
  m_popupMenu->attach( button );
  button->setLabel( _("Reset rotation") );

  // StyleBars Button generieren und an das Popup Menue anfuegen
  m_buttonStyleBars = new GuiQtMenuToggle( m_popupMenu, &m_styleBarsButtonListener );
  m_popupMenu->attach( m_buttonStyleBars->getElement() );
  m_buttonStyleBars->setLabel( _("Style bar") );

  // StyleSurface Button generieren und an das Popup Menue anfuegen
  m_buttonStyleSurface = new GuiQtMenuToggle( m_popupMenu, &m_styleSurfaceButtonListener );
  m_popupMenu->attach( m_buttonStyleSurface->getElement() );
  m_buttonStyleSurface->setLabel( _("Style surface") );

  // StyleContour Button generieren und an das Popup Menue anfuegen
  m_buttonStyleContour = new GuiQtMenuToggle( m_popupMenu, &m_styleContourButtonListener );
  m_popupMenu->attach( m_buttonStyleContour->getElement() );
  m_buttonStyleContour->setLabel( _("Style contour") );

  // DrawHiddenLines Button generieren und an das Popup Menue anfuegen
  m_buttonDrawHiddenLines = new GuiQtMenuToggle( m_popupMenu, &m_drawHiddenLinesButtonListener );
  m_popupMenu->attach( m_buttonDrawHiddenLines->getElement() );
  m_buttonDrawHiddenLines->setLabel( _("Draw hidden lines") );

  // Perspective Button generieren und an das Popup Menue anfuegen
  m_buttonPerspective = new GuiQtMenuToggle( m_popupMenu, &m_perspectiveButtonListener );
  m_popupMenu->attach( m_buttonPerspective->getElement() );
  m_buttonPerspective->setLabel( _("Perspective") );

  // DeleteText Button generieren und an das Popup Menue anfuegen
  button = new GuiQtMenuButton( m_popupMenu, &m_deleteTextButtonListener );
  m_popupMenu->attach( button->getElement() );
  button->setLabel( _("Delete Text Objects") );

  // ShowText Button generieren und an das Popup Menue anfuegen
  m_buttonShowText = new GuiQtMenuToggle( m_popupMenu, &m_showTextButtonListener );
  m_popupMenu->attach( m_buttonShowText->getElement() );
  m_buttonShowText->setLabel( _("Show Text Objects") );
  m_buttonShowText->setToggleStatus( m_showText );
#endif
  // Pulldown Menue PlotDeatails generieren und an das Popup Menue anfuegen
  createDetailMenu( m_popupMenu );
#if 1
  // Config Button generieren und an das Popup Menue anfuegen
  button = new GuiQtMenuButton( m_popupMenu, &m_configButtonListener );
  m_popupMenu->attach( button->getElement() );
  button->setDialogLabel( _("Configuration") );
#endif
  m_popupMenu->create();
  m_PopupMenuCreated = true;
}


/* --------------------------------------------------------------------------- */
/* popupMenu --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::popupMenu(const QContextMenuEvent* event){
  m_popupMenu->popup();
}

/* --------------------------------------------------------------------------- */
/* createDetailMenu --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::createDetailMenu( GuiPopupMenu *menu ){

  // Pulldown Menue PlotDeatails generieren und an das Popup Menue anfuegen
  GuiQtPulldownMenu *detailmenu = new GuiQtPulldownMenu( menu->getElement(), "Details" );
  detailmenu->setLabel( _("Detail options") );
  menu->attach( detailmenu->getElement() );
  detailmenu->setAlways();

#if 0
  // DrawMesh Button generieren und an das Pulldown Menue anfuegen
  m_buttonDrawMesh = new GuiQtMenuToggle( detailmenu, &m_drawMeshButtonListener );
  detailmenu->attach( m_buttonDrawMesh->getElement() );
  m_buttonDrawMesh->setLabel( _("Draw mesh") );

  // DrawShaded Button generieren und an das Pulldown Menue anfuegen
  m_buttonDrawShaded = new GuiQtMenuToggle( detailmenu, &m_drawShadedButtonListener );
  detailmenu->attach( m_buttonDrawShaded->getElement() );
  m_buttonDrawShaded->setLabel( _("Draw shaded") );
#endif

  // DrawContours Button generieren und an das Pulldown Menue anfuegen
  m_buttonDrawContours = new GuiQtMenuToggle( detailmenu, &m_drawContoursButtonListener );
  detailmenu->attach( m_buttonDrawContours->getElement() );
  m_buttonDrawContours->setLabel( _("Draw contours") );

  // DrawZones Button generieren und an das Pulldown Menue anfuegen
  m_buttonDrawZones = new GuiQtMenuToggle( detailmenu, &m_drawZonesButtonListener );
  detailmenu->attach( m_buttonDrawZones->getElement() );
  m_buttonDrawZones->setLabel( _("Draw interpolated") );
}


/* --------------------------------------------------------------------------- */
/* buildConfigDialog --                                                        */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::buildConfigDialog() {
  GuiFieldgroup     *group = 0;
  GuiQtFieldgroupLine *line = 0;
  GuiQtLabel     *label = 0;
  GuiQtMenuToggle     *toggle = 0;
  DataReference     *dref = 0;
  XferDataItem      *dataitem = 0;

  std::string formname;
  formname = "@Gui3dPlotConfigDialog" + m_name;
  GuiForm *form = GuiFactory::Instance()->createForm( formname );

  m_configDialog = form->getElement()->getQtElement()->getQtForm();
  m_configDialog->setTitle( _("ConfigDialog") );
  m_configDialog->resetCycleButton();
  m_configDialog->resetStandardForm();
//   m_configDialog->setApplicationModal();

  // Camera Axes Rotation
  group = GuiFactory::Instance() -> createFieldgroup( m_configDialog->getElement(), "ConfigParameters1");
  // Camera Axes Rotation
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  assert( line != 0 );
  label = new GuiQtLabel( line );
  label->setLabel( _("Camera Axes Rotation") );
  line->attach( label );

  // x axis
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  label = new GuiQtLabel( line );
  label->setLabel("X");
  line->attach( label );

  // create slider
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  dref = DataPoolIntens::getDataReference( m_drefStruct, "rotationX" );
  assert( dref != 0 );
m_drefCameraRotationX->SetValue(60);
 auto userAttr =  dynamic_cast<UserAttr*>(m_drefCameraRotationX->getUserAttr());
  userAttr->SetRange(-90, 90);
  userAttr->SetStep(10);
  dataitem = new XferDataItem();
  dataitem->setDataReference(m_drefCameraRotationX);
  auto *dataFieldRotX = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( line, dataitem ));
  dataFieldRotX->setLength(1);
  line->attach(dataFieldRotX);

  // y axis
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  label = new GuiQtLabel( line );
  label->setLabel("Y");
  line->attach( label );

  // create slider
  line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  dref = DataPoolIntens::getDataReference( m_drefStruct, "rotationY" );
  assert( dref != 0 );
m_drefCameraRotationY->SetValue(20);
  userAttr =  dynamic_cast<UserAttr*>(m_drefCameraRotationY->getUserAttr());
  userAttr->SetRange(-90, 90);
  userAttr->SetStep(10);
  dataitem = new XferDataItem();
  dataitem->setDataReference(m_drefCameraRotationY);
  auto *dataFieldRotY = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( line, dataitem ));
  dataFieldRotY->setLength(1);
  line->attach(dataFieldRotY);

  
  // dref = DataPoolIntens::getDataReference( m_drefStruct, "NumDistnLevels" );
  // assert( dref != 0 );
  // dataitem = new XferDataItem();
  // dataitem->setDataReference( dref );
  // m_fieldNumDistnLevels = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( line, dataitem ));
  // m_fieldNumDistnLevels->setLength( 10 );

  // // Customizing the Distribution Table
  // line = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  // assert( line != 0 );
  // label = new GuiQtLabel( line );
  // label->setLabel( _("Customizing the Distribution Table") );
  // line->attach( label );

  // m_configDialog->getElement()->attach(group->getElement());

  // // Distribution Table
  // group = GuiFactory::Instance() -> createFieldgroup( m_configDialog->getElement(), "ConfigParameters2" );

  // group->setTitle( _("Distribution Table") );
  // group->setTableSize( 5 );
  // group->setTableStep( 4 );

  // m_lineDistnTable = static_cast<GuiQtFieldgroupLine*>(group->addFieldgroupLine());
  // assert( m_lineDistnTable != 0 );

  // dref = DataPoolIntens::getDataReference( m_drefStruct, "DistnTable" );
  // assert( dref != 0 );
  // dataitem = new XferDataItem();
  // dataitem->setDataReference( dref );
  // XferDataItemIndex *index = dataitem->newDataItemIndex( 1 );
  // m_fieldDistnTable = dynamic_cast<GuiQtDataField*>(GuiFactory::Instance()->createDataField( m_lineDistnTable->getElement(), dataitem ));
  // m_fieldDistnTable->setLength( 10 );
  // label = new GuiQtLabel( line );
  // label->setLabel( _("Entries") );
  // m_lineDistnTable->attach( label );
  // m_lineDistnTable->attach( m_fieldDistnTable->getElement() );
  
  // Buttonbar generieren
  GuiButtonbar *bar = GuiFactory::Instance() -> createButtonbar( m_configDialog->getElement() );
  ///  m_configDialog->getElement()->attach( bar->getElement() );
  GuiButton *button;
  // Reset Button generieren und an das Buttonbar anfuegen
  m_cDresetButtonListener = new CDresetButtonListener( this );
  button = GuiFactory::Instance() -> createButton( bar->getElement(), m_cDresetButtonListener );
  button->setLabel( _("Reset") );
  // Close Button generieren und an das Buttonbar anfuegen
  m_cDcloseButtonListener = new CDcloseButtonListener( this );
  button = GuiFactory::Instance() -> createButton( bar->getElement(), m_cDcloseButtonListener );
  button->setLabel( _("Close") );

  m_configDialog->getElement()->attach(group->getElement());
}

/* --------------------------------------------------------------------------- */
/* createDataReference --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::createDataReference() {
  BUG(BugGui,"GuiQt3dPlot::createDataReference");

  DataPool &datapool = DataPoolIntens::getDataPool();
  std::string varname;
  varname = "@Gui3dPlotVar" + m_name;

  DataDictionary *dict_struct;
  dict_struct = datapool.AddToDictionary( ""
                                        , varname
					, DataDictionary::type_StructVariable
					, INTERNAL_XRT3DPLOT_STRUCT );
  assert( dict_struct != 0 );

  // Wir schützen die Variable vor dem Cycle-Clear. Sonst gehen immer
  // alle Einstellungen verloren.
  dict_struct->setItemProtected();
  static_cast<UserAttr*>(dict_struct->GetAttr())->SetEditable();

  m_drefStruct = DataPoolIntens::getDataReference( varname );
  assert( m_drefStruct != 0 );
  static_cast<UserAttr*>(m_drefStruct->getUserAttr())->SetEditable();

  m_drefCameraRotationX = DataPoolIntens::getDataReference( m_drefStruct, "rotationX" );
  assert( m_drefCameraRotationX != 0 );
  m_drefCameraRotationY = DataPoolIntens::getDataReference( m_drefStruct, "rotationY" );
  assert( m_drefCameraRotationY != 0 );

  m_drefDistnMethod = DataPoolIntens::getDataReference( m_drefStruct, "DistnMethod" );
  assert( m_drefDistnMethod != 0 );

  m_drefDistnTable = DataPoolIntens::getDataReference( m_drefStruct, "DistnTable" );
  assert( m_drefDistnTable != 0 );
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::update( UpdateReason reason ){
  BUG(BugGui, "GuiQt3dPlot::update");
  if (!m_widgetStack) return;

  switch( reason ){
  case reason_FieldInput:
    BUG_MSG( "reason_FieldInput" );
    if( !isDataItemUpdated(GuiQtManager::Instance().LastGuiUpdate()) )
      return;
    break;
  case reason_Process:
    BUG_MSG( "reason_Process" );
    if( !isDataItemUpdated(GuiQtManager::Instance().LastGuiUpdate()) )
      return;
    break;
  case reason_Cycle:
    BUG_MSG( "reason_Cycle" );
    ResetLastWebUpdated();  // reason_Always for webtens
    break;
  case reason_Cancel:
    BUG_MSG( "reason_Cancel" );
    ResetLastWebUpdated();  // reason_Always for webtens
    break;
  case reason_Always:
    BUG_MSG( "reason_Always" );
    ResetLastWebUpdated();  // reason_Always for webtens
    break;
  default:
    BUG_MSG( "unhandled reason" );
  }

  clearMessage();

  setAxesData();
  drawHeaderText();
  setAnnotationLabels();

  if (m_data){
#if HAVE_QGRAPHS
    auto plot = dynamic_cast<GuiQt3dBasePlot*>(m_widgetStack->currentWidget());
    plot->update(*m_data);
    plot->printLog();
#else
    m_data->updateData();
    m_contourWidget->update(*m_data);
#endif
  }

  if (m_plotStyle.getStyle() == CONTOUR) {
    drawMarkerLine();
  } else {
#if !HAVE_QGRAPHS
    m_plotStyle.setStyle(CONTOUR);
    std::cerr << "Warning, Overwrite 3d style." << std::endl;
#endif
  }

//   drawFooterText();
}

/* --------------------------------------------------------------------------- */
/* drawMarkerLine --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQt3dPlot::drawMarkerLine() {
#if !HAVE_QGRAPHS
  // delete old markerLines
  std::vector<QwtPlotCurve*>::iterator it;
  for (it = m_markerCurves.begin();
       it !=m_markerCurves.end(); ++it) {
    (*it)->detach();
  }
  m_markerCurves.erase(m_markerCurves.begin(), m_markerCurves.end());

  // draw new lines
  if (m_dataitems.size()>3) {
    for (int num=1; num  <= m_dataitems.size()-3; ++num) {
      GuiPlotDataItem *item = getPlotDataItem(compose("ZAXIS%1", num));
      XferDataItemIndex *iy = item->getDataItemIndexWildcard( 2 );
      long y_num = iy->getDimensionSize( item->Data() );

      XferDataItemIndex *ix =item->getMarkerLineXAxisXferData()->getDataItemIndexWildcard( 1 );
      int x_num = ix->getDimensionSize( item->getMarkerLineXAxisXferData()->Data() );

      y_num = y_num > x_num ? y_num : x_num;
      double* m_xArray = new double[y_num];
      double* m_yArray = new double[y_num];
      for (int i=0; i< y_num; ++i) {
        iy->setIndex( item->Data(), i );
        item->getValue( m_yArray[i] );
        m_yArray[i]*=item->getScaleFactor();

        ix->setIndex( item->getMarkerLineXAxisXferData()->Data(), i );
        item->getMarkerLineXAxisXferData()->getValue( m_xArray[i] );
        m_xArray[i]*=item->getMarkerLineXAxisScaleFactor();
      }
      QwtPlotCurve* plotCurve = new QwtPlotCurve( "label" );
      plotCurve->attach(m_contourWidget);
      plotCurve->setAxes( QwtPlot::xBottom, QwtPlot::yLeft );
      plotCurve->setSamples( m_xArray, m_yArray, y_num);
      QPen pen = plotCurve->pen();
      pen.setWidth(2);
      plotCurve->setPen(pen);
      delete [] m_xArray;
      delete [] m_yArray;
      m_markerCurves.push_back(plotCurve);
    }
  }
#endif
}

/* --------------------------------------------------------------------------- */
/* acceptIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::acceptIndex( const std::string &name, int inx ) {
  DataItemType::iterator iter;
  for( iter=m_dataitems.begin(); iter != m_dataitems.end(); ++iter ) {
    if( !((*iter).second->acceptIndex( name, inx )) )
      return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setIndex --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setIndex( const std::string &name, int inx ) {
  DataItemType::iterator iter;
  for( iter=m_dataitems.begin(); iter != m_dataitems.end(); ++iter )
    (*iter).second->setIndex( name, inx );
  update( reason_Always );

  std::vector<GuiQt3dPlot*>::iterator it = m_clonedList.begin();
  for (; it != m_clonedList.end(); ++it)
    (*it)->getElement()->setIndex(name, inx);
}

/* --------------------------------------------------------------------------- */
/* clone --                                                                    */
/* --------------------------------------------------------------------------- */
GuiElement* GuiQt3dPlot::clone() {
  GuiElement* baseElem = findElement( getName() );
  if (baseElem == this->getElement())
    m_clonedList.push_back( new GuiQt3dPlot( *this ) );
  else
    return baseElem->clone();
  return m_clonedList.back();
}

/* --------------------------------------------------------------------------- */
/* getCloneList --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQt3dPlot::getCloneList( std::vector<GuiElement*>& cList ) const {
  cList.clear();
  cList.insert(cList.begin(), m_clonedList.begin(), m_clonedList.end());
}

/* --------------------------------------------------------------------------- */
/* PrintType --                                                                */
/* --------------------------------------------------------------------------- */

HardCopyListener::FileFormat GuiQt3dPlot::getFileFormat() {
  return m_currentFileFormat;
}

/* --------------------------------------------------------------------------- */
/* MenuLabel --                                                                */
/* --------------------------------------------------------------------------- */

const std::string &GuiQt3dPlot::MenuLabel() {
  return m_menuText.empty() ? m_name : m_menuText;
}

/* --------------------------------------------------------------------------- */
/* isFileFormatSupported --                                                    */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::isFileFormatSupported( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats2::iterator it;
  bool result = false;
  for( it = m_supportedFileFormats.begin(); it != m_supportedFileFormats.end() && !result; ++it ){
    if( (*it).first == fileFormat )
      result = true;
  }
  return result;
}

/* --------------------------------------------------------------------------- */
/* isExportPrintFormat --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::isExportPrintFormat( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::PrintType  ptype = getPrintType(fileFormat);
  if (ptype==HardCopyListener::FILE_EXPORT ||
      ptype==HardCopyListener::OWN_CONTROL   )
    return true;
  return false;
}

/* --------------------------------------------------------------------------- */
/* getPrintType --                                                             */
/* --------------------------------------------------------------------------- */
HardCopyListener::PrintType GuiQt3dPlot::getPrintType( const HardCopyListener::FileFormat &fileFormat ){
  HardCopyListener::FileFormats2::iterator it = m_supportedFileFormats.find( fileFormat );
  if ( it == m_supportedFileFormats.end() ) {
    return HardCopyListener::NOT_SUPPORTED;
  } else
    return (*it).second;

}
/* --------------------------------------------------------------------------- */
/* write( InputChannelEvent &event ) --                                        */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::write( InputChannelEvent &event ) {
  ///  assert(false);
  BUG(BugGui, "GuiQt3dPlot::write");
  std::string msg;
  GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
  switch( hardcopy.FileFormat() ){
  case HardCopyListener::Postscript :
  case HardCopyListener::PDF :
  case HardCopyListener::PNG :
  case HardCopyListener::GIF :
    m_currentFileFormat = HardCopyListener::Postscript;
    break;
  case HardCopyListener::JPEG :
    m_currentFileFormat = HardCopyListener::JPEG;
    break;
  case HardCopyListener::SVG :
    m_currentFileFormat = HardCopyListener::SVG;
    break;
  default :
    msg = compose(_("%1: Selected file format is not supported."),getName() );
    printMessage( msg, GuiElement::msg_Error );
    return false;
  }
  // m_fpPrinter = 0;
  // m_fpPrinter = event.getFilePointer();
  // bool report = false;
  // writeFile( report );
  // return true;
  if( m_contourWidget == 0 ){
    create();
  }
  update(reason_Always);

  std::string tmp_eps_name;
  generateFileWithSvgGenerator( tmp_eps_name );
  // std::cout << "   GuiQTPlot::write file["<<tmp_eps_name<<"]\n";
  QFile tmp_eps(QString::fromStdString(tmp_eps_name));

  std::ostringstream os;
  if (tmp_eps.open(QIODevice::ReadOnly))
    os << tmp_eps.readAll().data();
  event.write( os );
  return true;
}

/* --------------------------------------------------------------------------- */
/* write( std::string fileName ) --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::write( const std::string &fileName) {
  BUG(BugGui, "GuiQt3dPlot::write");
  if( m_contourWidget == 0 ){
    create();
  }
  update(reason_Always);

  // Da von uns eps erwartet wird, muessen wir den Umweg ueber svg machen
  std::string ext;
  GuiQtPrinterDialog& hardcopy = GuiQtPrinterDialog::Instance();
  switch ( hardcopy.FileFormat()) {
    case HardCopyListener::Postscript:
    case HardCopyListener::PDF:
    case HardCopyListener::SVG:
      {
	std::string filename(fileName);
	return generateFileWithSvgGenerator( filename );
      }
    case HardCopyListener::JPEG:
      ext = "JPG";
      break;
    case HardCopyListener::PNG:
      ext = "PNG";
      break;
    case HardCopyListener::GIF:
      ext = "GIF";
      break;
    case HardCopyListener::BMP:
      ext = "BMP";
      break;
    case HardCopyListener::PPM:
      ext = "PPM";
      break;
    case HardCopyListener::TIFF:
      ext = "TIF";
      break;
    case HardCopyListener::XBM:
      ext = "XBM";
      break;
    case HardCopyListener::XPM:
      ext = "XPM";
      break;
    case HardCopyListener::HPGL:
      ext = "HPGL";
      break;
    case HardCopyListener::XML:
      ext = "XML";
      break;
    case HardCopyListener::JSON:
      ext = "JSON";
      break;
    case HardCopyListener::LaTeX:
      ext = "TEX";
      break;
    case HardCopyListener::GZ:
      ext = "GZ";
      break;
    case HardCopyListener::HTML:
      ext = "HTML";
      break;
    case HardCopyListener::ASCII:
    case HardCopyListener::Text:
      ext = "TXT";
      break;
    default:
      ext = "TXT";
      break;
  }
  if (ext.size()) {
    QWidget *w = (m_plotStyle.getStyle() == CONTOUR) ? (QWidget*)m_contourWidget : (QWidget*)NULL;
    QPixmap pm(w->width(),w->height());
    pm.fill();

    if (m_plotStyle.getStyle() == CONTOUR) {
#if HAVE_QGRAPHS
#else
      QwtPlotRenderer renderer;
      renderer.renderTo(m_contourWidget, pm);
#endif
    } else {
    }
    assert( !pm.isNull() );
    std::string fn = fileName;
    std::string::size_type  npos = fn.rfind("."+lower(ext));
    if (npos != (fn.size()-4))
      fn +lower(ext);
    if (!pm.save( QString::fromStdString(fn) ))
      GuiFactory::Instance()->showDialogInformation(this, _("unsuportted format"), _("unsuportted file format"));
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::saveFile( GuiElement *e ){
  GuiQtElement::saveFile(NULL);
  return true;
}

/* --------------------------------------------------------------------------- */
/* generateFileWithSvgGenerator( const std::string &fileName ) --              */
/* --------------------------------------------------------------------------- */

bool  GuiQt3dPlot::generateFileWithSvgGenerator(std::string& eps_result_file) {
  QSvgGenerator svg;
  QTemporaryFile tmp_svg("testXXXXXX.svg");
  auto ret = tmp_svg.open();

  // set svg properties
  svg.setFileName( tmp_svg.fileName() );

  // render svg
  if (m_plotStyle.getStyle() == CONTOUR) {
    if (!m_contourWidget)  return false;
    svg.setSize( (m_contourWidget->size().isNull()) ? m_contourWidget->sizeHint() : m_contourWidget->size());
    m_contourWidget->print( svg );
  }

  // only generating svg file => return
  std::string::size_type  pos = eps_result_file.rfind(".svg");
  if (pos == (eps_result_file.size()-4)) {  // end: .svg
    return tmp_svg.copy( QString::fromStdString(eps_result_file) );
  }

  return processConvert(tmp_svg.fileName().toStdString(), eps_result_file);
}

/* --------------------------------------------------------------------------- */
/* getDefaultSettings --                                                       */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::getDefaultSettings( HardCopyListener::PaperSize &size,
				    HardCopyListener::Orientation &orient,
				    int &quantity,
				    HardCopyListener::FileFormat &format,
				    double &lm, double &rm,
				    double &tm, double &bm ) {
  size = HardCopyListener::A4;
  orient = HardCopyListener::Landscape;
  quantity = 1;
  format = m_currentFileFormat;
  lm = 10;
  rm = 10;
  tm = 10;
  bm = 10;

  return true;
}

/* --------------------------------------------------------------------------- */
/* print --                                                                    */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::print(QPrinter* printer) {
  if( m_contourWidget == 0 ){
    create();
  }
  if (m_plotStyle.getStyle() == CONTOUR) {
    m_contourWidget->print(*printer);
  } else {
  }

}

/* --------------------------------------------------------------------------- */
/* isDataItemUpdated --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::isDataItemUpdated( TransactionNumber trans ) {
  BUG(BugGui,"GuiQt3dPlot::isDataItemUpdated");
  if (m_dataitems.size()>3) {
    GuiPlotDataItem *zaxis1 = getPlotDataItem("ZAXIS1");
    std::string s;
    if (zaxis1) {
      zaxis1->getAnnotationTitle(s);
    }
  }

  GuiPlotDataItem* plotitem;
  DataItemType::iterator iter;
  for( iter=m_dataitems.begin(); iter != m_dataitems.end(); ++iter ) {
    plotitem = (*iter).second;
    if( plotitem->isDataItemUpdated(trans) ) {
      BUG_MSG( "dataitem '" << plotitem->getName() <<"' is updated" );
      return true;
    }
  }
  if( m_drefStruct != 0 ) {
    if( m_drefStruct->isDataItemUpdated( DataReference::ValueUpdated, trans ) ){
      BUG_MSG( "struct '" << m_drefStruct->fullName( true ) << "' is updated" );
      return true;
    }
  }

  BUG_MSG( "No item are updated" );
  return false;
}

/* --------------------------------------------------------------------------- */
/* setDrawButtons --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setDrawButtons() {
  if( m_PopupMenuCreated ) {
#if 0
    m_buttonPerspective->setToggleStatus( m_plotStyle.getFlagDrawPerspective() );
    m_buttonDrawMesh->setToggleStatus( m_plotStyle.getFlagDrawMesh() );
    m_buttonDrawShaded->setToggleStatus( m_plotStyle.getFlagDrawShaded() );
#endif
    m_buttonDrawContours->setToggleStatus( m_plotStyle.getFlagDrawContours() );
    m_buttonDrawZones->setToggleStatus( m_plotStyle.getFlagDrawZones() );
  }
}

/* --------------------------------------------------------------------------- */
/* setStyleButtons --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setStyleButtons() {
  if( m_PopupMenuCreated ) {
    Style style = m_plotStyle.getStyle();
#if 1
    m_buttonStyleBars->setToggleStatus( ( style == BAR ? true : false ) );
    m_buttonStyleSurface->setToggleStatus( ( style == SURFACE ? true : false ) );
    m_buttonStyleContour->setToggleStatus( ( style == CONTOUR ? true : false ) );
#endif
  }
}

/* --------------------------------------------------------------------------- */
/* setPlotStyleBar --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setPlotStyleBar() {
  m_plotStyle.setStyle( BAR );
  setDetailStyle( true, true, true, true );
#if HAVE_QGRAPHS
  if (m_widgetStack) {
    if (!m_barWidget){
      m_barWidget = new GuiQtBarGraph(this);
      m_widgetStack->addWidget(m_barWidget);
    }
    m_widgetStack->setCurrentWidget(m_barWidget);
  }
  update( reason_Always );
#endif
}

/* --------------------------------------------------------------------------- */
/* setPlotStyleSurface --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setPlotStyleSurface() {
  m_plotStyle.setStyle( SURFACE );
  setDetailStyle( true, true, true, true );
  if (m_widgetStack) {
#if HAVE_QGRAPHS
    if (!m_contourWidget){
      m_contourWidget = new GuiQtSurfaceGraph(this);
      m_widgetStack->addWidget(m_contourWidget);
    }
    m_contourWidget->setPlotStyle(SURFACE);
#endif
    m_widgetStack->setCurrentWidget(m_contourWidget);
    update( reason_Always );
  }
}

/* --------------------------------------------------------------------------- */
/* setPlotStyleContour --                                                      */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setPlotStyleContour() {
  m_plotStyle.setStyle( CONTOUR );
  setDetailStyle( false, false, true, true );
  if (m_widgetStack){
    update( reason_Always );
#if HAVE_QGRAPHS
    if (!m_contourWidget){
      m_contourWidget = new GuiQtSurfaceGraph(this);
      m_widgetStack->addWidget(m_contourWidget);
    }
    m_contourWidget->setPlotStyle(CONTOUR);
#endif
    m_widgetStack->setCurrentWidget(m_contourWidget);
    update( reason_Always );
  }
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setTitle(const std::vector<std::string>& title) {
  if (title.size() == 0)
    return;

#if !HAVE_QGRAPHS
  if (m_contourWidget)
    m_contourWidget->setTitle( QString::fromStdString( title[0] ) );
#endif
}

/* --------------------------------------------------------------------------- */
/* setDetailStyle --                                                           */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setDetailStyle( bool mesh, bool shaded, bool contours, bool zones ) {
  m_plotStyle.setFlagDrawMesh( mesh );
  m_plotStyle.setFlagDrawShaded( shaded );
  m_plotStyle.setFlagDrawContours( contours );
  m_plotStyle.setFlagDrawZones( zones );

  setDrawButtons();
  setStyleButtons();

  m_plotStyle.setFlagStyleValid( true );
}

/* --------------------------------------------------------------------------- */
/* setMenuText --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::setMenuText( const std::string &menuText ) {
  m_menuText = menuText;
  return true;
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* setDataGrid --                                                              */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::setDataGrid() {
  BUG(BugGui,"GuiQt3dPlot::setDataGrid");
  int rows=0, cols=0;
  int x, y;

  bool make_irregular_grid = false;
//   double **values;

  // Datenitems fuer x-, y- und z-Achse zuordnen, z-Achse muss vorhanden sein
  GuiPlotDataItem *xaxis = getPlotDataItem("XAXIS");
  GuiPlotDataItem *yaxis = getPlotDataItem("YAXIS");
  GuiPlotDataItem *zaxis = getPlotDataItem("ZAXIS");

  // Rasterausdehnung berrechnen
  XferDataItemIndex *ixrows = zaxis->getDataItemIndexWildcard( 1 );
   rows = ixrows->getDimensionSize( zaxis->Data() );
   XferDataItemIndex *ixcols = zaxis->getDataItemIndexWildcard( 2 );
   for( int i=0; i < rows; i++ ) {
     ixrows->setIndex( zaxis->Data(), i );
     int tmp = ixcols->getDimensionSize( zaxis->Data() );
     if( tmp > cols ) cols = tmp;
   }

   // Debug Informationen
   int dims = zaxis->getNumDimensions();
   // Wenn x- und/oder y-Achse spezifiziert dann diese verwenden...
   if( xaxis ){
     if( xaxis->XferData() ){
       XferDataItemIndex *ix = xaxis->getDataItemIndexWildcard( 1 );
       int dimsize = ix->getDimensionSize( xaxis->Data() );
       x = 0;
       ix->setIndex( xaxis->Data(), x );
       while( xaxis->isValid() ){
	 x++;
	 ix->setIndex( xaxis->Data(), x );
       }
       rows = rows < x ? rows : x;
       make_irregular_grid = true;
     }
   }
   if( yaxis ){
     if( yaxis->XferData() ){
       XferDataItemIndex *iy = yaxis->getDataItemIndexWildcard( 1 );
       int dimsize = iy->getDimensionSize( yaxis->Data() );
       y = 0;
       iy->setIndex( yaxis->Data(), y );
       while( yaxis->isValid() ){
 	y++;
 	iy->setIndex( yaxis->Data(), y );
       }
       cols = cols < y ? cols : y;
       make_irregular_grid = true;
     }
   }
   // Die Dimension der Matrix muss mind. 2x2 sein, sonst laueft nichts
   if( rows < 2 || cols < 2 ) {
     BUG_EXIT("no valid data found => no grid");
     return;
   }
   // Raster erzeugen
//   if( make_irregular_grid ){
//     grid = Xrt3dMakeIrGridData( (int)rows, (int)cols
//                               , XRT3D_HUGE_VAL
//                               , TRUE );
//     values = grid->ig.values;
//   }
//   else {
//     grid = Xrt3dMakeGridData( (int)rows, (int)cols
//                             , XRT3D_HUGE_VAL
//                             , 1.0, 1.0  // step
//                             , 0.0, 0.0  // origin
//                             , TRUE );
//     values = grid->g.values;
//   }
  // Werte in Raster kopieren und skalieren
  for( x=0; x < rows; x++) {
    for( y=0; y < cols; y++) {
      ixrows->setIndex( zaxis->Data(), x );
      ixcols->setIndex( zaxis->Data(), y );
//       if( zaxis->getValue(values[x][y]) ) {
//         values[x][y] *= zaxis->getScaleFactor();
//       }
//       else {
//         values[x][y] = XRT3D_HUGE_VAL;
//       }
    }
  }
  // Wenn es sich um ein regulaeres Raster handelt,
  // ist die Arbeit somit beendet.
  if( !make_irregular_grid ) {
    return;
  }
  // Skalierung der x-Achse
  if( xaxis ) {
    if( xaxis->XferData() ) {
      XferDataItemIndex *ix = xaxis->getDataItemIndexWildcard( 1 );
      for( x=0; x < rows; x++) {
	ix->setIndex( xaxis->Data(), x );
// 	if( xaxis->getValue(grid->ig.xgrid[x]) )
// 	  grid->ig.xgrid[x] *= xaxis->getScaleFactor();
// 	else
// 	  grid->ig.xgrid[x] = 0;
      }
    }
    else {
      for( x=0; x < rows; x++) {
// 	grid->ig.xgrid[x] = (double)x;
      }
    }
  }
  // Skalierung der y-Achse
  if( yaxis ) {
    if( yaxis->XferData() ) {
      XferDataItemIndex *iy = yaxis->getDataItemIndexWildcard( 1 );
      for( y=0; y < cols; y++) {
	iy->setIndex( yaxis->Data(), y );
// 	if( yaxis->getValue(grid->ig.ygrid[y]) )
// 	  grid->ig.ygrid[y] *= yaxis->getScaleFactor();
// 	else
// 	  grid->ig.ygrid[y] = 0;
      }
    }
    else {
      for( y=0; y < cols; y++) {
// 	grid->ig.ygrid[y] = (double)y;
      }
    }
  }
//   return grid;
}

/* --------------------------------------------------------------------------- */
/* toggleStyle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::toggleStyle() {
  assert( false );
//   m_plotStyle.setFlagStyleBars( !m_plotStyle.getFlagStyleBars() );
//   setStyleButtons();
}

/* --------------------------------------------------------------------------- */
/* drawMesh --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawMesh() {
  setPlotStyleSurface();
  m_plotStyle.setFlagDrawMesh( !m_plotStyle.getFlagDrawMesh() );
}

/* --------------------------------------------------------------------------- */
/* drawShaded --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawShaded() {
  setPlotStyleSurface();
  m_plotStyle.setFlagDrawShaded( !m_plotStyle.getFlagDrawShaded() );
}

/* --------------------------------------------------------------------------- */
/* drawContours --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawContours(bool flag) {
  m_plotStyle.setFlagDrawContours(flag);
#if !HAVE_QGRAPHS
  if (m_contourWidget)
    m_contourWidget->showContour( flag );
#endif
}

/* --------------------------------------------------------------------------- */
/* drawZones --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawZones(bool flag) {
  m_plotStyle.setFlagDrawZones(flag);
#if !HAVE_QGRAPHS
  if (m_contourWidget)
    m_contourWidget->setResampleMode( flag ? QwtMatrixRasterData::BilinearInterpolation : QwtMatrixRasterData::NearestNeighbour);
#endif
  // make no sense => set contour mode off
  if (!flag) {
    drawContours(false);
    setDrawButtons();
  }
}

/* --------------------------------------------------------------------------- */
/* drawHiddenLines --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawHiddenLines() {
  setPlotStyleSurface();
  m_plotStyle.setFlagDrawHiddenLines( !m_plotStyle.getFlagDrawHiddenLines() );
}

/* --------------------------------------------------------------------------- */
/* drawPerspective --                                                          */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::drawPerspective() {
  if (m_plotStyle.getStyle() != CONTOUR) {
    m_plotStyle.setFlagDrawPerspective( !m_plotStyle.getFlagDrawPerspective() );
  }
}


/* --------------------------------------------------------------------------- */
/* resetScaleParameters --                                                     */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::resetScaleParameters() {
  if (m_plotStyle.getStyle() == CONTOUR) {
#if !HAVE_QGRAPHS
    m_contourWidget->resetScale();
#endif
  } else {
  }
}

/* --------------------------------------------------------------------------- */
/* resetRotationParameters --                                                  */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::resetRotationParameters() {
  if (m_plotStyle.getStyle() != CONTOUR) {
  }
}

/* --------------------------------------------------------------------------- */
/* deleteText --                                                               */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::deleteText() {
//   m_showText = true;
//   m_buttonShowText->setToggleStatus( true );
}

/* --------------------------------------------------------------------------- */
/* showText --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::showText( bool state ) {
//   m_showText = !m_showText;
//   m_buttonShowText->setToggleStatus( m_showText );
}

/* --------------------------------------------------------------------------- */
/* getPlotDataItem --                                                          */
/* --------------------------------------------------------------------------- */
GuiPlotDataItem* GuiQt3dPlot::getPlotDataItem(std::string axis_name, int num) {
  DataItemType::iterator iter;
  if (num >= 0) {
    iter = m_dataitems.find( compose(axis_name+"%1", num ));
  } else
    iter = m_dataitems.find( axis_name );
  if( iter != m_dataitems.end() ){
    return (*iter).second;
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* putDataItem --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiQt3dPlot::putDataItem( const std::string &name, GuiPlotDataItem *item ) {
  if( m_dataitems.insert(DataItemPair(name, item)).second ) {
    return true;
  }
  else {
    return false;
  }
}

/* --------------------------------------------------------------------------- */
/* openConfigDialog --                                                         */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::openConfigDialog() {
  if( m_configDialog == 0 )
    buildConfigDialog();

  assert( m_drefDistnMethod != 0 );

  int value = 0;
  m_drefDistnMethod->GetValue( value );
  m_configDialog->getElement()->manage();
}

/* --------------------------------------------------------------------------- */
/* cDresetEvent --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::cDresetEvent() {
//   for( int i=0; i < m_dataItems.size(); i++ ) { !!!!
//     GuiPlotDataItem* item = m_dataItems[i];
//     assert( item != 0 );
//     assert( i < m_cDyaxis1ToggleButton.size() );
//     assert( i < m_cDyaxis2ToggleButton.size() );
//     assert( i < m_cDxaxisToggleButton.size() );
//     m_drefYaxis1->SetValue( static_cast<int>(item->isYaxis1()), i );
//     m_cDyaxis1ToggleButton[i]->setToggleStatus( item->isYaxis1() );
//     m_drefYaxis2->SetValue( static_cast<int>(item->isYaxis2()), i );
//     m_cDyaxis2ToggleButton[i]->setToggleStatus( item->isYaxis2() );
//     m_drefXaxis->SetValue( static_cast<int>(item->isXaxis()), i );
//     m_cDxaxisToggleButton[i]->setToggleStatus( item->isXaxis() );
//   }

  update( reason_FieldInput );
}

/* --------------------------------------------------------------------------- */
/* cDcloseEvent --                                                             */
/* --------------------------------------------------------------------------- */

void GuiQt3dPlot::cDcloseEvent() {
  m_configDialog->getElement()->unmanage();
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQt3dPlot::serializeXML(std::ostream &os, bool recursive){
  return Gui3dPlot::serializeXML(os, recursive);
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  return Gui3dPlot::serializeJson(jsonObj, onlyUpdated);
}


/* --------------------------------------------------------------------------- */
/* serializeProtobuf --                                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
bool GuiQt3dPlot::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated){
  return Gui3dPlot::serializeProtobuf(eles, onlyUpdated);
}
#endif

/* --------------------------------------------------------------------------- */
/* PrintButtonListener  ButtonPressed --                                       */
/* --------------------------------------------------------------------------- */
void GuiQt3dPlot::PrintButtonListener::ButtonPressed() {
  GuiPrinterDialog::MyEventData event( ReportGen::PRINT );
  GuiFactory::Instance()->createPrinterDialog()->showDialog( m_plot, 0, &event );
}

/* --------------------------------------------------------------------------- */
/* writeSettings --                                                            */
/* --------------------------------------------------------------------------- */
#include <QSettings>
void GuiQt3dPlot::writeSettings() {
  return;
  if ( ! m_contourWidget ) {
    return;
  }

  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("Plot3d") );
  QString _tmp = QString::fromStdString(getName());
  settings->setValue(_tmp + ".countContourLevels", m_contourLevels);
  settings->setValue(_tmp + ".drawContour", m_plotStyle.getFlagDrawContours());
  settings->setValue(_tmp + ".drawInterpolated", m_plotStyle.getFlagDrawZones());
#if HAVE_QGRAPHS
#else
  if ( m_contourWidget->axisWidget(QwtPlot::yRight) ) {
    const QwtLinearColorMap *colorMap = (const QwtLinearColorMap*) m_contourWidget->axisWidget(QwtPlot::yRight)->colorMap();
    QList<QVariant> valueList;
    QStringList colorList;
    QVector< double > colors = colorMap->colorStops();
    const QwtInterval interval(0,1);
    for (int i=0; i < colors.size()-0; ++i) {
      valueList.append(colors.at(i));
      colorList.append(colorMap->color(interval, colors.at(i)).name(QColor::HexArgb) );
    }
    settings->setValue(_tmp + ".colorMap.valueList", QVariant(valueList));
    settings->setValue(_tmp + ".colorMap.colorList", QVariant(colorList));
  }
#endif
  settings->endGroup();
}

/* --------------------------------------------------------------------------- */
/* readSettings --                                                             */
/* --------------------------------------------------------------------------- */
void GuiQt3dPlot::readSettings() {
  return;
  m_valueList.clear();
  m_colorList.clear();
  QList<QVariant> valueList;
  QStringList colorList;
  QSettings *settings = GuiQtManager::Settings();
  settings->beginGroup( QString::fromStdString("Plot3d") );
  QString _tmp = QString::fromStdString(getName());
  m_contourLevels = settings->value(_tmp + ".countContourLevels", 10).toInt();
  m_plotStyle.setFlagDrawContours(settings->value(_tmp + ".drawContour",
                                                  m_plotStyle.getFlagDrawContours()).toBool());
  m_plotStyle.setFlagDrawZones(settings->value(_tmp + ".drawInterpolated",
                                               m_plotStyle.getFlagDrawZones()).toBool());
  valueList = settings->value(_tmp + ".colorMap.valueList", QList<QVariant>() ).toList();
  colorList = settings->value(_tmp + ".colorMap.colorList", QStringList() ).toStringList();
  int num = std::max( valueList.size(), colorList.size() );
  for (int i=0; i<num; ++i) {
    m_valueList.push_back( valueList.at(i).toDouble() );
    m_colorList.push_back( colorList.at(i).toStdString() );
  }
  settings->endGroup();
}

/* --------------------------------------------------------------------------- */
/* setAnnotationLabels --                                                      */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::setAnnotationLabels(){
  return true;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
QWidget* GuiQt3dPlot::myWidget() {
  return m_widgetStack;
}

/* --------------------------------------------------------------------------- */
/* isSurfaceType --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::isSurfaceType(){
  std::set<int> surfaceStyles = {CONTOUR, SURFACE};
  return surfaceStyles.find(m_plotStyle.getStyle()) != surfaceStyles.end();
}

/* --------------------------------------------------------------------------- */
/* isBarType --                                                                */
/* --------------------------------------------------------------------------- */
bool GuiQt3dPlot::isBarType(){
#if !HAVE_QGRAPHS
  return false;
#endif
  std::set<int> barStyles = {BAR};
  return barStyles.find(m_plotStyle.getStyle()) != barStyles.end();
}
