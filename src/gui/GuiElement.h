
#if !defined(GUI_ELEMENT_INCLUDED_H)
#define GUI_ELEMENT_INCLUDED_H
#include <shared_mutex>

#include <vector>
#include <map>
#include <list>
#include <ctime>
#include <iostream>
#include <assert.h>

#include "GuiIndexListener.h"
#include "GuiSerializableXml.h"
#include "app/AppData.h"
#include "app/LSPItem.h"
#include "datapool/TransactionOwner.h"
#include "utils/JsonUtils.h"

#if HAVE_PROTOBUF
#include "protobuf/GuiElement.pb.h"
#include "protobuf/Message.pb.h"
#endif

class GuiElement;
class GuiDataField;
class GuiTable;
class GuiTableActionFunctor;
class GuiDialog;
class GuiMotifElement;
class GuiXMLElement;
class GuiQtElement;
class GuiButtonbar;
class GuiForm;
class GuiScrolledText;
class GuiFieldgroup;
class GuiFieldgroupLine;
class HardCopyListener;
class GuiIndex;
class GuiNavigator;
class GuiList;
class GuiFolder;
class Gui2dPlot;
class Plugin;
class GuiPopupMenu;
class GuiPulldownMenu;
class GuiMenuButton;
class JobAction;
class XferDataItem;
class XferDataParameter;
class Stream;
class BasicStream;
class JobEngine;

typedef std::vector<GuiElement *>                         GuiElementList;
typedef std::list< std::pair<std::string, GuiElement *> > GuiNamedElementList;
typedef std::map<std::string, GuiElement *>               GuiSortedElementList;
typedef std::map<std::string, std::string>                GuiValueList;
typedef long    TransactionNumber;    // Transactions of Datapool

/** Base class for all gui objects

    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
    @version $Id: GuiElement.h,v 1.81 2008/07/08 07:39:23 amg Exp $
 */

class GuiElement : public GuiIndexListener
                 , public TransactionOwner
                 , public LSPItem
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiElement( GuiElement *parent, const std::string &name="" );
  virtual ~GuiElement();
