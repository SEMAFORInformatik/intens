
#include <qwidget.h>
#include <fstream>

#include "app/log.h"
#include "app/HelpText.h"
#include "app/HelpManager.h"
#include "gui/GuiForm.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiButton.h"
#include "gui/GuiLabel.h"
#include "gui/GuiDataField.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiScrolledlist.h"
#include "gui/qt/GuiQtElement.h"
#include "app/UiManager.h"
#include "app/AppData.h"
#include "app/DataPoolIntens.h"
#include "gui/GuiFactory.h"
// #include "utils/MultiLanguage.h"
#include "utils/utils.h"
#include "utils/StringUtils.h"

const char HelpText::s_TITLE_CHAR   = '#';
const char HelpText::s_CHAPTER_CHAR = '.';
const char HelpText::s_KEYWORD_CHAR = ':';

HelpText::HelpText(HelpManager *helpManager)
    : HelpFile(helpManager)
    , m_DialogWidget( 0 )
    , m_ChapterWidget( 0 )
    , m_TextWidget( 0 )
    , m_ListWidget( 0 )
    , m_IsPoppedUp( false )
    , m_ChapterList( 0 )
    , m_Selected( 0 )
    , m_NextListener( this )
    , m_PreviousListener( this )
    , m_CancelListener( this ){
}

HelpText::~HelpText(){
}

void HelpText::create(){
  // only used in webmode
  if(AppData::Instance().HeadlessWebMode() &&  m_DialogWidget == NULL) {
    createHelpDialog();
    initializeHelpDialog();
  }
}

/* --------------------------------------------------------------------------- */
/* createButtonbar --                                                          */
/* --------------------------------------------------------------------------- */

void HelpText::createButtonbar(){
  GuiButtonbar *buttonBar = GuiFactory::Instance() -> createButtonbar( 0 );

//   MultiLanguage &mls = MultiLanguage::Instance();

  GuiButton *button = GuiFactory::Instance() -> createButton( buttonBar->getElement(), &m_NextListener );
//   button->setLabel( mls.getString( "ML_BU_NEXT", ML_CONST_STRING ) );
  button->setLabel( _("Next") );

  button = GuiFactory::Instance() -> createButton( buttonBar->getElement(), &m_PreviousListener );
//   button->setLabel( mls.getString( "ML_BU_PREV", ML_CONST_STRING ) );
  button->setLabel( _("Previous") );

  button = GuiFactory::Instance() -> createButton( buttonBar->getElement(), &m_CancelListener );
//   button->setLabel( mls.getString( "ML_BU_CANCEL", ML_CONST_STRING ) );
  button->setLabel( _("Cancel") );
  buttonBar->getElement()->create();
  buttonBar->getElement()->manage();
}

/* --------------------------------------------------------------------------- */
/* helpDialog --                                                               */
/* --------------------------------------------------------------------------- */

void HelpText::helpDialog( int chapterNo ){
  if( m_DialogWidget == NULL ) {
    createHelpDialog();
    initializeHelpDialog();
  }
  if( !m_IsPoppedUp ) {
    openHelpFile();
    m_IsPoppedUp = true;
  }
  if( (chapterNo < 1) || (chapterNo > m_ChapterList.size()) ){
    return;
  }
  if ( chapterNo != m_Selected ) {
    loadChapter( (m_ChapterList[chapterNo -1]) );
    m_Selected = chapterNo;
    m_ListWidget->setSelectedPosition(chapterNo -1);
  }
  m_DialogWidget->getElement()->manage();
}


/* --------------------------------------------------------------------------- */
/* createHelpDialog --                                                         */
/* --------------------------------------------------------------------------- */
#include "gui/GuiFactory.h"

