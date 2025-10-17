
#if !defined(GUI_QWT_THERMO_H)
#define GUI_QWT_THERMO_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiThermo.h"

#include <qwt_thermo.h>

class QVBoxLayout;

class GuiQwtThermo : public GuiQtElement, public GuiThermo
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  GuiQwtThermo( GuiElement *parent, const std::string &name );
  GuiQwtThermo( const GuiQwtThermo & );
  virtual ~GuiQwtThermo();

private:
  GuiQwtThermo &operator=( const GuiQwtThermo &i );

  /*=============================================================================*/
  /* public member functions of GuiElement                                       */
  /*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Thermo; }
  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual bool destroy();
  virtual QWidget* myWidget();

  virtual GuiElement * getElement() { return this; }
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;

  virtual void getSize( int &x, int &y );
  virtual void update( UpdateReason );
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
public:

  virtual void setScaleFormat( const std::string& format );
  /*=============================================================================*/
  /* private member functions                                                    */
  /*=============================================================================*/
private:
  void setColor( double value );

  /*=============================================================================*/
  /* private members                                                             */
  /*=============================================================================*/
private:
  QwtThermo              *m_thermo;
  QWidget                *m_frame;
  QLabel                 *m_label;
  QLabel                 *m_unit;
  char          m_fmt;
  int           m_prec;
  QColor                  m_bgColorSave;
  std::vector<GuiQwtThermo*> m_clonedThermo;
};

#endif
