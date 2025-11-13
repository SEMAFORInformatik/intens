
#if !defined(COLORSET_INCLUDED_H)
#define COLORSET_INCLUDED_H

#include "app/LSPItem.h"
#include <string>
#include <vector>

class ColorSetItem;
class XferDataItem;
class Scale;
class QwtLinearColorMap;

class ColorSet : public LSPItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ColorSet();
  virtual ~ColorSet();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setLowerbound( int what, const std::string &val );
  bool setLowerbound( int what, XferDataItem *, Scale * );
  void setLowerbound( int what, double val );
  bool setUpperbound( int what, const std::string &val );
  bool setUpperbound( int what, XferDataItem *, Scale *scale );
  bool setUpperbound( int what, double val );
  void setInvalid();
  void setAlways();
  void setColors( const std::string &, const std::string & );
  void setColors( XferDataItem*, XferDataItem* );
  bool isDataItemUpdated( TransactionNumber trans ) const;

  ColorSetItem *getColorItem( const std::string &val );
  ColorSetItem *getColorItem( double val );
  ColorSetItem *getColorItemInvalid() const;
  ColorSetItem *getColorItemAlways() const;
  bool getInterpolatedColors( double val, std::string& background, std::string& foreground );
  typedef std::map<double, std::string> ColorMap;
  bool getColorMap(ColorMap& colorMap, double minValue, double maxValue, bool reverse);
  #ifdef HAVE_QT
  bool getColorMap(QwtLinearColorMap& colorMap, double minValue, double maxValue, bool reverse);
  #endif

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  typedef std::vector<ColorSetItem*> ColorList;

  ColorSetItem  *m_current;
  ColorList      m_list;
};


class ColorSetItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ColorSetItem()
    : m_lower_what( 99 )
    , m_upper_what( 99 )
    , m_xfer_lower_value(0)
    , m_xfer_upper_value(0)
    , m_xfer_background(0)
    , m_xfer_foreground(0)
    , m_scale_upper(0)
    , m_scale_lower(0)
  {}
  ~ColorSetItem();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setUpperbound( int what, const std::string &val ) { return false; }
  virtual bool setUpperbound( int what, double val ) { return false; }
  virtual bool setUpperbound( int what, XferDataItem *xfer, Scale *scale );
  void setColors( const std::string &bg, const std::string &fg );
  void setColors( XferDataItem *, XferDataItem * );
  bool isDataItemUpdated( TransactionNumber trans ) const;
  const std::string &background();
  const std::string &foreground();
  virtual bool equal( const std::string &val ){ return false; }
  virtual bool equal( double val ){ return false; }
  virtual bool invalid() const { return true; }
  virtual bool always() const { return false; }

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  int   m_lower_what;
  int   m_upper_what;
  XferDataItem *m_xfer_lower_value;
  XferDataItem *m_xfer_upper_value;
  Scale   *m_scale_lower;
  Scale   *m_scale_upper;

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string  m_background;
  std::string  m_foreground;
  XferDataItem *m_xfer_background;
  XferDataItem *m_xfer_foreground;
};


class ColorSetItemAlways : public ColorSetItem
{
/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool equal( double val ){ return true; }
  virtual bool equal( const std::string &val ){ return true; };
  virtual bool invalid() const { return false; }
  virtual bool always() const { return true; }
};

class ColorSetRealItem : public ColorSetItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ColorSetRealItem( int what, double d )
    : m_lower_value( d )
    , m_upper_value( std::numeric_limits<double>::lowest() ){
    m_lower_what = what;
  }

  ColorSetRealItem( int what, XferDataItem *xfer, Scale *scale )
    : m_lower_value( 0 )
    , m_upper_value( std::numeric_limits<double>::lowest() ){
    m_lower_what = what;
    m_xfer_lower_value = xfer;
    m_scale_lower = scale;
  }

  ~ColorSetRealItem(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setUpperbound( int what, double val );
  virtual bool equal( double val );
  virtual bool invalid() const { return false; }
  virtual bool always() const { return false; }

  double getLowerbound();
  double getUpperbound();

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
 public:
  double   m_lower_value;
  double   m_upper_value;
};


class ColorSetStringItem : public ColorSetItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  ColorSetStringItem( int what, const std::string &s )
    : m_lower_value( s ){
    m_lower_what = what;
  }

  ColorSetStringItem( int what, XferDataItem *xfer ){
    m_lower_what = what;
    m_xfer_lower_value = xfer;
  }

  ~ColorSetStringItem(){}

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool setUpperbound( int what, std::string &val );
  virtual bool equal( const std::string &val );
  virtual bool invalid() const { return false; }
  virtual bool always() const { return false; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::string  m_lower_value;
  std::string  m_upper_value;
};

#endif