protected:
  GuiElement( const GuiElement &el )
    : m_parent( 0 )
    , m_id(++s_next_id)
    , m_enabled(el.m_enabled)
    , m_drag_disabled(el.m_drag_disabled)
    , m_popupMenu(el.m_popupMenu)
    , m_helptext( el.m_helptext )
    , m_name( el.m_name )
    , m_classname( el.m_classname )
    , m_alignment( el.m_alignment )
    , m_rotation( el.m_rotation )
    , m_helptextStream( el.m_helptextStream )
    , m_x ( el.m_x )
    , m_y ( el.m_y )
    , m_rowspan ( el.m_rowspan )
    , m_colspan ( el.m_colspan )
    , m_formZOrder ( el.m_formZOrder )
    , m_openedByForm( el.m_openedByForm )
    , m_lastGuiUpdated( 0 )
    , m_lastWebUpdated( 0 )
    , m_attributeChanged( el.m_attributeChanged )
    , m_visible( el.m_visible )
    , m_hidden( el.m_hidden )
    , m_isShownWebApi(el.m_isShownWebApi) {
  }
  void setFormZOrder(int order) { m_formZOrder = order; }

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum Alignment
  { align_Default = 0
  , align_Left
  , align_Right
  , align_Center
  , align_Top
  , align_Bottom
  , align_Stretch
  };
  enum Orientation
  { orient_Default = 0
  , orient_Horizontal
  , orient_Vertical
  , orient_Both
  };
  enum ElementType
  { type_Manager = 0
  , type_Main
  , type_Form
  , type_Header
  , type_Container
  , type_Fieldgroup
  , type_FieldgroupLine
  , type_Folder
  , type_Index
  , type_IndexMenu
  , type_Text
  , type_Textfield
  , type_OptionMenu
  , type_ComboBox
  , type_Toggle
  , type_RadioButton
  , type_FieldButton
  , type_Label
  , type_Pixmap
  , type_Separator
  , type_Void
  , type_Stretch
  , type_ScrolledText
  , type_Scrolledlist
  , type_Table
  , type_TableLine
  , type_TableDataItem
  , type_TableLabelItem
  , type_Menubar
  , type_PulldownMenu
  , type_PopupMenu
  , type_MenuButton
  , type_MenuToggle
  , type_Buttonbar
  , type_Button
  , type_CycleButton
  , type_Message
  , type_Plugin
  , type_3dPlot
  , type_2dXrtPlot
  , type_List
  , type_XbaeList
  , type_TextDialog
  , type_Navigator
  , type_NavDiagram
  , type_NavIconView
  , type_Simpel
  , type_ListPlot
  , type_QWTPlot
  , type_Slider
  , type_Image
  , type_Thermo
  , type_ProgressBar
  };
  enum MessageType
  { msg_Error
  , msg_Warning
  , msg_Information
  , msg_Help
  };
  enum FlagStatus
  { status_Undefined
  , status_ON
  , status_OFF
  };
  enum UpdateReason
  { reason_FieldInput
  , reason_Unit
  , reason_Process
  , reason_Cycle
  , reason_Cancel
  , reason_Always // for internal use
  };
  enum GuiStyle
  { style_30
  , style_31
  };
  enum ButtonType
  {
  // first button
    button_Yes
  , button_Ok
  , button_Save
  // second button
  , button_No
  , button_Apply
  , button_Open
  // third button
  , button_Discard
  , button_Cancel
  , button_Abort
  , button_None
  };
  enum ScrollbarType
  { scrollbar_undefined
  , scrollbar_OFF
  , scrollbar_AS_NEEDED
  , scrollbar_ON
  };
  enum GuiButtonType
  { button_PIXMAP
  , button_TEXT
  , button_TEXT_PIXMAP
  };
  enum GuiAxisType { yLeft, yRight, xBottom, xTop, zAxis };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion liefert den Typ des entsprechenden GuiElements.
      @return Gültiger Typ (siehe ElementType)
   */
  virtual ElementType Type() = 0;

  /** Die Funktion fordert das GuiElement auf, seinen Graphicteil zu erstellen und
      fuer den Einsatz bereit zu halten.
  */
  virtual void create() = 0;

  /** Die Funktion install() ruft für das GuiElement el die Funktion create() mit
      dem entsprechenden parent Widget auf und ruft anschliessend auch manage() auf.
  */
  virtual void install( GuiElement *el ) {}

  /** Diese Funktion fordert das GuiElement auf, die Grösse neu zu berechnen. Wenn es
      sich um einen Container handelt muss dieser das resize() weiterleiten.
  */
  virtual void resize() {}

  /** Die Funktion fordert das GuiElement auf, den erstellten Graphicteil (siehe
      create() ) zu zeichnen. (in der Regel XtManageChild() )
  */
  virtual void manage() = 0;

  /** Dies ist die umgekehrte Funktion von manage(). Das Element ist anschliessend
      nicht mehr sichtbar. Achtung: Diese Funktion ist nicht bei allen Elementen
      implementiert.
  */
  virtual void unmanage() {}

  /** Die Funktion fordert das GuiElement auf, sich sichtbar zu machen, oder dieses
      Visible-Attribute für später zu speichern.
  */
  virtual void map() { assert(false); }

  /** Dies ist die umgekehrte Funktion von map().
      Die Funktion fordert das GuiElement auf, sich unsichtbar zu machen, oder dieses
      Visible-Attribute für später zu speichern.
  */
  virtual void unmap() { assert(false); }

  /** Diese Funktion zerstört den Widget-Tree ab dem entsprechenden GuiElement und
      initialisiert das GuiElement neu.Achtung: Diese Funktion ist nicht bei allen Elementen
      implementiert.
  */
  virtual bool destroy() = 0;

  /** Ist das GuiElement gerade sichtbar?
   */
  virtual bool isShown() = 0;

  /** Setzt die Sichtbarkeit des GuiElements für das WebApi. Defaultwert ist true
   */
  void setShownWebApi(bool yes);

  /** Ist das GuiElement über das WebApi sichtbar?
   */
  bool isShownWebApi();

  /** Diese Funktion sucht ab dem entsprechenden GuiElement in der Baumstruktur das
      GuiElement mit dem Pointer old_el und ersetzt es durch das GuiElement new_el.
      Das alte GuiElement old_el wird durch unmanage() entfernt. Wird es nicht mehr
      verwendet, so soll es mit destroy() initialisiert werden.
      Achtung: Diese Funktion ist nicht bei allen Elementen implementiert.
  */
  virtual bool replace( GuiElement *old_el, GuiElement *new_el ) { return false; }

  /** Mit dieser Funktion kann jedes GuiElement dem Dialog, d.h. allen GuiForm-Objekten
      den WaitCursor setzen und auch wieder nehmen. Nur Aktionen mit einem exclusiven
      Grab sollen diese Funktion verwenden.
  */
  virtual void waitCursor( bool wait, void *installer )
    { if( m_parent != 0 ) m_parent->waitCursor( wait, installer ); }

  /** Diese Funktion veranlasst, dass das GuiElement sowie alle ihm untergeordneten
      GuiElemente wenn moeglich editierbar sind. Sie stellt nach einem vorangegangenen
      Aufruf von disable() die ursprüngliche Situation wieder her.
  */
  virtual void enable();

  /** Diese Funktion veranlasst, dass das GuiElement sowie alle ihm untergeordneten
      GuiElemente nicht mehr editierbar sind.
  */
  virtual void disable();

  /** Diese Funktion veranlasst, dass das GuiElement sowie alle ihm untergeordneten
      GuiElemente nicht mehr tragbar ist.
  */
  virtual void disable_drag();

  /** Das GuiElement gibt an, ob es in einer GuiElement-Hierarchie korrekt integriert,
      resp. installiert ist. Dies ist der Fall, wenn das Parent-Element gesetzt ist.
      @return true -> Parent-Element ist gesetzt.
  */
  virtual bool installed() { return m_parent != 0; }

  /** Mit addTabGroup() fügt sich ein Objekt in die Tabulator-Liste ein. Mit der
      Tab-Taste wird zur Laufzeit von einer TabGroup zur nächsten gesprungen und das
      entsprechende Feld erhält den Focus.
   */
  virtual void addTabGroup() {}

  /** Mit removeTabGroup() wird ein Objekt aus der Tabulator-Liste gelöscht.
    */
  virtual void removeTabGroup() {}

  /** Mit setTabOrder() werden alle untergeordneten Objekte in der Tabulator-Liste
      eingetragen (siehe addTabGroup() ).
   */
  virtual void setTabOrder() {}

  /** Mit unsetTabOrder() werden alle untergeordneten Objekte aus der Tabulator-Liste
      gelöscht (siehe removeTabGroup() ).
   */
  virtual void unsetTabOrder() {}

  /** Durch den Aufruf von grabFocus() nimmt sich das Objekt den Focus().
   */
  virtual void grabFocus() {}

  /** Diese Funktion liefert über die beiden Parameter die Grösse des
      gezeichneten Elements.
   */
  virtual void getSize( int &, int &, bool hint=true) = 0;
  /** Diese Funktion gibt die Grösse als LogMeldung aus. */
  virtual void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);

  /** Mit dieser Funktion wird ein Objekt innerhalb eines Containers
      positioniert. Der Container ruft diese Funktion auf.
      param @x1 x-Koordinate der Position oben links
      param @y1 y-Koordinate der Position oben links
      param @x2 x-Koordinate der Position unten rechts
      param @y2 y-Koordinate der Position unten rechts
      param @xAlign Justierung waagrecht
      param @yAlign Justierung senkrecht
  */
  virtual void setPosition( int x1, int x2, Alignment xAlign,
			    int y1, int y2, Alignment yAlign );

  /** Diese Funktion vergrössert die Länge eines GuiElements.
   */
  virtual void stretchWidth( int width ) {}

  /** Mit der Funktion setAttachment() wird ein Objekt inner eines GuiContainers
      positioniert (XmForm-Widget). Der Container ruft diese Funktion auf. Er
      arbeitet mit FractionBase (siehe Motif XmForm).
      param x1 Attachment rechts ( <0 => rechter Rand)
      param x2 Attachment links ( <0 => linker Rand)
      param y1 Attachment oben ( <0 => oberer Rand)
      param y2 Attachment unten ( <0 => unterer Rand)
  */
  virtual void setAttachment( int x1, int x2, int y1, int y2 ) {}

  /** Mit der Funktion setAlignment() wird ein Objekt innerhalb eines gewissen
      Bereichs justiert (z.B rechts, links oder mitte).
      param align (siehe Definition in GuiElement.h)
  */
  virtual void setAlignment( GuiElement::Alignment align );
  /** gibt den Alignment-Typ zurück
      @returns alignement
  */
  virtual GuiElement::Alignment getAlignment();

  /** Mit der Funktion setRowSpan wird die Zahl der Spalten gesetzt
      über welches das GuiElement sich ausbreitet.
      param rowspan (Zahl der Spalten)
  */
  virtual void setRowSpan( int rowspan ) { m_rowspan = rowspan; }
  /** gibt den Zahl der Spalten zurück, über welches sich das GuiElement ausbreitet.
      @returns Zahl der Spalten
  */
  virtual int getRowSpan() { return m_rowspan; }

  /** Mit der Funktion setColSpan wird die Zahl der Spalten gesetzt
      über welches das GuiElement sich ausbreitet.
      param colspan (Zahl der Spalten)
  */
  virtual void setColSpan( int colspan ) { m_colspan = colspan; }
  /** gibt den Zahl der Spalten zurück, über welches sich das GuiElement ausbreitet.
      @returns Zahl der Spalten
  */
  virtual int getColSpan() { return m_colspan; }

  /** Mit der Funktion setRotation() wird der Winkel der Rotation gesetzt.
	  Dies geschieht zusätzlich zum Alignment.
      (nur wenige GuiElement verwenden dieses Attribut)
  */
  virtual void setRotation(int rotation );
  /** gibt den Winkel der Rotation zurück
      @returns Rotationswinkel
  */
  virtual int getRotation();
  /** Mit der Funktion setStylesheet() wird das Stylesheet gesetzt.
   */
  virtual void setStylesheet(const std::string& stylesheet);
  /** gibt das Stylesheet zurück
      @returns Stylesheet
  */
  virtual const std::string&  getStylesheet();
  /** Mit der Funktion setHelptextStream wird der Stream für den Helptext gesetzt.
   */
  virtual bool setHelptextStream( const std::string &headerText );

  virtual void setScrollbar( ScrollbarType sb ) {}
  virtual void setScrollbar( Orientation o, ScrollbarType sb ) {}
  virtual void setAutoScroll() {}
  virtual void setExpandable( FlagStatus ) {}
  virtual void setOrientation( GuiElement::Orientation orientation ) {}
  virtual GuiElement::Orientation getDialogExpandPolicy() { return orient_Default; }
  virtual GuiElement::Orientation getContainerExpandPolicy() { return orient_Default; }
  virtual void setPanedWindow( FlagStatus ) {}
  virtual void setToggleStatus( bool state ) {}
  virtual bool withScrollbars() const;
  virtual bool withPanedWindow();
  virtual bool withFrame();
  virtual void setFrame( FlagStatus s ){}
  virtual void setTitle( const std::string &title ) {}
  virtual void attach( GuiElement * ){ assert(false); }
  virtual void front( GuiElement * ){ assert(false); }

  virtual void setUseRuler() {} // makes Parser happy

  virtual bool setLabel( const std::string & ) { return false; }
  virtual bool setPixmap( const std::string &, bool withLabel=false ) { return false; }
  /** Diese Funktion setzt die Grösse des gezeichneten Elements.
      Das macht nur in wenigen Fällen Sinn, etwa bei einem Pixmap
      oder einem Button.
  */
  virtual void setElementSize( int width, int height ) { }

  virtual void update( UpdateReason ) = 0;
  virtual void updateForms( UpdateReason );
  virtual void updateNamedElements( UpdateReason );
  virtual void manageForm();
  /// change indicator
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false) { return false; };

  virtual bool handleTableAction( GuiTableActionFunctor &func );

  GuiElement *myParent( ElementType type );
  GuiElement *myParentNotType( ElementType type );

  virtual void printMessage( const std::string &, MessageType, time_t delay = 2 );
  static void printMessage( GuiElement *elem, const std::string &, MessageType, time_t delay = 2 );
  virtual void clearMessage();
  void setHelptext( const std::string &text );
  virtual bool hasHelptext();
  virtual bool showHelptext();
  virtual void getHelptext( std::string &text );

  virtual bool cloneable() { return false; }
  virtual GuiElement *clone() { return 0; }
  virtual bool cloneableForFieldgroupTable() { return false; }
  virtual void getCloneList(std::vector<GuiElement*>& cList) const {}

  /**
     Ist das Element streamable?
   */
  virtual BasicStream *streamableObject() { return 0; }

  static bool isGuiStyle_30();
  static bool isGuiStyle_31();

  /** GuiElement wird als xml serialisert.
      param @os Output Stream
      param @recursive Recursiv
  */
  virtual void serializeXML(std::ostream &os, bool recursive = false)=0;
  /** GuiElement wird als json Objekt serialisert.
      param @jsonObj Resultat Json Objekt
      param @onlyUpdated nur GuiElemente auflisten, die seit dem letzten GuiUpdate geändert haben
  */
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res) { }
  static bool isVisibleElementType(ElementType type);
  static void lspWrite( std::ostream &ostr );

  /** Setzen der Layout Position, welches von einem GridLayout oder aehnlichem benutzt wird
      param x (x-Wert)
      param x (y-Wert)
  */
  virtual void setLayoutPosition( int x, int y )   { m_x = x; m_y = y; }
  /** Abfrage der Layout Position, welches von einem GridLayout  oder aehnlichem benutzt wird
      param x (x-Wert)
      param x (y-Wert)
  */
  virtual void getLayoutPosition( int &x, int &y ) { x = m_x; y = m_y; }

  virtual void setPosition( int x, int y ) {};
  virtual void getPosition( int &x, int &y ) { x = 0; y = 0; }

  virtual std::string getElementName(){ return ""; }

  virtual GuiMotifElement *getMotifElement() { assert( false ); return 0; }
  virtual GuiQtElement *getQtElement() { assert( false ); return 0; }
  virtual GuiXMLElement *getXMLElement(){ assert( false ); return 0; };
  virtual GuiButtonbar *getButtonbar() { return 0; }
  virtual GuiForm *getForm() { return 0; }
  virtual GuiNavigator *getNavigator(){ assert(false); return 0; }
  virtual GuiList *getList(){ assert(false); return 0; }
  virtual HardCopyListener *getHardCopyListener() { return 0; }
  virtual GuiIndex *getGuiIndex() { return 0; }
  virtual GuiDialog *getDialog() { return 0; }
  virtual GuiFolder *getFolder() { return 0; }
  GuiPopupMenu *newPopupMenu();
  virtual GuiPopupMenu *getPopupMenu(){ return m_popupMenu; }
  virtual GuiMenuButton *getMenuButton(){ return 0; }
  virtual GuiPulldownMenu *getPulldownMenu(){ return 0; }
  virtual GuiTable *getTable(){ return 0; }
  virtual Plugin *getPlugin() { return 0; }
  virtual GuiDataField* getDataField() { return 0; }
  virtual GuiFieldgroup* getFieldgroup() { return 0; }
  virtual GuiFieldgroupLine* getFieldgroupLine() { return 0; }
  GuiForm *getMyForm();
  virtual Gui2dPlot* get2dPlot() { return 0; }

  TransactionOwner *transactionOwner(){ return this; }

  std::string ownerId() const { return m_name; }

  /** write own settings (into a config file) */
  virtual void writeSettings() {}

  /** JSF Corba:: get all visible Datapool Values */
  virtual void getVisibleDataPoolValues( GuiValueList& vmap ) {}

  /// File save functions
  virtual JobAction *getSaveAction( XferDataItem * ) { return 0; }
  /// File save functions with file name
  virtual JobAction *getSaveAction( const std::string& filename ) { return 0; }

  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);
  virtual std::string defaultVariantMethod(const std::string& method,
                                           const Json::Value& jsonArgs,
                                           JobEngine *eng);

