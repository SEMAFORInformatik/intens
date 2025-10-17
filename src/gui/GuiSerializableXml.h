
#if !defined(GUISERIALIZABLE_H)
#define GUISERIALIZABLE_H

#include <iostream>

class GuiSerializableToXml {
public:
  virtual void serialize( std::ostream &os )=0;
};

#endif
