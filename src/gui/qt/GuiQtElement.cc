
/* QT headers */
#include <qwidget.h>
#include <qlabel.h>
#include <QMenu>
#include <QSizePolicy>
#include <QPixmap>
#include <QPainter>
#include <QtSvg/QSvgGenerator>
#include <QTemporaryFile>
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QProcess>
#include <QStyle>


/* System headers */
#include <assert.h>
#include <string>

#include "utils/Debugger.h"
#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/FileUtilities.h"
#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/GuiScrolledText.h"
#include "gui/qt/QtIconManager.h"
#include "xfer/XferDataItem.h"
#include "app/ReportGen.h"
#include "app/AppData.h"

INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* -------------------------------------------------------------------------- */
/* Constructor GuiQtElement --                                                */
/* -------------------------------------------------------------------------- */

GuiQtElement::GuiQtElement( GuiElement *parent, const std::string &name )
    : GuiElement( parent, name )
  , m_action(0)
  , m_save_listener( this )
{}

GuiQtElement::GuiQtElement( const GuiQtElement &el )
    : GuiElement( el )
  , m_action(el.m_action)
  , m_save_listener( this )
{}

/* -------------------------------------------------------------------------- */
/* setPosition --                                                             */
/* -------------------------------------------------------------------------- */

void GuiQtElement::setPosition( int x, int y ){

  QWidget *w = myWidget();
  if( w == 0 )
    return;

  w -> setGeometry( x, y, w->width(), w->height() );
}

void GuiQtElement::getPosition( int &x, int &y ){
  QWidget *w = myWidget();
  if( w != 0 ){
    x = w->x();
    y = w->y();
  }
  else {
    x=y=0;
  }
}

//----------------------------------------------------
// myCurrentWidget
//----------------------------------------------------
QWidget *GuiQtElement::myCurrentWidget(){
  return myWidget();
}

/* --------------------------------------------------------------------------- */
/* setStylesheet --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtElement::setStylesheet(const std::string& stylesheet){
  if(getStylesheet() == stylesheet) {
    return;
  }
  GuiElement::setStylesheet(stylesheet);
  QWidget * widget = myCurrentWidget();
  if (widget) {
    widget->setStyleSheet(QString::fromStdString(stylesheet));
  }

  // clones
  std::vector<GuiElement*> cloneList;
  getCloneList(cloneList);
  for (std::vector<GuiElement*>::iterator it =  cloneList.begin();
       it != cloneList.end(); ++it) {
    (*it)->setStylesheet(stylesheet);
    widget = (*it)->getQtElement()->myCurrentWidget();
    if (widget) {
      widget->setStyleSheet(QString::fromStdString(stylesheet));
    }
  }
}

//----------------------------------------------------
// manage
//----------------------------------------------------
void GuiQtElement::manage(){
  if( myWidget() ){
    myWidget()->show();
  }
}

//----------------------------------------------------
// unmanage
//----------------------------------------------------
void GuiQtElement::unmanage(){
  if( myWidget() && getName().size()){
    myWidget()->hide();
  }
}

//----------------------------------------------------
// map
//----------------------------------------------------
void GuiQtElement::map(){

  // clones
  std::vector<GuiElement*> cloneList;
  getCloneList(cloneList);
  if (cloneList.size()) {
    for (std::vector<GuiElement*>::iterator it =  cloneList.begin();
	 it != cloneList.end(); ++it) {
      (*it)->map();
    }
  }
  if( myWidget() ){
    myWidget()->show();
    if (!getVisibleFlag())
      update( GuiElement::reason_Always );
  }
  setVisibleFlag(true);
  // adapt max size
  GuiElement* myDialog = myParent(type_Form);
  if (myDialog) {
	dynamic_cast<GuiQtForm*>(myDialog)->startTimerSetMaximumSize();
  }
}

//----------------------------------------------------
// unmap
//----------------------------------------------------
void GuiQtElement::unmap(){
  // clones
  std::vector<GuiElement*> cloneList;
  getCloneList(cloneList);
  if (cloneList.size()) {
    for (std::vector<GuiElement*>::iterator it =  cloneList.begin();
	 it != cloneList.end(); ++it) {
      (*it)->unmap();
    }
  }
  setVisibleFlag(false);
  if( myWidget() && getName().size()){
    myWidget()->hide();
  }
  // adapt max size
  GuiElement* myDialog = myParent(type_Form);
  if (myDialog) {
	dynamic_cast<GuiQtForm*>(myDialog)->startTimerSetMaximumSize();
  }
}

//----------------------------------------------------
// unmap
//----------------------------------------------------
bool GuiQtElement::isShown(){
  return myWidget() ? myWidget()->isVisible() : false;
}
/* --------------------------------------------------------------------------- */
/* setAttachment --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtElement::setAttachment( int x1, int x2, int y1, int y2 ){
  BUG_PARA(BugGui,"GuiQtElement::setAttachment"
                 ," x1=" << x1 << ", x2=" << x2 << ", y1=" << y1 << ", y2=" << y2);
  QWidget *w = myWidget();
  if( w == 0 )
    return;
  // Left Attachment
  if( x1 < 0 ){
    //XtVaSetValues( w
    //             , XmNleftAttachment	 , XmATTACH_FORM
    //             , NULL );
  }
  else{
    //XtVaSetValues( w
    //             , XmNleftAttachment     , XmATTACH_POSITION
    //             , XmNleftPosition       , x1
    //             , NULL );
  }

  // Right Attachment
  if( x2 < 0 ){
    //XtVaSetValues( w
    //             , XmNrightAttachment    , XmATTACH_FORM
    //             , NULL );
  }
  else{
    //XtVaSetValues( w
    //             , XmNrightAttachment    , XmATTACH_POSITION
    //             , XmNrightPosition      , x2
    //             , NULL );
  }

  // Top Attachment
  if( y1 < 0 ){
    //XtVaSetValues( w
    //             , XmNtopAttachment      , XmATTACH_FORM
    //             , NULL );
  }
  else{
    //XtVaSetValues( w
    //             , XmNtopAttachment      , XmATTACH_POSITION
    //             , XmNtopPosition        , y1
    //             , NULL );
  }
  // Bottom Attachment
  if( y2 < 0 ){
    //XtVaSetValues( w
    //             , XmNbottomAttachment   , XmATTACH_FORM
    //             , NULL );
  }
  else{
    //XtVaSetValues( w
    //             , XmNbottomAttachment   , XmATTACH_POSITION
    //             , XmNbottomPosition     , y2
    //             , NULL );
  }
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */

