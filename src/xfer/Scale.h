
#ifndef SCALE_H
#define SCALE_H

#include "datapool/DataPool.h"
#include "xml/Serializable.h"

class GuiIndexListener;


class Scale: public Serializable
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  Scale();
  Scale( double value, char _operator = '*', double shift=0. );
  virtual ~Scale();
private:
  Scale( const Scale &scale );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual void setValue( double value ){ m_value = value; }
  virtual double getValue() const;
  virtual void setShift( double shift ){ m_shift = shift; }
  virtual double getShift() const;
  virtual void setIndex( int wildcardNumberBackwards, int value ){}
  virtual bool isUpdated( TransactionNumber trans, bool gui = false ){ return false; }
  virtual DATAAttributeMask getAttributes( TransactionNumber t ){ return 0; }
  virtual Scale *copy();
  void setValue( char _operator ){ m_operator = _operator; }

  virtual void registerIndex( GuiIndexListener *listener ){}
  virtual void unregisterIndex( GuiIndexListener *listener ){}
  virtual bool acceptIndex( const std::string &, int ){ return true; }
  virtual bool setIndex( const std::string &, int ){ return false; }

  void marshal( std::ostream &os );
  Serializable *unmarshal( const std::string &element, const XMLAttributeMap &attributeList );
  void setText( const std::string &text );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual Scale &operator=( const Scale &scale );

/*=============================================================================*/
/* protected Data                                                              */
/*=============================================================================*/
protected:
  char   m_operator;
  double m_value;
  double m_shift;
};

#endif
