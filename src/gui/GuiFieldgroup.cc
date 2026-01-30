
#include "utils/HTMLConverter.h"

#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiElement.h"
#include "gui/GuiIndex.h"
#include "gui/GuiVoid.h"
#include "gui/GuiStretch.h"
#include "gui/GuiIndexMenu.h"
#include "gui/GuiFactory.h"
#include "job/JobManager.h"
#include "utils/StringUtils.h"

GuiFieldgroup::FieldgroupMap GuiFieldgroup::s_fieldgroupmap;

INIT_LOGGER();

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

GuiFieldgroup::GuiFieldgroup( GuiElement *parent, const std::string &name )
  : m_function( 0 )
  , m_indexMenu( true )
  , m_indexLabel( true )
  , m_index_alignment( GuiElement::align_Center )
  , m_navigation( GuiElement::orient_Horizontal )
  , m_orientation( GuiElement::orient_Horizontal )
  , m_tableaction_func( 0 )
  , m_index( 0 )
  , m_container( parent, GuiElement::type_Fieldgroup )
  , m_tablesize( 0 )
  , m_tablestep( -1 )
  , m_tablemin( 0 )
  , m_tablemax( -1 )
  , m_tableposition( 1 )
  , m_use_frame( false )
  , m_name( name )
  , m_margin(2)
  , m_spacing(2)
  , m_align_fields( false )
  , m_title_alignment( GuiElement::align_Default )
  , m_running_key( 0 )
  , m_accordion(false)
  , m_accordion_open(false)
  , m_webapi_publish(false)
{
  s_fieldgroupmap.insert( FieldgroupMap::value_type( name, this ) );
}

GuiFieldgroup::GuiFieldgroup( const GuiFieldgroup &fg )
  : m_function(fg.m_function )
  , m_indexMenu( fg.m_indexMenu )
  , m_indexLabel( fg.m_indexLabel )
  , m_index_alignment( fg.m_index_alignment )
  , m_navigation( fg.m_navigation )
  , m_orientation( fg.m_orientation )
  , m_tableaction_func( 0 )
  , m_index( fg.m_index )
  , m_tablesize( fg.m_tablesize )
  , m_tablestep( fg.m_tablestep )
  , m_tablemin( fg.m_tablemin )
  , m_tablemax( fg.m_tablemax )
  , m_tableposition( fg.m_tableposition )
  , m_use_frame( fg.m_use_frame )
  , m_margin(fg.m_margin)
  , m_spacing(fg.m_spacing)
  , m_align_fields(fg.m_align_fields)
  , m_title_alignment( fg.m_title_alignment )
  , m_running_key( 0 )
  , m_container( fg.m_container )
  , m_title( fg.m_title  )
  , m_accordion(fg.m_accordion)
  , m_accordion_open(fg.m_accordion_open)
  , m_webapi_publish(fg.m_webapi_publish)
{
}

GuiFieldgroup::~GuiFieldgroup(){
  if( m_index != 0 ){
    GuiElementList::iterator I;
    for( I=m_container.begin(); I != m_container.end(); ++I ){
      ///      GuiQtFieldgroupLine *fl = static_cast<GuiFieldgroupLine*>(*I);
      m_index->unregisterIndexedElement( (*I) );
     }
  }
}

/* --------------------------------------------------------------------------- */
/* setFunction --                                                              */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::setFunction( JobFunction *func ) {
  assert( func != 0 );
  m_function = func;
  m_function->setUsed();
}

/* --------------------------------------------------------------------------- */
/* hideIndexMenu --                                                            */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::hideIndexMenu(){
  m_indexMenu = false;
}

/* --------------------------------------------------------------------------- */
/* hideIndexLabel --                                                           */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::hideIndexLabel(){
  m_indexLabel = false;
}

/* --------------------------------------------------------------------------- */
/* setIndexAlignment --                                                        */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::setIndexAlignment( GuiElement::Alignment align ){
  m_index_alignment = align;
}
/* --------------------------------------------------------------------------- */
/* getFieldgroup --                                                            */
/* --------------------------------------------------------------------------- */

GuiFieldgroup * GuiFieldgroup::getFieldgroup( const std::string &id ){
  FieldgroupMap::iterator I=s_fieldgroupmap.find( id );
  if( I == s_fieldgroupmap.end() )
    return 0; // not found
  return (*I).second;
}