bool GuiQtElement::destroy(){
  std::cerr << "GuiQtElement::destroy: Fatal !! not implemented Type["<<Type()<<"]" << std::endl;
  QWidget *w = myWidget();
  if( w != 0 ){
    //printWidgetNameTree( w, std::cerr ); std::cerr << std::endl;
    //printWidgetClassTree( w, std::cerr ); std::cerr << std::endl;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* printWidgetName --                                                          */
/* --------------------------------------------------------------------------- */

// void GuiQtElement::printWidgetNameTree( Widget w, std::ostream &ostr ){
//   if( w == 0 ){
//     ostr << ".[0]<no widget>";
//     return;
//   }
//   if( w->core.parent != 0 ){
//     printWidgetNameTree( w->core.parent, ostr );
//   }
//   if( w->core.name ){
//     ostr << "." << "[" << w << "]" << w->core.name;
//   }
//   else{
//     ostr << "." << "[" << w << "]<no name>";
//   }
// }

/* --------------------------------------------------------------------------- */
/* printWidgetTree --                                                          */
/* --------------------------------------------------------------------------- */

// void GuiQtElement::printWidgetClassTree( Widget w, std::ostream &ostr ){
//   if( w == 0 ){
//     ostr << ".<no widget>";
//     return;
//   }
//   if( w->core.parent != 0 ){
//     printWidgetClassTree( w->core.parent, ostr );
//   }
//   if( w->core.widget_class == 0 ){
//     ostr << ".<no widgetclass>";
//     return;
//   }
//   if( w->core.widget_class->core_class.class_name ){
//     ostr << "." << w->core.widget_class->core_class.class_name;
//   }
//   else{
//     ostr << ".<no classname>";
//   }
// }


/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* addBorders --                                                               */
/* --------------------------------------------------------------------------- */

// void GuiQtElement::addBorders( Widget w, Dimension &width, Dimension &height ){
//   if( w == 0 ) return;

//   Dimension borderWidth = 0;
//   XtVaGetValues( w
// 	       , XmNborderWidth	, &borderWidth
//                , NULL );
//   width  += (borderWidth*2);
//   height += (borderWidth*2);
// }

// /* --------------------------------------------------------------------------- */
// /* getMargins --                                                               */
// /* --------------------------------------------------------------------------- */

// void GuiQtElement::getMargins( Widget w, int &width, int &height ){
//   if( w == 0 ) return;

//   Dimension marginHeight = 0, marginWidth = 0;
//   XtVaGetValues( w
// 	       , XmNmarginHeight       , &marginHeight
//                , XmNmarginWidth        , &marginWidth
//                , NULL );
//   width  = (int)marginWidth;
//   height = (int)marginHeight;
// }

// /* --------------------------------------------------------------------------- */
// /* getShadows --                                                               */
// /* --------------------------------------------------------------------------- */

// void GuiQtElement::getShadows( Widget w, int &width, int &height ){
//   if( w == 0 ) return;

//   Dimension shadow;
//   XtVaGetValues( w
// 	       , XmNshadowThickness    , &shadow
//                , NULL );
//   width  = (int)(shadow*2);
//   height = (int)(shadow*2);
// }

// /* --------------------------------------------------------------------------- */
// /* getScrollbarSize --                                                         */
// /* --------------------------------------------------------------------------- */

// void GuiQtElement::getScrollbarSize( Widget sw, int &width, int &height ){
//   if( sw == 0 ) return;

//   Widget	hw = 0 ,vw = 0;
//   Dimension	w,h,shadow,spacing;

//   width  = 0;
//   height = 0;

//   XtVaGetValues( sw
// 	       , XmNhorizontalScrollBar	, &hw
// 	       , XmNverticalScrollBar	, &vw
// 	       , XmNspacing             , &spacing
// 	       , XmNshadowThickness     , &shadow
// 	       , NULL );

//   if ( hw != NULL ) {
//     height = (int)(spacing + shadow*2);
//     XtVaGetValues( hw
// 	         , XmNheight		, &h
// 	         , NULL );
//     height += (int)(h + 4); // Warum gerade 4 ist leider unklar. bh
//   }
//   if ( vw != NULL ) {
//     width  = (int)(spacing + shadow*2);
//     XtVaGetValues( vw
// 	         , XmNwidth		, &w
// 	         , NULL );
//     width += (int)(w + 4); // Warum gerade 4 ist leider unklar. bh
//   }
// }

/* --------------------------------------------------------------------------- */
/* getQtAlignment --                                                           */
/* --------------------------------------------------------------------------- */

Qt::AlignmentFlag GuiQtElement::getQtAlignment( ){
  return getQtAlignment(getAlignment());
}

Qt::AlignmentFlag GuiQtElement::getQtAlignment( Alignment align ){
  switch( align ){
  case align_Default:
    return (Qt::AlignmentFlag) 0;
  case align_Left:
    return Qt::AlignLeft;
  case align_Right:
    return Qt::AlignRight;
  case align_Center:
    return Qt::AlignHCenter;
  case align_Top:
    return Qt::AlignTop;
  case align_Bottom:
    return Qt::AlignBottom;
  case align_Stretch:
    return (Qt::AlignmentFlag) 0;
  default:
    break;
  }
  return Qt::AlignLeft;
}

/* --------------------------------------------------------------------------- */
/* enterEvent --                                                               */
/* --------------------------------------------------------------------------- */
void GuiQtElement::enterEvent ( QEnterEvent *e )
{
  if ( myWidget() ) myWidget()->setToolTip( "" ); // to be sure, we cleared previously set tooltip
  if( !showHelptext() ) return;
  if( !hasHelptext() ) return;

  std::string text;
  getHelptext( text );
  if (text.size()) {
    if (AppData::Instance().Helpmessages() == AppData::StatusBarType) {
      printMessage( text, msg_Help, 0 );
    } else if (AppData::Instance().Helpmessages() == AppData::ToolTipType) {
      QString s = QString::fromStdString(text);
#if QT_VERSION > 0x050200
      myWidget()->setToolTipDuration(AppData::Instance().ToolTipDuration());
#endif
      myWidget()->setToolTip( s );
    }
  }
}


/* --------------------------------------------------------------------------- */
/* grabShortcutAction --                                                       */
/* --------------------------------------------------------------------------- */
void GuiQtElement::grabShortcutAction( std::string& accelerator_key, QAction *act ) {
  if (getParent())
    getParent()->getQtElement()->grabShortcutAction(accelerator_key, act);
}

/* --------------------------------------------------------------------------- */
/* grabFocus --                                                                */
/* --------------------------------------------------------------------------- */
void GuiQtElement::grabFocus() {
  if (myWidget())
    myWidget()->setFocus();
}

/* --------------------------------------------------------------------------- */
/* getSize --                                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtElement::getSize(int &w, int &h, bool hint) {
  if (myWidget()) {
    w = 0;
    h = 0;
    // if (!myWidget()->isVisible()) {
    //   return;
    // }
    QSize s = hint ? myWidget()->sizeHint() : myWidget()->size();
    if (s.isValid()) {
      w= s.width();
      h= s.height();
    }
  }
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */
void GuiQtElement::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds) {
  int w, wh, h, hh;
  getSize(wh, hh, true);
  getSize(w, h, false);
  QSize minSize = myWidget()->minimumSize();

  while(--intent>0) os << "  ";
  if (getName().size() > 0) {
    os << "Name[" << getName() << "] ";
  }
  os << "Type[" << StringType()
     << "] HintSize[" << wh << ", " << hh
     << "] Size[" << w << ", " << h
     << "] minimumSize[" << minSize.width() << ", " << minSize.height() << "]\n";
}

/* --------------------------------------------------------------------------- */
/* getDialog --                                                                 */
/* --------------------------------------------------------------------------- */

GuiDialog *GuiQtElement::getDialog(){
  GuiElement *el = myParent( type_Form );
  if( el == 0 ){
    return 0;
  }
  GuiQtForm *form=0;
  form = el->getQtElement()->getQtForm();
  return form -> getDialog();
}

/* --------------------------------------------------------------------------- */
/* getDialog --                                                                 */
/* --------------------------------------------------------------------------- */

GuiQtDialog *GuiQtElement::getQtDialog(){
  GuiElement *el = myParent( type_Form );
  if( el == 0 ){
    return 0;
  }
  GuiQtForm *form=0;
  form = el->getQtElement()->getQtForm();
  return form -> getQtDialog();
}

/* -------------------------------------------------------------------------- */
/* getSaveAction --                                                           */
/* -------------------------------------------------------------------------- */
JobAction* GuiQtElement::getSaveAction( XferDataItem *xfer ) {
  if( xfer ){
    if( xfer->getDataType() != DataDictionary::type_String ){
      return 0;
    }
  }
  FileJobAction *action = new FileJobAction(this, false);
  action->setXfer( xfer );
  action->setSilent();
  return action;
}

/* -------------------------------------------------------------------------- */
/* getSaveAction --                                                           */
/* -------------------------------------------------------------------------- */
JobAction* GuiQtElement::getSaveAction( const std::string& filename ) {
  FileJobAction *action = new FileJobAction(this, false);
  action->setFilename( filename );
  action->setSilent();
  return action;
}

/* --------------------------------------------------------------------------- */
/* saveFile --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiQtElement::saveFile( FileJobAction *action ){
  BUG( BugGui, "GuiQtElement::saveFile" );
  std::vector<HardCopyListener::FileFormat> formats;
  m_action = action;

  if( 0/*isLogOn()*/ ){
    std::stringstream logmsg;
    logmsg << DateAndTime() << ": BEGIN : SAVE FileStream " <<
      getName() << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }

  XferDataItem *xfer = 0;
  if( action ){
    std::string s;
    xfer = action->getXfer();
    if( xfer ){
      xfer->getValue( s );
    } else {
      s = action->getFilename();
    }
    if( !s.empty() ){
      m_save_listener.FileSelected( s, HardCopyListener::NONE, "" );
      return;
    }
  } else {
    formats.push_back(HardCopyListener::PNG);
  }
  if (m_dir.empty()) m_dir = ".";

  // get filter
  QtIconManager::IconTypeMap::iterator tit = QtIconManager::getIconTypeMap().begin();
  m_filter = "";
  for( ; tit != QtIconManager::getIconTypeMap().end(); ++tit){
    if (m_filter.size())
      m_filter += ";;";
    m_filter += tit->second.filter;
  }

  GuiFactory::Instance()->showDialogFileSelection
    ( 0
      , compose(_("Save %1"),getName())
      , m_filter
      , m_dir
      , &m_save_listener
      , DialogFileSelection::Save
      , &formats
      , DialogFileSelection::AnyFile
      , getName()
      );

}

