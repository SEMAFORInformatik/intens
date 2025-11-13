#include "gui/GuiElement.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiForm.h"
#include "gui/GuiPopupMenu.h"
#include "gui/GuiFactory.h"
#include "gui/IconManager.h"
#include "gui/UnitManager.h"
#include "utils/Debugger.h"
#include "utils/FileUtilities.h"
#include "utils/base64.h"
#include "utils/StringUtils.h"
#include "utils/JsonUtils.h"
#include "utils/gettext.h"
#include "app/AppData.h"
#include "app/App.h"
#include "parser/Flexer.h"
#include "streamer/StreamManager.h"
#include <regex>

INIT_LOGGER();

// initialize
int    GuiElement::s_clone_number = 0;
int    GuiElement::s_next_id = 0;
GuiElement::GuiStyle GuiElement::s_gui_style = GuiElement::style_31;
GuiSortedElementList GuiElement::s_namedElementList;
GuiElementList GuiElement::s_elementList;
int    GuiElement::s_maxFormOrder = -1;
int    GuiElement::s_topFormOrder = -1;
GuiForm* GuiElement::s_topForm = 0;
extern int   PAlineno;

#if HAVE_PROTOBUF && __clang__
/*
  Incomplete/Incompatible Abseil Libraries

  2. Use the Same Compiler Flags

  Abseil can behave differently depending on the compiler flags (like C++ standard versions).
  Make sure both Abseil and your project are compiled using the same -std=c++XX (e.g., -std=c++17 or -std=c++20).

  3. Force Instantiation
  You may need to explicitly instantiate the operator in one of your source files:
  !!! This is usually not ideal unless you're managing Abseil directly, but it can solve missing symbols like this.
*/
template absl::log_internal::LogMessage& absl::log_internal::LogMessage::operator<< <unsigned long>(unsigned long const&);
template absl::log_internal::LogMessage& absl::log_internal::LogMessage::operator<< <int>(int const&);
#endif

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiElement::GuiElement( GuiElement *parent, const std::string &name )
  : m_parent( parent )
  , m_name(name)
  , m_popupMenu(0)
  , m_id(++s_next_id)
  , m_enabled( true)
  , m_drag_disabled( false )
  , m_x(0)
  , m_y(0)
  , m_rowspan(1)
  , m_colspan(1)
  , m_formZOrder(-1)
  , m_openedByForm(0)
  , m_alignment(align_Default)
  , m_rotation(0)
  , m_helptextStream(0)
  , m_lastGuiUpdated(0)
  , m_lastWebUpdated(0)
  , m_visible( true )
  , m_hidden( false )
  , m_attributeChanged(false)
  , m_isShownWebApi(true) {
  if ( !name.empty() ){
    GuiSortedElementList::const_iterator el =  s_namedElementList.find(name);
    if( el != s_namedElementList.end()){
      throw "already declared";
    }
    s_namedElementList.insert( GuiNamedElementList::value_type(name, this) );
  }
  s_elementList.push_back(this);

  if (AppData::Instance().LspWorker() && App::HasInstance() && App::Instance().HasFlexer()) {
    auto filename = App::Instance().getFlexer()->getCurrentFilename();
    if (filename.ends_with('"')) {
      filename.pop_back();
    }
    setLSPFilename(filename);
    // m_rep->dictitem->setFilename(filename);
    int lineNo = PAlineno -(*(App::Instance().getFlexer()->YYText() ) == '\n' || ! *(App::Instance().getFlexer()->YYText()));
    // m_rep->dictitem->setLineno(lineNo);
    setLSPLineno(lineNo);
  }
}

