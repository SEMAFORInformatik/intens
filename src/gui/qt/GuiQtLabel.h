
#if !defined(GUI_QT_LABEL_INCLUDED_H)
#define GUI_QT_LABEL_INCLUDED_H

#include "gui/qt/GuiQtElement.h"
#include "gui/GuiLabel.h"

class QLabel;
class QWidget;
class QComboBox;
#include <qstringlist.h>


class GuiQtLabel : public GuiQtElement, public GuiLabel
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtLabel( GuiElement *parent );
  virtual ~GuiQtLabel();
protected:
  GuiQtLabel( const GuiQtLabel &label );

/*=============================================================================*/
/* public member functions of GuiQtElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Label; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &, int );
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtLabel( *this ); }
  virtual GuiElement *getElement() { return this; }
  virtual void setOrientation( GuiElement::Orientation orientation ) { m_orientation = orientation; };
  /** Get ExpandPolicy of QtElements inside Dialog.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();

/*=============================================================================*/
/* public member functions of GuiField                                         */
/*=============================================================================*/
public:
  virtual bool setLabel( const std::string & );
  virtual bool setPixmap( const std::string &, bool withLabel=false );

  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setTitle( const std::string & );
  void setIndexed() { m_indexed = true; }
  const std::string getLabel() { return m_label; }
  std::string getSimpleLabel();
  bool isLabelPixmap() { return m_label_pixmap; }

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setLabelString( const std::string & );
  bool isMultiFont();
  bool setLabelList();
  bool isMultiLine( const QString &s );

private slots:
  void comboxBoxActivate(const QString& str);
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  std::vector<QLabel *>  m_QLabelVec;
  QStringList            m_labelList;
  QWidget               *m_labelwidget;
  QComboBox*             m_combobox;

  int m_firstSeparatorPos;       // first_multistringSeparatorPos

  std::string  m_label;
  bool         m_label_pixmap;
  bool         m_indexed;
  bool         m_use_titlefont;
  Orientation  m_orientation;
  UserAttr    *m_userattr;
  std::string  m_userattr_fullname;
};

#endif
