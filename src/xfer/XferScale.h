
#ifndef XFER_SCALE_H
#define XFER_SCALE_H

#include "xfer/Scale.h"
#include "datapool/DataPool.h"

class XferDataItem;


class XferScale : public Scale
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  XferScale( char _operator = '*' );
  XferScale( XferDataItem *item, char _operator = '*' );
private:
  XferScale( const XferScale &scale );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual Scale *copy();
  virtual ~XferScale();
  bool isUpdated( TransactionNumber trans, bool gui = false );
  double getValue() const;
  virtual void setValue( double value, char _operator = '*' ){
    m_operator = _operator; m_value = value;
  }
  XferDataItem *getDataItem(){ return m_dataItem; }
  void setDataItem( char _operator, XferDataItem *item ){
    m_dataItem = item;
    m_operator = _operator;
  }
  DATAAttributeMask getAttributes( TransactionNumber t );

  void reset(){ m_value = 1; m_dataItem = 0; }
  virtual void setIndex( int wildcardNumberBackwards, int value );

  virtual void registerIndex( GuiIndexListener *listener );
  virtual void unregisterIndex( GuiIndexListener *listener );
  virtual bool acceptIndex( const std::string &, int );
  virtual bool setIndex( const std::string &, int );

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual Scale &operator=( const Scale &scale );

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/

private:
  XferDataItem  *m_dataItem;
};

#endif