GuiElement::~GuiElement(){
  if (!m_name.empty()) {
    GuiSortedElementList::iterator el =  s_namedElementList.find(m_name);
    assert( el != s_namedElementList.end());
    s_namedElementList.erase( el );
  }
  if( m_helptextStream != 0 ){
    StreamManager::Instance().deleteStream( m_helptextStream );
  }
  if( m_popupMenu )
    delete m_popupMenu;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* enable --                                                                   */
/* --------------------------------------------------------------------------- */

void GuiElement::enable(){
  if (!m_enabled) ResetLastWebUpdated();  // reason_Always for webtens
  m_enabled = true;

  // enable cloned
  std::vector<GuiElement*> cList;
  getCloneList(cList);
  std::vector<GuiElement*>::iterator it = cList.begin();
  for (; it != cList.end(); ++it)
    (*it)->enable();
}

/* --------------------------------------------------------------------------- */
/* disable --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiElement::disable(){
  if (m_enabled) ResetLastWebUpdated();  // reason_Always for webtens
  m_enabled = false;
  // disable cloned
  std::vector<GuiElement*> cList;
  getCloneList(cList);
  std::vector<GuiElement*>::iterator it = cList.begin();
  for (; it != cList.end(); ++it)
    (*it)->disable();
}

/* --------------------------------------------------------------------------- */
/* disable drag --                                                                  */
/* --------------------------------------------------------------------------- */

void GuiElement::disable_drag(){
  if (m_drag_disabled) ResetLastWebUpdated();  // reason_Always for webtens
  m_drag_disabled = true;
  // disable cloned
  std::vector<GuiElement*> cList;
  getCloneList(cList);
  std::vector<GuiElement*>::iterator it = cList.begin();
  for (; it != cList.end(); ++it)
    (*it)->disable_drag();
}

/* --------------------------------------------------------------------------- */
/* setClass --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiElement::setClass(const std::string &classname){
  m_classname = classname;

  // setClass cloned
  std::vector<GuiElement*> cList;
  getCloneList(cList);
  std::vector<GuiElement*>::iterator it = cList.begin();
  for (; it != cList.end(); ++it)
    (*it)->setClass(classname);
}

/* --------------------------------------------------------------------------- */
/* getElementId --                                                             */
/* --------------------------------------------------------------------------- */
const std::string GuiElement::getElementId() {
  std::ostringstream os;
  os << StringType() << "_" << m_id;
  return os.str() ;
}

/* --------------------------------------------------------------------------- */
/* getElementIntId --                                                             */
/* --------------------------------------------------------------------------- */
const int GuiElement::getElementIntId() {
  return m_id;
}

/* --------------------------------------------------------------------------- */
/* StringType() --                                                             */
/* --------------------------------------------------------------------------- */
const std::string GuiElement::StringType() {
  switch (Type()) {
  case type_Main:
    return "Main";
  case type_Form:
    return "Form";
  case type_Header:
    return "Header";
  case type_Container:
    return "container";
  case type_Fieldgroup:
    return "Fieldgroup";
  case type_FieldgroupLine:
    return "FieldgroupLine";
  case type_Folder:
    return "Folder";
  case type_Index:
    return "Index";
  case type_IndexMenu:
    return "IndexMenu";
  case type_Text:
    return "Text";
  case type_Textfield:
    return "Textfield";
  case type_ComboBox:
    return "ComboBox";
  case type_Toggle:
    return "Toggle";
  case type_RadioButton:
    return "Radio";
  case type_Button:
  case type_FieldButton:
  case type_CycleButton:
    return "Button";
  case type_Label:
    return "Label";
  case type_Pixmap:
    return "Pixmap";
  case type_Separator:
    return "Separator";
  case type_Void:
    return "Void";
  case type_Stretch:
    return "Stretch";
  case type_ScrolledText:
    return "ScrolledText";
  case type_Scrolledlist:
    return "List";
  case type_Table:
    return "Table";
  case type_TableLine:
    return "TableLine";
  case type_TableDataItem:
    if (getDataField()) {
      if (getDataField()->Attr()->IsCombobox()) {
        std::string s;
        getDataField()->getValue(s);
        return (getDataField()->Attr()->DataSetIndexed() && s.empty()) ?
          "TableDataItem" : "TableComboBoxDataItem";
      }
      if (getDataField()->Attr()->IsRadio())
        return "TableRadioDataItem";
      if (getDataField()->Attr()->IsToggle())
        return "TableToggleDataItem";
      if (getDataField()->Attr()->IsColorPicker())
        return "TableColorPickerDataItem";
      if (getDataField()->Attr()->IsButton())
        return "TableButtonDataItem";
    }
    return "TableDataItem";
  case type_Menubar:
    return "Menubar";
  case type_PulldownMenu:
    return "PulldownMenu";
  case type_PopupMenu:
    return "PopupMenu";
  case type_MenuButton:
    return "MenuButton";
  case type_MenuToggle:
    return "MenuToggle";
  case type_Buttonbar:
    return "Buttonbar";
  case type_Message:
    return "Message";
  case type_Plugin:
    return "Plugin";
  case type_3dPlot:
    return "3DPlot";
  case type_QWTPlot:
  case type_2dXrtPlot:
    return "2DPlot";
  case type_List:
    return "List";
  case type_TextDialog:
    return "TextDialog";
  case type_Navigator:
    return "Navigator";
  case type_NavDiagram:
    return "Diagram";
  case type_NavIconView:
    return "IconView";
  case type_Simpel:
    return "SimpelPlot";
  case type_ListPlot:
    return "ListPlot";
  case type_Slider:
    return "Slider";
  case type_Image:
    return "Image";
  case type_Thermo:
    return "Thermo";
  case type_ProgressBar:
    return "ProgressBar";
  default:
    // ignore 'type_Manager', 'type_OptionMenu', 'type_TableLabelItem'
    ;
  }
  std::ostringstream os;
  os << Type();
  return os.str();
}

#if HAVE_PROTOBUF
const in_proto::GuiElement::Type GuiElement::ProtobufType() {
  switch (Type()) {
    case type_Main:
    case type_Form:
      return in_proto::GuiElement::Type::GuiElement_Type_EForm;
    case type_Header:
      return in_proto::GuiElement::Type::GuiElement_Type_EHeader;
    case type_Container:
      return in_proto::GuiElement::Type::GuiElement_Type_EContainer;
    case type_Fieldgroup:
      return in_proto::GuiElement::Type::GuiElement_Type_EFieldGroup;
    case type_FieldgroupLine:
      return in_proto::GuiElement::Type::GuiElement_Type_EFieldGroupLine;
    case type_Folder:
      return in_proto::GuiElement::Type::GuiElement_Type_EFolder;
    case type_Index:
      return in_proto::GuiElement::Type::GuiElement_Type_EIndex;
    case type_IndexMenu:
      return in_proto::GuiElement::Type::GuiElement_Type_EIndexMenu;
    case type_Text:
      return in_proto::GuiElement::Type::GuiElement_Type_EText;
    case type_Textfield:
      return in_proto::GuiElement::Type::GuiElement_Type_ETextfield;
    case type_ComboBox:
      return in_proto::GuiElement::Type::GuiElement_Type_EComboBox;
    case type_Toggle:
      return in_proto::GuiElement::Type::GuiElement_Type_EToggle;
    case type_RadioButton:
      return in_proto::GuiElement::Type::GuiElement_Type_ERadio;
    case type_Button:
    case type_FieldButton:
      return in_proto::GuiElement::Type::GuiElement_Type_EButton;
    case type_CycleButton:
      return in_proto::GuiElement::Type::GuiElement_Type_ECycleButton;
    case type_Label:
      return in_proto::GuiElement::Type::GuiElement_Type_ELabel;
    case type_Pixmap:
      return in_proto::GuiElement::Type::GuiElement_Type_EPixmap;
    case type_Separator:
      return in_proto::GuiElement::Type::GuiElement_Type_ESeparator;
    case type_Void:
      return in_proto::GuiElement::Type::GuiElement_Type_EVoid;
    case type_Stretch:
      return in_proto::GuiElement::Type::GuiElement_Type_EStretch;
    case type_ScrolledText:
      return in_proto::GuiElement::Type::GuiElement_Type_EScrolledText;
    case type_Scrolledlist:
      return in_proto::GuiElement::Type::GuiElement_Type_EList;
    case type_Table:
      return in_proto::GuiElement::Type::GuiElement_Type_ETable;
    case type_Menubar:
      return in_proto::GuiElement::Type::GuiElement_Type_EMenubar;
    case type_PulldownMenu:
      return in_proto::GuiElement::Type::GuiElement_Type_EPulldownMenu;
    case type_MenuButton:
      return in_proto::GuiElement::Type::GuiElement_Type_EMenuButton;
    case type_MenuToggle:
      return in_proto::GuiElement::Type::GuiElement_Type_EMenuToggle;
    case type_Buttonbar:
      return in_proto::GuiElement::Type::GuiElement_Type_EButtonbar;
    case type_Message:
      return in_proto::GuiElement::Type::GuiElement_Type_EMessage;
    case type_Plugin:
    case type_3dPlot:
      return in_proto::GuiElement::Type::GuiElement_Type_EPlot3D;
    case type_QWTPlot:
    case type_2dXrtPlot:
      return in_proto::GuiElement::Type::GuiElement_Type_EPlot2D;
    case type_List:
      return in_proto::GuiElement::Type::GuiElement_Type_EList;
    case type_Navigator:
      return in_proto::GuiElement::Type::GuiElement_Type_ENavigator;
    case type_NavDiagram:
      return in_proto::GuiElement::Type::GuiElement_Type_EDiagram;
    case type_NavIconView:
      return in_proto::GuiElement::Type::GuiElement_Type_EIconView;
    case type_Slider:
      return in_proto::GuiElement::Type::GuiElement_Type_ESlider;
    case type_Thermo:
      return in_proto::GuiElement::Type::GuiElement_Type_EThermo;
    case type_ProgressBar:
      return in_proto::GuiElement::Type::GuiElement_Type_EProgressbar;
    case type_TableDataItem:
      if (getDataField()) {
        if (getDataField()->Attr()->IsCombobox()) {
          std::string s;
          getDataField()->getValue(s);
          return (getDataField()->Attr()->DataSetIndexed() && s.empty()) ?
            in_proto::GuiElement::Type::GuiElement_Type_ETableDataItem : in_proto::GuiElement::Type::GuiElement_Type_ETableComboBoxDataItem;
        }
        if (getDataField()->Attr()->IsRadio())
          return in_proto::GuiElement::Type::GuiElement_Type_ETableRadioDataItem;
        if (getDataField()->Attr()->IsToggle())
          return in_proto::GuiElement::Type::GuiElement_Type_ETableToggleDataItem;
        if (getDataField()->Attr()->IsColorPicker())
          return in_proto::GuiElement::Type::GuiElement_Type_ETableColorPickerDataItem;
        if (getDataField()->Attr()->IsButton())
          return in_proto::GuiElement::Type::GuiElement_Type_ETableButtonDataItem;
      default:
        // ignore 'type_Manager', 'type_OptionMenu', 'type_TableLine'
        ;
      }
      return in_proto::GuiElement::Type::GuiElement_Type_ETableDataItem;
  }
  return in_proto::GuiElement::Type::GuiElement_Type_Unknown;
}
#endif
/* --------------------------------------------------------------------------- */
/* StringExpandType() --                                                       */
/* --------------------------------------------------------------------------- */
const std::string GuiElement::StringExpandType() {
  GuiElement::Orientation o = (Type() == type_Form || Type() == type_Main) ? getDialogExpandPolicy() :
    getContainerExpandPolicy();
  switch (o) {
  case orient_Horizontal:
    return "horizontal";
  case orient_Vertical:
    return "vertical";
  case orient_Both:
    return "both";
  default:
    return "none";
  }
}

/* --------------------------------------------------------------------------- */
/* StringAlignmentType() --                                                    */
/* --------------------------------------------------------------------------- */
const std::string GuiElement::StringAlignmentType(Alignment align) {
  switch (align) {
  case align_Default:
    return "default";
  case align_Left:
    return "left";
  case align_Right:
    return "right";
  case align_Center:
    return "center";
  case align_Top:
    return "top";
  case align_Bottom:
    return "bottom";
  case align_Stretch:
    return "stretch";
  default:
    return "none";
  }
}

/* --------------------------------------------------------------------------- */
/* writeJsonProperties() --                                                    */
/* --------------------------------------------------------------------------- */
void GuiElement::writeJsonProperties(Json::Value& jsonObj) {
  GuiElement* cont = getParent();
  if (getName().size() > 0)
    jsonObj["name"] = getName();
  jsonObj["id"] = getElementId();
  jsonObj["type"] = StringType();
  jsonObj["alignment"] = GuiElement::StringAlignmentType(m_alignment);
  jsonObj["form_name"] = getMyForm() ? getMyForm()->getElement()->getName() : getMainForm()->getElement()->getName();
  jsonObj["enabled"] = isEnabled();

  if (!Class().empty()) jsonObj["class"] = Class();

  if (Type() == type_Table || Type() == type_List || Type() == type_Folder ||
      Type() == type_Container || Type() == type_Fieldgroup ||
      Type() == type_Pixmap || Type() == type_ScrolledText ||
      Type() == type_Scrolledlist || Type() == type_Plugin ||
      Type() == type_3dPlot || Type() == type_QWTPlot ||
      Type() == type_Image || Type() == type_Thermo ||
      Type() == type_Text) {
    std::string s;
    int width(0), height(0), fwidth(0), fheight(0), cwidth(0), cheight(0);
    getSize(width, height);
    jsonObj["width"] = Type() == type_Table ? int(width * 1.15) : width;
    jsonObj["height"] = Type() == type_Table ? int(height * 1.15) : height;
    jsonObj["expand"] = StringExpandType();
    if (hasHelptext()) {
      getHelptext(s);
      jsonObj["helptext"] = s;
    }

    // all named guiElement (FIELDGRPOUP, PLOT2D, ...)
    if (getName().size()) {
      jsonObj["visible"] = getVisibleFlag();
    }

    //
    if (cwidth != 0 && cheight) {
      jsonObj["relative_container_horizontal"] = ((double) width) / cwidth;
      jsonObj["relative_container_vertical"] = ((double) height) / cheight;
      jsonObj["relative_form_horizontal"] = ((double) width) / fwidth;
      jsonObj["relative_form_vertical"] = ((double) height) / fheight;
    }
  }

  // column and row span
  if (getColSpan() != 1)
  jsonObj["colspan"] = getColSpan();
  if (getRowSpan() != 1)
  jsonObj["rowspan"] = getRowSpan();
}

#if HAVE_PROTOBUF
const in_proto::GuiElement::Alignment GuiElement::ProtoAlignmentType(Alignment align) {
  switch (align) {
  case align_Default:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Default;
  case align_Left:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Left;
  case align_Right:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Right;
  case align_Center:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Center;
  case align_Top:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Top;
  case align_Bottom:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Bottom;
  case align_Stretch:
    return in_proto::GuiElement::Alignment::GuiElement_Alignment_Stretch;
  }
  return in_proto::GuiElement::Alignment::GuiElement_Alignment_Default;
}

const in_proto::GuiElement::Orientation GuiElement::ProtoOrientationType() {
  GuiElement::Orientation o = (Type() == type_Form || Type() == type_Main) ? getDialogExpandPolicy() :
    getContainerExpandPolicy();
  switch (o) {
  case orient_Horizontal:
    return in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal;
  case orient_Vertical:
    return in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical;
  case orient_Both:
    return in_proto::GuiElement::Orientation::GuiElement_Orientation_Both;
  default:
    return in_proto::GuiElement::Orientation::GuiElement_Orientation_None;
  }
}

in_proto::GuiElement* GuiElement::writeProtobufProperties() {
  auto element = new in_proto::GuiElement;
  element->set_name(getName());
  element->set_id(getElementIntId());
  element->set_alignment(ProtoAlignmentType(m_alignment));
  element->set_enabled(isEnabled());

  if (!Class().empty()) element->set_styleclass(Class());

  if (Type() == type_Table || Type() == type_List || Type() == type_Folder ||
      Type() == type_Container || Type() == type_Fieldgroup ||
      Type() == type_Pixmap || Type() == type_ScrolledText ||
      Type() == type_Scrolledlist || Type() == type_Plugin ||
      Type() == type_3dPlot || Type() == type_QWTPlot ||
      Type() == type_Image || Type() == type_Thermo ||
      Type() == type_Text) {
    std::string s;
    int width(0), height(0), fwidth(0), fheight(0), cwidth(0), cheight(0);
    getSize(width, height);
    element->set_width(Type() == type_Table ? int(width * 1.15) : width);
    element->set_height(Type() == type_Table ? int(height * 1.15) : height);
    element->set_expand(ProtoOrientationType());
    if (hasHelptext()) {
      getHelptext(s);
      element->set_helptext(s);
    }

    // all named guiElement (FIELDGRPOUP, PLOT2D, ...)
    if (getName().size()) {
      element->set_visible(getVisibleFlag());
    }
  }
  //
  // column and row span
  if (getColSpan() != 1)
    element->set_colspan(getColSpan());
  if (getRowSpan() != 1)
    element->set_rowspan(getRowSpan());


  return element;
}
#endif
/* --------------------------------------------------------------------------- */
/* writeDataTypeJsonProperties() --                                            */
/* --------------------------------------------------------------------------- */
void GuiElement::writeDataTypeJsonProperties(Json::Value& jsonObj, XferDataItem *item, XferDataParameter* param) {
    double min = param->DataItem()->getUserAttr()->getMin();
    double max = param->DataItem()->getUserAttr()->getMax();
    char radix_point = RealConverter::decimalPoint();

    // pattern
    if (param->DataItem()->getUserAttr()->Pattern().size()) {
      jsonObj["pattern_value"] = param->DataItem()->getUserAttr()->Pattern();
    }

    jsonObj["value"] = "";  // default value
    switch( item->getDataType() ){
    case DataDictionary::type_Integer:{
      jsonObj["datatype"] = "integer";
      int i;
      std::string s;
      if (item->isValid() && item->getValue(i)){
        jsonObj["value"] = i;
        param->getFormattedValue( s );
        trim(s);
        jsonObj["formatted_value"] = s;
      }
      // range
      if (AppData::Instance().GuiWithRangeCheck()) {
        if (min != std::numeric_limits<double>::min() ) {
          jsonObj["min_value"] = floor(0.5+min);
        }
        if (max != std::numeric_limits<double>::max() ) {
          jsonObj["max_value"] = floor(0.5+max);
        }
      }

      // pattern
      if (!jsonObj.isMember("pattern_value")){
        jsonObj["pattern_value"] = "^[+-]?[0-9]+([eE][+]?[0-9]+)?$";
      }
      break;
    }
    case DataDictionary::type_Real:{
      jsonObj["datatype"] = "double";
      jsonObj["precision"] = param->getPrecision();
      double d;
      std::string s;
      if (item->isValid() && item->getValue(d)){
        jsonObj["value"] = ch_semafor_intens::JsonUtils::getRealValueNotJson5(d);
        param->getFormattedValue( s );
        trim(s);
        jsonObj["formatted_value"] = s;
      }
      // range
      if (AppData::Instance().GuiWithRangeCheck()) {
        if (min != std::numeric_limits<double>::min() ) {
          jsonObj["min_value"] = min;
        }
        if (max != std::numeric_limits<double>::max() ) {
          jsonObj["max_value"] = max;
        }
      }

      // pattern
      if (!jsonObj.isMember("pattern_value")){
        std::string regexp("^[-+]?([0-9]*[\\.])?[0-9]+([eE][-+]?[0-9]+)?$"); // "^[-+]?\\([0-9]*[\\%1]\\)?[0-9]+\\([eE][-+]?\d+\\)?$"
                                   ///        std::string regexp(compose("^[-+]?\\([0-9]*[\\%1]\\)?[0-9]+\\([eE][-+]?\d+\\)?",
                                   //                                    "\\({0,1}[1-9]+\\%1{0,1}[1-9]*\\%2[1-9]+\\%1{0,1}[1-9]*\\){0,1}",
                           //                                  '.'/*radix_point*/));
        jsonObj["pattern_value"] = regexp;
      }
      break;
    }
    case DataDictionary::type_String:{
      // datatype
      switch(param->DataItem()->getUserAttr()->StringType()) {
      case UserAttr::string_kind_date:
        jsonObj["datatype"] = "string_date";
        break;
      case UserAttr::string_kind_time:
        jsonObj["datatype"] = "string_time";
        break;
      case UserAttr::string_kind_datetime:
        jsonObj["datatype"] = "string_datetime";
        break;
      case UserAttr::string_kind_value:
        jsonObj["datatype"] = "string_value";
        break;
      case UserAttr::string_kind_password:
        jsonObj["datatype"] = "string_password";
        break;
      default:
        jsonObj["datatype"] = "string";
      }
      // mimetype
      std::string s;
      if (item->isValid() && item->getValue(s)){
        jsonObj["value"] = s;
      }
      jsonObj["mimetype"] = FileUtilities::getDataMimeType(s);

      // special case pixmap, value could be a file name
      if (Type() == GuiElement::type_Pixmap && s.size() < 200) {
        IconManager::ICON_TYPE icon_type;
        std::string icon, file, content;
        if (IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
          jsonObj["value"] = content;
          jsonObj["filename"] = s;
          IconManager::IconTypeMap::iterator it = IconManager::Instance().getIconTypeMap().find(icon_type);
          jsonObj["mimetype"] = it != IconManager::Instance().getIconTypeMap().end() ?it->second.mimeType : "";
        }
      }

      // // pattern
      // if (param->DataItem()->getUserAttr()->StringType() != UserAttr::string_kind_date &&
      //     param->DataItem()->getUserAttr()->Pattern().size()) {
      //   jsonObj["pattern_value"] = param->DataItem()->getUserAttr()->Pattern();
      // }

      break;
    }
    case DataDictionary::type_CharData:{
      jsonObj["datatype"] = "cdata";
      std::string s;
      if (item->isValid() && item->getValue(s)){
        std::string sBase64;
        base64encode(reinterpret_cast<const unsigned char*>(s.c_str()),
                     s.size(), sBase64, false);
        jsonObj["mimetype"] = FileUtilities::getDataMimeType(s);
        if (jsonObj["mimetype"].asString().rfind("image/") == 0) {
          std::ostringstream os;
          os << "data:" << FileUtilities::getDataMimeType(s) << ";base64," << sBase64;
          jsonObj["value"] = os.str();
        } else  {
          jsonObj["value"] = sBase64;
        }
        BUG_DEBUG("CData, Mimetype["<<FileUtilities::getDataMimeType(s)<<"] size["<<sBase64.size()<<"]");
        BUG_INFO("CData, Mimetype["<<FileUtilities::getDataMimeType(s)<<"] size["<<sBase64.size()<<"] vn["<<param->DataItem()->getFullName(true)<<"] Id["<<jsonObj["id"].asString()<<"] ");
      }

      // special case pixmap, value could be a file name
      if (Type() == GuiElement::type_Pixmap && s.size() < 200) {
        IconManager::ICON_TYPE icon_type;
        std::string icon, file, content;
        if (IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
          jsonObj["value"] = content;
          jsonObj["filename"] = s;
          IconManager::IconTypeMap::iterator it = IconManager::Instance().getIconTypeMap().find(icon_type);
          jsonObj["mimetype"] = it != IconManager::Instance().getIconTypeMap().end() ?it->second.mimeType : "";
        }
      }
      break;
    }
    case DataDictionary::type_Complex:{
      jsonObj["datatype"] = "complex";
      dComplex dC;
      if (item->isValid() && param->DataItem()->getValue(dC)){
        std::ostringstream os;
        os << dC;
        jsonObj["value"] = os.str();
      }

      // pattern
      char real_img_delimeter = radix_point == ',' ? ';'  : ',';
      std::string regexp( compose("\\({0,1}[1-9]+\\%1{0,1}[1-9]*\\%2[1-9]+\\%1{0,1}[1-9]*\\){0,1}",
                                  radix_point, real_img_delimeter) );
      jsonObj["pattern_value"] = regexp;
      break;
    }
    default:
      break;
    }
  }

/* --------------------------------------------------------------------------- */
/* writeDataTypeJsonProperties() --                                            */
/* --------------------------------------------------------------------------- */
#if HAVE_PROTOBUF
in_proto::ValueInfo* GuiElement::writeDataTypeProtobufProperties(XferDataItem *item, XferDataParameter* param) {
  auto element = new in_proto::ValueInfo;
  double min = param->DataItem()->getUserAttr()->getMin();
  double max = param->DataItem()->getUserAttr()->getMax();
  char radix_point = RealConverter::decimalPoint();

  // pattern
  if (param->DataItem()->getUserAttr()->Pattern().size()) {
    element->set_pattern_value(param->DataItem()->getUserAttr()->Pattern());
  }

  element->set_string_value("");
  switch( item->getDataType() ){
  case DataDictionary::type_Integer:{
    element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_Integer);
    int i;
    std::string s;
    if (item->isValid() && item->getValue(i)){
      element->set_int_value(i);
      param->getFormattedValue( s );
      trim(s);
      element->set_formatted_value(s);
    }
    // range
    if (AppData::Instance().GuiWithRangeCheck()) {
      if (min != std::numeric_limits<double>::min() ) {
        element->set_min_value(floor(0.5+min));
      }
      if (max != std::numeric_limits<double>::max() ) {
        element->set_max_value(floor(0.5+max));
      }
    }

    // pattern
    if (element->pattern_value().length() == 0){
      element->set_pattern_value("^[+-]?[0-9]+([eE][+]?[0-9]+)?$");
    }
    break;
  }
  case DataDictionary::type_Real:{
    element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_Double);
    element->set_precision(param->getPrecision());
    double d;
    std::string s;
    if (item->isValid() && item->getValue(d)){
      element->set_double_value(d);
      param->getFormattedValue( s );
      trim(s);
      element->set_formatted_value(s);
    }
    // range
    if (AppData::Instance().GuiWithRangeCheck()) {
      if (min != std::numeric_limits<double>::min() ) {
        element->set_min_value(min);
      }
      if (max != std::numeric_limits<double>::max() ) {
        element->set_max_value(max);
      }
    }

    // pattern
    if (element->pattern_value().length() == 0){
      std::string regexp("^[-+]?([0-9]*[\\.])?[0-9]+([eE][-+]?[0-9]+)?$"); // "^[-+]?\\([0-9]*[\\%1]\\)?[0-9]+\\([eE][-+]?\d+\\)?$"
                                 ///        std::string regexp(compose("^[-+]?\\([0-9]*[\\%1]\\)?[0-9]+\\([eE][-+]?\d+\\)?",
                                 //                                    "\\({0,1}[1-9]+\\%1{0,1}[1-9]*\\%2[1-9]+\\%1{0,1}[1-9]*\\){0,1}",
                         //                                  '.'/*radix_point*/));
      element->set_pattern_value(regexp);
    }
    break;
  }
  case DataDictionary::type_String:{
    // datatype
    switch(param->DataItem()->getUserAttr()->StringType()) {
    case UserAttr::string_kind_date:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_StringDate);
      break;
    case UserAttr::string_kind_time:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_StringTime);
      break;
    case UserAttr::string_kind_datetime:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_StringDateTime);
      break;
    case UserAttr::string_kind_value:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_StringValue);
      break;
    case UserAttr::string_kind_password:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_StringPassword);
      break;
    default:
      element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_String);
    }
    // mimetype
    std::string s;
    if (item->isValid() && item->getValue(s)){
      element->set_string_value(s);
    }
    element->set_mimetype(FileUtilities::getDataMimeType(s));

    // special case pixmap, value could be a file name
    if (Type() == GuiElement::type_Pixmap && s.size() < 200) {
      IconManager::ICON_TYPE icon_type;
      std::string icon, file, content;
      if (IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
        element->set_string_value(content);
        element->set_filename(s);
        IconManager::IconTypeMap::iterator it = IconManager::Instance().getIconTypeMap().find(icon_type);
        element->set_mimetype(it != IconManager::Instance().getIconTypeMap().end() ?it->second.mimeType : "");
      }
    }

    break;
  }
  case DataDictionary::type_CharData:{
    element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_CDATA);
    std::string s;
    if (item->isValid() && item->getValue(s)){
      std::string sBase64;
      base64encode(reinterpret_cast<const unsigned char*>(s.c_str()),
                   s.size(), sBase64, false);
      element->set_mimetype(FileUtilities::getDataMimeType(s));
      if (element->mimetype().rfind("image/") == 0) {
        std::ostringstream os;
        os << "data:" << FileUtilities::getDataMimeType(s) << ";base64," << sBase64;
        element->set_string_value(os.str());
      } else  {
        element->set_string_value(sBase64);
      }
      BUG_DEBUG("CData, Mimetype["<<FileUtilities::getDataMimeType(s)<<"] size["<<sBase64.size()<<"]");
    }

    // special case pixmap, value could be a file name
    if (Type() == GuiElement::type_Pixmap && s.size() < 200) {
      IconManager::ICON_TYPE icon_type;
      std::string icon, file, content;
      if (IconManager::Instance().getDataUrlFileContent(icon_type, s, content)) {
        element->set_string_value(content);
        element->set_filename(s);
        IconManager::IconTypeMap::iterator it = IconManager::Instance().getIconTypeMap().find(icon_type);
        element->set_mimetype(it != IconManager::Instance().getIconTypeMap().end() ?it->second.mimeType : "");
      }
    }
    break;
  }
  case DataDictionary::type_Complex:{
    element->set_datatype(in_proto::ValueInfo::DataType::ValueInfo_DataType_Complex);
    dComplex dC;
    if (item->isValid() && param->DataItem()->getValue(dC)){
      std::ostringstream os;
      os << dC;
      element->set_string_value(os.str());
    }

    // pattern
    char real_img_delimeter = radix_point == ',' ? ';'  : ',';
    std::string regexp( compose("\\({0,1}[1-9]+\\%1{0,1}[1-9]*\\%2[1-9]+\\%1{0,1}[1-9]*\\){0,1}",
                                radix_point, real_img_delimeter) );
    element->set_pattern_value(regexp);
    break;
  }
  default:
    break;
  }
  return element;
}
#endif
/* --------------------------------------------------------------------------- */
/* setPosition --                                                              */
/* --------------------------------------------------------------------------- */

