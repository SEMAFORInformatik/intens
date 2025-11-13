
#if !defined(GUI_FOLDERGROUP_INCLUDED_H)
#define GUI_FOLDERGROUP_INCLUDED_H

#include "app/LSPItem.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"
#include "gui/GuiElement.h"

/* forward declarations */
class GuiFolder;
class JobFunction;

class GuiFolderGroup : public LSPItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiFolderGroup( const std::string &groupname, GuiFolder *folder, int page )
    : m_groupname( groupname )
    , m_folder( folder )
    , m_page_number( page )
    , m_function( 0 )
    , m_running_key( 0 ){
  }
  virtual ~GuiFolderGroup(){
  }

  enum OmitMap
  { omit_Default = 0
  , omit_TTRAIL = 1
  , omit_Visibility = 2
  };

private:
  GuiFolderGroup( const GuiFolderGroup &g );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setFunction( JobFunction *func );
  JobFunction *getFunction();
  bool FolderIsEqual( GuiFolder *fol );
  bool EntryIsEqual( GuiFolder *fol, int page );
  bool PageIsActive();
  void activatePage( GuiFolderGroup::OmitMap omit_map );
  /** only used for folder with visible tabs */
  void hidePage();
  int PageNumber();
  void startFunction();
  void endFunction( bool error );
  void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* public static member functions                                              */
/*=============================================================================*/
public:
  static bool checkDuplicateFolderGroup( GuiFolder *, const std::string & );
  static bool addFolderGroupEntry( const std::string &name, GuiFolder *fol,
				   int n, JobFunction *func );
  static bool FolderGroupExists( const std::string & );
  static void activateFolderGroups( GuiFolder *fol, int page );
  static void activateByName( GuiFolder *fol, int page, const std::string &name, GuiFolderGroup::OmitMap omit_map );
  static void hideByName( const std::string &name );
  static int  firstAktiveGroup( GuiFolder *fol );
  static bool FolderGroupIsActive( const std::string &name, GuiFolder *fol );
  static bool getTabName(GuiFolder *fol, int page, std::string &name);
  /** is there any folder where this folder tab name is active
   */
  static bool isActive( const std::string &name );
  static void lspWrite( std::ostream &ostr );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  bool setRunningMode();
  void unsetRunningMode();

/*=============================================================================*/
/* private definitions                                                         */
/*=============================================================================*/
private:
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiFolderGroup *fg, JobFunction *f )
      : JobStarter( f )
      , m_foldergroup( fg ){}
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult rslt );
  private:
    GuiFolderGroup  *m_foldergroup;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::multimap<std::string, GuiFolderGroup *> GuiFolderGroupList;
  std::string       m_groupname;
  GuiFolder        *m_folder;
  int               m_page_number;
  JobFunction      *m_function;
  int               m_running_key;

  static GuiFolderGroupList    m_foldergrouplist;
};

#endif