/* --------------------------------------------------------------------------- */
/* streamableObject --                                                         */
/* --------------------------------------------------------------------------- */
BasicStream* GuiFieldgroup::streamableObject() {
  // search for first streamable child
  BasicStream* retStream=0;
  for( GuiElementList::iterator it = m_container.begin();
       it != m_container.end(); ++it ){
    BasicStream* stream = (*it)->streamableObject();
    if (stream) {
      if (!retStream)
        retStream = stream;
      else {
        std::cerr << "Mulitiple streamable GuiElements found in '"
                  <<  getElement()->getName() << "', will use first.\n";
      }
    }
  }
  return retStream;
}

/* --------------------------------------------------------------------------- */
/* addFieldgroupLine --                                                        */
/* --------------------------------------------------------------------------- */

GuiFieldgroupLine *GuiFieldgroup::addFieldgroupLine(){
  if(m_tablesize > 0 && m_tablestep > 0 && !m_container.size()){
    createArrowbar();
  }
  GuiFieldgroupLine *fl = GuiFactory::Instance()->createFieldgroupLine(this->getElement());
  ///new GuiQtFieldgroupLine( this );
  m_container.push_back(fl->getElement());
  if( m_index != 0 ){
    m_index->registerIndexedElement(fl->getElement());
  }
  return fl;
}

/* --------------------------------------------------------------------------- */
/* addArrowbar --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::addArrowbar(){
  if( m_tablesize == 0 || m_tablestep == 0 ){
    return false;
  }
  int lines = (int)m_container.size();
  if( m_index != 0 ){
    lines--;
  }
  if( lines == 0 ){
    // Die erste Arrowbar wird automatisch angelegt. Dieser Aufruf bleibt
    // ohne Wirkung, wird jedoch akzeptiert.
    return true;
  }
  createArrowbar();
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTitle --                                                                 */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::setTitle( const std::string &title ){
  if( title.empty() ){
    return;
  }
  m_title = title;
}

