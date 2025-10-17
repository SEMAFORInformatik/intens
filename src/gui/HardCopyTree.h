
#if !defined(HARDCOPY_TREE_H)
#define HARDCOPY_TREE_H

#include "app/HardCopyListener.h"

class HardCopyTree;
class GuiPulldownMenu;
class HardCopyFolder;
class MotifHardCopyElement;
class QtHardCopyElement;
class GuiQtElement;
class GuiMotifElement;
class GuiEventData;

class HardCopyElement
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  HardCopyElement( const std::string &label, HardCopyFolder *parent )
    : m_label( label )
      , m_parent( parent ) {}
  virtual ~HardCopyElement() {}

  typedef std::list<HardCopyElement *> ElementList;
/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual void createMenu( GuiPulldownMenu *menu, GuiEventData *event = 0 ) = 0;
  virtual void makeVisible() = 0;
  virtual MotifHardCopyElement *getMotifHardCopyElement(){ assert( false );return 0; }
  virtual QtHardCopyElement *getQtHardCopyElement(){ assert( false );return 0; }

  const std::string &label(){ return m_label; }
  HardCopyFolder *parent(){ return m_parent; }

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
  HardCopyFolder       *m_parent;
  std::string           m_label;
};

class HardCopyFolder : public HardCopyElement
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  HardCopyFolder( HardCopyTree *tree )
    : m_hctree(tree)
      , m_visible( false )
      , HardCopyElement( "", 0 ) {}
  HardCopyFolder( const std::string &label, HardCopyFolder *parent, HardCopyTree *tree )
    : HardCopyElement( label, parent )
      , m_visible( true )
    , m_hctree(tree) {
  }
  virtual ~HardCopyFolder() {}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual HardCopyFolder *addFolder( const std::string & ) = 0;
  virtual void addObject( const std::string &, HardCopyListener * ) = 0;
  virtual void addSeparator() = 0;

  void createMenu( GuiPulldownMenu *, GuiEventData *event = 0 );
  const ElementList &elements(){ return m_elements; }
  void addElement( HardCopyElement *e );
  HardCopyTree *tree(){ return m_hctree; }
  bool visible(){ return m_visible; }

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
  ElementList           m_elements;
  HardCopyTree         *m_hctree;
  bool                  m_visible;
};


class HardCopyObject : public HardCopyElement
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  HardCopyObject( const std::string &label
                , HardCopyFolder *parent
                , HardCopyListener *hdc
	        , HardCopyTree     *tree)
    : HardCopyElement( label, parent )
    , m_listener( hdc )
    , m_hctree(tree) {
  }
  virtual ~HardCopyObject() {}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual void setSelected( bool ) = 0;

  int getIndex() { return m_listener->getIndex(); }
  void createMenu( GuiPulldownMenu *, GuiEventData *event = 0 );
  HardCopyTree *tree(){ return m_hctree; }
  HardCopyListener *listener(){ return m_listener; }
/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
  HardCopyListener  *m_listener;
  HardCopyTree      *m_hctree;

};

class HardCopyTree{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  HardCopyTree(){}
  virtual ~HardCopyTree() {
    delete m_hardCopyFolder;
  }

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  void setHardCopyFolder( HardCopyFolder *f ){ m_hardCopyFolder = f; }
  HardCopyFolder *folder(){ return m_hardCopyFolder; }
  virtual void createMenu( GuiPulldownMenu *, GuiEventData *event = 0 );
  void addObject( const std::string &s, HardCopyListener *hcl ){
    m_hardCopyFolder->addObject( s, hcl );
  }
  virtual GuiQtElement *getQtElement(){assert( false );return 0; };
  virtual GuiMotifElement *getMotifElement(){assert( false );return 0; };
  void serializeXML(std::ostream &os, bool recursive = false);

/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
private:

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
  HardCopyFolder *m_hardCopyFolder;
};

#endif
