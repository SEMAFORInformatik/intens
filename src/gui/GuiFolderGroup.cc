
#include "utils/HTMLConverter.h"
#include "gui/GuiManager.h"
#include "gui/GuiFolderGroup.h"
#include "gui/GuiFolder.h"
#include "job/JobManager.h"
#include "app/App.h"
#include "parser/Flexer.h"
#include "job/JobFunction.h"

GuiFolderGroup::GuiFolderGroupList  GuiFolderGroup::m_foldergrouplist;

extern int   PAlineno;
INIT_LOGGER();

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::setFunction( JobFunction *func ){
  assert( func != 0 );
  m_function = func;
  func->setUsed();
}

/* --------------------------------------------------------------------------- */
/* getFunction --                                                              */
/* --------------------------------------------------------------------------- */

JobFunction *GuiFolderGroup::getFunction(){
  return m_function;
}

/* --------------------------------------------------------------------------- */
/* FolderIsEqual --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::FolderIsEqual( GuiFolder *fol ){
  return fol == m_folder;
}

/* --------------------------------------------------------------------------- */
/* EntryIsEqual --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::EntryIsEqual( GuiFolder *fol, int page ){
  return (fol == m_folder || m_folder->isFolderClonedBy(fol)) && page == m_page_number;
}

/* --------------------------------------------------------------------------- */
/* PageIsActive --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::PageIsActive(){
  return m_folder->PageIsActive( m_page_number );
}

/* --------------------------------------------------------------------------- */
/* activatePage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::activatePage( GuiFolderGroup::OmitMap omit_map ){
  BUG_PARA( BugGuiFolder, "activatePage"
	    , "PageNumber=" << m_page_number<< ", Groupname=" << m_groupname );

  m_folder->activatePage( m_page_number, omit_map, false );
}

/* --------------------------------------------------------------------------- */
/* hidePage --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::hidePage(){
  BUG_PARA( BugGuiFolder, "hidePage"
	    , "PageNumber=" << m_page_number<< ", Groupname=" << m_groupname );
  BUG_INFO("hidePage, PageNumber=" << m_page_number<< ", Groupname=" << m_groupname );

  m_folder->hidePage( m_page_number );
}

/* --------------------------------------------------------------------------- */
/* PageNumber --                                                               */
/* --------------------------------------------------------------------------- */

int GuiFolderGroup::PageNumber(){
  return m_page_number;
}

/* --------------------------------------------------------------------------- */
/* startFunction --                                                            */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::startFunction(){
  if( m_function == 0 ){
    return;
  }

  // Zu diesem Zeitpunkt darf keine Funktion laufen, sonst haben wir
  // ein grosses Problem.
  if( !setRunningMode() ){ assert( false ); }

  Trigger *trigger = new Trigger( this, m_function );
  trigger->setReason( JobElement::cll_Input );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // endFunction() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // löscht sich am Ende selbständig.
}

