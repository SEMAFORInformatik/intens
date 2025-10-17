
#if !defined(GUI_QT_TEXTFIELD_INCLUDED_H)
#define GUI_QT_TEXTFIELD_INCLUDED_H

#include <qlineedit.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qcursor.h>
#include "gui/qt/GuiQtDataField.h"

class QDate;
class QWidget;
class QComboBox;
class QStackedWidget;


#define SELF_MADE_VALIDATION_TILL 0x050000


/** Ein GuiQtTextfield-Objekt beinhaltet ein einzeiliges Ein- und/oder Ausgabefeld
    für eine Datapool-variable.

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiQtTextfield.h,v 1.44 2008/07/22 06:00:18 amg Exp $
*/
class GuiQtTextfield : public GuiQtDataField
{
  Q_OBJECT
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtTextfield( GuiElement *parent );
  GuiQtTextfield( const GuiQtTextfield &textfield );

  virtual ~GuiQtTextfield(){}
private:
  GuiQtTextfield &operator=( const GuiQtTextfield & );

/*=============================================================================*/
/* private classes (MyQDateTimeEdit, MyQDateEdit, MyQTimeEdit, MyQComboBox)    */
/*=============================================================================*/
 private:

  class MyQDateTimeEdit : public QDateTimeEdit {
  public:
    MyQDateTimeEdit( QWidget* parent, GuiQtTextfield *e );
    virtual ~MyQDateTimeEdit() {}

    void    setValid(bool b) { b_valid = b; }
    virtual QString textFromDateTime(const QDateTime& datetime) const;
  private:
    bool    isValid() const { return b_valid; }
    // virtual member functions of QDateEdit
    virtual void contextMenuEvent( QContextMenuEvent* );
    virtual void enterEvent ( QEnterEvent* );
    virtual void leaveEvent ( QEvent* );
    virtual void keyPressEvent(QKeyEvent* );

  private:
    GuiQtTextfield *m_textfield;
    bool  b_valid;
  };

  class MyQDateEdit : public QDateEdit {
  public:
    MyQDateEdit( QWidget* parent, GuiQtTextfield *e );
    virtual ~MyQDateEdit() {}
    void    setEditMode() { b_editmode = true; }
    void    setValid(bool b) { b_valid = b; }
    bool    isValid() const { return b_valid; }

  private:
    // virtual member functions of QDateEdit
    virtual void enterEvent ( QEnterEvent* );
    virtual void leaveEvent ( QEvent* );
    virtual void keyPressEvent(QKeyEvent* );
    virtual void focusOutEvent( QFocusEvent* );
    virtual void wheelEvent ( QWheelEvent * event ) {} //ignore
    virtual QString textFromDateTime(const QDateTime& datetime) const;
    //    virtual QDateTime dateTimeFromText(const QString& str) const;
    virtual QValidator::State validate ( QString & input, int & pos ) const;
    virtual  void fixup ( QString & input ) const;
  public:
    static QChar getDelimeter() { return s_delimeter; }
    static QString getDisplayFormat() { return s_displayFormat; }
    QLineEdit * lineEdit () const { return QDateEdit::lineEdit(); }

  private:
    GuiQtTextfield *m_textfield;
    bool  b_editmode;
    bool  b_valid;
    QRegularExpressionValidator* m_regv;
    static QString            s_lastEditString;
    static QValidator::State  s_lastEditState;
    static QChar              s_delimeter;
    static QString            s_displayFormat;
  };

  class MyQTimeEdit : public QTimeEdit {
  public:
    MyQTimeEdit( QWidget* parent, GuiQtTextfield *e)
      : QTimeEdit( parent ), m_textfield( e ), b_valid( false ) {
      setDisplayFormat("hh:mm");
    }
    virtual ~MyQTimeEdit() {}
    void    setValid(bool b) { b_valid = b; }
    bool    isValid() const { return b_valid; }

  private:
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void show();
    virtual void keyPressEvent(QKeyEvent* e);
  private:
    GuiQtTextfield *m_textfield;
    bool  b_valid;
  };

  class MyQComboBox : public QComboBox {
  public:
    MyQComboBox( GuiQtTextfield *e );
    virtual ~MyQComboBox() {}
    void setFont( const QFont& font );
    // virtual member functions of QComboBox
    virtual bool event(QEvent* e);
    virtual void enterEvent ( QEnterEvent * );
    virtual void leaveEvent ( QEvent * );
    virtual void wheelEvent(QWheelEvent* e);
  private:
    GuiQtTextfield *m_textfield;
  };

/*=============================================================================*/
/* public member functions of GuiElement                                       */
/*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Textfield; }
  virtual void create();
  virtual void manage() {}
  virtual bool destroy();
  virtual void enable();
  virtual void disable();
  virtual void update( UpdateReason );
  virtual QWidget* myWidget();
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone() { return new GuiQtTextfield( *this ); }
  virtual void serializeXML( std::ostream &os, bool recursive = false );
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

/*=============================================================================*/
/* public member functions of GuiDataField                                     */
/*=============================================================================*/
public:
  virtual bool setLength( int len );
  virtual int getLength(){ return m_length; }
  virtual bool setPrecision( int prec );
  int  getPrecision();
  virtual bool setScalefactor( Scale *scale );
  virtual Scale* getScalefactor();
  virtual bool setThousandSep();

/*=============================================================================*/
/* public member functions of ConfirmationListener                             */
/*=============================================================================*/
public:
  virtual void confirmYesButtonPressed();
  virtual void confirmNoButtonPressed();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual bool installDataItem( XferDataItem *dataitem );
  virtual XferDataItem *replaceDataItem( XferDataItem *dataitem );
  virtual GuiQtDataField *CloneForFieldgroupTable();

  void FinalWork( const std::string newValue );

  static void setCursor(QLineEdit* lineEdit, std::string& text, int old_retCursorPos=0);
  static bool isArrowKeyAllowed() { return m_arrowKeyAllowed; }

  void setRelativeCursorPos(int p) { m_relCursorPos = p; }
/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  virtual bool setInputValue();
  virtual void protect();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  void setColors();
  void setAlarmColors();
  XferParameter::InputStatus checkFormatPrivate( std::string newValue );
  void FinalWorkOk();

  // facility methods for date edit fields
  QWidget* createDateWidget();
  void setDate(std::string str, Qt::DateFormat format);
  bool getDate(std::string &str, Qt::DateFormat format);
  /**  @return current json object from combobox widget */
  std::string getCurrentJsonObjectComboBox();

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  private slots:
    void textEdited(const QString& str);
    void dateChanged();
    void timeChanged();
    void datetimeChanged();
    void lostFocus();
    void comboxBoxActivate(const QString& str);
    void dateSelectionChanged();
    void dateSelected(const QDate& date);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  int                     m_length;
  bool                    m_initialLength;
  QWidget*                m_dateEdit;
  QLineEdit*              m_lineEdit;
  QWidget*                m_textwidget;
  QComboBox*              m_combobox;
  QStackedWidget*         m_widgetStack;
  bool                    m_invertLabelAlignment;

  bool                    m_textChanged;
  int                     m_relCursorPos;
  // ein flag zum blocken der ArrowKeys, wenn ein Fehler aufgetreten ist
  static bool             m_arrowKeyAllowed;
  static int              m_oldCursorPos;

  friend class ArrowKeyLineEdit;
};

#endif