/*=============================================================================*/
/* static member functions                                                     */
/*=============================================================================*/
public:
  static GuiElement*      findElement( const std::string & );
  static GuiElement*      findElementId( const std::string & );
  static void             findElementType( GuiElementList& res, ElementType type );

  /** Diese Funktion liefert den Pointer auf das gewuenschte ScrolledText-Object.
      Falls kein ScrolledText mit diesem Namen existiert, wird der Pointer 0 geliefert.
   */
  static GuiScrolledText* getScrolledText( const std::string &name );

  /** Diese Funktion liefert den Pointer auf die gewuenschte Form. Falls keine
      Form mit diesem Namen existiert, wird der Pointer 0 geliefert.
   */
  static GuiForm*         getForm( const std::string &name );

  /** Diese Funktion teilt uns mit, ob bereits eine Main-Form existiert und liefert
      den Pointer Derselbigen.
   */
  static GuiForm*         getMainForm();

  /** Diese Funktion liefert den Typ als String des entsprechenden GuiElements.
      @return Typ als String
   */
  const std::string StringType();

  /** Diese Funktion liefert den Typ als protobuf enum des entsprechenden GuiElements.
      @return Typ als Protobuf enum
   */
#if HAVE_PROTOBUF
  const in_proto::GuiElement::Type ProtobufType();
