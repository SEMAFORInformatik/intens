
#include <typeinfo>
#include <QDate>
#include <QLocale>

#include "utils/Debugger.h"
#include "app/DataPoolIntens.h"
  #include "datapool/DataItem.h"

#include "xfer/XferParameterString.h"

#include "gui/qt/GuiQtNavTextfield.h"
#include "gui/qt/GuiQtManager.h"

/* --------------------------------------------------------------------------- */
/* Constructor / Destructor --                                                 */
/* --------------------------------------------------------------------------- */
GuiQtNavTextfield::GuiQtNavTextfield( GuiElement *parent, XferDataItem *dataitem )
  : GuiQtDataField( parent ){
  installDataItem( dataitem );
}
GuiQtNavTextfield::GuiQtNavTextfield( GuiElement *parent, const std::string& label, const std::string& labelColor )
  : GuiQtDataField( parent )
  , m_label( label )
  , m_labelColor( labelColor ) {
}

GuiQtNavTextfield::~GuiQtNavTextfield(){
}

/* --------------------------------------------------------------------------- */
/* installDataItem --                                                          */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::installDataItem( XferDataItem *dataitem ){
  if( !GuiQtDataField::installDataItem( dataitem ) ){
    return false;
  }
  return false;
}

/*=============================================================================*/
/*  FieldIsEditable                                                            */
/*=============================================================================*/
bool GuiQtNavTextfield::FieldIsEditable(){
  return GuiQtDataField::FieldIsEditable();
}

/* --------------------------------------------------------------------------- */
/* finalWorkOk --                                                              */
/* --------------------------------------------------------------------------- */
void GuiQtNavTextfield::finalWorkOk(){
  BUG(BugGuiFld,"GuiQtNavTextfield::finalWorkOk");
  doFinalWork();
  s_DialogIsAktive = false;
}

/* --------------------------------------------------------------------------- */
/* confirmYesButtonPressed --                                                  */
/* --------------------------------------------------------------------------- */
void GuiQtNavTextfield::confirmYesButtonPressed(){
  finalWorkOk();
}

/* --------------------------------------------------------------------------- */
/* setWidth --                                                                 */
/* --------------------------------------------------------------------------- */
void GuiQtNavTextfield::setWidth( int width ){
  if( width != 0 && !(typeid(*m_param) == typeid(XferParameterString)))
    m_param->setLength( width );
}

/* --------------------------------------------------------------------------- */
/* setPrecision --                                                             */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::setPrecision( int prec ){
  return m_param->setPrecision( prec );
}

/* --------------------------------------------------------------------------- */
/* setScalefactor --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::setScalefactor( Scale *scale ){
  return m_param->setScalefactor( scale );
}

/* --------------------------------------------------------------------------- */
/* setThousandSep --                                                           */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::setThousandSep(){
  return m_param->setThousandSep();
}

/* --------------------------------------------------------------------------- */
/* geValue --                                                                  */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::getValue( std::string &s ) {
  if (m_label.size() > 0) {
    // fixed label
    s= m_label;
    return true;
  }
  return m_param->DataItem()->getValue( s );
}

/* --------------------------------------------------------------------------- */
/* getFormattedValue --                                                         */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::getFormattedValue( std::string &s ){
  BUG( BugGuiFld, "GuiQtNavTextfield::getFormattedValue");
  if (m_label.size() > 0) {
    // fixed label
    s= m_label;
    return true;
  }
  m_param->getFormattedValue( s );
  UserAttr::STRINGtype m_type= m_param->DataItem()->getUserAttr()->StringType();
  if (m_param->DataItem()->getDataType()) {
    QString qvalue = QString::fromStdString(s).trimmed();
    switch( m_param->DataItem()->getUserAttr()->StringType() ) {
      case UserAttr::string_kind_date:
        s = QLocale::system().toString(QDate::fromString(qvalue, Qt::ISODate),
                                       QLocale::ShortFormat).toStdString();
        break;
      case UserAttr::string_kind_time:
        s = QLocale::system().toString(QTime::fromString( qvalue, Qt::ISODate),
                                       QLocale::ShortFormat).toStdString();
        break;
      case UserAttr::string_kind_datetime:
        s = QLocale::system().toString(QDateTime::fromString( qvalue, Qt::ISODate).toLocalTime(),
                                       QLocale::ShortFormat).toStdString();
        break;
      case UserAttr::string_kind_password:
        s = "***";
      default:
        break;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getColor --                                                                 */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::getColor( std::string &color ){
  BUG( BugGuiFld, "GuiNavTextfield::getColor");
  if (m_label.size() > 0) {
    // fixed label
    color = m_labelColor;
    return true;
  }

  getAttributes();
  color = "#000000";
  if( m_attr_mask & DATAcolor1 ){
    color = GuiQtManager::Instance().color1ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor2 ){
    color = GuiQtManager::Instance().color2ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor3 ){
    color = GuiQtManager::Instance().color3ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor4 ){
    color = GuiQtManager::Instance().color4ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor5 ){
    color = GuiQtManager::Instance().color5ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor6 ){
    color = GuiQtManager::Instance().color6ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor7 ){
    color = GuiQtManager::Instance().color7ForegroundColor().name().toStdString();
  }
  else if( m_attr_mask & DATAcolor8 ){
    color = GuiQtManager::Instance().color8ForegroundColor().name().toStdString();
  }
  else{
    color = "#000000";
  }
  return !color.empty();
}

/*=============================================================================*/
/* finalWork                                                                   */
/*=============================================================================*/
void GuiQtNavTextfield::finalWork( const std::string &value ){
  m_tmpValue = value;
  BUG(BugGuiFld,"GuiQtNavTextfield::FinalWork");
  if( checkFormat() == XferParameter::status_Changed ){
    BUG_EXIT("checkFormat Changed");
    std::string error_msg;
    s_TargetsAreCleared = m_param->streamTargetsAreCleared(error_msg);
    if( s_TargetsAreCleared ){ // Targets sind ok
      finalWorkOk();
    }
    else
      confirm(error_msg);
  }
}

/* --------------------------------------------------------------------------- */
/* checkFormat --                                                              */
/* --------------------------------------------------------------------------- */
XferParameter::InputStatus  GuiQtNavTextfield::checkFormat(){
  BUG(BugGuiFld,"GuiQtNavTextfield::checkFormat");
  return m_param->checkFormat( m_tmpValue );
}

/* --------------------------------------------------------------------------- */
/* setInputValue --                                                            */
/* --------------------------------------------------------------------------- */
bool GuiQtNavTextfield::setInputValue(){
  BUG(BugGuiFld,"GuiQtNavTextfield::setInputValue");
  if( m_param->isEmpty( m_tmpValue ) ){
    m_param->clear();
    return true;
  }
  if( m_param->setFormattedValue( m_tmpValue ) ){
    return true;
  }
  BUG_EXIT("setValue failed");
  return false;
}