void GuiElement::setPosition( int x1, int x2, GuiElement::Alignment xAlign
                            , int y1, int y2, GuiElement::Alignment yAlign ){
  BUG_PARA(BugGui,"GuiElement::setPosition"
                 ,"x1=" << x1 << ", x2=" << x2 << ", y1=" << y1 << ", y2=" << y2);
//   if( myWidget() == 0 ){
//     BUG_EXIT("<no widget>");
//     return;
//   }
  int width  = 0;
  int height = 0;
  getSize( width, height );
  BUG_MSG("width = " << width << ", height = " << height);

  int x, y;

  if( x2 - x1 < width ){
    BUG_MSG("WARNING: no place for width " << width);
    x = x1; // kein Platz
  }
  else{
    switch( xAlign ){
    case GuiElement::align_Default:
    case GuiElement::align_Left:
      x = x1;
      break;
    case GuiElement::align_Right:
      x = x2 - width;
      break;
    case GuiElement::align_Center:
      x = x1 + ((x2 - width) - x1) / 2;
      break;
    case GuiElement::align_Stretch:
      x = x1;
      stretchWidth( x2 - x1 );
      break;
    default:
      BUG_MSG("WARNING: bad x alignment");
      x = x1;
      break;
    }
  }

  if( y2 - y1 < height ){
    BUG_MSG("WARNING: no place for height " << height);
    y = y1; // kein Platz
  }
  else{
    switch( yAlign ){
    case GuiElement::align_Default:
    case GuiElement::align_Top:
      y = y1;
      break;
    case GuiElement::align_Bottom:
      y = y2 - height;
      break;
    case GuiElement::align_Center:
      y = y1 + ((y2 - height) - y1) / 2;
      break;
    default:
      BUG_MSG("WARNING: bad y alignment");
      y = y1;
      break;
    }
  }

  BUG_MSG("x = " << x << ", y = " << y);
  setPosition( x, y );
}


