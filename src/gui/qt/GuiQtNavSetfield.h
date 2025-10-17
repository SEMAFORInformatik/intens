
#ifndef GUI_QT_NAV_SETFIELD_H
#define GUI_QT_NAV_SETFIELD_H

#include "xfer/XferConverter.h"
#include "gui/qt/GuiQtNavTextfield.h"
#include "gui/qt/GuiQtNavSetfield.h"

class DataSet;

class GuiQtNavSetfield : public GuiQtNavTextfield
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
 public:
  GuiQtNavSetfield ( GuiElement *parent, XferDataItem *dataitem );
  virtual ~GuiQtNavSetfield();

 private:
  GuiQtNavSetfield( const GuiQtNavSetfield & );
  GuiQtNavSetfield &operator=( const GuiQtNavSetfield &textfield );

  /*=============================================================================*/
  /* public member functions of GuiDataField                                     */
  /*=============================================================================*/
 public:
  virtual bool FieldIsEditable();

  /*=============================================================================*/
  /* public member functions of GuiNavTextfield                               */
  /*=============================================================================*/
 public:
  virtual bool getFormattedValue( std::string &s );
  virtual void setWidth( int width );

  /*=============================================================================*/
  /* protected member functions                                                  */
  /*=============================================================================*/
 protected:
  virtual bool setInputValue();

  /*=============================================================================*/
  /* private member functions                                                  */
  /*=============================================================================*/
 private:
  bool installDataset( XferDataItem *dataitem );

  /*=============================================================================*/
  /* private data                                                                */
  /*=============================================================================*/
 private:
  DataSet                           *m_dataset;
  StringConverter                   *m_conv;
};

#endif
