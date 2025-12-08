
#if !defined(_USERATTR_INCLUDED_H)
#define _USERATTR_INCLUDED_H

#include <string>
#include <vector>

#include "datapool/DataUserAttr.h"

class DataDictionary;
class DataReference;
class JobFunction;
class GuiPopupMenu;


class UserAttr : public DataUserAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  UserAttr();
  UserAttr( const UserAttr & );
  UserAttr( DataDictionary * pDict );
  virtual ~UserAttr();

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum GUItype {
     gui_kind_field = 0
    ,gui_kind_button
    ,gui_kind_slider
    ,gui_kind_toggle
    ,gui_kind_label
    ,gui_kind_combobox
    ,gui_kind_radiobutton
    ,gui_kind_progress
  };
  enum STRINGtype {
     string_kind_none = 0
    ,string_kind_date
    ,string_kind_time
    ,string_kind_datetime
    ,string_kind_value
    ,string_kind_password
  };
  typedef std::vector<std::string> TagList;

#define HIDEnothing       0L
#define HIDEhidden        1L
#define HIDEtransient     (1L<<1)
#define APPLYdbUnit       (1L<<2)

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual DataUserAttr * NewClass(DataDictionary *pDict) const;
  virtual DataUserAttr * copyClass() const;
  virtual DATAAttributeMask getAttributes() const;
  virtual void write( std::ostream &ostr, const std::vector<std::string> &attrs ) const;

  /** Diese Funktion wird beim erstellen eines DataItems aufgerufen. Sie hat die
      Möglichkeit, an den Attributen des DataItems Korrekturen vorzunehmen.
      @param item Referenz auf das DataItem-Objekt
   */
  virtual void fixupAttributes( DataItem &item );

  void SetEditable();
  void SetOptional();
  void SetLockable();
  void SetScalar();
  void SetCell();
  void SetHidden();
  void SetPersistent();
  void SetStringType(STRINGtype type);
  void SetWheelEvent();
  void SetNoDependencies();

  void setFunction( JobFunction *func );
  void setFocusFunction( JobFunction *func );
  JobFunction *getFunction();
  JobFunction *getFocusFunction();
  void setPopupMenu( GuiPopupMenu *, unsigned int idx=0 );
  GuiPopupMenu *getPopupMenu( unsigned int idx=0 );
  void setPopupDropMenu( GuiPopupMenu * );
  GuiPopupMenu *getPopupDropMenu() const;
  void setTag( const std::string &tag );
  bool hasTag( const std::string &tag );
  void clearTags();
  TagList getTagList();

  void SetDbAttr(const std::string &name);
  void SetDbUnit(const std::string &name);
  void SetDbTransient();
  void SetLabel(const std::string &label);
  void SetUnit(const std::string &unit);
  void SetPattern(const std::string &pattern);
  void SetHelptext(const std::string &text);
  void SetClassname(const std::string &name, const std::string &fullname=std::string());
  void SetPlaceholder(const std::string &text);
  void SetStylesheet(const std::string &stylesheet);
  void SetMaxOccurs(const int num);
  void SetDataSetName(const std::string &name, bool bIndexed=false );
  void setColorSetName( const std::string &name );
  void setColorPicker();
  void setNoColorBit();
  void SetButton();
  void SetSlider();
  void SetProgress();
  void SetRange( double min, double max ){
    m_min = min;
    m_max = max;
  }
  double getMin(){ return m_min; }
  double getMax(){ return m_max; }
  void SetStep( double step ){
    m_step = step;
  }
  double getStep(){ return m_step; }
  void SetToggle();
  void SetRadioButton();
  void SetCombobox();
  void SetLabelType();

  bool IsEditable() const;
  bool IsOptional() const;
  bool IsLockable() const;
  bool IsScalar()   const;
  bool IsCell()   const;
  bool IsHidden()   const;
  bool IsPersistent() const;
  STRINGtype StringType() const;
  bool hasWheelEvent() const;
  bool hasNoDependencies() const;
  bool IsDbTransient() const;

  /** Die Funktion zeigt anhand der Information im Parameter mask an, ab die Daten
      im DataItem serialisierbar sind.
      \param mask
      \return true => Daten sind serialisierbar
  */
  virtual bool isSerializable( const SerializableMask mask ) const;

  const std::string &Label( DataReference * );
  const std::string &DbAttr()const;
  const std::string &DbUnit()const;
  const std::string Unit(bool unitManagerFeature=true, DataReference* data_ref=0);
  const std::string &Pattern()const;
  const std::string &Helptext()const;
  const std::string &Classname(const std::string &fullname=std::string());
  const std::string &Placeholder()const;
  const std::string &Stylesheet()const;
  int  MaxOccurs()const;
  const std::string &DataSetName()const;
  bool DataSetIndexed()const;
  const std::string &ColorsetName() const;
  bool IsColorPicker() const;
  bool IsColorBit()const;
  bool IsField()const;
  void unsetInvalidEntry();
  bool hasInvalidEntry() const;
  bool IsButton()const;
  bool IsToggle()const;
  bool IsRadio()const;
  bool IsCombobox()const;

  GUItype GuiKind()const;

  void operator=( const UserAttr & );

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  std::string                 m_Helptext;
  std::string                 m_Label;
  std::string                 m_Unit;
  std::string                 m_pattern;
  std::string                 m_DataSetName;
  bool                        m_DataSetIndexed;
  std::string                 m_colorsetname;
  bool                        m_colorpicker;
  bool                        m_isColorBit;
  std::string                 m_dbattrname;
  std::string                 m_dbunitname;
  bool                        m_dbtransient;  // ignored for persistence operations
  // used by slider
  double                      m_min;
  double                      m_max;
  double                      m_step;

  bool                        m_Editable;
  bool                        m_Optional;
  bool                        m_Lockable;

  GUItype                     m_gui_type;
  JobFunction                *m_function;
  JobFunction                *m_focus_function;
  std::vector<GuiPopupMenu*>  m_popupMenu;
  GuiPopupMenu*               m_popupDropMenu;
  std::string                 m_className;
  std::map<std::string, std::string> m_classNameMap;
  std::string                 m_placeholder;
  std::string                 m_stylesheet;
  int                         m_maxOccurs;
  bool                        m_isScalar;
  bool                        m_isCell;
  bool                        m_isHidden;      // means: do not transfer it to matlab
  bool                        m_isPersistent;  // means: this item is stored in db
  TagList                     m_tags;
  STRINGtype                  m_typeString;      // if string should be handled as date, jsonValue
  bool                        m_hasWheelEvent; // means: textfield will process a wheel event
  bool                        m_hasNoDependencies; // target streams will not be checked
  bool                        m_isArrowKeys;   // if numbers should be changed by arrow_keys
  bool                        m_invalidEntry;  // Nur für OptionMenu & ComboBox
};

#endif // !defined(_USERATTR_INCLUDED_H)
