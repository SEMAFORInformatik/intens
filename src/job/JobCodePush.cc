
#include "job/JobIncludes.h"

#include "job/JobCodePush.h"
#include "job/JobEngine.h"
#include "job/JobStackData.h"
#include "job/JobStackDataReal.h"
#include "job/JobStackDataInteger.h"
#include "job/JobStackDataString.h"
#include "job/JobStackDataEndOfLine.h"
#include "job/JobStackDataVariable.h"
#include "job/JobDataReference.h"
#include "app/DataPoolIntens.h"
#include "gui/GuiIndex.h"
#include "gui/GuiList.h"
#include "datapool/DataPool.h"

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePopAndDelete::execute( JobEngine *eng ){
  JobStackDataPtr dat( eng->pop() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushInvalid::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushInvalid::execute");
  eng->pushInvalid();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushReal::execute( JobEngine *eng ){
  BUG_PARA(BugJobCode,"JobCodePushReal::execute", m_value );
  eng->push( new JobStackDataReal( m_value ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushInteger::execute( JobEngine *eng ){
  BUG_PARA(BugJobCode,"JobCodePushInteger::execute", m_value );
  eng->push( new JobStackDataInteger( m_value ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushSortOrder::execute( JobEngine *eng ){
  BUG_PARA(BugJobCode,"JobCodePushSortOrder::execute", m_listId );
  GuiElement *e = GuiElement::findElement( m_listId );
  assert( e != 0 );
  assert( e->Type() == GuiElement::type_List );
  GuiList *l = e->getList();
  assert( l != 0 );

  eng->push( new JobStackDataInteger( l->sortOrder() ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushString::execute( JobEngine *eng ){
  BUG_PARA(BugJobCode,"JobCodePushString::execute", m_value );
  eng->push( new JobStackDataString( m_value ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushEndOfLine::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushEndOfLine::execute" );
  eng->push( new JobStackDataEndOfLine() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushReason::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushReason::execute");
  eng->pushReason( m_reason );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushNewValue::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushNewValue::execute");
  JobStackItemPtr data( eng->getNewData() );
  eng->push( data );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushNewValid::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushNewValid::execute");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushOldValue::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushOldValue::execute");
  JobStackItemPtr data( eng->getOldData() );
  eng->push( data );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushOldValid::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushOldValid::execute");
  return op_FatalError;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushColIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushColIndex::execute");
  eng->pushIndex( -1, 0 );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushRowIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushRowIndex::execute");
  eng->pushIndex( -1, 1 );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushDiagramXPos::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushDiagramXPos::execute");
  eng->pushDiagramXPos();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushDiagramYPos::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushDiagramYPos::execute");
  eng->pushDiagramYPos();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushSortCriteria::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushSortCriteria::execute");
  eng->pushSortCriteria();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushIndex::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushIndex::execute");
  JobStackDataPtr data( eng->pop() );
  int index = 0;

  if( data->getIntegerValue( index ) ){
    eng->pushIndex( -1, index );
  }
  else{
    eng->pushInvalid();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushUnits::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushUnits::execute");
  JobStackDataPtr data( eng->pop() );

  std::string units;
  data->getUnits( units );
  eng->push( new JobStackDataString( units ) );

  BUG_EXIT( "Units : " << units );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushLabel::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushLabel::execute");
  JobStackDataPtr data( eng->pop() );

  std::string label;
  data->getLabel( label );
  eng->push( new JobStackDataString( label ) );

  BUG_EXIT( "Label : " << label );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushIndexOfLevel::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushIndexOfLevel::execute");
  JobStackDataPtr datalevel( eng->pop() );
  JobStackDataPtr dataindex( eng->pop() );
  int level = 0;
  int index = 0;

  if( datalevel->getIntegerValue( level ) && dataindex->getIntegerValue( index ) ){
    eng->pushIndex( level, index );
  }
  else{
    eng->pushInvalid();
  }
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushIndexNumber::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushIndexNumber::execute");
  eng->pushIndexNumber();
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushError::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushError::execute");
  int err = eng->getError() ? 1 : 0;
  eng->push( new JobStackDataInteger( err ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushVariable::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodePushVariable::execute");
  m_data->updateDataReference( eng );
  eng->push( new JobStackDataVariable( m_data ) );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetCycleNumber::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetCycleNumber::execute");
  JobStackDataPtr i( eng->pop() );
  JobStackDataPtr d( eng->pop() );
  d->setCycleNumber( i );
  eng->pushData( d );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetThis::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetThis::execute");
  DataReference *dr = eng->getNewThisReference();
  if( dr == 0 ){
    return op_Aborted;
  }
  m_data->setDataRoot( dr );
  DataDictionary *dict = DataPool::getDatapool().GetRootDictVar();
  BUG_EXIT( "THIS == " << dr->fullName() << ", Type == " << dr->getDataType() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetBase::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetBase::execute");
  DataReference *dr = eng->getBaseDataReference();
  if( dr == 0 ){
    return op_Aborted;
  }
  m_data->setDataRoot( dr );
  DataDictionary *dict = DataPool::getDatapool().GetRootDictVar();
  BUG_EXIT( "Base == " << dr->fullName() << ", Type == " << dr->getDataType() );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetSource::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetSource::execute");
  DataReference *dr = eng->getSourceDataReference();

  // haben wir eine Liste
  int idx = 0;
  if ( m_dataIdx)  {
    m_dataIdx->Data().GetValue( idx );
  }
  std::vector<DataReference*> drList = eng->getSourceListDataReference();
  if (drList.size()) {
    if (idx < drList.size())
      dr = drList[idx];
    else
      // a invalid dataRef
      dr = DataPoolIntens::getDataPool().newDataReference();
  }

  if (idx > drList.size())
     // a invalid dataRef
    dr = DataPoolIntens::getDataPool().newDataReference();

  if( dr == 0 )
    return op_Aborted;
  m_data->setDataRoot( dr );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodeSetSource2::execute( JobEngine *eng ){
  BUG(BugJobCode,"JobCodeSetSource2::execute");
  DataReference *dr = eng->getSource2DataReference();
  if( dr == 0 )
    return op_Aborted;
  m_data->setDataRoot( dr );
  return op_Ok;
}

/* --------------------------------------------------------------------------- */
/* execute --                                                                  */
/* --------------------------------------------------------------------------- */

JobElement::OpStatus JobCodePushGuiIndex::execute( JobEngine *eng ){
  int inx = m_index->getIndex();
  BUG_PARA(BugJobCode,"JobCodePushGuiIndex::execute", inx );
  JobStackDataInteger *s = new JobStackDataInteger( inx );
  eng->push( s );
  return op_Ok;
}
