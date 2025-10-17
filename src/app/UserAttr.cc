#include <string>
#include <limits>

#include "app/UserAttr.h"
#include "app/DataPoolIntens.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataReference.h"
#include "datapool/DataItem.h"
#include "job/JobFunction.h"
#include "gui/UnitManager.h"
#include "utils/HTMLConverter.h"

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

UserAttr::UserAttr( DataDictionary * pDict )
  : m_Editable( false )
    , m_Optional( false )
    , m_Lockable( false )
    , m_isScalar( false )
    , m_isMatrix( false )
    , m_isCell( false )
    , m_isHidden( false )
    , m_colorpicker( false )
    , m_isColorBit( true )
    , m_dbtransient( false )
    , m_isPersistent( false )
    , m_typeString( string_kind_none )
    , m_gui_type( gui_kind_field )
    , m_hasWheelEvent( false )
    , m_hasNoDependencies( false )
    , m_function( 0 )
    , m_focus_function( 0 )
    , m_popupDropMenu ( 0 )
    , m_maxOccurs( 1 )
  , m_min( std::numeric_limits<double>::min() ) // 0
  , m_max( std::numeric_limits<double>::max() ) // 99
    , m_step( 1 )
    , m_invalidEntry( true ){
}
UserAttr::UserAttr()
  : m_Editable( false )
  , m_Optional( false )
  , m_Lockable( false )
  , m_isScalar( false )
  , m_isMatrix( false )
  , m_isCell( false )
  , m_dbtransient( false )
  , m_isHidden( false )
  , m_colorpicker( false )
  , m_isColorBit( true )
  , m_isPersistent( false )
  , m_typeString( string_kind_none )
  , m_gui_type( gui_kind_field )
  , m_hasWheelEvent( false )
  , m_hasNoDependencies( false )
  , m_function( 0 )
  , m_focus_function( 0 )
  , m_popupDropMenu( 0 )
  , m_maxOccurs( 1 )
  , m_min( std::numeric_limits<double>::min() ) // 0
  , m_max( std::numeric_limits<double>::max() ) // 99
  , m_invalidEntry( true ){
}

UserAttr::UserAttr( const UserAttr &u ){
  *this = u;
}

UserAttr::~UserAttr(){
}

/*=============================================================================*/
/* operators                                                                   */
/*=============================================================================*/

void UserAttr::operator=( const UserAttr &attr ){
  m_Helptext              = attr.m_Helptext;
  m_Label                 = attr.m_Label;
  m_Unit                  = attr.m_Unit;
  m_pattern               = attr.m_pattern;
  m_DataSetName           = attr.m_DataSetName;
  m_DataSetIndexed        = attr.m_DataSetIndexed;
  m_colorsetname          = attr.m_colorsetname;
  m_colorpicker           = attr.m_colorpicker;
  m_isColorBit            = attr.m_isColorBit;
  m_dbattrname            = attr.m_dbattrname;
  m_dbunitname            = attr.m_dbunitname;
  m_dbtransient           = attr.m_dbtransient;
  m_min                   = attr.m_min;
  m_max                   = attr.m_max;
  m_step                  = attr.m_step;
  m_Editable              = attr.m_Editable;
  m_Optional              = attr.m_Optional;
  m_Lockable              = attr.m_Lockable;
  m_gui_type              = attr.m_gui_type;
  m_function              = attr.m_function;
  m_focus_function        = attr.m_focus_function;
  m_popupMenu             = attr.m_popupMenu;
  m_popupDropMenu         = attr.m_popupDropMenu;
  m_className             = attr.m_className;
  m_placeholder           = attr.m_placeholder;
  m_stylesheet            = attr.m_stylesheet;
  m_maxOccurs             = attr.m_maxOccurs;
  m_isScalar              = attr.m_isScalar;
  m_isMatrix              = attr.m_isMatrix;
  m_isCell                = attr.m_isCell;
  m_isHidden              = attr.m_isHidden;
  m_isPersistent          = attr.m_isPersistent;
  m_tags                  = attr.m_tags;
  m_typeString            = attr.m_typeString;
  m_hasWheelEvent         = attr.m_hasWheelEvent;
  m_hasNoDependencies     = attr.m_hasNoDependencies;
  m_isArrowKeys           = attr.m_isArrowKeys;
  m_invalidEntry          = attr.m_invalidEntry;
}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* NewClass --                                                                 */
/* --------------------------------------------------------------------------- */