/* --------------------------------------------------------------------------- */
/* withScrollbars --                                                           */
/* --------------------------------------------------------------------------- */

bool GuiElement::withScrollbars() const{
  if( m_parent != 0 ){
    return m_parent->withScrollbars();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* withPanedWindow --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiElement::withPanedWindow(){
  if( m_parent != 0 ){
    return m_parent->withPanedWindow();
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* withFrame --                                                                */
/* --------------------------------------------------------------------------- */

bool GuiElement::withFrame(){
  if( isGuiStyle_30() ){
    if( m_parent != 0 ){
      return m_parent->withFrame();
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* myParent --                                                                 */
/* --------------------------------------------------------------------------- */

GuiElement *GuiElement::myParent( ElementType type ){
  if( type == type_Form ){
    // Bei der Frage nach der Form liefern wir auch die MainForm.
    if( Type() == type_Main ){
      return this;
    }
  }
  if( type == Type() ){
    return this;
  }
  if( m_parent == 0 ){
    return 0;
  }
  return m_parent->myParent( type );
}

/* --------------------------------------------------------------------------- */
/* myParentNotType --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement *GuiElement::myParentNotType( ElementType type ){
  if( type != Type() ){
    return this;
  }
  if( m_parent == 0 ) return 0;
  return m_parent->myParentNotType( type );
}
/* --------------------------------------------------------------------------- */
/* updateForms --                                                              */
/* --------------------------------------------------------------------------- */

void GuiElement::updateForms( UpdateReason reason ){
  if( m_parent == 0 ){
    update( reason );
  }
  else{
    m_parent->updateForms( reason );
  }
}

/* --------------------------------------------------------------------------- */
/* updateNamedElements --                                                      */
/* --------------------------------------------------------------------------- */

void GuiElement::updateNamedElements( UpdateReason reason) {
  GuiSortedElementList::const_iterator el;
  for( el = s_namedElementList.begin(); el != s_namedElementList.end(); ++el ){
    (*el).second->update( reason );
  }
}

/* --------------------------------------------------------------------------- */
/* manageForm --                                                               */
/* --------------------------------------------------------------------------- */

void GuiElement::manageForm(){
  if( m_parent != 0 ){
    m_parent->manageForm();
  }
}

/* --------------------------------------------------------------------------- */
/* printMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiElement::printMessage( const std::string &msg, MessageType type, time_t delay ){
  if( m_parent != 0 ){
    BUG_DEBUG("printMessage: " << compose("type[%1], message[%2]", type, msg));
    m_parent->printMessage( msg, type, delay );
  }
}

void GuiElement::printMessage( GuiElement *elem, const std::string &msg, MessageType type, time_t delay ){
  BUG_DEBUG("printMessage: " << compose("type[%1], message[%2]", type, msg));
  if( elem ){
    elem->printMessage( msg, type, delay );
  }
  else{
    GuiForm *form=getMainForm();
    if( !form ){
      return;
    }
    form->getElement()->printMessage( msg, type, delay );
  }
}

/* --------------------------------------------------------------------------- */
/* clearMessage --                                                             */
/* --------------------------------------------------------------------------- */

void GuiElement::clearMessage(){
  if( m_parent != 0 ){
    m_parent->clearMessage();
  }
}

/* --------------------------------------------------------------------------- */
/* setHelptext --                                                              */
/* --------------------------------------------------------------------------- */

void GuiElement::setHelptext( const std::string &text ){
  m_helptext = text;
}

/* --------------------------------------------------------------------------- */
/* getHelptext --                                                              */
/* --------------------------------------------------------------------------- */

void GuiElement::getHelptext( std::string &text ){
  if (m_helptextStream) {
    std::ostringstream ostr;
    m_helptextStream->write( ostr );
    text = ostr.str();
  } else {
    text = m_helptext;
  }
}

/* --------------------------------------------------------------------------- */
/* hasHelptext --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiElement::hasHelptext(){
  return m_helptextStream ? true : !m_helptext.empty();
}

/* --------------------------------------------------------------------------- */
/* showHelptext --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiElement::showHelptext(){
  return AppData::Instance().Helpmessages() != AppData::NoneType;
}

/* --------------------------------------------------------------------------- */
/* setHelptextStream --                                                        */
/* --------------------------------------------------------------------------- */
bool GuiElement::setHelptextStream( const std::string &stream ) {
  Stream *s = StreamManager::Instance().getStream(stream );
  if( s != 0 ) {
    m_helptextStream = s;
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* handleTableAction --                                                        */
/* --------------------------------------------------------------------------- */

bool GuiElement::handleTableAction( GuiTableActionFunctor &func ){
  if( m_parent != 0 ){
    return m_parent->handleTableAction( func );
  }
  return false; // failed
}

/* --------------------------------------------------------------------------- */
/* isGuiStyle_30 --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiElement::isGuiStyle_30() {
  return s_gui_style == style_30;
}

/* --------------------------------------------------------------------------- */
/* isGuiStyle_31 --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiElement::isGuiStyle_31() {
  return s_gui_style == style_31;
}

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/


/* --------------------------------------------------------------------------- */
/* searchNamedElement --                                                       */
/* --------------------------------------------------------------------------- */

GuiElement *GuiElement::searchNamedElement( GuiNamedElementList &elist, const std::string &name ){
  GuiNamedElementList::iterator el;
  for( el = elist.begin(); el != elist.end(); ++el ){
    if( (*el).first == name ){
      return (*el).second;
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setName --                                                                   */
/* --------------------------------------------------------------------------- */

// void GuiElement::setName( const std::string &name ) {
//   if (name.size()) {
//     GuiSortedElementList::const_iterator el =  s_namedElementList.find(name);
//     assert( el == s_namedElementList.end());
//     s_namedElementList.insert( GuiNamedElementList::value_type(name, this) );
//   }
//   m_name = name;
// }
/* --------------------------------------------------------------------------- */
/* findElement --                                                              */
/* --------------------------------------------------------------------------- */

GuiElement *GuiElement::findElement( const std::string &name ){
  GuiSortedElementList::iterator el =  s_namedElementList.find(name);
  if( el !=  s_namedElementList.end() ){
    return (*el).second;
  }
  return 0;
}

void GuiElement::lspWrite( std::ostream &ostr ){
  std::regex regex("[a-zA-Z][a-zA-Z_0-9]*");
  for (auto pair : s_namedElementList) {
    auto name = pair.first;
    if (!std::regex_match(name, regex)){
      continue;
    }
    auto ele = pair.second;
    ostr << "<ITEM name=\"" << name << "\"";
    ostr << " uiele=\"1\"";
    ostr << " file=\"" << ele->LSPFilename() << "\"";
    ostr << " line=\"" << ele->LSPLineno() << "\"";
    ostr << ">" << std::endl;
    ostr << "</ITEM>" << std::endl;
  }
}

/* --------------------------------------------------------------------------- */
/* findElementId --                                                            */
/* --------------------------------------------------------------------------- */

GuiElement *GuiElement::findElementId(const std::string &id){
  int num_id = -1;
  try {
    num_id = std::stoi(id);
  } catch (const std::exception& e) {
    // string wasn't a number
  }
  GuiElementList::const_iterator el;
  for( el = s_elementList.begin(); el != s_elementList.end(); ++el ){
    if (num_id != -1 && (*el)->getElementIntId() == num_id)
      return (*el);
    if ((*el)->getElementId() == id)
      return (*el);
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* findElementType --                                                          */
/* --------------------------------------------------------------------------- */

void GuiElement::findElementType( GuiElementList& res, ElementType type ) {
//   std::cout << "GuiElement::findElementType()\n";
  GuiSortedElementList::const_iterator el;
  for( el = s_namedElementList.begin(); el != s_namedElementList.end(); ++el ){
//     std::cout << " + Type():["<<(*el).second->Type()<<"]\n";
    if( (*el).second->Type() == type ){
      res.push_back( (*el).second );
    }
  }
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiElement::serializeJson(Json::Value& jsonObj, bool onlyUpdated){
  jsonObj["info"] = "Not implemented yet, gui type: " + StringType();
  return false;
}
#if HAVE_PROTOBUF
bool GuiElement::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  BUG_ERROR("SERIALIZING NOT IMPLEMENTED FOR " << StringType() << typeid(this).name());
  return false;
}
#endif

/* --------------------------------------------------------------------------- */
/* findElementType --                                                          */
/* --------------------------------------------------------------------------- */

bool GuiElement::isVisibleElementType(ElementType type) {
  switch (type) {
  case type_Index:
  case type_Text:
  case type_Textfield:
  case type_ComboBox:
  case type_RadioButton:
  case type_Toggle:
  case type_Button:
  case type_CycleButton:
  case type_FieldButton:
  case type_Pixmap:
  case type_ScrolledText:
  case type_Scrolledlist:
  case type_Fieldgroup:
  case type_Table:
  case type_List:
  case type_Navigator:
  case type_NavDiagram:
  case type_NavIconView:
  case type_Simpel:
  case type_ListPlot:
  case type_QWTPlot:
  case type_3dPlot:
  case type_Slider:
  case type_Image:
  case type_Thermo:
  case type_IndexMenu:
  case type_MenuButton:
  case type_Menubar:
      return true;
  default:
    return false;
  }
}

/* --------------------------------------------------------------------------- */
/* getMainForm --                                                              */
/* --------------------------------------------------------------------------- */

GuiForm* GuiElement::getMainForm(){
  GuiElementList el;
  GuiElement::findElementType(el, GuiElement::type_Main);
  assert (el.size() <= 1); // zu einem fruehen Zeitpunkt kann das 0 sein.
  if (el.size() == 1) return el[0]->getForm();
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getForm --                                                                  */
/* --------------------------------------------------------------------------- */

GuiForm* GuiElement::getForm( const std::string &name ){
  GuiElement *element = GuiElement::findElement( name );
  if( element != 0 ){
    if( element->Type() == GuiElement::type_Form ){
      return element->getForm();
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* getScrolledText --                                                          */
/* --------------------------------------------------------------------------- */

GuiScrolledText* GuiElement::getScrolledText( const std::string &name ){
  GuiElement *element = GuiElement::findElement( name );
  if( element != 0 ){
    if( element->Type() == GuiElement::type_ScrolledText ){
      return dynamic_cast<GuiScrolledText*>(element);
    }
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* setShownWebApi --                                                           */
/* --------------------------------------------------------------------------- */
void GuiElement::setShownWebApi(bool yes) {
  m_isShownWebApi = yes;
}

/* --------------------------------------------------------------------------- */
/* setShownWebApi --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiElement::isShownWebApi() {
  return m_isShownWebApi;
}

/* --------------------------------------------------------------------------- */
/* setAlignment --                                                             */
/* --------------------------------------------------------------------------- */

void GuiElement::setAlignment( GuiElement::Alignment align ){
  m_alignment = align;
}

/* --------------------------------------------------------------------------- */
/* getAlignment --                                                             */
/* --------------------------------------------------------------------------- */

GuiElement::Alignment GuiElement::getAlignment(){
  return m_alignment;
}

/* --------------------------------------------------------------------------- */
/* setRotation --                                                             */
/* --------------------------------------------------------------------------- */

void GuiElement::setRotation( int rotation ){
  m_rotation = rotation;
}

/* --------------------------------------------------------------------------- */
/* getRotation --                                                             */
/* --------------------------------------------------------------------------- */

int GuiElement::getRotation(){
  return m_rotation;
}

/* --------------------------------------------------------------------------- */
/* setStylesheet --                                                            */
/* --------------------------------------------------------------------------- */

void GuiElement::setStylesheet(const std::string& stylesheet){
  m_stylesheet = stylesheet;
  update( reason_Always );
}

/* --------------------------------------------------------------------------- */
/* getStylesheet --                                                            */
/* --------------------------------------------------------------------------- */

const std::string& GuiElement::getStylesheet(){
  return m_stylesheet;
}

/* --------------------------------------------------------------------------- */
/* getParentForm --                                                            */
/* --------------------------------------------------------------------------- */

GuiForm *GuiElement::getMyForm(){
  if( Type() == type_Form || Type() == type_Main ){
    assert( getForm() != 0 );
    return getForm();
  }
  else if( m_parent != 0 ){
    return m_parent->getMyForm();
  }
  return 0;
}

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */

std::string GuiElement::variantMethod(const std::string& method,
                                      const Json::Value& jsonArgs,
                                      JobEngine *eng){
  return defaultVariantMethod(method, jsonArgs, eng);
}

/* --------------------------------------------------------------------------- */
/* defaultVariantMethod --                                                     */
/* --------------------------------------------------------------------------- */

std::string GuiElement::defaultVariantMethod(const std::string& method,
                                             const Json::Value& jsonArgs,
                                             JobEngine *eng){
  Json::Value jsonElem = Json::Value(Json::objectValue);
  jsonElem["status"] = "Unknown";
  if (lower(method) == "lastguiupdated") {
    jsonElem["transactionNumber"] = (int) LastGuiUpdated();
  }
  if (!jsonElem.isNull()) {
    BUG_INFO("defaultVariantMethod[" << method << "], Return: "
             << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return std::string("");
}

/* --------------------------------------------------------------------------- */
/* setLastWebUpdated --                                                        */
/* --------------------------------------------------------------------------- */

void GuiElement::setLastWebUpdated(){
  m_lastWebUpdated = DataPoolIntens::Instance().CurrentTransaction();
}

//=============================================================================//
// newPopupMenu                                                                //
//=============================================================================//
GuiPopupMenu* GuiElement::newPopupMenu(){
  if( m_popupMenu )
    delete m_popupMenu;
  m_popupMenu = GuiFactory::Instance()->createPopupMenu(this);
  m_popupMenu->setTearOff( true );
  return m_popupMenu;
}

/* --------------------------------------------------------------------------- */
/* printSizeInfo --                                                            */
/* --------------------------------------------------------------------------- */
void GuiElement::printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds) {
  if (getName().size() == 0) return;
  while(--intent) os << "  ";
  os << "Name[" << getName() << "] Type[" << StringType() << "]\n";
}

/* --------------------------------------------------------------------------- */
/* acquireFormOrderNumber (WebApi) --                                          */
/* --------------------------------------------------------------------------- */
void GuiElement::acquireFormOrderNumber() {
  if (isShown() && Type() != GuiElement::type_Main) {
    s_topFormOrder = m_formZOrder;
    s_topForm = getForm();
    //std::cout << getName() << ":: RETURN aquireFormOrderNumber: lastFormOrder: " << s_topFormOrder << ", myFormOrder: " <<  m_formZOrder << ", topFormName: " << s_topForm << std::endl;
    return;
  }

  m_formZOrder = ++s_maxFormOrder;
  s_topFormOrder = m_formZOrder;
  m_openedByForm = s_topForm;
  s_topForm = getForm();
  BUG_INFO("Name: " << getName() << ":: aquireFormOrderNumber: lastFormOrder: " << s_topFormOrder << ", myFormOrder: " <<  m_formZOrder << ", topFormName: " << s_topForm);
}

/* --------------------------------------------------------------------------- */
/* releaseFormOrderNumber (WebApi) --                                          */
/* --------------------------------------------------------------------------- */
void GuiElement::releaseFormOrderNumber() {
  int max(0);
  GuiElementList mlist, m_formList;
  GuiElement::findElementType(mlist, GuiElement::type_Main);
  GuiElement::findElementType(m_formList, GuiElement::type_Form);
  m_formList.insert(m_formList.end(), mlist.begin(), mlist.end());
  for(GuiElementList::iterator it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if (((*it)->getForm()->isShown() && (*it) != this) &&
        max < (*it)->getFormOrder())
      max = (*it)->getFormOrder();
  }
  s_maxFormOrder = max;
  s_topFormOrder = m_openedByForm ? m_openedByForm->getElement()->getFormOrder() : 0;

  // get new top form
  for(GuiElementList::iterator it = m_formList.begin(); it != m_formList.end(); ++it ) {
    if (((*it)->getForm()->isShown() && (*it) != this) &&
        s_topFormOrder == (*it)->getFormOrder())
      s_topForm = (*it)->getForm();
  }

  BUG_INFO("Name: " << getName() << ":: releaseFormOrderNumber: maxFormOrder: " << max << ", topFormOrder: " << s_topFormOrder);
}

/* --------------------------------------------------------------------------- */
/* getTopFormName (WebApi) --                                                  */
/* --------------------------------------------------------------------------- */
std::string GuiElement::getTopFormName() {
  if (s_topForm && s_topForm->getElement()) {
    BUG_INFO("getTopFormName: " << s_topForm->getElement()->getName());
    return s_topForm->getElement()->getName();
  }
  return "";
}

/* --------------------------------------------------------------------------- */
/* setTopForm (WebApi) --                                                      */
/* --------------------------------------------------------------------------- */
void GuiElement::setTopForm(GuiForm* form) {
  s_topForm = form;
  s_topFormOrder = form->getElement()->getFormOrder();
  BUG_INFO("setTopForm: " << form->getElement()->getName() << ", zOrder: " << s_topFormOrder);
  //std::cout << "setTopForm: " << form->getElement()->getName() << ", zOrder: " << s_topFormOrder<<"\n";
}

/* --------------------------------------------------------------------------- */
/* updateScale --                                                              */
/* --------------------------------------------------------------------------- */
bool GuiElement::updateScale(XferDataParameter* param) {
  if (!param || !param->DataItem())
    return false;
  DataDictionary::DataType dataType(param->DataItem()->getDataType());
  if (dataType != DataDictionary::type_Real &&
      dataType != DataDictionary::type_Integer){
    return false;
  }

  double factorOld = param->getScalefactor() ? param->getScalefactor()->getValue() : 1.;
  double shiftOld = param->getScalefactor() ? param->getScalefactor()->getShift() : 0;
  UnitManager::Unit* unit = UnitManager::Instance().getUnitData(param->DataItem()->getUserAttr()->Unit(false));
  double factor = unit ? unit->factor : 1.;
  double shift = unit ? unit->shift : 0;
  if (factor == factorOld && shift == shiftOld)
    return false;
  bool use_divide = unit ? unit->use_divide : false;
  param->setScalefactor(new Scale(factor, (use_divide ? '/' : '*'), shift));
  return true;
}
