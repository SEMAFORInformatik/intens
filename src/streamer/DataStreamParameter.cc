
#include <string>
#include <typeinfo>
#include <cmath>

#include "utils/Debugger.h"

#include "app/DataPoolIntens.h"
#include "parser/IdManager.h"
#include "streamer/StreamParameterGroup.h"
#include "streamer/TableStreamGroup.h"
#include "streamer/DataStreamParameter.h"
#include "streamer/StructStreamParameter.h"

#define SAY(X) // std::cout << "SAY: " << X << std::endl;

static bool setMatrix( XferDataItem *ditem, std::vector< dComplex >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd );
static bool setMatrix( XferDataItem *ditem, std::vector< double >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd );
static bool setMatrix( XferDataItem *ditem, std::vector< int >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd );
static bool setMatrix( XferDataItem *ditem, std::vector< std::string >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd );

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

DataStreamParameter::DataStreamParameter( XferDataItem *ditem
                                        , int level
                                        , StructStreamParameter *p
                                        , bool isMandatory
                                        , char delimiter
                                        , bool locale)
  : StreamParameter(delimiter)
  , m_dataitem(ditem)
  , m_indexNum(0)
  , m_level(level)
  , m_parent(p)
  , m_isScalar(false)
  , m_isCell(false)
  , m_locale(locale)
  , m_isMandatory(isMandatory){
  BUG( BugStreamer, "DataStreamParameter::Constructor" );
}

DataStreamParameter::DataStreamParameter( const DataStreamParameter &d )
  : m_indexNum(d.m_indexNum)
  , m_level(d.m_level)
  , m_parent( d.m_parent )
  , m_isScalar( false )
  , m_isCell( false )
  , m_isMandatory(d.m_isMandatory){
  BUG( BugStreamer, "DataStreamParameter::Copy-Constructor" );

  m_dataitem= new XferDataItem( *d.m_dataitem );
  BUG_EXIT( "item=" << m_dataitem->getFullName( true ) );
}

DataStreamParameter::~DataStreamParameter(){
  if( m_dataitem != 0 ){
    delete m_dataitem;
  }
}

/*=============================================================================*/
/* member functions                                                            */
/*=============================================================================*/

/* --------------------------------------------------------------------------- */
/* createDataItemIndexes --                                                    */
/* --------------------------------------------------------------------------- */

int DataStreamParameter::createDataItemIndexes( ){
  BUG(BugStreamer,"createDataItemIndexes");

  if( m_parent != 0 ){
    return m_parent->createDataItemIndexes(m_dataitem);
  }
  return m_dataitem->getNumIndexes();
}

/* --------------------------------------------------------------------------- */
/* setDataItemIndexes --                                                       */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setDataItemIndexes(){
  BUG(BugStreamer,"setDataItemIndexes");

  m_dataitem->setDimensionIndizes();
}

/* --------------------------------------------------------------------------- */
/* setDimensionIndizes --                                                      */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setDimensionIndizes(int level){

  if( m_dataitem!=0 ){
    if( level<0 )
      m_dataitem->setDimensionIndizes();
    else
      m_dataitem->setDimensionIndizes(level);
  }
}

/* --------------------------------------------------------------------------- */
/* removeDataItemIndexes --                                                    */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::removeDataItemIndexes(){
  BUG(BugStreamer,"removeDataItemIndexes");

  if( m_parent != 0 ){
    m_parent->removeDataItemIndexes(m_dataitem);
  }
}

/* --------------------------------------------------------------------------- */
/* data --                                                                     */
/* --------------------------------------------------------------------------- */

const DataReference &DataStreamParameter::data(){
  assert( m_dataitem!=0 );
  return *m_dataitem->Data();
}

/* --------------------------------------------------------------------------- */
/* setLevel --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setLevel( int level ){
  m_level=level;
}

/* --------------------------------------------------------------------------- */
/* getDimensionSize --                                                         */
/* --------------------------------------------------------------------------- */