#endif

  /** Diese Funktion liefert den ExpandType als String des entsprechenden GuiElements.
      @return ExpandType als String
   */
  const std::string StringExpandType();

  /** Diese Funktion liefert den AlignmentType als String des entsprechenden GuiElements.
      @return AlignmentType als String
   */
  static const std::string StringAlignmentType(Alignment align);

#if HAVE_PROTOBUF
  static const in_proto::GuiElement::Alignment ProtoAlignmentType(Alignment align);
  const in_proto::GuiElement::Orientation ProtoOrientationType();
#endif

  /** Diese Funktion schreibt die Export Properties in das json-Objekt.
   */
  void writeJsonProperties(Json::Value& jsonObj);

#if HAVE_PROTOBUF
  in_proto::GuiElement* writeProtobufProperties();
#endif
  /** Diese Funktion schreibt den DataType-Export Properties in das json-Objekt.
   */
  void writeDataTypeJsonProperties(Json::Value& jsonObj, XferDataItem *item, XferDataParameter* param);
#if HAVE_PROTOBUF
  in_proto::ValueInfo* writeDataTypeProtobufProperties(XferDataItem *item, XferDataParameter* param);
#endif

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Mit setParent kann das Parent-Element eines GuiElement neu gesetzt werden.
      Verschiedene Gui-Funktionen benoetigen diesen Link.
      @param parent Direkt übergeordnetes Parent-GuiElement.
  */
  void setParent( GuiElement *parent ) { m_parent = parent; }

  /** returns the parent GuiElement if there is one
  */
  GuiElement *getParent(){ return m_parent; }

  /** returns the name of the GuiElement */
  const std::string &getName() const { return m_name; }

  /** set class  name */
  void setClass(const std::string &classname);
  /** get class  name */
  const std::string &Class() const { return m_classname; }

  /** get enabled flag */
  virtual bool isEnabled() { return m_enabled; }
  virtual bool isDragDisabled() { return m_drag_disabled; }

  /** set visible flag, runtime use */
  void setHiddenFlag() {
    m_hidden = true;
  };

  /** get visible flag, runtime use */
  bool getHiddenFlag() { return m_hidden; };

  /** get transaction number of last gui update
   */
  TransactionNumber  LastGuiUpdated() { return m_lastGuiUpdated; }

  /** set transaction number of last gui update
   */
  void setLastGuiUpdated(TransactionNumber trans) { m_lastGuiUpdated = trans; }

  const std::string getElementId();
  const int getElementIntId();

  /** set attribute changed flag */
  void setAttributeChangedFlag(bool changed) { m_attributeChanged |= changed; }

  /** reset attribute changed flag */
  void resetAttributeChangedFlag() { m_attributeChanged = false; }

  /** get attribute changed flag */
  bool getAttributeChangedFlag() { return m_attributeChanged; }

  /** reset last web update, like a reason always or cycle
   */
  void ResetLastWebUpdated() { m_lastWebUpdated = 0; }

  /** get transaction number of last web update
   */
  TransactionNumber  LastWebUpdated() { return m_lastWebUpdated; }

  /** set transaction number of last web update
   */
  void setLastWebUpdated();

  /** only used  by webtens */
  const int getFormOrder() const { return m_formZOrder; }

  /** only used  by webtens */
  static void setTopForm(GuiForm* form);

  /** only used  by webtens */
  static std::string getTopFormName();

  /** get visible flag, runtime use */
  bool getVisibleFlag() { return m_visible; };

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  static GuiElement *searchNamedElement( GuiNamedElementList &, const std::string & );
  static int getCloneNumber() { return ++s_clone_number; }

  /** only used  by webtens */
  void acquireFormOrderNumber();
  /** only used  by webtens */
  void releaseFormOrderNumber();

  /** set visible flag, runtime use */
  void setVisibleFlag(bool visible) {
    m_attributeChanged |= (m_visible != visible);
    m_visible = visible;
  };

  /** update Scale instance of XferDataParameter */
  bool updateScale(XferDataParameter* param);

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  GuiElement       *m_parent;
  std::string       m_helptext;
  Stream           *m_helptextStream;
  Alignment         m_alignment;
  int               m_rotation;
  std::string       m_stylesheet;
  std::string       m_name;
  std::string       m_classname;
  int               m_id;
  bool              m_enabled;
  bool              m_drag_disabled;

  static int        s_clone_number;
  static int        s_next_id;
  static GuiStyle   s_gui_style;
  static GuiSortedElementList s_namedElementList;
  static GuiElementList s_elementList;// WEBAPI
  static GuiForm*   s_topForm;        // WEBAPI top form
  static int        s_topFormOrder;   // WEBAPI
  static int        s_maxFormOrder;   // WEBAPI
  int               m_formZOrder;     // WEBAPI
  GuiForm*          m_openedByForm;   // WEBAPI opened by form

  int               m_x;
  int               m_y;
  int               m_rowspan;
  int               m_colspan;
  TransactionNumber m_lastGuiUpdated;
  TransactionNumber m_lastWebUpdated;
  GuiPopupMenu     *m_popupMenu;

  // visible/attribute flag
  bool              m_visible;  // runtime flag
  bool              m_hidden;   // parser/init flag
  bool              m_attributeChanged;
  bool              m_isShownWebApi;
 };

#endif
