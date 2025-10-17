
#ifndef XMLXPATHACTION_H
#define XMLXPATHACTION_H

#include <string>
#include "job/JobAction.h"
#include "app/HardCopyListener.h"
#include "utils/IntensThread.h"
#include "gui/FileSelectListener.h"
#include "operator/Worker.h"

class XMLXPathFactory;
class XPathExpr;
class FileStream;

class XMLXPathJobAction  : public JobAction
			 , public WorkerController
			 , public FileSelectListener
			 , private ThreadListener {
public:
  XMLXPathJobAction(FileStream *, XferDataItem *xferdataitem, XferDataItem *xferdataitemDest);
  virtual ~XMLXPathJobAction();

/*=============================================================================*/
/* member functions of JobAction                                        */
/*=============================================================================*/
public:
  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
protected:
  virtual void backFromJobController( JobResult rslt ){}
  //===========================================================================
  // FileSelectListener-Interface
public:
  virtual void FileSelected( const std::string &filename
			     , const HardCopyListener::FileFormat format
			     , const std::string dir );
  virtual void canceled();

  //========================================================================
  // public functions from WorkerController Interface
  //========================================================================
  virtual JobAction::JobResult work();
  virtual void startWorker(){}
  virtual void backFromWorker( JobAction::JobResult );

/*=============================================================================*/
/* private ThreadListener function                                             */
/*=============================================================================*/
 private:
  virtual void startThread();

private:
  XPathExpr         *m_xpathExpr;
  FileStream        *m_fileStream;
  std::string        m_filename;
  XferDataItem      *m_xferDataItem;
  XferDataItem      *m_xferDataItemDest;
  IntensThread              m_thread;

  // flags for new xpath factory
  bool               m_initNewFactory;
  bool               m_processNewFactory;
};

#endif
