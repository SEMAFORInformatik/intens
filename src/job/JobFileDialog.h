
#if !defined(JOB_FILEDIALOG_H)
#define JOB_FILEDIALOG_H

#include "job/JobController.h"
#include "job/JobEngine.h"
#include "job/JobCodeFile.h"
#include "gui/FileSelectListener.h"

class GuiElement;

class JobFileDialog : public JobAction
                    , public FileSelectListener
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  JobFileDialog( JobEngine *eng );
  virtual ~JobFileDialog();

/*=============================================================================*/
/* public Functions                                                            */
/*=============================================================================*/
public:
  void setElement( GuiElement *el ) { m_element = el; }
  void setDirname( const std::string &dirname ) { m_dirname = dirname; }
  void setFilter( const std::string &filter ) { m_filter = filter; }
  void showDirOnly( bool dironly ) { m_dironly = dironly; }
  void openMode( bool open ) { m_open = open; }
  void setDirnameXfer( XferDataItem *dirnamexfer ) { m_dirnamexfer = dirnamexfer; }
  void setDialog( JobCodeFileDialog *dialog ) { m_dialog = dialog; }

  virtual void printLogTitle( std::ostream &ostr ){}
  virtual void startJobAction();
  virtual void stopJobAction();
  virtual void serializeXML(std::ostream &os, bool recursive = false){}

  virtual void FileSelected( const std::string &filename
			     , const HardCopyListener::FileFormat ff
			     , const std::string dir );
  virtual void canceled();

/*=============================================================================*/
/* protected Functions                                                         */
/*=============================================================================*/
protected:
  virtual void backFromJobController( JobResult rslt ){}

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiElement   *m_element;
  JobEngine    *m_eng;
  std::string   m_dirname;
  std::string   m_filter;
  bool          m_dironly;
  bool          m_open;
  XferDataItem *m_dirnamexfer;
  JobCodeFileDialog *m_dialog;
};

#endif