void HelpText::createHelpDialog(){
  int		 function;
  std::string title;
  GuiButtonbar*  bar;
  std::string name = m_Title;
  replaceAll(name, " ", "_");

  m_DialogWidget = GuiFactory::Instance()->createForm(name);
  m_DialogWidget->setTitle( "HelpDialog" );
  m_DialogWidget->resetCycleButton();
  m_DialogWidget->hasCloseButton( false );
  ////  m_dialog->getElement()->getDialog()->setApplicationModal();

  // Chapter generieren
  GuiFieldgroup* fieldgroup = GuiFactory::Instance()->createFieldgroup(m_DialogWidget->getElement(), "" );
  GuiFieldgroupLine* line = GuiFactory::Instance()->createFieldgroupLine(fieldgroup->getElement());
  DataPoolIntens &dpi = DataPoolIntens::Instance();
  std::string vn(compose("@HelpDialog%1", name));
  dpi.getDataPool().AddToDictionary("", vn,  DataDictionary::type_String );

  DataReference* dref = dpi.getDataReference(vn);
  m_labelXfer = new XferDataItem(dref);
  m_labelXfer->getUserAttr()->SetLabelType();

  m_ChapterWidget = GuiFactory::Instance()->createDataField( line->getElement(), m_labelXfer );
  m_ChapterWidget->getElement()->setAlignment( GuiElement::align_Center );
  m_ChapterWidget->setLength(40);
  m_DialogWidget->getElement()->attach(m_ChapterWidget->getElement());

  // ScrolledText generieren
  m_TextWidget = GuiFactory::Instance()->createScrolledText(m_DialogWidget->getElement(), "content@"+name);
  m_TextWidget->setOptionLines(15);
  m_TextWidget->setOptionLength(40);
  m_DialogWidget->getElement()->attach(m_TextWidget->getElement());

  // Scrolledlist generieren
  m_ListWidget = GuiFactory::Instance()->createScrolledlist(m_DialogWidget->getElement(), this);
  m_ListWidget->setHeaderLabel(m_Title);
  m_ListWidget->setColumnLength(40);
  m_ListWidget->setTableSize(std::min((int)m_ChapterList.size(), 10));
  m_DialogWidget->getElement()->attach(m_ListWidget->getElement());

  // Buttonbar generieren
  bar = GuiFactory::Instance()->createButtonbar(m_DialogWidget->getElement());
  GuiButton* button = GuiFactory::Instance() -> createButton(bar->getElement(), &m_NextListener);
  button->setLabel( _("Next") );
  button = GuiFactory::Instance() -> createButton(bar->getElement(), &m_PreviousListener);
  button->setLabel( _("Previous") );
  button = GuiFactory::Instance() -> createButton(bar->getElement(), &m_CancelListener);
  button->setLabel( _("Close") );

  m_DialogWidget->getElement()->create();
}

/* --------------------------------------------------------------------------- */
/* initializeHelpDialog --                                                     */
/* --------------------------------------------------------------------------- */

void HelpText::initializeHelpDialog(){
  m_chapterIter = m_ChapterList.begin();
  m_ListWidget->createSelectionList();
  m_ListWidget->setEditable(false);

  std::string str;
  int		 n = 0;
  HelpChapterList::iterator chapterIter;
  for( chapterIter = m_ChapterList.begin(); chapterIter != m_ChapterList.end(); chapterIter++ ){
    n++;
    // str = XmStringCreateSimple( const_cast<char *>((*chapterIter).m_Title.c_str()) );
    // XmListAddItem( m_ListWidget, str, 0 );
    // XmStringFree( str );
  }
  assert( n == m_ChapterList.size() );
}


#ifdef HAVE_QT
/* --------------------------------------------------------------------------- */
/* createScrolledList --                                                       */
/* --------------------------------------------------------------------------- */
void HelpText::createScrolledList( QWidget* parent ){


  m_ListWidget->setHeaderLabel(m_Title);
  assert ( false );
}

/* --------------------------------------------------------------------------- */
/* createScrolledText --                                                       */
/* --------------------------------------------------------------------------- */
void HelpText::createScrolledText( QWidget* parent ){
  assert ( false );
}

#else

/* --------------------------------------------------------------------------- */
/* createScrolledList --                                                       */
/* --------------------------------------------------------------------------- */
void HelpText::createScrolledList( void* h ){
  assert ( false );
}

/* --------------------------------------------------------------------------- */
/* createScrolledText --                                                       */
/* --------------------------------------------------------------------------- */
void HelpText::createScrolledText( void* h ){
  assert ( false );
}