/* -------------------------------------------------------------------------- */
/* generateFileWithSvgGenerator --                                            */
/* -------------------------------------------------------------------------- */

bool GuiQtElement::generateFileWithSvgGenerator(QIODevice* ioDevice, std::string& filename, bool bPrinter) {
  QSvgGenerator svg;
  QTemporaryFile tmp_svg(QString::fromStdString(compose("%1%2tmpXXXXXX.svg", QDir::tempPath().toStdString(),QDir::separator().toLatin1())));
  if(!tmp_svg.open()) return false;
  ReportGen::Instance().newTmpFile( tmp_svg.fileName().toStdString() );

  // set svg properties
  if (filename.size()) { svg.setFileName(tmp_svg.fileName()); }
  if ( FileUtilities::getSuffix(filename) == "svg") {
    if (ioDevice) { svg.setOutputDevice(ioDevice); }  // output device überschreibt
  }
  svg.setSize( (myWidget()->size().isNull()) ? 1.5*myWidget()->sizeHint() :  2*myWidget()->size());

  // render svg
  QPainter painter;
  painter.begin(&svg);
  if (bPrinter) // printer -> only draw children
    myWidget()->render(&painter, QPoint(),QRegion(), QWidget::DrawChildren);
  else
    myWidget()->render(&painter);
  painter.end();

  // only generating svg file => return
  QSaveFile* qFile = dynamic_cast<QSaveFile*>(ioDevice);
  if ( FileUtilities::getSuffix(filename) == "svg") {
    // weil das copy den bestehenden File nicht überschreibt, löschen wir ihn zuerst
    QFile rf(QString::fromStdString(filename));
    rf.remove();
    if (qFile) { qFile->commit(); }
    return qFile ? qFile->commit() : tmp_svg.copy( QString::fromStdString(filename) );
  }

  return processConvert(tmp_svg.fileName().toStdString(), filename);
}

