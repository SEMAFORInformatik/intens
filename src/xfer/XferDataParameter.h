
#if !defined(XFER_DATAPARAMETER_INCLUDED_H)
#define XFER_DATAPARAMETER_INCLUDED_H

#include "xfer/XferParameter.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferConverter.h"

class XferDataParameter : public XferParameter
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  XferDataParameter( DataReference *dref=0 )
    : m_dataitem( 0 ){
    if( dref != 0 ){
      m_dataitem = new XferDataItem( dref );
    }
  }
  XferDataParameter( XferDataParameter &param ){
    m_dataitem = new XferDataItem( *param.m_dataitem );
  }
  virtual ~XferDataParameter(){
    delete m_dataitem;
  }

/*******************************************************************************/
/* public meber functions                                                      */
/*******************************************************************************/
public:
  virtual XferDataParameter *clone() = 0;
  void setDataItem( XferDataItem *item ) { assert(m_dataitem==0); m_dataitem = item; }
  void resetDataItem() { m_dataitem = 0; }
  XferDataItem *DataItem() { return m_dataitem; }
  DataReference *Data() { assert(m_dataitem!=0); return m_dataitem->Data(); }

  virtual void clear(){ assert(m_dataitem!=0); m_dataitem->clear(); }
  virtual void clearRange(){ assert(m_dataitem!=0); m_dataitem->clearRange(); }

  XferDataItemIndex *newDataItemIndex( int level ){
    assert(m_dataitem!=0);
    return m_dataitem->newDataItemIndex( level );
  }
  virtual bool setLength( int len ) { return false; }
  virtual int getLength(){ return 0; }
  virtual bool setPrecision( int prec ) { return false; }
  virtual int  getPrecision() { return -1; }
  virtual bool setScalefactor( Scale *scale )=0;
  virtual Scale* getScalefactor() { return NULL;};
  virtual bool setThousandSep() { return false; }
  std::string getName()const{ return m_dataitem != 0 ? m_dataitem->getName(): ""; }
  std::string getFullName()const{ return m_dataitem != 0 ? m_dataitem->getFullName( true ): ""; }
  bool isUpdated( TransactionNumber trans, bool gui = false ){
    return  m_dataitem == 0 ? false : ( m_dataitem->isUpdated( trans, gui ) || isConverterUpdated( trans, gui ) ); }

  bool setInvalid() { assert(m_dataitem!=0);return m_dataitem->setInvalid(); }
  bool lockValue() { assert(m_dataitem!=0);return m_dataitem->lockValue();}
  bool unlockValue() { assert(m_dataitem!=0);return m_dataitem->unlockValue();}

  virtual DATAAttributeMask getAttributes( TransactionNumber t ){
    return Data()->getAttributes( t );
  }
  const std::string getStylesheet( TransactionNumber t, bool& changed ){
    return Data()->getStylesheet( t, changed);
  }
  UserAttr *getUserAttr() { assert(m_dataitem!=0);return m_dataitem->getUserAttr(); }

  bool streamTargetsAreCleared(std::string& error_msg) {
    assert(m_dataitem!=0);
    return m_dataitem->StreamTargetsAreCleared(error_msg);
  }

  virtual void registerIndex( GuiIndexListener *listener ){}
  virtual void unregisterIndex( GuiIndexListener *listener ){}
  virtual bool acceptIndex( const std::string &, int ){ return true; }
  virtual bool setIndex( const std::string &, int ){ return false; }

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
protected:
  void converterSetScaleIndexes( Converter &conv );
private:
  virtual bool isConverterUpdated( TransactionNumber trans, bool gui = false )=0;

/*******************************************************************************/
/* private Data                                                                */
/*******************************************************************************/
protected:
  XferDataItem        *m_dataitem;

};

#endif