/* --------------------------------------------------------------------------- */
/* setTableSize --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setTableSize( int tablesize ){
  m_tablesize = tablesize;
  if( m_tablestep < 0 ){
    m_tablestep = m_tablesize;
  }
  return (tablesize > 0 && tablesize <= 20);
}

/* --------------------------------------------------------------------------- */
/* setTableStep --                                                             */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setTableStep( int tablestep ){
  if( tablestep < 0 ){
    return true; // use default step
  }
  m_tablestep = tablestep;
  if( tablestep == 0 ){
    return true;
  }
  if( tablestep > m_tablesize ){
    return false;
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setTableIndexRange --                                                       */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setTableIndexRange( int minIndex, int maxIndex ){
  int cntLines(maxIndex - minIndex + 1);
  if( maxIndex > 0 ) {
    maxIndex -= (m_tablesize - 1);
    if( maxIndex < minIndex ){
      maxIndex = minIndex;
    }
    getElement()->setAttributeChangedFlag(m_tablemax == maxIndex);
    m_tablemax = maxIndex;
  } else {
    m_tablemax = -1;
  }
  getElement()->setAttributeChangedFlag(m_tablemin == minIndex);
  m_tablemin = minIndex;

  GuiFieldgroupLine *line = 0;
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    assert( (*it)->Type() == GuiElement::type_FieldgroupLine );
    line = static_cast<GuiFieldgroupLine *>((*it)->getFieldgroupLine());
    assert(line);
    if( line->isArrowbar() ){
      line->setTableIndexRange( minIndex, maxIndex );
    }
    line->setTableSizeVisibleLines( std::min(cntLines, m_tablesize) );
  }

  // clones
  std::vector<GuiElement*> cloneList;
  getElement()->getCloneList(cloneList);
  for (std::vector<GuiElement*>::iterator it =  cloneList.begin();
       it != cloneList.end(); ++it) {
    (*it)->getFieldgroup()->setTableIndexRange(minIndex, maxIndex);
  }

  return true;
}

/* --------------------------------------------------------------------------- */
/* getTableSize --                                                             */
/* --------------------------------------------------------------------------- */

int GuiFieldgroup::getTableSize(){
  return m_tablesize;
}

/* --------------------------------------------------------------------------- */
/* setTablePosition --                                                         */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setTablePosition( int pos ){
  m_tableposition = pos;
  return (pos > 0);
}

/* --------------------------------------------------------------------------- */
/* getTablePosition --                                                         */
/* --------------------------------------------------------------------------- */

int GuiFieldgroup::getTablePosition(){
   return m_tableposition;
}

/* --------------------------------------------------------------------------- */
/* setOrientation --                                                           */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::setOrientation( GuiElement::Orientation orient ){
  m_orientation = orient;
  if( m_index != 0 ){
    m_index->setOrientation( m_orientation );
  }
  m_navigation = orient;
}

/* --------------------------------------------------------------------------- */
/* setMargins --                                                               */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setMargins( int margin, int spacing) {
  if (margin < 100)
    m_margin = margin;
  if (spacing >= 0 &&  spacing < 100)
    m_spacing = spacing;
  return true;
}

/* --------------------------------------------------------------------------- */
/* setGuiIndex --                                                              */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::setGuiIndex(GuiIndex* index) {
  m_index = index;
  return true;
}

/* --------------------------------------------------------------------------- */
/* createArrowbar --                                                           */
/* --------------------------------------------------------------------------- */

void GuiFieldgroup::createArrowbar(){
  assert( m_tablesize > 0 );
  assert( !m_name.empty() );

  GuiFieldgroupLine *arrowbar = GuiFactory::Instance()->createFieldgroupLine(this->getElement());
  arrowbar->setArrowbar();

  m_container.push_back( arrowbar->getElement() );
  std::string name = m_name + "@fg_index@";
  if( m_index == 0 ){
    m_index = GuiFactory::Instance()->createGuiIndex(this->getElement(), name);
  }
  else{
    if (m_index->cloneable()) {
      m_index = m_index->clone()->getGuiIndex();
      m_index->getElement()->setParent(getElement());
    }
    else assert(false);
  }
  m_index->getElement()->setColSpan(m_tableposition); // for vertical and horizontal!
  m_index->setName( "@fg_index@" );
  m_index->setOrientation( m_orientation );
  m_index->setMinIndex( m_tablemin );
  if( m_tablemax >= 0 ){
    m_index->setMaxIndex( m_tablemax );
  }
  m_index->setStep( m_tablestep );
  if (m_navigation == GuiElement::orient_Vertical)
    m_index->setPlusMinusStyle();

  arrowbar->attach(m_index->getElement());

  // amg Indizes gibs jetzt nur noch wenn auch das Indexpopupmenu gewuenscht ist
  if (!m_indexLabel)
    return;

  GuiIndexMenu *menu;
  for( int ii=0; ii<m_tablesize; ii++ ){
    GuiIndexMenu::HideFlag hideFlag = (m_indexMenu && m_indexLabel) ? GuiIndexMenu::HideNone
      : m_indexLabel ? GuiIndexMenu::HideMenu : GuiIndexMenu::HideLabel;
    menu = GuiFactory::Instance()->createIndexMenu(this->getElement(), hideFlag);
    menu->setOffset( ii );
    menu->setMinIndex( m_tablemin );
    menu->getElement()->setAlignment( m_index_alignment );
    menu->setOrientation( m_orientation );
    m_index->registerIndexedElement( menu->getElement() );
    arrowbar->attach( menu->getElement() );
  }
}

/* --------------------------------------------------------------------------- */
/* setRunningMode --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiFieldgroup::setRunningMode(){
  m_running_key = JobManager::Instance().setRunningMode( m_running_key );
  return m_running_key != 0;
}

/* --------------------------------------------------------------------------- */
/* unsetRunningMode --                                                         */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::unsetRunningMode(){
  if( m_running_key != 0 ){
    JobManager::Instance().unsetRunningMode( m_running_key );
    m_running_key = 0;
  }
}

/* --------------------------------------------------------------------------- */
/* handleTableAction --                                                        */
bool GuiFieldgroup::handleTableAction( GuiTableActionFunctor &func ){
  BUG(BugGui,"GuiFieldgroup::handleTableAction");

  // Zu diesem Zeitpunkt darf keine Funktion laufen, sonst haben wir
  // ein grosses Problem.
  if( !setRunningMode() ){ assert( false ); }

  m_tableaction_list.clear();
  m_tableaction_func = func.clone();
  assert( m_tableaction_func != 0 );

  GuiManager::Instance().initErrorBox();

  bool ok = true;
  GuiElementList::iterator it;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    if( (*it)->Type() == GuiElement::type_FieldgroupLine ){
      GuiFieldgroupLine *line = (*it)->getFieldgroupLine();
      GuiDataField *f = line->getActionDataField();
      if( f != 0 ){
       // etwas kompliziert, aber wir stellen sicher, dass bei doppelten Eintraegen
       // nur einmal die Aktion ausgef�hrt wird
       //
       m_tableaction_iter = m_tableaction_list.begin();
       for(;m_tableaction_iter != m_tableaction_list.end();
           ++m_tableaction_iter ){
         if ( (*((*m_tableaction_iter)->Data()) == *(f->Data())) &&
                  (*m_tableaction_iter)->Data()->fullName() == f->Data()->fullName()) {
           break;
         }
       }
       if (m_tableaction_iter == m_tableaction_list.end())
         m_tableaction_list.push_back( f );
      }
    }
  }

  // Zuerst werden alle Datapool-Aktionen durchgef�hrt. Erst nach diesen
  // Arbeiten werden die vorhandenen Functions aufgerufen.
  for( m_tableaction_iter = m_tableaction_list.begin();
       m_tableaction_iter != m_tableaction_list.end();
       ++m_tableaction_iter ){
    (*m_tableaction_func)( *(*m_tableaction_iter) );
  }

  // Nun beginnt der letzte Teil: die Functions. Bei jeder Function
  // verlassen wir das sichere Terain und geben die Kontrolle ab.
  GuiManager::Instance().initErrorBox();

  m_tableaction_iter = m_tableaction_list.begin();
  nextTableAction( false );
  return ok;
}