/* -------------------------------------------------------------------------- */
/* processInkscapeConvert --                                                  */
/* -------------------------------------------------------------------------- */
bool processInkscapeConvert(const std::string &inFilename, std::string &outFilename, std::string suffix) {
  if (suffix != "pdf" && suffix != "eps")
    // no inkscape converting
    return false;

  QString program("inkscape");
  QStringList arguments = {
    "-D",
    QString::fromStdString("--export-filename=" + outFilename),
    QString::fromStdString(inFilename)
  };
  QProcess proc;
  proc.start(program, arguments);
  bool finished = proc.waitForFinished();  // wait for inkscape command to finish, using default timeout of 30s
  QProcess::ExitStatus exitStatus = proc.exitStatus();
  int exitCode = proc.exitCode();
  bool success = (finished && exitStatus == QProcess::NormalExit && exitCode == 0);
  if(finished && !success) {
    std::string cmd = program.toStdString() + " " + arguments.join(" ").toStdString();
    // !finished: inkscape not found -> ok, convert will be tried
    // finished && !success: inkscape had a problem -> warning
    BUG_WARN("command '" << cmd << "' failed: "
             << "exitStatus = " << exitStatus
             << ", exitCode: " << exitCode);
  }
  return success;
}
/* -------------------------------------------------------------------------- */
/* processConvert --                                                          */
/* -------------------------------------------------------------------------- */

