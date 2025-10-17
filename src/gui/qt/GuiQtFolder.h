
#if !defined(GUI_QT_FOLDER_H)
#define GUI_QT_FOLDER_H

#include <qobject.h>

#include "gui/GuiFolder.h"
#include "gui/qt/GuiQtElement.h"

class GuiOrientationContainer;
class QTabWidget;

class GuiQtFolder : public GuiQtElement, public GuiFolder{
  Q_OBJECT
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
public:
  GuiQtFolder( GuiElement *parent, const std::string &name );
  virtual ~GuiQtFolder();
private:
  GuiQtFolder( const GuiQtFolder &fd );

  /*=============================================================================*/
  /* private slots                                                               */
  /*=============================================================================*/
private slots:
  void currentChangedCB( int index );
  void tabMoved( int from, int to );

  /*=============================================================================*/
  /* public member functions of GuiElement                                       */
  /*=============================================================================*/
public:
  virtual GuiElement::ElementType Type() { return GuiElement::type_Folder; }
  virtual void create();
  virtual void manage();
  virtual void unmanage();
  virtual void enable();
  virtual void disable();
  virtual void update( UpdateReason );
  virtual void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual bool replace( GuiElement *old_el, GuiElement *new_el ){
    return GuiFolder::replace( old_el, new_el );
  }
  virtual QWidget* myWidget(){ return m_folder; }
  virtual bool cloneable() { return true; }
  virtual GuiElement *clone();
  virtual void getCloneList(std::vector<GuiElement*>& cList) const;

  virtual void setTabOrder();
  virtual void unsetTabOrder();
  /** Fragt nach der ExpandPolicy des QtElements.
   */
  virtual Qt::Orientations getExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Container.
   */
  virtual GuiElement::Orientation getContainerExpandPolicy();
  /** Fragt nach der ExpandPolicy des QtElements fuer den Dialog.
   */
  virtual GuiElement::Orientation getDialogExpandPolicy();
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated=false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif
  virtual void getVisibleElement(GuiElementList& res);
  virtual GuiFolder *getFolder() { return this; }
  virtual void writeSettings();
  void readSettings();

/*=============================================================================*/
/* public member functions of GuiFolder                                        */
/*=============================================================================*/
public:
  virtual GuiOrientationContainer *addFolderPage(const std::string &name,
                                                 const std::string pixmap, bool hidden=false);
  virtual bool PageIsActive( int );
  virtual void activatePage( int page, GuiFolderGroup::OmitMap omit_map, bool force );
  /** only used for folder with visible tabs */
  virtual void hidePage( int );
  virtual bool isHiddenPage( int );
  virtual GuiElement *getElement(){ return this; }
  virtual bool isFolderClonedBy(GuiFolder*);

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  /** get tab index (of m_elements) of wished page
   */
  int getTabIndex( int page );

  /** Die Funktion liefert die mit dem Index inxed gewünschte
      Seitennummer (page) des Folders.
   */
  int getTabPage( int index );

  /** get QWidget* und tabLabel of wished folder tab.
   */
  QWidget* getTabWidget(int page, std::string& tabLabel);

  /** add tab if previously removed
   */
  void  validateTab( int page );

  /** get hidden page index (increased by previously hidden tabs)
   */
  void  getHiddenIndex( int& page );


  int transferIndexFromMoved( int );
  int transferIndexToMoved( int );

/*=============================================================================*/
/* private members                                                             */
/*=============================================================================*/
private:
  QWidget       *m_folder;
  int            m_activePage;
  int            m_activeWebPage;
  bool           m_disabled_CB;

  GuiNamedElementList m_movedElements;
};


#endif