/* --------------------------------------------------------------------------- */
/* nextTableAction --                                                          */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::nextTableAction( bool error ){
//   assert(false);
  if( error ){
    finalTableAction( false );
    return;
  }

  if( m_tableaction_iter == m_tableaction_list.end() ){
    finalTableAction( true );
    return;
  }
  GuiDataField *field = (*m_tableaction_iter);
  m_tableaction_iter++;

  JobFunction *func = field->getFunction();
  if( func == 0 ){
    nextTableAction( false );
    return;
  }

  // Nun wird der Trigger (JobStarter) initialisiert.
  Trigger *trigger = new Trigger( this, func );
  trigger->setReason( m_tableaction_func->getReason() );
  trigger->setIndex( m_tableaction_func->getIndex() );

  trigger->startJob();
  // Hier geben wir die Kontrolle ab. Nach Beendigung der Function gehts mit
  // nextTableAction() weiter. Der Aufruf erfolgt durch den Trigger. Der Trigger
  // l�scht sich am Ende selbst�ndig.
}

/* --------------------------------------------------------------------------- */
/* finalTableAction --                                                         */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::finalTableAction( bool ok ){
  // Die Functor-Kopie muss wieder gel�scht werden.
  delete m_tableaction_func;
  m_tableaction_func = 0;
  m_tableaction_list.clear();

  if( ok ){
    getElement()->updateForms(GuiElement::reason_FieldInput);
    // nicht QCursor&Qt::waitCursor ...
    getElement()->waitCursor( false, this );
  }
  else{
    getElement()->updateForms(GuiElement::reason_Cancel);
    GuiManager::Instance().showErrorBox( this->getElement() );
  }
  unsetRunningMode();
}

/* --------------------------------------------------------------------------- */
/* backFromJobStarter --                                                       */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::Trigger::backFromJobStarter( JobAction::JobResult rslt  ){
  m_fieldgroup->nextTableAction( rslt != JobAction::job_Ok );
}


/* --------------------------------------------------------------------------- */
/* serializeXML --                                                             */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::serializeXML( std::ostream &os, bool recursive ){
  os << "<intens:Fieldgroup";
  std::string s( Name() );
  HTMLConverter::convert2HTML( s );
  os << " name=\"" << s <<"\"";
  s = getTitle();
  HTMLConverter::convert2HTML( s );
  os << " label=\"" << s <<"\"";

  os << " orientation=";
  if( m_orientation == GuiElement::orient_Vertical )
    os << "\"vertical\"";
  else
    os << "\"horizontal\"";

  os << " frame=";
  if( m_use_frame )
    os << "\"true\"";
  else
    os << "\"false\"";

  os << ">" << std::endl;
  serializeContainerElements( os );

  os << "</intens:Fieldgroup>"<<std::endl;
}

/* --------------------------------------------------------------------------- */
/* serializeJson --                                                            */
/* --------------------------------------------------------------------------- */

