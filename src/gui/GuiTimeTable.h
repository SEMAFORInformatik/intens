
#ifndef GUITIMETABLE_H
#define GUITIMETABLE_H

class GuiElement;
class XferDataItem;

class GuiTimeTable
{
 public:
  GuiTimeTable() {}
  virtual ~GuiTimeTable() {}

  virtual void setSize( int x, int y ) = 0;
  virtual bool setFromStringDateTime( XferDataItem *dataitem ) = 0;
  virtual bool setToStringDateTime( XferDataItem *dataitem ) = 0;
  virtual bool setToStringTime( XferDataItem *dataitem ) = 0;
  virtual bool setAppointment( XferDataItem *dataitem ) = 0;

  virtual GuiElement *getElement() = 0;
};

#endif
