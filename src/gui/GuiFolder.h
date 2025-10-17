
#if !defined(GUI_FOLDER_H)
#define GUI_FOLDER_H

#include "gui/GuiElement.h"
#include "gui/GuiFolderGroup.h"

class GuiOrientationContainer;

class GuiFolder{

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiFolder()
    : m_button_placement( GuiElement::align_Top )
      , m_label_orientation( GuiElement::orient_Default )
      , m_tab_stretched( false )
      , m_tab_expandable( false )
      , m_tab_movable( false )
      , m_hideButtons( false )
      , m_showAlwaysButtons( false ) {
  }
  virtual ~GuiFolder();
protected:
  GuiFolder( const GuiFolder &fd );

/*=============================================================================*/
/* member functions of GuiFolder                                               */
/*=============================================================================*/
public:
  virtual GuiOrientationContainer *addFolderPage(const std::string &name,
                                                 const std::string pixmap, bool hidden=false) = 0;
  virtual bool PageIsActive( int ) = 0;
  virtual void activatePage( int page, GuiFolderGroup::OmitMap omit_map, bool force ) = 0;
  GuiElement* getPage( int page) ;
  /** only used for folder with visible tabs */
  virtual void hidePage( int ) = 0;
  virtual bool isHiddenPage( int ) = 0;
  virtual GuiElement *getElement() = 0;

  void hideButtons(){ m_hideButtons=true; }
  bool IsHideButton(){ return m_hideButtons; }
  void showAlwaysButtons(){ m_showAlwaysButtons=true; }
  bool isShowAlwaysButton(){ return m_showAlwaysButtons; }
  void setButtonPlacement( GuiElement::Alignment );
  void setLabelOrientation( GuiElement::Orientation );
  void setTabStretch( bool );
  bool tabStretched();
  void setTabExpandable( bool );
  bool tabExpandable();
  void setTabMovable( bool );
  bool tabMovable();
  int NumberOfPages();
  virtual bool isFolderClonedBy(GuiFolder*) = 0;
  /** get hidden page index (increased by previously hidden tabs)
   */
  virtual void getHiddenIndex( int& page ) = 0;

protected:
  void setTabOrder();
  void unsetTabOrder();
  void update( GuiElement::UpdateReason );
  virtual void draw(){}
  bool replace( GuiElement *old_el, GuiElement *new_el );
  void serializeXML(std::ostream &os, bool recursive = false);
  void serializePage( std::ostream &os, const std::string &pageId );
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated=false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated=false);
#endif

/*=============================================================================*/
/* member Data                                                                */
/*=============================================================================*/
protected:
  GuiNamedElementList      m_elements;
  GuiElement::Alignment    m_button_placement;
  bool                     m_hideButtons;
  bool                     m_showAlwaysButtons;

  std::vector<GuiElement*>  m_clonedFolder;
private:
  GuiElement::Orientation  m_label_orientation;
  bool                     m_tab_stretched;
  bool                     m_tab_expandable;
  bool                     m_tab_movable;
};

#endif
