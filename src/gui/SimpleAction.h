
#ifndef GUI_SIMPLE_ACTION_H
#define GUI_SIMPLE_ACTION_H

class SimpleAction
{
public:
  class Object
    {
    public:
      Object(){}
      virtual ~Object(){}
      virtual void start() = 0;
    };
  SimpleAction( SimpleAction::Object *obj ):m_object( obj ){}
protected:
  void remove() { delete this; }
  virtual ~SimpleAction(){ m_object->start(); }
private:
  Object   *m_object;
};

#endif