#endif

/* --------------------------------------------------------------------------- */
/* openHelpFile --                                                             */
/* --------------------------------------------------------------------------- */

bool HelpText::openHelpFile(){
  m_File.open(m_FileName.c_str());
  if ( m_File.fail() ) {
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* closeHelpFile --                                                            */
/* --------------------------------------------------------------------------- */

void HelpText::closeHelpFile(){
  m_File.close();
}

/* --------------------------------------------------------------------------- */
/* Next --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpText::next(){
  if ( m_Selected >= m_ChapterList.size() ){
    return;
  }
  m_Selected++;
  m_ListWidget->setSelectedPosition(m_Selected-1);
  loadChapter( (m_ChapterList[m_Selected -1]) );
}

/* --------------------------------------------------------------------------- */
/* Previous --                                                                 */
/* --------------------------------------------------------------------------- */

void HelpText::previous(){
  if ( m_Selected < 2 ){
    return;
  }
  m_Selected--;
  m_ListWidget->setSelectedPosition(m_Selected-1);
  loadChapter( (m_ChapterList[m_Selected -1]) );
}

/* --------------------------------------------------------------------------- */
/* Cancel --                                                                   */
/* --------------------------------------------------------------------------- */

void HelpText::cancel(){

  if ( m_DialogWidget != NULL ) {
    if ( m_IsPoppedUp ) {
#ifdef HAVE_QT
      m_DialogWidget->getElement()->unmanage();
#endif
      closeHelpFile();
      m_IsPoppedUp = false;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* loadChapter --                                                              */
/* --------------------------------------------------------------------------- */

void HelpText::loadChapter( const HelpChapter &chapter ){

  std::string	 text;
  std::string         buffer;
  long int	 len;

  Llog( LogDebug, "loadChapter", "loadChapter\n" );

  if(m_File.eof())
    m_File.clear();

  m_File.seekg(chapter.m_Start, std::ios::beg);
  if ( m_File.fail() ) {
    Llog( LogWarning, "loadChapter"
	  , _("Internal Error while reading '%s'")
	, m_FileName.c_str() );
    return;
  }

  len = chapter.m_End - chapter.m_Start;
  std::getline( m_File, text );

  while ( (len > text.size()) && (!m_File.eof())) {
    text += '\n';
    std::getline( m_File, buffer );
    text += buffer;
  }
  m_TextWidget->clearText();
  m_TextWidget->writeText(text);
  m_ChapterWidget->setValue(chapter.m_Title);
  m_ChapterWidget->getElement()->update(GuiElement::reason_FieldInput);
}

/* --------------------------------------------------------------------------- */
/* installChapter --                                                           */
/* --------------------------------------------------------------------------- */

HelpText::HelpChapter *HelpText::installChapter(){
  assert(m_File.is_open());

  HelpChapter	 chapter;
  std::string    line;
  char		 c;
  std::string	 keyName;
  std::string    spaces( " \t\n" );

  //Titel
  std::getline( m_File, line );
    if ( (line.find_first_not_of( spaces) == std::string::npos) || (m_File.eof()) ){
      line = _("<untitled>");
  }
  chapter.m_Title = line;


  //KeyWord
  m_File.get( c );
  if( c == s_KEYWORD_CHAR ){
    while ( c == s_KEYWORD_CHAR && !m_File.eof() ) {
      std::getline( m_File, line );
      if ( !(line.find_first_not_of( spaces ) == std::string::npos)){
	std::string::size_type first = 0, last = 0;
	while( last != std::string::npos ){
	  if( (first = line.find_first_not_of( spaces, last )) != std::string::npos ){
	    last = line.find_first_of( spaces, first );
	    if( last != std::string::npos ){
	      keyName = line.substr( first, last - first );
	    }
	    else{
	      keyName = line.substr( first );
	    }
	    installHelpKey( keyName, m_ChapterList.size() + 1 );
	  }
	}
      }
      m_File.get( c );
    }
    m_File.seekg( -1, std::ios::cur );
  }
  else{
    m_File.seekg( -1, std::ios::cur );
  }
  m_ChapterList.push_back( chapter );
  return &(m_ChapterList.back());
}

/* --------------------------------------------------------------------------- */
/* inspectHelpText --                                                          */
/* --------------------------------------------------------------------------- */

bool HelpText::inspectHelpText(){
  char	          c;
  std::string          str;
  std::string          spaces( " \t\n" );
//   MultiLanguage  &mls = MultiLanguage::Instance();
  HelpChapter    *chapter=0;

  //File an Startposition setzen
  m_File.seekg( 0, std::ios::beg );
  if ( m_File.fail() ) {
    Llog( LogWarning, "inspectHelpText"
// 	, mls.getString( "ML_HM_EMPTY_HELPFILE", ML_CONST_STRING ).c_str()
	  , _("Helpfile '%s' is empty")
	, m_FileName.c_str() );
    return true;
  }

  //Titel
  m_File.get(c);
  if ( c != s_TITLE_CHAR ) {
    Llog( LogWarning ,"inspectHelpfile"
// 	, mls.getString( "ML_HM_MISS_TITLE", ML_CONST_STRING ).c_str()
	  , _("No title specified in Helpfile '%s'")
	, m_FileName.c_str() );
    return true;
  }
  std::getline( m_File, m_Title );
  if ( (m_Title.find_first_not_of( spaces ) == std::string::npos) ){
//     m_Title = mls.getString( "ML_HM_UNDEF_TITLE", ML_CONST_STRING );
    m_Title = _("<untitled>");
  }

  //Kapitel
  while ( !m_File.eof() ) {
    m_File.get(c);
    if(!m_File.eof()){
      if ( c == s_CHAPTER_CHAR ) {
	chapter = 0;
	chapter = installChapter();
	assert( chapter != 0 );
	chapter->m_Start = m_File.tellg();
	chapter->m_End   = (-1) + m_File.tellg();
      }
      else{
	m_File.seekg(-1, std::ios::cur);
	std::getline( m_File, str );
	if( chapter != 0 ){
	  chapter->m_End += str.size() + 1;
	}
      }
      Llog( LogDebug, "inspectHelpText"
	    , "next line at %d\n" ,(int)m_File.tellg());
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* installHelpCallback --                                                      */
/* --------------------------------------------------------------------------- */

void HelpText::installHelpCallback(){
  UImanager &ui = UImanager::Instance();
  ui.addHelp( m_Title, this );

  if( false ){
    Llog( LogWarning, "HPinstallHelpText"
// 	  , MultiLanguage::Instance().getString( "ML_HM_FILE_TIT_IS_INST", ML_CONST_STRING ).c_str()
	  , _("Help title '%s' already specified.")
	  , m_Title.c_str() );
  }
  else{
    Llog( LogDebug, "HPinstalledHelpfile"
	  , _("Helpfile '%s' installed\n")
	  , m_Title.c_str() );
  }
}

/* --------------------------------------------------------------------------- */
/* help --                                                                     */
/* --------------------------------------------------------------------------- */

void HelpText::help( const std::string &keyName, const int chapterNo){
  helpDialog( chapterNo );
}

/* --------------------------------------------------------------------------- */
/* start --                                                                    */
/* --------------------------------------------------------------------------- */
bool HelpText::start() {
  m_chapterIter = m_ChapterList.begin();
  return true;
}

/* --------------------------------------------------------------------------- */
/* getNextItem --                                                              */
/* --------------------------------------------------------------------------- */
bool HelpText::getNextItem( std::string &label ) {
  if (m_chapterIter == m_ChapterList.end())
    return false;
  label = (*m_chapterIter).m_Title.c_str();
  ++m_chapterIter;
  return true;
}

/* --------------------------------------------------------------------------- */
/* activated --                                                                */
/* --------------------------------------------------------------------------- */
void HelpText::activated(int index) {
  if (index >= m_ChapterList.size())
    return;
  m_Selected = index + 1;
  loadChapter( (m_ChapterList[index]) );
}

/* --------------------------------------------------------------------------- */
/* selected --                                                                 */
/* --------------------------------------------------------------------------- */
void HelpText::selected(int index, bool flag) {
}
