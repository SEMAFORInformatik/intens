
#include "utils/utils.h"
#include "datapool/DataItem.h"
#include "datapool/DataElement.h"
#include "datapool/DataReference.h"
#include "datapool/DataVector.h"
#include "datapool/DataAlterFunctor.h"
#include "datapool/DataInx.h"
#include "datapool/DataLogger.h"

/*=============================================================================*/
/* Initialisation                                                              */
/*=============================================================================*/

BUG_INIT_COUNT(DataElement);

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataElement::DataElement(){
  BUG_INCR_COUNT;
}

DataElement::DataElement( const DataElement &ref )
  : DataContainer( ref )
  , DataAttributes( ref ){
  BUG_INCR_COUNT;
  LOG_DEBUG("DataElement::DataElement(copy-constructor)");
}

DataElement::~DataElement(){
  BUG_DECR_COUNT;
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* getStructElement --                                                         */
/* --------------------------------------------------------------------------- */

const DataStructElement &DataElement::getStructElement() const{
  LOG_DEBUG( "Es handelt sich nicht um ein DataStructElement-Objekt" );
  std::cerr << "ElementType=" << getElementType()
            << ", ContainerType=" << getContainerType() << std::endl;
  assert( false );
  throw 0;
}

/* --------------------------------------------------------------------------- */
/* setCompareAttributesOfAllElements --                                        */
/* --------------------------------------------------------------------------- */

bool DataElement::setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle ){
  return true;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataElement::isValid() const{
  return isAttributeSet( DATAisValid );
}

/* --------------------------------------------------------------------------- */
/* hasValidElements --                                                         */
/* --------------------------------------------------------------------------- */

bool DataElement::hasValidElements( bool incl_itemattr ) const{
  return isAttributeSet( DATAisValid );
}

/* --------------------------------------------------------------------------- */
/* hasSerializableValidElements --                                             */
/* --------------------------------------------------------------------------- */

bool DataElement::hasSerializableValidElements( const SerializableMask flags ) const{
  return isValid();
}

/* --------------------------------------------------------------------------- */
/* setValid --                                                                 */
/* --------------------------------------------------------------------------- */

void DataElement::setValid(){
  setAttributes( DATAisValid, 0 );
  markDataContainerValueUpdated();
}

/* --------------------------------------------------------------------------- */
/* setInvalid --                                                               */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::setInvalid(){
  if( isValid() ){
    setAttributes( 0, DATAisValid );
    markDataContainerValueUpdated();
    return ValueUpdated;
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* isLocked --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataElement::isLocked() const{
  return isAttributeSet( DATAisLocked );
}

/* --------------------------------------------------------------------------- */
/* setLocked --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::setLocked(){
  if( setAttributes( DATAisLocked, 0 ) ){
    markDataContainerDataUpdated();
    return DataUpdated;
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* setUnlocked --                                                              */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::setUnlocked(){
  if( setAttributes( 0, DATAisLocked ) ){
    markDataContainerDataUpdated();
    return DataUpdated;
  }
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int DataElement::getAllDataDimensionSize( DataContainer::SizeList &dimsize,
					  int dim,
					  bool incl_attr ) const
{
  return isValid() ? 1 : 0;
}

/* --------------------------------------------------------------------------- */
/* getAllDataDimensionSize --                                                  */
/* --------------------------------------------------------------------------- */

int DataElement::getAllDataDimensionSize( DataContainer::SizeList &dimsize,
					  int dim,
					  DataInx &index ) const
{
  assert( dim < (int)dimsize.size() );

  int inx = 0;
  if( !index.isWildcard() ){
    inx = index.getIndex();
  }
  if( inx > 0 ){
    return 0;
  }
  int max_dims = isValid() ? 1 : 0;
  dimsize[dim] = maximum( max_dims, dimsize[dim] );
  return 1;
}

/* --------------------------------------------------------------------------- */
/* getValidDimensionSize --                                                    */
/* --------------------------------------------------------------------------- */

int DataElement::getValidDimensionSize( bool incl_itemattr ) const{
  return isValid() ? 1 : 0;
}

/* --------------------------------------------------------------------------- */
/* assignCorrespondingDataItem --                                              */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::assignCorrespondingDataItem( const DataElement &el ){
  LOG_DEBUG( "not a DataStructElement");
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* clearCorrespondingDataItem --                                               */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::clearCorrespondingDataItem( const DataDictionary &dict ){
  LOG_DEBUG( "not a DataStructElement");
  return NoUpdate;
}

/* --------------------------------------------------------------------------- */
/* clearElement --                                                             */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::clearElement(){
  return clearValue();
}

/* --------------------------------------------------------------------------- */
/* clearContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::clearContainer( bool incl_itemattr ){
  return clearElement();
}

/* --------------------------------------------------------------------------- */
/* eraseContainer --                                                           */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::eraseContainer(){
  return clearElement();
}

/* --------------------------------------------------------------------------- */
/* writeXML --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataElement::writeXML( std::ostream &ostr
                          , std::vector<std::string> &attrs
                          , bool is_structitem
                          , int level
                          , bool debug
                          , bool isScalar )
{
  LOG_DEBUG("write");
  if( debug )
    for( int i=0; i<level; i++ ) ostr << " ";
  ostr << "<element/>\n";
  return true;
}

/* --------------------------------------------------------------------------- */
/* writeDP --                                                                  */
/* --------------------------------------------------------------------------- */

void DataElement::writeDP( std::ostream &ostr, int i, int index ) const{
  indent( i, ostr ) << "<EL";
  if( index >= 0 ){
    ostr << " index=\"" << index << "\"";
  }
  if( isValid() ){
    ostr << " value=\"";
    writeDPvalue( ostr );
    ostr << "\"";
  }

#if !defined(__OMIT_DP_TIMESTAMPS__)
  if( getDataContainerDataUpdated() > 0 ){
    ostr << " dat_ts=\"" << getDataContainerDataUpdated() << "\"";
  }
  if( getDataContainerValueUpdated() > 0 ){
    ostr << " val_ts=\"" << getDataContainerValueUpdated() << "\"";
  }
#endif
  writeDPattributes( ostr );
  ostr << "/>" << std::endl;
}

/* --------------------------------------------------------------------------- */
/* alterData --                                                                */
/* --------------------------------------------------------------------------- */

DataElement::UpdateStatus DataElement::alterData( DataItem &previous_item
                                                , DataAlterFunctor &func
                                                , DataInx &index )
{
  BUG( BugRef, "alterData" );

  // Wir müssen auf dem letzten Level der Indexliste angelangt sein !
  if( !index.isLastLevel() ){
    BUG_MSG( "Remaining Levels: " << index.printRemainingLevels() );
  }

  return func.alterData( *this );
}

/* --------------------------------------------------------------------------- */
/* getDataElement --                                                           */
/* --------------------------------------------------------------------------- */

DataElement &DataElement::getDataElement( DataItem &item,
                                          DataIndexList &inxlist,
                                          int index )
{
  return *this;
}

/* --------------------------------------------------------------------------- */
/* getDataContainer --                                                         */
/* --------------------------------------------------------------------------- */

DataContainer &DataElement::getDataContainer( DataItem &item,
                                              DataIndexList &inxlist,
                                              int index,
                                              int sz ){
  return *this;
}

/* --------------------------------------------------------------------------- */
/* result_of_compare --                                                        */
/* --------------------------------------------------------------------------- */

DataCompareResult DataElement::result_of_compare( bool left_valid, bool right_valid, bool is_equal ){
  if( left_valid ){
    if( right_valid ){
      if( is_equal ){
        setAttribute( DATAisEqual );
        return DATAisEqual; // Keine Differenz
      }
      else{
        setAttribute( DATAisDifferent );
        return DATAisDifferent;
      }
    }
    else{ // nur links
      setAttribute( DATAisLeftOnly );
      return DATAisLeftOnly;
    }
  }
  else{
    if( right_valid ){ // nur rechts
      setAttribute( DATAisRightOnly );
      return DATAisRightOnly;
    }
    else{
      return 0;
    }
  }
}

/* --------------------------------------------------------------------------- */
/* isCompareDimension --                                                       */
/* --------------------------------------------------------------------------- */

void DataElement::setCompareDimension(){
  assert( false );
}

/* --------------------------------------------------------------------------- */
/* isCompareDimension --                                                       */
/* --------------------------------------------------------------------------- */

bool DataElement::isCompareDimension() const{
  return false;
}