bool GuiFieldgroup::serializeJson(Json::Value& jsonObj, bool onlyUpdated) {
  if (onlyUpdated && !getElement()->getAttributeChangedFlag()) {
    return false;
  }
  std::string s( Name() );
  getElement()->writeJsonProperties(jsonObj);
  HTMLConverter::convert2HTML( s );
  jsonObj["name"] = s;
  s = getTitle();
  HTMLConverter::convert2HTML( s );
  jsonObj["label"] = s;
  jsonObj["label_alignment"] = GuiElement::StringAlignmentType(m_title_alignment);
  jsonObj["orientation"] = m_orientation == GuiElement::orient_Vertical ? "vertical" : "horizontal";
  jsonObj["frame"] = m_use_frame;
  jsonObj["scrollbars"] = getElement()->withScrollbars();
  // overlay geometry
  Json::Value jsonSObj= Json::Value(Json::objectValue);
  jsonSObj["x"] = m_overlayGeometry.xpos;
  jsonSObj["y"] = m_overlayGeometry.ypos;
  jsonSObj["width"] = m_overlayGeometry.width;
  jsonSObj["height"] = m_overlayGeometry.height;
  jsonObj["overlay_geometry"] = jsonSObj;
  jsonObj["accordion"] = m_accordion;
  if (m_accordion){
    jsonObj["accordion_open"] = m_accordion_open;
  }
  std::string str;
  join(m_cssGridTemplateColumns, ' ', str);
  jsonObj["grid_template_columns"] = str;
  join(m_stretchFactorColumns, ' ', str);
  jsonObj["stretch_factor_columns"] = str;

  Json::Value jsonAry = Json::Value(Json::arrayValue);
  serializeContainerElements(jsonAry, onlyUpdated);

  jsonObj["element"] = jsonAry;
  bool ret = getElement()->getAttributeChangedFlag();
  getElement()->resetAttributeChangedFlag();
  return ret;
}

#if HAVE_PROTOBUF
bool GuiFieldgroup::serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated) {
  auto element = eles->add_field_groups();
  if (onlyUpdated && !getElement()->getAttributeChangedFlag()) {
    return false;
  }
  std::string s( Name() );
  HTMLConverter::convert2HTML( s );
  auto base = getElement()->writeProtobufProperties();
  element->set_allocated_base(base);
  base->set_name(s);
  s = getTitle();
  HTMLConverter::convert2HTML( s );
  element->set_label(s);
  element->set_label_alignment(GuiElement::ProtoAlignmentType(m_title_alignment));
  element->set_orientation(m_orientation == GuiElement::orient_Vertical ?
                                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Vertical :
                                            in_proto::GuiElement::Orientation::GuiElement_Orientation_Horizontal );
  element->set_frame(m_use_frame);
  element->mutable_grid_template_columns()->Assign(m_cssGridTemplateColumns_proto.begin(), m_cssGridTemplateColumns_proto.end());
  serializeContainerElements(eles, element, onlyUpdated);
  element->set_scrollbars(getElement()->withScrollbars());
  if( m_function != 0 )
    element->set_action(m_function->Name());
  element->set_accordion(m_accordion);
  if (m_accordion){
    element->set_accordion_open(m_accordion_open);
  }
  auto overlay = element->mutable_overlay();
  overlay->set_x(m_overlayGeometry.xpos);
  overlay->set_y(m_overlayGeometry.ypos);
  overlay->set_width(m_overlayGeometry.width);
  overlay->set_height(m_overlayGeometry.height);

  bool ret = getElement()->getAttributeChangedFlag();
  return ret;
}
#endif

/* --------------------------------------------------------------------------- */
/* doNormalisation --                                                          */
/* --------------------------------------------------------------------------- */
void GuiFieldgroup::doNormalisation() {
  GuiElementList::iterator it;
  GuiElementList::const_iterator it2;
  int hsFac, vsFac;

  // get max line length
  int cnt_colums(0), cnt_rows(m_container.size());
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    GuiFieldgroupLine *line = (*it)->getFieldgroupLine();
    int c(0);
    for(it2 =  static_cast<GuiFieldgroupLine*>(line)->getElementList().begin();
        it2 != static_cast<GuiFieldgroupLine*>(line)->getElementList().end(); ++it2){
      c += (*it2)->getColSpan();
      if (c > cnt_colums) cnt_colums = c;
    }
  }
  m_cssGridTemplateColumns = std::vector<std::string>(cnt_colums, "max-content");
#if HAVE_PROTOBUF
  m_cssGridTemplateColumns_proto = std::vector<in_proto::FieldGroup::GridTemplateField>(cnt_colums, in_proto::FieldGroup::GridTemplateField());