bool GuiQtElement::processConvert( const std::string &inFilename, std::string &outFilename) {
  BUG_PARA( BugGui, "GuiQtElement::processConvert","inFile '"
           << inFilename << "' outFile '" << outFilename << "'");

  // detect output filename suffix
  QFileInfo ofinfo( QString::fromStdString(outFilename) );
  if (ofinfo.suffix().size() == 0) {
    BUG_MSG("no suffix detected");
  }
  QString suffix = ofinfo.suffix().size() ? ofinfo.suffix() : "eps";

  // no output filename => create temporary filename
  if (outFilename.size() == 0) {
    std::string tmp_out_filename;
    QTemporaryFile tmp_eps( QString::fromStdString(compose("%1%2XXXXXX.%3", QDir::tempPath().toStdString(),QDir::separator().toLatin1(), suffix.toStdString())) );
    if(!tmp_eps.open()) return false;
    //  tmp_out_filename
    outFilename =  tmp_eps.fileName().toStdString();
    ReportGen::Instance().newTmpFile( outFilename );
    tmp_eps.remove();
  }

  // special case converting svg to eps/pdf with inkscape
  if (processInkscapeConvert(inFilename, outFilename, suffix.toStdString()))
    return true;

  // convert to eps format
  QString program("convert");
  QStringList arguments = {
    QString::fromStdString(inFilename),
    QString::fromStdString(outFilename)
  };
  QProcess proc;
  proc.start(program, arguments);
  if (proc.waitForFinished()) {
    return true;
  } else {
    GuiFactory::Instance()->showDialogWarning(NULL, _("title"),
                                              _("no convert command tool installed.\n\n(see at http://www.imagemagick.org)"), NULL);
  }

  return false;
}

