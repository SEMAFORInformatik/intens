
#if !defined(VAR_STREAM_PARAMETER_H)
#define VAR_STREAM_PARAMETER_H

#include "streamer/DataStreamParameter.h"

//*************************************************
// VAR_STREAM_PARAMETER
//*************************************************

class VarStreamParameter : public DataStreamParameter{
 public:
  VarStreamParameter( XferDataItem *varDataItem, XferDataItem *dataItem,
					  StreamParameterGroup *parent, int level, char delimiter, bool locale );
  virtual ~VarStreamParameter();

  // public functions of DataStreamParameter
  virtual void setIndex( const std::string &name, int val );
  virtual void setIndex( int pos, int val );
  virtual void setValue( const std::string &v ){
    if( !checkCurrent() ) return; m_current->setValue( v );
  }
  virtual bool getValue( std::string &v )const{
    if( !checkCurrent() )
      return false;
    return m_current->getValue( v );
  }
  virtual void setValue( double v ){
    if( !checkCurrent() )
      return; m_current->setValue( v );
  }
  virtual bool getValue( double &v )const{
    if( !checkCurrent() )
      return false;
    return m_current->getValue( v );
  }
  virtual void setValue( const dComplex &v ){
    if( !checkCurrent() ) return;
    m_current->setValue( v );
  }
  virtual bool getValue( dComplex &v )const{
    if( !checkCurrent() )
      return false;
    return m_current->getValue( v );
  }
  virtual void setValue( int v ){
    if( !checkCurrent() ) return;
    m_current->setValue( v );
  }
  virtual bool getValue( int &v )const{
    if( !checkCurrent() ) return false;
    return m_current->getValue( v );
  }
  virtual bool setValue( const std::vector<int> &dims
			  , const std::vector<int>& vals, int level=-1 ){
    if( !checkCurrent() ) return false;
    return m_current-> setValue( dims, vals, level );
  }
  virtual bool setValue( const std::vector<int> &dims
			 , const std::vector<double>& revals
			 , const std::vector<double>& imvals
			  , const std::vector<int> &inds, int level =-1 ){
    if( !checkCurrent() ) return false;
    return m_current->setValue( dims, revals, imvals, inds, level );
  }
  virtual bool setValue( const std::vector<int> &dims
			  , const std::vector<dComplex>& cvals, int level =-1 ){
    if( !checkCurrent() )
      return false; return m_current-> setValue( dims, cvals, level );
  }
  virtual bool setValue( const std::vector<int> &dims
			  , const std::vector<std::string>& svals, int level=-1 ){
    if( !checkCurrent() ) return false;
    return m_current->setValue( dims, svals, level );
  }
  virtual std::string getFullName( bool withIndexes )const{
    if( !checkCurrent() ) return "";
    return m_current->getFullName( withIndexes );
  }
  virtual const std::string getLabel() const {
    if( !checkCurrent() ) return "";
    return m_current->getLabel();
  }

  // pubic functions of StreamParameter
  virtual bool read( std::istream & istr ){
    if( !checkCurrent() ) return false;
    return m_current->read( istr );
  }
  virtual bool write( std::ostream & ostr ){
    if( !checkCurrent() ) return true;
    return m_current->write( ostr );
  }
  virtual void resetContinuousUpdate(){
    if( !checkCurrent() ) return;
    m_current-> resetContinuousUpdate();
  }
  virtual bool setContinuousUpdate( bool flag ){
    if( !checkCurrent() ) return false;
    return m_current->setContinuousUpdate( flag );
  }
  virtual void clearRange( bool incl_itemattr=false ){
    if( !checkCurrent() )
    return; m_current->clearRange( incl_itemattr );
  }
  virtual size_t getDimensionSize(int ndim) const{
    if( !checkCurrent() ) return 0;
    return m_current->getDimensionSize(ndim);
  }
  virtual bool getFrameBoxSize( int &x, int &y ){
    if( !checkCurrent() ) return false;
    return m_current->getFrameBoxSize( x, y );
  }
  virtual void setFrameBoxSize( const int x, const int y ){
    if( !checkCurrent() ) return;
    m_current->setFrameBoxSize( x, y );
  }
  virtual bool isValid(bool validErrorMsg=false){
    if( !checkCurrent() ) return false;
    return m_current->  isValid(validErrorMsg); }

  virtual std::string getName()const{
    if( !checkCurrent() ) return "";
    return m_current->getName();
  }
  virtual void putValues( StreamDestination *dest, bool transposed ){
    if( !checkCurrent() ) return;
    m_current->putValues( dest, transposed );
  }
  virtual void getValues( StreamSource *src ){
    m_current->getValues( src );
  }
  virtual bool isScalar() const {
    if( !checkCurrent() ) return false;
    return m_current->isScalar(); }
  virtual void resolveVars();

  // pubic functions of GuiIndexListener
  virtual bool acceptIndex( const std::string &s, int i){
    if( !checkCurrent() ) return false;
    return m_current->acceptIndex( s, i );
  }
  virtual bool isIndexActivated() {
    if( !checkCurrent() ) return false;
    return m_current->isIndexActivated();
  }

  // public functions
  void addLevel( const std::string &name );
  void setItem( XferDataItem *xfer ){ m_item = xfer; }
  void setConverterValues( int width, int prec, Scale *scale, bool tsep, bool tlocale ){
    m_length = width;
    m_prec = prec;
    m_scale = scale;
    m_tsep = tsep;
	setLocale(tlocale);
  }

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

 private:
  bool checkCurrent() const{ return m_current != 0; }
  DataStreamParameter  *m_current;
  StreamParameterGroup *m_parent;
  std::string           m_suffix;
  XferDataItem         *m_item;
  int                   m_length;
  int                   m_prec;
  Scale                *m_scale;
  bool                  m_tsep;
  //  char                  m_delimiter;
};
#endif