size_t DataStreamParameter::getDimensionSize(int ndim) const {
  BUG_PARA(BugStreamer,"getDimensionSize","ndim=" << ndim);

  if( m_dataitem==0 ){
    return 0;
  }
  return m_dataitem->getDimensionSize(ndim);
}

/* --------------------------------------------------------------------------- */
/* clearRange --                                                               */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::clearRange( bool incl_itemattr ){
  BUG(BugStreamer,"clearRange");
  if( m_dataitem != 0 ){
    m_dataitem->clearRangeMaxLevel( incl_itemattr );
  }
}

/* --------------------------------------------------------------------------- */
/* setDbItemsNotModified --                                                    */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setDbItemsNotModified(){
  BUG(BugStreamer,"setDbItemsNotModified");

  if( m_dataitem == 0 ){
    BUG_EXIT( "no item available" );
    return;
  }

  m_dataitem->setDbItemsNotModified();
}

/* --------------------------------------------------------------------------- */
/* fixupItemStreams --                                                         */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::fixupItemStreams( BasicStream &stream ){
  BUG_PARA( BugStreamer, "fixupItemStreams", "Item=" << m_dataitem->getFullName( true ) );
  DataPoolIntens::Instance().fixupItemStreams( stream, m_dataitem->Data() );
} //

/* --------------------------------------------------------------------------- */
/* createIndexes --                                                            */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::createIndexes( const std::vector<int> &dims ){
  BUG(BugStreamer,"createIndexes");

//   cerr << "DataStreamParameter::createIndexes " << getFullName(false) << endl;
  if( m_parent != 0 ){
    m_parent->removeDataItemIndexes(m_dataitem);
    m_parent->createDataItemIndexes(m_dataitem);
  }
  // Before we are going to create the required indexes
  // we must remove any existing ones:
  eraseIndexes( );

  // now they can be created
  int numdims=dims.size();
  XferDataItemIndex *index = 0;
  assert( numdims > 0 );
  int firstIndexNum = m_dataitem->getFirstIndexNum(m_level) +1;
//   cerr << "DataStreamParameter::createIndexes numdims=" << numdims << " first=" << firstIndexNum << endl;
  for( int i=0; i<numdims; i++ ){
    index = m_dataitem->newDataItemIndex( m_level );
    assert( index != 0 );

    index->setIndexNum( firstIndexNum+i );
    index->setLowerbound( 0 );
    if( dims[i] > -1 )
      index->setUpperbound( dims[i] );
  }
}

/* --------------------------------------------------------------------------- */
/* eraseIndexes --                                                             */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::eraseIndexes(){
  if( m_dataitem != 0 ){
    m_dataitem->eraseDataItemIndex( m_level );
  }
}