/* -------------------------------------------------------------------------- */
/* writeFile --                                                               */
/* -------------------------------------------------------------------------- */

void GuiQtElement::writeFile(QIODevice* ioDevice, const std::string &filename, bool bPrinter ){
  BUG_PARA( BugGui, "GuiQtElement::writeFile","File '" << filename << "'" );
  QWidget* w = myWidget();
  bool widget_hide(!isShown());

  // 2014-03-27 create widget if not exists
  if (!w) {
    if (getMyForm()) {
      getMyForm()->getElement()->getQtElement()->create();
    }
    updateForms(reason_Always);
    w = myWidget();
  }
  if (widget_hide) {
    manage();
    w->show();
  }
  update(reason_Always);

  if (!w) {
    GuiFactory::Instance()->showDialogInformation(this,
                                                  compose(_("File '%1' not yet created!"), filename),
                                                  compose(_("Please create GuiElement '%1' first."), getName()));
    endFileStream( JobAction::job_Aborted );
    return;
  }

  // only generating svg file => return
  std::string suffix = FileUtilities::getSuffix(filename);
  if (suffix == "eps" || suffix == "svg" || suffix == "pdf") {  // vector graphic
    std::string fn(filename);
    bool ret;
    if (ioDevice) {
      ret = generateFileWithSvgGenerator(ioDevice, fn, bPrinter);
    } else {
      // QSaveFile file(filename.c_str());
      // ret = generateFileWithSvgGenerator(&file, fn, bPrinter);
      // file.commit();
      ret = generateFileWithSvgGenerator(0, fn, bPrinter);
    }
    BUG_INFO("eps file["<<filename<<"]  RET["<<ret<<"] name["<<getName()<<"]");
    endFileStream( JobAction::job_Ok );
    return;
  }

  QSize s(w->sizeHint());
  QPixmap pm(s.width(), s.height());
  w->resize(s.width(), s.height());
  pm.fill();
  QPainter p(&pm);
  BUG_INFO("writeFile name: " << filename << ", Size: " << w->width() << ", " <<  w->height());
  if (bPrinter) // printer -> only draw children
    w->render(&p, QPoint(),QRegion(), QWidget::DrawChildren);
  else
    w->render(&p);
  if (ioDevice) {
    pm.save(ioDevice, "png");
  } else {
    pm.save(QString::fromStdString(filename));
    endFileStream( JobAction::job_Ok );
  }
  if (widget_hide) {
    w->hide();
  }
}


/* --------------------------------------------------------------------------- */
/* streamableObject - -                                                        */
/* --------------------------------------------------------------------------- */

BasicStream* GuiQtElement::streamableObject() {
  return dynamic_cast<BasicStream*>(this);
}

/* --------------------------------------------------------------------------- */
/* endFileStream --                                                            */
/* --------------------------------------------------------------------------- */

void GuiQtElement::endFileStream( JobAction::JobResult rslt ){

  if( m_action )
    m_action->endFileStream( rslt );
  if( 0/*isLogOn()*/ ){
    std::ostringstream logmsg;
    logmsg << DateAndTime() << ": ";
    switch( rslt ){
    case JobAction::job_Ok :
    case JobAction::job_Nok :
      logmsg << "END : ";
      break;
    case JobAction::job_Aborted :
      logmsg << "ABORT : ";
      break;
    case JobAction::job_Canceled :
      logmsg << "CANCEL : ";
      break;
    case JobAction::job_FatalError :
      logmsg << "FATAL ERROR : ";
      break;
    default :
      break;
    }
    logmsg << "GuiElement " << getName() << std::endl;
    GuiFactory::Instance()->getLogWindow()->writeText( logmsg.str() );
  }
}

/* -------------------------------------------------------------------------- */
/* GuiQtElement::printOrientations --                                         */
/* -------------------------------------------------------------------------- */
std::string printOrientations(Qt::Orientations pol) {
  std::string ret;
  if (pol & Qt::Horizontal)
    ret = "Horizontal";
  if (pol & Qt::Vertical) {
    ret += std::string(ret.size() ? ", ": "") + "Vertical";
  }
  return ret;
}