DataUserAttr *UserAttr::NewClass(DataDictionary *pDict) const{
  return pDict ? new UserAttr(pDict) : new UserAttr();
}

/* --------------------------------------------------------------------------- */
/* copyClass --                                                                */
/* --------------------------------------------------------------------------- */

DataUserAttr * UserAttr::copyClass() const {
  return new UserAttr( *this );
}

/* --------------------------------------------------------------------------- */
/* getAttributes --                                                            */
/* --------------------------------------------------------------------------- */

DATAAttributeMask UserAttr::getAttributes() const{
  DATAAttributeMask mask = 0;
  if( m_Editable )
    mask = mask | DATAeditable;
  if( m_Optional )
    mask = mask | DATAoptional;
  if( m_Lockable )
    mask = mask | DATAlockable;
  if( !m_colorsetname.empty() )
    mask = mask | DATAuseColorset;
  if( m_gui_type == gui_kind_label )
    mask = mask | DATAtypeLabel;
  return mask;
}

/* --------------------------------------------------------------------------- */
/* fixupAttributes --                                                          */
/* --------------------------------------------------------------------------- */

void UserAttr::fixupAttributes( DataItem &item ){
  // amg: 2007-08-16 : wenn alle drei flags ungesetzt sind, sollte zumindest
  // das DATAIsReadOnly gesetzt sein

  DATAAttributeMask  mask = item.getAttributes( 0 );

  if( item.getDataType() != DataDictionary::type_StructVariable ){
    if( ( mask & DATAIsEditable ) == 0 &&
        ( mask & DATAIsReadOnly ) == 0 &&
        ( mask & DATAeditable )   == 0 ){
      item.setAttributes( DATAIsReadOnly, DATAIsEditable );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* write --                                                                    */
/* --------------------------------------------------------------------------- */

void UserAttr::write( std::ostream &ostr, const std::vector<std::string> &attrs ) const{
  std::vector<std::string>::const_iterator iter;
  if( m_dbtransient )
    return;

  for( iter = attrs.begin(); iter != attrs.end(); ++iter ){
    if( *iter == "label" && m_Label.size()>0 ){
      std::string s(m_Label);
      HTMLConverter::convert2HTML( s );
      ostr << " label = \"" << s << "\"";
    } else if( *iter == "unit" && m_Unit.size()>0 )
      ostr << " unit = \"" << m_Unit << "\"";
    else if( *iter == "pattern" && m_pattern.size()>0 ){
      std::string s(m_pattern);
      HTMLConverter::convert2HTML( s );
      ostr << " pattern = \"" << m_pattern << "\"";
    }else if( *iter == "helptext" && m_Helptext.size() ){
      std::string s(m_Helptext);
      HTMLConverter::convert2HTML( s );
      ostr << " helptext = \"" << s << "\"";
    }else if( *iter == "scalar" && m_isScalar )
      ostr << " scalar=\"true\"";
    else if( *iter == "matrix" && m_isMatrix )
      ostr << " matrix=\"true\"";
    else if( *iter == "dbattr" && m_dbattrname.size()>0 )
      ostr << " dbattr = \"" << m_dbattrname << "\"";
    else if( *iter == "dbunit" ){
      std::string dbunit = m_dbunitname.empty() ? UnitManager::Instance().getDbUnit(m_Unit)
        : m_dbunitname;
      if(!dbunit.empty()){
        ostr << " dbunit = \"" << dbunit << "\"";
      }
    }
    else if( *iter == "function" && m_function )
      ostr << " function = \"" << m_function->Name() << "\"";
    else if( *iter == "focus_function" && m_focus_function )
      ostr << " focus_function = \"" << m_focus_function->Name() << "\"";
  }
}

/* --------------------------------------------------------------------------- */
/* set<Functions> --                                                           */
/* --------------------------------------------------------------------------- */

void UserAttr::SetEditable()                       { m_Editable = true; }
void UserAttr::SetOptional()                       { m_Optional = true; }
void UserAttr::SetLockable()                       { m_Lockable = true; }
void UserAttr::SetScalar()                         { m_isScalar = true; }
void UserAttr::SetMatrix()                         { m_isMatrix = true; }
void UserAttr::SetCell()                           { m_isCell = true; }
void UserAttr::SetHidden()                         { m_isHidden = true; }
void UserAttr::SetPersistent()                     { m_isPersistent = true; }
void UserAttr::SetStringType(UserAttr::STRINGtype type) { m_typeString = type; }
void UserAttr::SetWheelEvent()                     { m_hasWheelEvent = true; }
void UserAttr::SetNoDependencies()                 { m_hasNoDependencies = true; }

void UserAttr::SetLabel(const std::string &label)       { m_Label = label; }
void UserAttr::SetDbAttr(const std::string &name)       { m_dbattrname = name; }
void UserAttr::SetDbUnit(const std::string &name)       { m_dbunitname = name; }
void UserAttr::SetDbTransient()                         { m_dbtransient = true; }
void UserAttr::SetUnit(const std::string &unit)         { m_Unit = unit; }
void UserAttr::SetPattern(const std::string &pattern)   { m_pattern = pattern; }
void UserAttr::SetHelptext(const std::string &text)     { m_Helptext = text; }
void UserAttr::SetClassname(const std::string &name, const std::string &fullname) {
  if (fullname.empty()) {
     m_className = name;
     return;
  }
  m_classNameMap[fullname] = name;
}
void UserAttr::SetPlaceholder(const std::string &text)    { m_placeholder = text; }
void UserAttr::SetStylesheet(const std::string &stylesheet)    { m_stylesheet = stylesheet; }
void UserAttr::SetMaxOccurs(const int num)         { m_maxOccurs = num; }
void UserAttr::SetDataSetName(const std::string &name, bool bIndexed)  { m_DataSetName = name; m_DataSetIndexed = bIndexed; }
void UserAttr::setColorSetName( const std::string &name ) { m_colorsetname = name; }
void UserAttr::setColorPicker() { m_colorpicker = true; }
void UserAttr::setNoColorBit() { m_isColorBit = false; }
void UserAttr::SetButton()                         { m_gui_type = gui_kind_button; }
void UserAttr::SetSlider()                         { m_gui_type = gui_kind_slider; }
void UserAttr::SetToggle()                         { m_gui_type = gui_kind_toggle; }
void UserAttr::SetRadioButton()                    { m_gui_type = gui_kind_radiobutton; }
void UserAttr::SetCombobox()                       { m_gui_type = gui_kind_combobox; }
void UserAttr::SetLabelType()                      { m_gui_type = gui_kind_label; }
void UserAttr::unsetInvalidEntry()                 { m_invalidEntry = false; }

/* --------------------------------------------------------------------------- */
/* get<Functions> --                                                           */
/* --------------------------------------------------------------------------- */

bool UserAttr::IsEditable()             const { return m_Editable; }
bool UserAttr::IsOptional()             const { return m_Optional; }
bool UserAttr::IsLockable()             const { return m_Lockable; }
bool UserAttr::IsScalar()               const { return m_isScalar; }
bool UserAttr::IsMatrix()               const { return m_isMatrix; }
bool UserAttr::IsCell()                 const { return m_isCell; }
bool UserAttr::IsHidden()               const { return m_isHidden; }
bool UserAttr::IsPersistent()           const { return m_isPersistent; }
UserAttr::STRINGtype UserAttr::StringType() const { return m_typeString; }
bool UserAttr::hasWheelEvent()          const { return m_hasWheelEvent; }
bool UserAttr::hasNoDependencies()      const { return m_hasNoDependencies; }
bool UserAttr::IsDbTransient()          const { return m_dbtransient; }

const std::string &UserAttr::DbAttr()   const { return m_dbattrname; }
const std::string &UserAttr::DbUnit()   const { return m_dbunitname; }
const std::string UserAttr::Unit(bool unitManagerFeature, DataReference* data_ref) {
  if (unitManagerFeature && AppData::Instance().UnitManagerFeature()) {
    std::string s(UnitManager::Instance().getComboBoxData(m_Unit, this, data_ref));
    return s.empty() ? m_Unit : s;
  }
  return m_Unit;
}
const std::string &UserAttr::Pattern() const { return m_pattern; }
const std::string &UserAttr::Helptext() const { return m_Helptext; }
const std::string &UserAttr::Classname(const std::string &fullname) {
  if (fullname.empty()) {
    return m_className;
  }
  return m_classNameMap[fullname];
}
const std::string &UserAttr::Placeholder() const { return m_placeholder; }
const std::string &UserAttr::Stylesheet() const { return m_stylesheet; }
int           UserAttr::MaxOccurs()    const { return m_maxOccurs; }
const std::string &UserAttr::DataSetName()  const { return m_DataSetName; }
bool UserAttr::DataSetIndexed()        const { return m_DataSetIndexed; }
const std::string &UserAttr::ColorsetName() const { return m_colorsetname; }
bool UserAttr::IsColorPicker() const { return m_colorpicker; }
bool UserAttr::IsColorBit() const { return m_isColorBit; }
bool UserAttr::IsField()               const { return m_gui_type == gui_kind_field; }
bool UserAttr::IsButton()              const { return m_gui_type == gui_kind_button; }
bool UserAttr::IsToggle()              const { return m_gui_type == gui_kind_toggle; }
bool UserAttr::IsRadio()               const { return m_gui_type == gui_kind_radiobutton; }
bool UserAttr::IsCombobox()            const { return m_gui_type == gui_kind_combobox; }
UserAttr::GUItype UserAttr::GuiKind()  const { return m_gui_type; }
bool UserAttr::hasInvalidEntry()       const { return m_invalidEntry; }

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */

void UserAttr::setFunction( JobFunction *func ){
  m_function = func;
  if( m_function != 0 )
    m_function->setUsed();
}

/* --------------------------------------------------------------------------- */
/* setFocusFunction --                                                         */
/* --------------------------------------------------------------------------- */

void UserAttr::setFocusFunction( JobFunction *func ){
  m_focus_function = func;
  if( m_focus_function != 0 )
    m_focus_function->setUsed();
}

/* --------------------------------------------------------------------------- */
/* getFunction --                                                              */
/* --------------------------------------------------------------------------- */

JobFunction *UserAttr::getFunction(){
  return m_function;
}

/* --------------------------------------------------------------------------- */
/* getFocusFunction --                                                         */
/* --------------------------------------------------------------------------- */

JobFunction *UserAttr::getFocusFunction(){
  return m_focus_function;
}

/* --------------------------------------------------------------------------- */
/* setPopupMenu --                                                             */
/* --------------------------------------------------------------------------- */

void UserAttr::setPopupMenu( GuiPopupMenu *menu, unsigned int idx ){
  while(idx >= m_popupMenu.size()) {
    m_popupMenu.push_back(NULL);
  }
  m_popupMenu[idx] = menu;
}

/* --------------------------------------------------------------------------- */
/* getPopupMenu --                                                              */
/* --------------------------------------------------------------------------- */

GuiPopupMenu *UserAttr::getPopupMenu(unsigned int idx){
  if (idx < m_popupMenu.size())
    return m_popupMenu[idx];
  return NULL;
}

/* --------------------------------------------------------------------------- */
/* setPopupDropMenu --                                                         */
/* --------------------------------------------------------------------------- */

void UserAttr::setPopupDropMenu( GuiPopupMenu *menu ){
  m_popupDropMenu = menu;
}

/* --------------------------------------------------------------------------- */
/* getPopupDropMenu --                                                         */
/* --------------------------------------------------------------------------- */

GuiPopupMenu *UserAttr::getPopupDropMenu() const {
  return m_popupDropMenu;
}

/* --------------------------------------------------------------------------- */
/* setTag --                                                                   */
/* --------------------------------------------------------------------------- */

void UserAttr::setTag( const std::string &tag ){
  m_tags.push_back( tag );
}

/* --------------------------------------------------------------------------- */
/* hasTag --                                                                   */
/* --------------------------------------------------------------------------- */

bool UserAttr::hasTag( const std::string &tag ){
  TagList::iterator it;
  for( it = m_tags.begin(); it != m_tags.end(); ++it ){
    if( (*it) == tag ){
      return true;
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* clearTags --                                                                */
/* --------------------------------------------------------------------------- */

void UserAttr::clearTags() {
  m_tags.clear();
}

/* --------------------------------------------------------------------------- */
/* getTagList --                                                               */
/* --------------------------------------------------------------------------- */
UserAttr::TagList UserAttr::getTagList() {
  return m_tags;
}

/* --------------------------------------------------------------------------- */
/* Label --                                                                    */
/* --------------------------------------------------------------------------- */

const std::string &UserAttr::Label( DataReference *ref ) {
  if( m_Label.empty() ){
    if( ref != 0 )
      m_Label = ref->nodeName();
  }
  return m_Label;
}

/* --------------------------------------------------------------------------- */
/* isSerializable --                                                           */
/* --------------------------------------------------------------------------- */

bool UserAttr::isSerializable( const SerializableMask mask ) const{
  // hidden
  if( m_isHidden ){
    if( mask & HIDEhidden ){
      return false;
    }
  }

  // transient
  if( m_dbtransient ){
    if( mask & HIDEtransient ){
      return false;
    }
  }
  return true;
}
