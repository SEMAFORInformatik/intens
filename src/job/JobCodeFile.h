
#if !defined(JOB_CODE_FILE_INCLUDED_H)
#define JOB_CODE_FILE_INCLUDED_H

#include "job/JobCodeExec.h"

class FileStream;

/** Mit einem Objekt dieser Klasse wird ein File-Dialog
 */
class JobCodeFileDialog : public JobCodeExec
{
public:
 JobCodeFileDialog( const std::string &dirname
                    , const std::string &filter
                    , bool dironly
                    , bool open
                    , XferDataItem *dirnamexfer )
    : m_dirname( dirname )
    , m_filter( filter )
    , m_dironly( dironly )
    , m_open( open )
    , m_dirnamexfer ( dirnamexfer )
  {}
  virtual ~JobCodeFileDialog(){}

  void setDirname( const std::string &dirname );

  /** Diese Funktion ruft den Confirmation-Dialog auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string m_dirname;
  const std::string m_filter;
  bool m_dironly;
  bool m_open;
  XferDataItem *m_dirnamexfer;
};

/** Mit einem Objekt dieser Klasse wird ein SaveFile
 */
class JobCodeSaveFile : public JobCodeExec
{
public:
 JobCodeSaveFile(FileStream *filestream, GuiElement* guiElement,
                 bool readStack, const std::string &filename,
                 bool readBaseStack, const std::string &base_filename )
    : m_fileStream( filestream )
    , m_guiElement( guiElement )
    , m_readStack( readStack )
    , m_filename( filename )
    , m_readBaseStack( readBaseStack )
    , m_baseFilename( base_filename )
  {}
  virtual ~JobCodeSaveFile(){}
  /** Diese Funktion ruft den SaveFile auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  FileStream*       m_fileStream;
  GuiElement*       m_guiElement;
  bool              m_readStack;
  bool              m_readBaseStack;
  const std::string m_filename;
  const std::string m_baseFilename;
};

/** Mit einem Objekt dieser Klasse wird ein OpenFile
 */
class JobCodeOpenFile : public JobCodeExec
{
public:
 JobCodeOpenFile(FileStream *filestream,
				 bool readStack, const std::string &filename )
    : m_fileStream( filestream )
    , m_readStack( readStack )
    , m_filename( filename )
  {}
  virtual ~JobCodeOpenFile(){}
  /** Diese Funktion ruft den OpenFile auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  FileStream*       m_fileStream;
  bool              m_readStack;
  const std::string m_filename;
};

#endif
