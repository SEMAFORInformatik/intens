
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include <stdio.h>

#include <iostream>

#include "app/ReportGen.h"
#include "utils/Date.h"
#include "gui/FileSelectListener.h"
#include "gui/GuiEventData.h"
#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtScrolledText.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/QtDialogFileSelection.h"
#include "gui/qt/QtMultiFontString.h"

class MyQFileDialog : public QFileDialog{
public :
  MyQFileDialog(){}
  ~MyQFileDialog(){}
protected:
  virtual void closeEvent( QCloseEvent*){
  }
};


INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

QtDialogFileSelection::QtDialogFileSelection()
  : m_title( "FileSelection" )
    , m_filter( "*" )
    , m_no_file_exist( false )
    , m_eventcontrol( 0 ){
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

bool QtDialogFileSelection::showDialog( GuiElement *element
					, const std::string &title
					, const std::string &currentFilter
					, const std::string &directory
					, FileSelectListener *listener
					, Type type
					, const std::vector<HardCopyListener::FileFormat> * const formats
					, Mode mode ){
  BUG_DEBUG("QtDialogFileSelection::showDialog,  currentFilter: "<<currentFilter);
  QString cwdIn = QDir::currentPath();

  QString selectedFile;
  QString qCurrentFilter( QString::fromStdString(currentFilter) );
  qCurrentFilter = qCurrentFilter.split(";;").at(0);
  if (mode == DirectoryOnly || mode == Directory) {
    selectedFile = getDirectoryName(QString::fromStdString(directory),
				    NULL, QString::fromStdString(title),
				    QFileDialog::ShowDirsOnly);
  } else
  if (type == Save) {
    selectedFile = getSaveFileName(QString::fromStdString(directory),
				   QString::fromStdString(currentFilter),
				   NULL, QString::fromStdString(title),
				   &qCurrentFilter);
  } else {
    selectedFile = getOpenFileName(QString::fromStdString(directory),
				   QString::fromStdString(currentFilter),
				   NULL, QString::fromStdString(title),
				   &qCurrentFilter );
  }
  if( selectedFile.length() ){
    std::string selectedFilter( qCurrentFilter.toStdString() );
    HardCopyListener::FileFormat ff;
    std::string suffix = getFileformatAndSuffix( selectedFilter, ff );
    std::string file = ReportGen::Instance().appendExtension( selectedFile.toStdString(), suffix );
    BUG_DEBUG("Got file["<<  file <<"]  selectedFile["<<  selectedFile.toStdString() <<"]  filter["<<selectedFilter<<"]");
    listener->FileSelected( file, ff, selectedFile.toStdString() );
  }
  else
    if (listener) listener->canceled();

  // Heavy WindowsBug: file dialog can change current working directory
  QString cwdOut = QDir::currentPath();
  if (cwdIn != cwdOut) {
    std::ostringstream os;
    os << "CWD has changed during FileDialog In["<<cwdIn.toStdString()
       << "] Out[" << cwdOut.toStdString() << "]"
       << ", do revert";
    GuiFactory::Instance()->getLogWindow()->writeText( DateAndTime() + ": " + os.str() );
    QDir::setCurrent(cwdIn);
  }
 return true;
}

/* --------------------------------------------------------------------------- */
/* getSuffix --                                                                */
/* --------------------------------------------------------------------------- */
std::string QtDialogFileSelection::getFileformatAndSuffix( const std::string selectedFilter, HardCopyListener::FileFormat &format ){
  std::string suffix;
  if( selectedFilter.find("pdf") != std::string::npos ){
    suffix = "pdf";
    format = HardCopyListener::PDF;
  }
  else if( selectedFilter.find("jp") != std::string::npos ){
    suffix = "jpeg";
    format = HardCopyListener::JPEG;
  }
  else if( selectedFilter.find("png") != std::string::npos ){
    suffix = "png";
    format = HardCopyListener::PNG;
  }
  else if( selectedFilter.find("ps") != std::string::npos ){
    suffix = "ps";
    format = HardCopyListener::Postscript;
  }
  else if( selectedFilter.find("hpgl") != std::string::npos ){
    suffix = "hpgl";
    format = HardCopyListener::HPGL;
  }
  else if( selectedFilter.find("gif") != std::string::npos ){
    suffix = "gif";
    format = HardCopyListener::GIF;
  }
  else if( selectedFilter.find("txt") != std::string::npos ){
    suffix = "txt";
    format = HardCopyListener::ASCII;
  }
  else if( selectedFilter.find("xml") != std::string::npos ){
    suffix = "xml";
    format = HardCopyListener::XML;
  }
  else if( selectedFilter == "Text (*.txt)" ){
    suffix = "txt";
    format = HardCopyListener::Text;
  }
  else if( selectedFilter.find("tex") != std::string::npos ){
    suffix = "tex";
    format = HardCopyListener::LaTeX;
  }
  else if( selectedFilter.find("gz") != std::string::npos ){
    suffix = "gz";
    format = HardCopyListener::GZ;
  }
  else if( selectedFilter.find("svg") != std::string::npos ){
    suffix = "svg";
    format = HardCopyListener::SVG;
  }
  else if( selectedFilter.find("html") != std::string::npos || selectedFilter.find("htm") != std::string::npos ){
    suffix = "html";
    format = HardCopyListener::HTML;
  }
  else {
    format = HardCopyListener::NONE;
    std::string::size_type n = selectedFilter.find_last_of( '.' );
    if ( n != std::string::npos ) {
      std::string::size_type nn = selectedFilter.find_first_of( ')', n );
      if ( nn != std::string::npos ) {
	suffix = selectedFilter.substr( n+1, (nn-1) - n );
      }
    }
    // std::cerr << "no such filter: " << selectedFilter << std::endl;
    // std::cerr << "suffix: " << suffix << std::endl;
    // std::cerr << "QtDialogFileSelection.cc : 155 -->>> FIX IT !!!!!" << std::endl;
  }
  return suffix;
}

/* --------------------------------------------------------------------------- */
/* createFilter --                                                             */
/* --------------------------------------------------------------------------- */
QString QtDialogFileSelection::createFilter( const std::vector<HardCopyListener::FileFormat> *formats ){
  if( !formats )
    return "";

  QString filter;
  std::vector<HardCopyListener::FileFormat>::const_iterator iter;
  for( iter = formats->begin(); iter != formats->end(); ++iter ){
    switch( *iter ){
    case HardCopyListener::Postscript :
      filter.append( "Postscript (*.ps);;" );
	break;
    case HardCopyListener::PDF :
      filter.append( "PDF (*.pdf);;" );
      break;
    case HardCopyListener::ASCII :
      filter.append( "ASCII (*.txt);;" );
      break;
    case HardCopyListener::JPEG :
      filter.append( "JPEG (*.jpg, *.jpeg);;" );
      break;
    case HardCopyListener::PNG :
      filter.append( "PNG (*.png);;" );
      break;
    case HardCopyListener::GIF :
      filter.append( "GIF (*.gif);;" );
      break;
    case HardCopyListener::HPGL :
      filter.append( "HPGL (*.hpgl);;" );
      break;
    case HardCopyListener::XML :
      filter.append( "XML (*.xml);;" );
      break;
    case HardCopyListener::LaTeX :
      filter.append( "LaTeX (*.tex);;" );
      break;
    case HardCopyListener::Text :
      filter.append( "Text (*.txt);;" );
      break;

    default :
      assert( false );
      break;
    }
  }
  return filter;
}

/* --------------------------------------------------------------------------- */
/* getDirName --                                                          */
/* --------------------------------------------------------------------------- */
QString QtDialogFileSelection::getDirectoryName ( const QString & startWith
						 , QWidget * parent
						 , const QString & caption
						 , QFileDialog::Options options ){

  BUG_DEBUG("QtDialogFileSelection::getDirectoryName, StartWith: " << startWith.toStdString());
  GuiEventLoopListener *eventcontrol = new GuiEventLoopListener( false );
  GuiQtManager::Instance().attachEventLoopListener( eventcontrol );

  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();
  QString file = QFileDialog::getExistingDirectory( parent, caption, QDir(startWith).absolutePath(), options );
  if ( activeWindow )
    // restore activeWindow
    activeWindow->activateWindow();

  eventcontrol->goAway();
  return file;
}

/* --------------------------------------------------------------------------- */
/* getSaveFileName --                                                          */
/* --------------------------------------------------------------------------- */
QString QtDialogFileSelection::getSaveFileName ( const QString & startWith
						 , const QString & filter
						 , QWidget * parent
						 , const QString & caption
						 , QString * selectedFilter
						 , QFileDialog::Options options ){

  BUG_DEBUG("QtDialogFileSelection::getSaveFileName, StartWith: " << startWith.toStdString()
            << ", filter: " << filter.toStdString() << ", selFilter: " << selectedFilter);
  GuiEventLoopListener *eventcontrol = new GuiEventLoopListener( false );
  GuiQtManager::Instance().attachEventLoopListener( eventcontrol );

  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();
  QString file = QFileDialog::getSaveFileName( parent, caption, QDir(startWith).absolutePath(),  filter, selectedFilter, options );
  if ( activeWindow )
    // restore activeWindow
    activeWindow->activateWindow();

  eventcontrol->goAway();
  return file;
}

/* --------------------------------------------------------------------------- */
/* getOpenFileName --                                                          */
/* --------------------------------------------------------------------------- */
QString QtDialogFileSelection::getOpenFileName ( const QString & startWith
						 , const QString & filter
						 , QWidget * parent
						 , const QString & caption
						 , QString * selectedFilter
						 , QFileDialog::Options options){

  BUG_DEBUG("QtDialogFileSelection::getOpenFileName, StartWith[" << startWith.toStdString()
            << ", Filter: " << filter.toStdString() << ", selectedFilter: " << selectedFilter);
  GuiEventLoopListener *eventcontrol = new GuiEventLoopListener( false );
  GuiQtManager::Instance().attachEventLoopListener( eventcontrol );

  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();

  QString file = QFileDialog::getOpenFileName( parent, caption, QDir(startWith).absolutePath(),  filter, selectedFilter, options );
  if ( activeWindow )
    // restore activeWindow
    activeWindow->activateWindow();

  eventcontrol->goAway();
  return file;
}

/* --------------------------------------------------------------------------- */
/* installEventLoop --                                                         */
/* --------------------------------------------------------------------------- */
void QtDialogFileSelection::installEventLoop(){
  assert( m_eventcontrol == 0 );
  m_eventcontrol = new GuiEventLoopListener( false );
  GuiQtManager::Instance().attachEventLoopListener( m_eventcontrol );
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */
void QtDialogFileSelection::unmanage(){
  //  removeParentWaitCursor();
  if( m_eventcontrol != 0 ){
    m_eventcontrol->goAway();
  }
}
