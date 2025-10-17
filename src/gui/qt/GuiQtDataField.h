
#if !defined(GUI_QT_DATAFIELD_INCLUDED_H)
#define GUI_QT_DATAFIELD_INCLUDED_H

#include <QColor>

#include "gui/qt/GuiQtElement.h"
#include "xfer/XferDataParameter.h"

#include "gui/GuiDataField.h"

class XferDataItem;
class XferDataItemIndex;
class JobFunction;
class GuiEnterListener;
class ColorSet;
class QLineEdit;
class QColor;

/** Das GuiMotifDataField ist die Basisklasse aller GuiElemente, welche mit einer
    Datenvariablen verbunden sind, welche auf der Oberfläche gezeigt wird.

    @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtDataField.h,v 1.26 2006/04/13 07:18:42 amg Exp $
*/
class GuiQtDataField : public GuiQtElement
		     , public GuiDataField
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtDataField( GuiElement *parent, const std::string name="" );
  GuiQtDataField( const GuiQtDataField &field );

  virtual ~GuiQtDataField();
  GuiElement* getElement() { return this; }
  GuiDataField* getDataField() { return this; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  void setValidator( QLineEdit *editor );
  virtual bool setPrecision( int ) { return false; }
  virtual bool setThousandSep() { return false; }
  virtual bool setScalefactor( Scale *scale ){ return false; }
  virtual bool setLength( int );
  virtual int getLength();
  virtual bool cloneableForFieldgroupTable() { return GuiDataField::cloneableForFieldgroupTable(); }
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &, int );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);

  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::DataField* eles, bool onlyUpdated = false);
#endif
  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap );
  virtual void updateWidgetProperty();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
public:
  /**
     @return false: default color from ini-File, true if colorset/colobit, locked, ... (modifiable colors)
   */
  bool getTheColor( QColor &background,
		    QColor &foreground,
		    int &dark_fac,
		    bool useColorSetOnly = false );
  static bool getNamedColors( const std::string &_background,
                              const std::string &_foreground,
                              QColor &background,
                              QColor &foreground );
  virtual void getColorStrings( std::string &background,
                                std::string &foreground );
  virtual void doFinalWork();
  static bool getStandardColor(XferDataParameter& m_param, QColor& background, QColor& foreground, bool useColorSetOnly = false);
protected:
  void setMyColors();
  bool getMyColors( QColor &background, QColor &foreground );
  std::string getColorString( QColor &color );
  virtual bool setInputValue() { return false; }
  virtual void EndOfWorkFinished();

/*=============================================================================*/
/* private definitions                                                         */
/*=============================================================================*/
/*=============================================================================*/
/* private  Data                                                               */
/*=============================================================================*/
private:
  int         m_length;
  QColor      m_background;
  QColor      m_foreground;
protected:
  std::string m_css_name;
};

#endif
