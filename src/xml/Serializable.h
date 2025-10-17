
#if !defined(SERIALIZABLE_H)
#define SERIALIZABLE_H

#include <string>

class XMLAttributeMap;

/** Der Serializer instanziert und konfiguriert Objekte, welche in xml beschrieben sind.
*/

class XMLAttributeMap;

class Serializable {
public:
  virtual void marshal( std::ostream &os )=0;
  virtual Serializable * unmarshal( const std::string &element, const XMLAttributeMap &attributeList)=0;
  virtual void setText( const std::string &text )=0;
};

#endif
