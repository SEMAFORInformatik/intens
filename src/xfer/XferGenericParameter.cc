
#include "app/DataPoolIntens.h"
#include "XferGenericParameter.h"
#include "XferDataItem.h"

INIT_LOGGER();

/** utility function
 */
static XferDataItem *getDataItem( const char * paramName ){
    DataReference *d = DataPoolIntens::getDataReference(paramName);
    if( d == 0 ){
	return 0;
    }
    XferDataItem *x = new XferDataItem( d );
    return x;
}

 XferGenericParameter::XferParameterMap XferGenericParameter::xfermap;

XferGenericParameter::XferGenericParameter(XferDataItem *x):index(0){
    xferItem = x;
    if( x != 0 ){
	index = xferItem->newDataItemIndex();
    }
}

XferGenericParameter * XferGenericParameter::get(const char *paramName){
    XferParameterMap::iterator I = xfermap.find( paramName );
    XferGenericParameter *p=0;
    if( I == xfermap.end() ){
	XferDataItem *x = getDataItem( paramName );
	p = new XferGenericParameter( x );
	xfermap[paramName] = p;
	return p;
    }
    p = (*I).second;
    if( p -> checkItem( paramName ) ){
	return p;
    }
    BUG_ERROR( "XferGenericParameter: Undeclared Reference "
		   << paramName );
    return 0;
}
bool XferGenericParameter::checkItem( const char *paramName ){
    if( xferItem != 0 )
	return true;
    xferItem = getDataItem( paramName );
    if( xferItem != 0 ){
	index = xferItem->newDataItemIndex();
	return true;
    }
    return false;
}

double XferGenericParameter::getDoubleValue( ){
    double d;
    xferItem->setIndex( 0, 0 );
    xferItem -> getValue( d );
    return d;
}
std::string XferGenericParameter::getStringValue( ){
    std::string s;
    xferItem->setIndex( 0, 0 );
   xferItem -> getValue( s );
    return s;
}

std::vector<double> XferGenericParameter::getDoubleVector(){
    std::vector<double> vec;
    index->setLowerbound( 0 );
    xferItem->setDimensionIndizes();

    DataContainer::SizeList dims;
    int numdims = xferItem->getAllDataDimensionSize( dims );
    if( numdims > 0 ){
	double d;
	for( int i = 0; i < dims[0]; i++ ){
	    xferItem->setIndex( 0, i );
	    if( xferItem->getValue(d) ){
		vec.push_back( d );
		BUG_DEBUG(  "[ " << i << " ] => " << d );
	    }
	}
    }
    else {
	BUG_DEBUG(  xferItem->getName() << " empty" );
    }

    return vec;

}

void XferGenericParameter::setValue( double d ){
    xferItem -> setValue( d );
}

void XferGenericParameter::setValue( std::string &s ){
    xferItem -> setValue( s );
}

void XferGenericParameter::setValue( const std::vector<double> &v){
    index->setLowerbound( 0 );
    xferItem->setDimensionIndizes();
    xferItem->clearRangeMaxLevel();

    std::vector<double>::const_iterator diter = v.begin();
    for( int i=0; diter != v.end(); ++diter, ++i ){
	BUG_DEBUG(  "[ " << i << " ] <= " << *diter );
	xferItem->setIndex( 0, i );
	xferItem->setValue( *diter );
    }
}

//void XferGenericParameter::setValue( const std::vector<std::string> &v){
//}