#endif
  m_stretchFactorColumns = std::vector<std::string>(cnt_colums, "0");

  // nomalizeLines columns
  std::vector<int> rowSpan;
  for( it = m_container.begin(); it != m_container.end(); ++it ){
    GuiFieldgroupLine *line = (*it)->getFieldgroupLine();
    int c(0);
    // get omit colums
    int omit_c = 0;
    for (int i=0; i < rowSpan.size(); ++i) {
      if (rowSpan[i]) { ++omit_c; rowSpan[i] -= 1;}
    }
    // normalize ColSpan
    // get count of stretch in line
    int stretchSum = 0;
    for(it2 =  static_cast<GuiFieldgroupLine*>(line)->getElementList().begin();
        it2 != static_cast<GuiFieldgroupLine*>(line)->getElementList().end(); ++it2){
      // stretch
      if ( (*it2)->Type() == GuiElement::type_Stretch ) {
        GuiStretch* stretch = dynamic_cast<GuiStretch*>( (*it2) );
        if (stretch) {
          stretch->getStretchFactor( hsFac, vsFac );
          stretchSum += hsFac;
        }
      }
    }
    for(it2 =  static_cast<GuiFieldgroupLine*>(line)->getElementList().begin();
        it2 != static_cast<GuiFieldgroupLine*>(line)->getElementList().end(); ++it2){
      // stretch
      if ( (*it2)->Type() == GuiElement::type_Stretch ) {
        GuiStretch* stretch = dynamic_cast<GuiStretch*>( (*it2) );
        if (stretch) {
          stretch->getStretchFactor( hsFac, vsFac );
          if (hsFac > 0) {
            std::ostringstream os;
            os << "minmax(max-content, "<<100.*hsFac/stretchSum<<"%)";
            m_cssGridTemplateColumns[c] = os.str();
#if HAVE_PROTOBUF
            m_cssGridTemplateColumns_proto[c].set_min_max(100.*hsFac/stretchSum);
#endif
            os.str("");
            os  << hsFac;
            m_stretchFactorColumns[c] = os.str();
          }
        }
      }
      //
      c += (*it2)->getColSpan();
      if ((*it2)->getRowSpan() > 1)
        rowSpan.push_back((*it2)->getRowSpan()-1);
    }
    if ((c + omit_c) != cnt_colums) {
      GuiElement *elem = line->getElementList().back();
      if ( line->getElementList().size() == 1 &&
           ( elem->Type() == GuiElement::type_Pixmap ||
             elem->Type() == GuiElement::type_Label  ||
             elem->Type() == GuiElement::type_Separator) ) {
        // single pixmap, label or separator: use all columns as in desktop mode
        elem->setColSpan(elem->getColSpan() + (cnt_colums - (c + omit_c)));
      } else {
        GuiVoid*  voidfield = new GuiVoid(line->getElement());
        line->attach( voidfield );
        voidfield->setColSpan(0 + (cnt_colums - (c + omit_c)));
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* variantMethod --                                                            */
/* --------------------------------------------------------------------------- */
std::string GuiFieldgroup::variantMethod(const std::string& method,
                                         const Json::Value& jsonArgs,
                                         JobEngine *eng) {
  Json::Value jsonElem = Json::Value(Json::objectValue);
  if (lower(method) == "menubuttonaction") {
    if (jsonArgs.isMember("id") && jsonArgs["id"].isInt()) {
      GuiButtonListener* btnLsnr = GuiButtonListener::getButtonListenerByMenuButtonId(jsonArgs["id"].asInt());
      if (btnLsnr) {
        btnLsnr->ButtonPressed();
        jsonElem["status"] = "OK";
      } else {
        jsonElem["message"] = "Wrong Parameter";
        jsonElem["status"] = "Error";
      }
    }
  }

  if (!jsonElem.isNull()) {
    BUG_INFO("variantMethod Method[" << method << "], Args["
              << ch_semafor_intens::JsonUtils::value2string(jsonArgs) << "]  Return: "
              << ch_semafor_intens::JsonUtils::value2string(jsonElem));
    return ch_semafor_intens::JsonUtils::value2string(jsonElem);
  }
  return getElement()->defaultVariantMethod(method, jsonArgs, eng);
}

void GuiFieldgroup::setOverlayGeometry(int xpos, int ypos, int width, int height){
  m_overlayGeometry = OverlayGeometry(xpos, ypos, width, height);
}