/* --------------------------------------------------------------------------- */
/* endFunction --                                                              */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::endFunction( bool error ){
  if( error ){
    m_folder->getElement()->updateForms( GuiElement::reason_Cancel );
    GuiManager::Instance().showErrorBox( m_folder->getElement() );
  }
  else{
    m_folder->getElement()->updateForms( GuiElement::reason_FieldInput );
  }
  unsetRunningMode();
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::setRunningMode(){
  m_running_key = JobManager::Instance().setRunningMode( m_running_key );
  return m_running_key != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::unsetRunningMode(){
  if( m_running_key != 0 ){
    JobManager::Instance().unsetRunningMode( m_running_key );
    m_running_key = 0;
  }
}
/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  m_foldergroup->endFunction( rslt != JobAction::job_Ok );
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiFolderGroup::serializeXML(std::ostream &os, bool recursive){
  os << "<intens:FolderGroup";
  std::string s( m_groupname );
  HTMLConverter::convert2HTML( s );
  os << " name=\"" << s << "\"";
  os << ">" << std::endl;
  m_folder->getElement()->serializeXML(os, recursive);
  os<<"</intens:FolderGroup>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* checkDuplicateFolderGroup --                                                */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::checkDuplicateFolderGroup( GuiFolder *fol, const std::string &name ){
  GuiFolderGroupList::iterator grp = m_foldergrouplist.find( name );

  while( grp != m_foldergrouplist.end() ){
    if( grp->first != name ){
      return false;
    }
    if( grp->second->FolderIsEqual( fol ) ){
      return true;
    }
    grp++;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* addFolderGroupEntry --                                                      */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::addFolderGroupEntry( const std::string &name
					  , GuiFolder *fol
					  , int num
					  , JobFunction *func )
{
  if( checkDuplicateFolderGroup( fol, name ) ){
    return false;
  }

  GuiFolderGroup * grp = new GuiFolderGroup( name, fol, num );
  if( func != 0 ){
    grp->setFunction( func );
    func->setUsed();
  }
  if (AppData::Instance().LspWorker()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    grp->setLSPFilename(filename);
    grp->setLSPLineno(lineNo);
  }
  m_foldergrouplist.insert( GuiFolderGroupList::value_type( name, grp ) );
  return true;
}

/* --------------------------------------------------------------------------- */
/* FolderGroupExists --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::FolderGroupExists( const std::string &name ){
  GuiFolderGroupList::iterator grp = m_foldergrouplist.find( name );
  return grp != m_foldergrouplist.end();
}

/* --------------------------------------------------------------------------- */
/* activateFolderGroups --                                                     */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::activateFolderGroups( GuiFolder *fol, int page ){
  BUG_PARA( BugGuiFolder, "activateFolderGroups", "page=" << page );

  GuiFolderGroupList::iterator grp;
  for( grp = m_foldergrouplist.begin(); grp != m_foldergrouplist.end(); ++grp ){
    if( grp->second->EntryIsEqual( fol, page ) ){
      activateByName( fol, page, grp->first, GuiFolderGroup::omit_Default );
      grp->second->startFunction();
      if (grp->second->m_folder->isFolderClonedBy(fol) == false)  // activate later
	return;
    }
    // activated cloned folder tabs
    if (grp->second->m_page_number == page &&
	grp->second->m_folder->isFolderClonedBy(fol)) {
      grp->second->m_folder->activatePage( page, GuiFolderGroup::omit_Default, false );
      return;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* activateByName --                                                           */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::activateByName( GuiFolder *fol,
				     int page,
				     const std::string &name,
				     GuiFolderGroup::OmitMap omit_map )
{
  BUG_PARA( BugGuiFolder, "activateByName"
	    , "page=" << page << ", name=" << name );

  GuiFolderGroupList::iterator grp = m_foldergrouplist.find( name );
  while( grp != m_foldergrouplist.end() ){
    if( grp->first != name ){
      return;
    }
    if( !grp->second->EntryIsEqual( fol, page ) ){
      if( fol==0 || !grp->second->PageIsActive() ){
	grp->second->activatePage( omit_map );
      }
    }
    grp++;
  }
}

/* --------------------------------------------------------------------------- */
/* hideByName --                                                               */
/* --------------------------------------------------------------------------- */

void GuiFolderGroup::hideByName(  const std::string &name ){
  BUG_PARA( BugGuiFolder, "hideByName", "name=" << name );
  BUG_INFO("hideByName, name=" << name );

  GuiFolderGroupList::iterator grp = m_foldergrouplist.find( name );
  while( grp != m_foldergrouplist.end() ){
    if( grp->first != name ){
      return;
    }
    grp->second->hidePage();
    grp++;
  }
}

/* --------------------------------------------------------------------------- */
/* firstAktiveGroup --                                                         */
/* --------------------------------------------------------------------------- */

int GuiFolderGroup::firstAktiveGroup( GuiFolder *fol ){
  GuiFolderGroupList::iterator grp;

  for( grp = m_foldergrouplist.begin(); grp != m_foldergrouplist.end(); ++grp ){
    if( grp->second->FolderIsEqual( fol ) ){
      // Nun haben wir den Namen einer Gruppe dieses Folders
      if( FolderGroupIsActive( grp->first, fol ) ){
	return grp->second->PageNumber();
      }
    }
  }
  return -1;
}

/* --------------------------------------------------------------------------- */
/* FolderGroupIsActive --                                                      */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::FolderGroupIsActive( const std::string &name, GuiFolder *fol ){
  GuiFolderGroupList::iterator grp = m_foldergrouplist.find( name );

  while( grp != m_foldergrouplist.end() ){
    if( grp->first != name ){
      return false;
    }
    if( !grp->second->FolderIsEqual( fol ) ){
      if( grp->second->PageIsActive() ){
	return true;
      }
    }
    grp++;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isActive --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::isActive( const std::string &name ){
  // is there any folder where this folder name is active
  GuiFolderGroupList::iterator grp;
  for( grp = m_foldergrouplist.begin(); grp != m_foldergrouplist.end(); ++grp ){
    if( grp->first == name && grp->second->PageIsActive()){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* isActive --                                                                 */
/* --------------------------------------------------------------------------- */

bool GuiFolderGroup::getTabName( GuiFolder *fol, int page, std::string &name){
  name = "";
  for(GuiFolderGroupList::iterator grp = m_foldergrouplist.begin();
      grp != m_foldergrouplist.end(); ++grp ) {
    if (grp->second->FolderIsEqual(fol)) {
      if (grp->second->m_page_number == page) {
        name = grp->second->m_groupname;
        return true;
      }
    }
  }
  return false;
}

void GuiFolderGroup::lspWrite( std::ostream &ostr ){
  for (auto pair : m_foldergrouplist) {
    auto name = pair.first;
    auto ele = pair.second;
    ostr << "<ITEM name=\"" << name << "\"";
    ostr << " uiele=\"1\"";
    ostr << " page=\"1\"";
    ostr << " file=\"" << ele->LSPFilename() << "\"";
    ostr << " line=\"" << ele->LSPLineno() << "\"";
    ostr << ">" << std::endl;
    ostr << "</ITEM>" << std::endl;

  }
}