/* -------------------------------------------------------------------------- */
/* GuiQtElement::printSizePolicy --                                           */
/* -------------------------------------------------------------------------- */
std::string printSizePolicy(QSizePolicy pol) {
  std::string ret("H{");
  bool note=false;
  if (pol.horizontalPolicy() & QSizePolicy::ExpandFlag)
    ret += "Expand", note=true;
  if (pol.horizontalPolicy() & QSizePolicy::ShrinkFlag)
    ret += std::string(note ? ", " : "") + "Shrink", note=true;
  if (pol.horizontalPolicy() & QSizePolicy::GrowFlag)
    ret += std::string(note ? ", " : "") + "Grow";
  note = false;
  ret += "} V{";
  if (pol.verticalPolicy() & QSizePolicy::ExpandFlag)
    ret += "Expand", note=true;
  if (pol.verticalPolicy() & QSizePolicy::ShrinkFlag)
    ret += std::string(note ? ", " : "") + "Shrink", note=true;
  if (pol.verticalPolicy() & QSizePolicy::GrowFlag)
    ret += std::string(note ? ", " : "") + "Grow";
  ret += "}";
  return ret;
}

/* -------------------------------------------------------------------------- */
/* GuiQtElement::setDebugTooltip --                                           */
/* -------------------------------------------------------------------------- */
void GuiQtElement::setDebugTooltip() {
#if defined(_DEBUG)
  // only if debug Gui is enabled
  if (!Debugger::DebugFlagEnabled( BugGui ) ) return;

  std::ostringstream os;
  std::string str(getName());
  if (str.size() == 0) {
    if (Type() == type_Container) str = "Container";
    if (str.size() == 0)
      os << "Unkown: " << Type();
    os << "Type[" << str << "]";
  }
  if (os.str().size() == 0)
    os << " Name["<<getName()<<"]";
  os << " expandPolicy intern["<<printOrientations(getExpandPolicy())<<"] container["<<getContainerExpandPolicy()<<"] dialog["<<getDialogExpandPolicy()<<"]";
  QString s = QString::fromStdString(os.str());
  if (myWidget()) {
#if QT_VERSION > 0x050200
    myWidget()->setToolTipDuration(AppData::Instance().ToolTipDuration());
#endif
    myWidget()->setToolTip(s);
  }
#endif
}

/* -------------------------------------------------------------------------- */
/* GuiQtElement::updateWidgetProperty --                                      */
/* -------------------------------------------------------------------------- */
void GuiQtElement::updateWidgetProperty(){
  if (!myWidget())
    return;
  // set class name
  // myWidget()->property("class") may be QString or QStringList
  // .toStringList().join(" ") works for both
  QString qstrOld(myWidget()->property("class").toStringList().join(" "));
  if (QString::compare(qstrOld, Class().c_str()) == 0)
    return;
  setAttributeChangedFlag(true);
  QStringList slist = QString::fromStdString(Class()).split(" ");
  if (slist.size() > 1) {
    const QVariant qvar(slist);
    myWidget()->setProperty("class", qvar);
  } else {
    myWidget()->setProperty("class", QString::fromStdString(Class()));
  }
  BUG_DEBUG("Set class property to: " << Class()
            << "(Type: " << Type() << ", Name: " << getName() << ")");

  myWidget()->style()->unpolish(myWidget());
  myWidget()->style()->polish(myWidget());

  myWidget()->update();
}

/* -------------------------------------------------------------------------- */
/* FileJobAction::FileJobAction --                                            */
/* -------------------------------------------------------------------------- */
GuiQtElement::FileJobAction::FileJobAction(GuiQtElement *e, bool open )
  : m_elem( e )
  , m_open( open )
  , m_xfer( 0 ) {
}

/* --------------------------------------------------------------------------- */
/* startJobAction --                                                           */
/* --------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::startJobAction() {
  if( m_open )
    assert(false);
  /* 	m_stream->openFile( this ); */
  else
    m_elem->saveFile( this );
}
/* -------------------------------------------------------------------------- */
/* FileJobAction::printLogTitle --                                            */
/* -------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::printLogTitle( std::ostream &ostr ) {
  ostr << _("GuiElement ");
}

/* -------------------------------------------------------------------------- */
/* FileJobAction::endFileStream --                                            */
/* -------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::endFileStream( JobResult rslt ) {
  endJobAction( rslt );
}

/* -------------------------------------------------------------------------- */
/* FileJobAction::setFilename --                                              */
/* -------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::setFilename( const std::string& filename ){
	  m_filename = filename;
}

/* -------------------------------------------------------------------------- */
/* FileJobAction::setXfer --                                                  */
/* -------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::setXfer( XferDataItem *xfer ){
      m_xfer = xfer;
}

/* -------------------------------------------------------------------------- */
/* FileJobAction::serializeXML --                                             */
/* -------------------------------------------------------------------------- */
void GuiQtElement::FileJobAction::serializeXML(std::ostream &os, bool recursive){
  os << "<GuiQtElement_FileJobAction name=\"" << Name() << "\">" << std::endl;
  os << "</GuiQtElement_FileJobAction>" <<  std::endl;
}