/* --------------------------------------------------------------------------- */
/* setindex --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setIndex( int pos, int v ){
  BUG_PARA( BugStreamer, "setIndex", "pos= " << pos << " v=" << v );
  if( m_dataitem!=0 ){
    m_dataitem->setIndex(pos,v);
  }
}

/* --------------------------------------------------------------------------- */
/* setindex --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setIndex( const std::string &name, int index ) {
  BUG_PARA( BugStreamer, "setIndex", "name=" << name << ", index=" << index );
  if( m_dataitem!=0 ){
    m_dataitem->setIndex( name, index );
  }
  else{
    BUG_EXIT( "no DataItem" );
  }
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setValue( const std::string &v ){
  /* 2011-01-12 amg DB-Filter:
   * von einem db select koennen auch NULL-Werte kommen
   * die hier als leeren String ankommen, diese sollten auch
   * geschrieben werden.
   */
  if( m_dataitem != 0 /*&& !v.empty()*/ ){
    m_dataitem->setValue(v);
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::getValue( std::string &v )const{
  return m_dataitem == 0 ? false : m_dataitem->getValue(v);
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setValue( double v ){
  if( m_dataitem != 0 ){
    if (std::isnan(v)) {
      m_dataitem->setInvalid();
    } else
    m_dataitem->setValue(v);
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::getValue( double &v )const{
  return m_dataitem == 0 ? false : m_dataitem->getValue(v);
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setValue( const dComplex &v ){
  if( m_dataitem != 0 ){
    m_dataitem->setValue(v);
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::getValue( dComplex &v )const{
  return m_dataitem == 0 ? false : m_dataitem->getValue(v);
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setValue( int v ){
  if(m_dataitem != 0 ){
    if (std::isnan(v)) {
      m_dataitem->setInvalid();
    } else
    m_dataitem->setValue(v);
  }
}

/* --------------------------------------------------------------------------- */
/* getValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::getValue( int &v )const{
  return m_dataitem == 0 ? false : m_dataitem->getValue(v);
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::setValue( const std::vector<int> &dims
				    , const std::vector<double>& revals
				    , const std::vector<double>& imvals
				    , const std::vector<int> &inds
				    , int level ){
  BUG_PARA( BugStreamer, "setValue (double)", getName() );
  if( m_dataitem==0 ) return false;

  bool b;
  int numdims=dims.size();
  if( numdims < 1 )
    return false;
  // Die Indizes dieses Levels werden immer neu erstellt.
  createIndexes( dims );
  setDimensionIndizes(level);

  int indexnum = m_dataitem->getFirstIndexNum( m_level );
  assert( indexnum >= 0 );

  if( m_dataitem->Data()->getDataType() == DataDictionary::type_Complex ){
    int nsize=revals.size();
    if( nsize < imvals.size() ){
      nsize=imvals.size();
    }
    std::vector<dComplex> cvals(nsize);
    if( imvals.size()==0 ){
      std::copy( revals.begin(), revals.end(), cvals.begin() );
    }
    else {
      for( int n=0; n<nsize; ++n ){
	cvals[n]=dComplex( revals[n], imvals[n] );
      }
    }
    std::vector<dComplex>::const_iterator diter=cvals.begin();
    std::vector<int>::const_iterator inditer=inds.begin();
    std::vector<int>::const_iterator inditerEnd=inds.end();
    b=setMatrix( m_dataitem, diter, numdims, 0, indexnum, dims, inditer, inditerEnd );
  }
  else {
    std::vector<double>::const_iterator diter=revals.begin();
    std::vector<int>::const_iterator inditer=inds.begin();
    std::vector<int>::const_iterator inditerEnd=inds.end();
    b=setMatrix( m_dataitem, diter, numdims, 0, indexnum, dims, inditer, inditerEnd);
  }
  eraseIndexes( );
  return b;
}

bool DataStreamParameter::setValue( const std::vector<int> &dims
				    , const std::vector<int>& vals, int level ){
  BUG_PARA( BugStreamer, "DataStreamParameter::setValue (int)", getName() );
  if( m_dataitem==0 ) return false;

  bool b;
  int numdims=dims.size();
  if( numdims < 1 )
    return false;

  // Die Indizes dieses Levels werden immer neu erstellt.
  createIndexes( dims );
  setDimensionIndizes(level);

  int indexnum = m_dataitem->getFirstIndexNum( m_level );
  assert( indexnum >= 0 );

  std::vector<int>::const_iterator diter=vals.begin();
  std::vector<int>inds(vals.size(),1);
  std::vector<int>::const_iterator inditer=inds.begin();
  std::vector<int>::const_iterator inditerEnd=inds.end();
  b=setMatrix( m_dataitem, diter, numdims, 0, indexnum, dims, inditer, inditerEnd );

  eraseIndexes( );
  return b;
}

bool DataStreamParameter::setValue( const std::vector<int> &dims
				    , const std::vector<dComplex>& cvals, int level ){
  BUG_PARA( BugStreamer, "DataStreamParameter::setValue (complex)", getName() );
  if( m_dataitem==0 ) return false;

  bool b;
  int numdims=dims.size();
  if( numdims < 1 )
    return false;

  // Die Indizes dieses Levels werden immer neu erstellt.
  createIndexes( dims );
  setDimensionIndizes(level);

  int indexnum = m_dataitem->getFirstIndexNum( m_level );
  assert( indexnum >= 0 );

  if( m_dataitem->Data()->getDataType() == DataDictionary::type_Complex ){
    std::vector<dComplex>::const_iterator diter=cvals.begin();
    std::vector<int>inds(cvals.size(),1);
    std::vector<int>::const_iterator inditer=inds.begin();
    std::vector<int>::const_iterator inditerEnd=inds.end();
    b=setMatrix( m_dataitem, diter, numdims, 0, indexnum, dims, inditer, inditerEnd );
  }
  else {
    size_t nsize=cvals.size();
    std::vector<double> rvals(nsize);
    for( size_t n=0; n<nsize; ++n ){
      rvals[n]=std::abs( cvals[n] );
    }
    std::vector<double>::const_iterator diter=rvals.begin();
    std::vector<int>inds(rvals.size(),1);
    std::vector<int>::const_iterator inditer=inds.begin();
    std::vector<int>::const_iterator inditerEnd=inds.end();
    b=setMatrix( m_dataitem, diter, numdims, 0, indexnum, dims, inditer, inditerEnd);
  }
  eraseIndexes( );
  return b;
}

/* --------------------------------------------------------------------------- */
/* setValue --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::setValue( const std::vector<int> &dims
				    , const std::vector<std::string>& svals, int level ){
  BUG_PARA( BugStreamer, "DataStreamParameter::setValue (std::string)", getName() );
  if( m_dataitem==0 ) return false;

  int numdims=dims.size();
  if( numdims < 1 )
    return false;
  // Die Indizes dieses Levels werden immer neu erstellt.
//   cerr << "dims: ";
//   copy( dims.begin(), dims.end(), ostream_iterator<int>( cerr, " " ) );
//   cerr << "\nvals: ";
//   copy( svals.begin(), svals.end(), ostream_iterator<std::string>( cerr, " " ) );
//   cerr << endl;
  createIndexes( dims );
  setDimensionIndizes(level);
  int indexnum = m_dataitem->getFirstIndexNum( m_level );
  assert( indexnum >= 0 );

  std::vector<std::string>::const_iterator siter=svals.begin();
  std::vector<int> inds(svals.size(),1);
  std::vector<int>::const_iterator inditer=inds.begin();
  std::vector<int>::const_iterator inditerEnd=inds.end();
  bool b=setMatrix( m_dataitem, siter, numdims, 0, indexnum, dims, inditer, inditerEnd );
  eraseIndexes( );
  return b;
}

/* --------------------------------------------------------------------------- */
/* isScalar --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isScalar() const {
  return m_isScalar;
}

/* --------------------------------------------------------------------------- */
/* isCell --                                                                   */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isCell() const {
//   if( m_isCell ) cerr << getName() << " is cell\n";
  return m_isCell;
}

/* --------------------------------------------------------------------------- */
/* isHidden --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isHidden() const {
  if( m_dataitem != 0 ){
    UserAttr *attr = m_dataitem->getUserAttr();
    if( attr != 0 ){
      return attr -> IsHidden();
    }
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* setMatrix --                                                                */
/* --------------------------------------------------------------------------- */

static bool setMatrix( XferDataItem *ditem, std::vector< double >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd ){
  int i;

  if( idim < ndims-1 ){
    int dims = vdims[idim];
    for( i = 0; i < dims; i++ ){
      ditem->setIndex( idim+indexnum, i );
      setMatrix( ditem, diter
		 , ndims   // Dimensions
		 , idim + 1      // current Index in dim
		 , indexnum
		 , vdims
		 , inds
		 , indsEnd
		 );
    }
  }
  else{
    int dims = vdims[idim];
    for( i = 0; i < dims && inds!=indsEnd; i++ ){
//          cerr << " idim " << idim << " i " << " val " << *diter << endl;
      ditem->setIndex( idim+indexnum, i );
      if( *inds >0 )
	ditem->setValue( *diter );
      else
	ditem->setInvalid();
      ++diter;
      ++inds;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setMatrix --                                                                */
/* --------------------------------------------------------------------------- */

static bool setMatrix( XferDataItem *ditem, std::vector< int >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd ){
  int i;

  if( idim < ndims-1 ){
    int dims = vdims[idim];
    for( i = 0; i < dims; i++ ){
      ditem->setIndex( idim+indexnum, i );
      setMatrix( ditem, diter
		 , ndims   // Dimensions
		 , idim + 1      // current Index in dim
		 , indexnum
		 , vdims
		 , inds
		 , indsEnd
		 );
    }
  }
  else{
    int dims = vdims[idim];
    for( i = 0; i < dims && inds!=indsEnd; i++ ){
//          cerr << " idim " << idim << " i " << " val " << *diter << endl;
      ditem->setIndex( idim+indexnum, i );
      if( *inds >0 )
	ditem->setValue( *diter );
      else
	ditem->setInvalid();
      ++diter;
      ++inds;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setMatrix --                                                                */
/* --------------------------------------------------------------------------- */

static bool setMatrix( XferDataItem *ditem, std::vector< dComplex >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd ){
  int i;

  if( idim < ndims-1 ){
    int dims = vdims[idim];
    for( i = 0; i < dims; i++ ){
      ditem->setIndex( idim+indexnum, i );
      setMatrix( ditem, diter
		 , ndims   // Dimensions
		 , idim + 1      // current Index in dim
		 , indexnum
		 , vdims
		 , inds
		 , indsEnd
		 );
    }
  }
  else{
    int dims = vdims[idim];
    for( i = 0; i < dims && inds!=indsEnd; i++ ){
//          cerr << " idim " << idim << " i " << " val " << *diter << endl;
      ditem->setIndex( idim+indexnum, i );
      if( *inds >0 )
	ditem->setValue( *diter );
      else
	ditem->setInvalid();
      ++diter;
      ++inds;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* setMatrix --                                                                */
/* --------------------------------------------------------------------------- */

static bool setMatrix( XferDataItem *ditem, std::vector< std::string >::const_iterator &diter
		       , int ndims, int idim, int indexnum
		       , const std::vector<int> &vdims
		       , std::vector<int>::const_iterator &inds
		       , std::vector<int>::const_iterator &indsEnd ){
  int i;

  if( idim < ndims-1 ){
    int dims = vdims[idim];
    for( i = 0; i < dims; i++ ){
      ditem->setIndex( idim+indexnum, i );
      setMatrix( ditem, diter
		 , ndims   // Dimensions
		 , idim + 1      // current Index in dim
		 , indexnum
		 , vdims
		 , inds
		 , indsEnd
		 );
    }
  }
  else{
    int dims = vdims[idim];
    for( i = 0; i < dims && inds!=indsEnd; i++ ){
//          cerr << " idim " << idim << " i " << " val " << *diter << endl;
      ditem->setIndex( idim+indexnum, i );
      if( *inds >0 )
	ditem->setValue( *diter );
      else
	ditem->setInvalid();
      ++diter;
      ++inds;
    }
  }
  return true;
}

/* --------------------------------------------------------------------------- */
/* getValues --                                                                */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::getValues( StreamSource *src ){
  if( src != 0 )
    src->getValues( *this );
}

/* --------------------------------------------------------------------------- */
/* newDataItemIndex --                                                         */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::newDataItemIndex( int level, const std::string &name, int value ){
  BUG( BugStreamer, "newDataItemIndex" );

  XferDataItemIndex *inx = m_dataitem->newDataItemIndex( level );
  if( inx == 0 ){
    return false;
  }
  inx -> setIndexNum( m_indexNum );
  if( !name.empty() ){
    inx -> setIndexName( name, "" );
  }
  if( value >= 0 ){
    inx -> setLowerbound( value );
  }
  m_indexNum++;
  BUG_EXIT( "index found. indexNum=" << m_indexNum );
  return true;
}

/* --------------------------------------------------------------------------- */
/* marshal --                                                                  */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::marshal( std::ostream &os ){
//   cerr<<"DataStreamParameter::marshal  not implemented yet !!!"<<endl;
  if( m_dataitem != 0 ){
    os << "<DataStreamParameter";
    os << " indexNum=\"" << m_indexNum << "\"";
    os << " level=\"" << m_level << "\"";
    if( m_isScalar )
      os << " isScalar=\"true\"";
    if( m_isCell )
      os << " isCell=\"true\"";
    if( m_isMandatory )
      os << " isMandatory=\"true\"";
    os << ">\n";

    m_dataitem -> marshal ( os );
    os << "</DataStreamParameter>\n";
  }
}

/* --------------------------------------------------------------------------- */
/* unmarshal --                                                                */
/* --------------------------------------------------------------------------- */

Serializable *DataStreamParameter::unmarshal( const std::string &element, const XMLAttributeMap &attributeList ){
  std::cerr<<"DataStreamParameter::unmarshal  not implemented yet !!!"<<std::endl;
  return this;
}

/* --------------------------------------------------------------------------- */
/* setText --                                                                  */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setText( const std::string &text ){
}

/* --------------------------------------------------------------------------- */
/* getIndexName --                                                             */
/* --------------------------------------------------------------------------- */

std::string DataStreamParameter::getIndexName( int i ) const {
  BUG_PARA( BugStreamer, "getIndexName", "i=" << i );
  if( m_dataitem == 0 ){
    BUG_EXIT( "no DataItem" );
    return "";
  }
  XferDataItemIndex *inx=m_dataitem->getIndex(i);
  if( inx == 0 ){
    BUG_EXIT("no XferDataItemIndex[" << i << "]" );
    return "";
  }
  BUG_EXIT( "return " << inx->IndexName() );
  return inx->IndexName();
}

/* --------------------------------------------------------------------------- */
/* setTargetStream --                                                          */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setTargetStream( BasicStream *s ){
  DataPoolIntens::Instance().setTargetStream( m_dataitem->Data(), s );
}

/* --------------------------------------------------------------------------- */
/* unlinkIndex --                                                              */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::unlinkIndex( StreamParameterGroup *group ){
  BUG( BugStreamer, "unlinkIndex" );
  if( m_dataitem == 0 ){
    BUG_EXIT("no DataItem" );
    return;
  }

  if( m_dataitem->getNumIndexes() <= 0 ){
    BUG_EXIT( "no index" );
    return;
  }
  BUG_MSG( "DataItem is " << m_dataitem->getFullName(true) );

  for( int i=0; i<m_dataitem -> getNumIndexes(); ++i ){
    std::string index_name=m_dataitem -> getIndex(i) -> IndexName();
    if ( index_name.size() > 0 ){
      if( IdManager::Instance().getType( index_name ) != IdManager::id_Index ){
	StreamParameterGroup *pgroup=group->findIndex( index_name );
	if( pgroup == 0 ){  // index not found, let's find the top most parent
	  StreamParameterGroup *pg=group;
	  do {
	    pgroup = pg;
	    pg = pg -> getParent();
	  } while( pg != 0 && !pg->hasIndex() && typeid( *pg ) == typeid( TableStreamGroup ) );
	}
	if( pgroup )
	  pgroup -> removeIndexedParameter( this );
      }
    }
  }
}

/* --------------------------------------------------------------------------- */
/* setDbItemTimestamp --                                                       */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setDbItemTimestamp( TransactionNumber trans ){
  if( m_dataitem != 0 ){
    m_dataitem->setDbItemTimestamp( trans );
  }
}

/* --------------------------------------------------------------------------- */
/* isDbItemUpdated --                                                          */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isDbItemUpdated() const {
  BUG_PARA(BugStreamer,"isDbItemUpdated", m_dataitem->getFullName(true) );
  if( m_dataitem == 0 )
    return false;
  bool updated = m_dataitem->isDbItemUpdated();
  if( updated ){
    BUG_MSG( " is updated" );
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* isDbValueUpdated --                                                         */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isDbValueUpdated() const {
  BUG_PARA(BugStreamer, "isDbValueUpdated", m_dataitem->getFullName(true) );
  if( m_dataitem == 0 )
    return false;
  bool updated = m_dataitem->isDbValueUpdated();
  SAY(" -- DataStreamParameter::isDbValueUpdated == " << updated);
  if( updated ){
    BUG_MSG( " is updated" );
  }
  return updated;
}

/* --------------------------------------------------------------------------- */
/* isValid --                                                                  */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isValid(bool validErrorMsg){
  return m_dataitem == 0 ? false : m_dataitem->isValid();
}

/* --------------------------------------------------------------------------- */
/* getName --                                                                  */
/* --------------------------------------------------------------------------- */

std::string DataStreamParameter::getName() const{
  return m_dataitem == 0 ? "" : m_dataitem->getName();
}

/* --------------------------------------------------------------------------- */
/* getFullName --                                                              */
/* --------------------------------------------------------------------------- */

std::string DataStreamParameter::getFullName( bool withIndexes )const{
  return m_dataitem == 0 ? "" : m_dataitem->getFullName(withIndexes);
}

/* --------------------------------------------------------------------------- */
/* getLabel --                                                                 */
/* --------------------------------------------------------------------------- */

const std::string DataStreamParameter::getLabel() const{
  return m_dataitem == 0 ? "" : m_dataitem->getLabel();
}

/* --------------------------------------------------------------------------- */
/* DataItem --                                                                 */
/* --------------------------------------------------------------------------- */

XferDataItem *DataStreamParameter::DataItem() const{
  return m_dataitem;
}

/* --------------------------------------------------------------------------- */
/* getLevel --                                                                 */
/* --------------------------------------------------------------------------- */

int DataStreamParameter::getLevel() const{
  return m_level;
}

/* --------------------------------------------------------------------------- */
/* setScalar --                                                                */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setScalar( bool is_scalar ){
  m_isScalar = is_scalar;
}

/* --------------------------------------------------------------------------- */
/* setCell --                                                                  */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setCell( bool is_cell ){
  m_isCell = is_cell;
}

/* --------------------------------------------------------------------------- */
/* isMandatory --                                                              */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isMandatory() const{
  return m_isMandatory;
}

/* --------------------------------------------------------------------------- */
/* setMandatory --                                                             */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setMandatory(){
  m_isMandatory = true;
}

/* --------------------------------------------------------------------------- */
/* isLocale --                                                                 */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::isLocale() const{
  return m_locale;
}

/* --------------------------------------------------------------------------- */
/* setLocale --                                                                */
/* --------------------------------------------------------------------------- */

void DataStreamParameter::setLocale(bool locale){
  m_locale = locale;
}

/* --------------------------------------------------------------------------- */
/* getInvalidItems --                                                          */
/* --------------------------------------------------------------------------- */

bool DataStreamParameter::getInvalidItems( std::vector< std::string > &invaliditems ){
  if( m_isMandatory && !isValid() ){
    invaliditems.push_back( getLabel() + " (" + m_dataitem->getFullName(true) + ")");
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------- */
/* getFirstIndexNum --                                                         */
/* --------------------------------------------------------------------------- */

int DataStreamParameter::getFirstIndexNum() const{
  return m_dataitem ? m_dataitem->getFirstIndexNum( m_level ) : 0;
}

/* --------------------------------------------------------------------------- */
/* getNumIndexes --                                                            */
/* --------------------------------------------------------------------------- */

int DataStreamParameter::getNumIndexes() const{
  return m_dataitem ? m_dataitem->getNumIndexes() : 0;
}
