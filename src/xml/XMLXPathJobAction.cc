#ifdef HAVE_QT
//Qt includes
#include "gui/qt/QtDialogFileSelection.h"
#endif
#include <iostream>
#include "XMLXPathFactory.h"
#include "XMLXPathJobAction.h"
#include "app/HardCopyListener.h"
#include "operator/FileStream.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"

/*********************************************************
     Constructor
     Initialize instance (and create a DOM tree)
**********************************************************/
XMLXPathJobAction::XMLXPathJobAction(FileStream *fs, XferDataItem *xferdataitem,
				     XferDataItem *xferdataitemDest)
: m_fileStream(fs), m_xferDataItem(xferdataitem), m_xferDataItemDest(xferdataitemDest)
  , m_initNewFactory(false)
  , m_processNewFactory(false)
  , m_xpathExpr(NULL)
  , m_thread(this) {
}

/*********************************************************
     Destructor
**********************************************************/
XMLXPathJobAction::~XMLXPathJobAction(){
  if (m_xpathExpr)
    delete m_xpathExpr;
}

/*------------------------------------------------------------
  startThread --
  ------------------------------------------------------------*/
void  XMLXPathJobAction::startThread() {
  XMLXPathFactory &xPathFactory = XMLXPathFactory::Instance( m_filename );
  m_thread.lock();
  m_processNewFactory = false;
  m_thread.unlock();
//   return &xPathFactory;
}

/*********************************************************
     startJobAction
**********************************************************/
void XMLXPathJobAction::startJobAction(){
  std::string xmlFile = XMLXPathFactory::getLastXMLFile();
#if defined HAVE_QT && !defined HAVE_HEADLESS
  if ( xmlFile.empty() ) {
    QString ret =
      QtDialogFileSelection::getOpenFileName( QString(), "*.xml", NULL, "title" );
    if (ret.length()) {
      FileSelected( ret.toStdString(), HardCopyListener::NONE, "" );
    } else
      endJobAction( JobAction::job_Canceled );
  } else
    FileSelected(xmlFile, HardCopyListener::NONE, "" );
#else
  if ( xmlFile.empty() ){
    assert( false );
    //m_fileStream->openFile(this);
  }
  else
    FileSelected(xmlFile, HardCopyListener::NONE, "" );
#endif
}

/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */

void XMLXPathJobAction::serializeXML(std::ostream &os, bool recursive){
  os << "<xmlxpath name=\"" << Name() << "\">" << std::endl;
  os << "</xmlxpath>" <<  std::endl;
}

//
void XMLXPathJobAction::FileSelected( const std::string &filename
				      , const HardCopyListener::FileFormat format
				      , const std::string dir ){

  std::vector<std::string> xPathQueries;
  m_filename = filename;

  if (!m_xferDataItem) {
    endJobAction( JobAction::job_Aborted );
  }
  if (m_xpathExpr)
    delete m_xpathExpr;
  m_xpathExpr = new XPathExpr(m_xferDataItem);

  try {

    if ( !XMLXPathFactory::ExistsInstance( filename ) ) {
      //      cout << "  CREATE\n";
      // create an XMLXPathFactory instance
        // start thread
      while (m_initNewFactory) {
	//	cout << "Sorry only a single instance can created at once\n";
	//	sleep(1);
      }
      m_thread.lock();
      m_initNewFactory = true;
      m_processNewFactory = true;
      m_thread.unlock();

      m_thread.start();
      Worker *w = new Worker( this );
      w->start();
    }

    XMLXPathFactory &xPathFactory = XMLXPathFactory::Instance( filename );

    if (xPathFactory.doQuery( *m_xpathExpr, m_xferDataItemDest ) ) {
      bool ret;
      if (xPathFactory.getNumResultNodes() < 500) {
	//	cout << "SMALL INSERT\n";
	ret = xPathFactory.doInsert( *m_xpathExpr, m_xferDataItemDest );
      } else {
	//	cout << "HUGE INSERT => do not process\n";
	Worker *w = new Worker( this );
	w->start();

	xPathFactory.doInsert( *m_xpathExpr, m_xferDataItemDest, 500 );
	return;
      }
    } else {
      endJobAction( JobAction::job_Ok ); //_Aborted );
      return;
    }

  } catch(...) {
    endJobAction( JobAction::job_Aborted );
    return;
  }
  endJobAction( JobAction::job_Ok );
}

void XMLXPathJobAction::canceled(){
  endJobAction( JobAction::job_Canceled );
}

//========================================================================
// public functions from WorkerController Interface
//========================================================================

JobAction::JobResult XMLXPathJobAction::work() {
  return JobAction::job_Ok;
}
/*********************************************************
     workFailed
     from WorkerListener Interface
**********************************************************/
void XMLXPathJobAction::backFromWorker( JobAction::JobResult ) {
  if (m_initNewFactory) {
    m_thread.lock();
    m_initNewFactory = false;
    m_thread.unlock();

    endJobAction( JobAction::job_Ok );
  }
  endJobAction( JobAction::job_Ok );
}
/*********************************************************
     workFailed
     from WorkerListener Interface
**********************************************************/
void XMLXPathJobAction::stopJobAction() {
  m_thread.lock();
  if( m_initNewFactory )
    m_thread.terminate();
  m_initNewFactory = false;
  m_thread.unlock();
  endJobAction( JobAction::job_Aborted );
}