/* -------------------------------------------------------------------------- */
/* FileSelected --                                                            */
/* -------------------------------------------------------------------------- */

void GuiQtElement::SaveListener::FileSelected( const std::string &fn
					     , const HardCopyListener::FileFormat ff
					     , const std::string dir ){
  BUG( BugGui, "FileStream::SaveListener::FileSelected" );
  m_stream->setDirname( dir );
  std::string filename = fn;

  m_stream->writeFile(0, filename, true);  // background transparent
}

/* -------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                 */
/* -------------------------------------------------------------------------- */

void GuiQtElement::SaveListener::confirmYesButtonPressed(){
  BUG( BugGui, "FileStream::confirmYesButtonPressed" );
  m_stream->writeFile(0, m_filename, true);  // background transparent
}

/* -------------------------------------------------------------------------- */
/* confirmNoButtonPressed --                                                  */
/* -------------------------------------------------------------------------- */

void GuiQtElement::SaveListener::confirmNoButtonPressed(){
  BUG( BugGui, "FileStream::confirmNoButtonPressed" );
  m_filename="";
  canceled();
}

/* ------------------------------------------------------------------------- */
/* canceled --                                                                */
/* -------------------------------------------------------------------------- */

void GuiQtElement::SaveListener::canceled(){
  BUG( BugGui, "FileStream::SaveListener::canceled" );
  m_stream->endFileStream( JobAction::job_Canceled );
}

/* --------------------------------------------------------------------------- */
/* getExpandPolicy --                                                          */
/* --------------------------------------------------------------------------- */

Qt::Orientations GuiQtElement::getExpandPolicy() {
//   std::cout << " Not Implemented ExpandData  Type["<<Type()<<"]  SP["<< (myWidget() ? myWidget()->sizePolicy().expandingDirections() : (Qt::Orientations)-1)<<"]\n" << std::flush;
  if( myWidget() )
    return myWidget()->sizePolicy().expandingDirections();
  return Qt::Orientations();
}

/* --------------------------------------------------------------------------- */
/* getContainerExpandPolicy --                                                 */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtElement::getContainerExpandPolicy() {
  return getGuiOrientation(getExpandPolicy());
}

/* --------------------------------------------------------------------------- */
/* getDialogExpandPolicy --                                                    */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtElement::getDialogExpandPolicy() {
  return getContainerExpandPolicy();
}

/* --------------------------------------------------------------------------- */
/* getGuiOrientation --                                                        */
/* --------------------------------------------------------------------------- */

GuiElement::Orientation GuiQtElement::getGuiOrientation(Qt::Orientations o) {
  switch(o) {
  case 0:
    return GuiElement::orient_Default;
  case Qt::Horizontal:
    return GuiElement::orient_Horizontal;
  case Qt::Vertical:
    return GuiElement::orient_Vertical;
  case Qt::Horizontal|Qt::Vertical:
    return GuiElement::orient_Both;
  }
  return GuiElement::orient_Default;
}

/* --------------------------------------------------------------------------- */
/* getScrollbarPolicy --                                                       */
/* --------------------------------------------------------------------------- */

Qt::ScrollBarPolicy GuiQtElement::getScrollbarPolicy( GuiElement::ScrollbarType sb ) const{
  switch( sb ){
  case GuiElement::scrollbar_undefined:
  case GuiElement::scrollbar_OFF:
    return Qt::ScrollBarAlwaysOff;
  case GuiElement::scrollbar_AS_NEEDED:
    return Qt::ScrollBarAsNeeded;
  case GuiElement::scrollbar_ON:
    return Qt::ScrollBarAlwaysOn;
  default:
    return Qt::ScrollBarAlwaysOff;
  }
}

/* --------------------------------------------------------------------------- */
/* getMenuName --                                                              */
/* --------------------------------------------------------------------------- */
const std::string GuiQtElement::getMenuName(GuiElement* parent, const std::string &name) {
  std::string ret(name);
  if (ret.empty())
    return ret;
  while(parent) {
    if (parent->Type() != GuiElement::type_PulldownMenu &&
        parent->Type() != GuiElement::type_Menubar ) {
      return "";
    }
    if (parent->getName().size()) {
      ret = parent->getName() + "->" + ret;
    }
    parent = parent->getParent();
  }
  return ret;
}
