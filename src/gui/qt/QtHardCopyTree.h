
#if !defined(QT_HARDCOPY_TREE_H)
#define QT_HARDCOPY_TREE_H

#include "app/HardCopyListener.h"
#include "gui/HardCopyTree.h"
#include "gui/qt/GuiQtElement.h"

class QtHardCopyTree;
class GuiPulldownMenu;
class GuiEventData;

class QtHardCopyFolder : public HardCopyFolder{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  QtHardCopyFolder( HardCopyTree *tree )
    : HardCopyFolder( tree ){}
  QtHardCopyFolder( const std::string &label, HardCopyFolder *parent, HardCopyTree *tree )
    : HardCopyFolder( label, parent, tree ){
  }
  virtual ~QtHardCopyFolder() {}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual void makeVisible(){}
  virtual HardCopyFolder *addFolder( const std::string & );
  virtual void addObject( const std::string &, HardCopyListener * );
  virtual void addSeparator();

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
};

class QtHardCopyObject : public HardCopyObject
{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  QtHardCopyObject( const std::string &label
		    , HardCopyFolder *parent
		    , HardCopyListener *hdc
		    , HardCopyTree     *tree)
    : HardCopyObject( label, parent, hdc, tree ){
  }
  virtual ~QtHardCopyObject() {}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual void setSelected( bool ){}
  virtual void makeVisible(){}

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
};


class QtHardCopySeparator : public HardCopyElement{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  QtHardCopySeparator()
    : HardCopyElement( "", 0 ){}
  virtual ~QtHardCopySeparator() {}

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual void createMenu( GuiPulldownMenu *menu, GuiEventData *event = 0 );
  virtual void makeVisible(){}

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
};

class QtHardCopyTree : public HardCopyTree
		     , public GuiQtElement{
/*******************************************************************************/
/* Constructor / Destructor                                                    */
/*******************************************************************************/
public:
  QtHardCopyTree()
    : GuiQtElement( 0 ){
    setHardCopyFolder( new QtHardCopyFolder( this ) );
  }
  virtual ~QtHardCopyTree() {
  }

/*******************************************************************************/
/* public member functions                                                     */
/*******************************************************************************/
public:
  virtual GuiElement *getElement(){ return this; }
  virtual ElementType Type() { return type_Plugin; }
  virtual void create() {}
  virtual void manage() {}
  virtual void update( UpdateReason ) {}
  virtual QWidget* myWidget(){ return 0; }
  virtual void serializeXML(std::ostream &os, bool recursive = false);
/*******************************************************************************/
/* private member functions                                                    */
/*******************************************************************************/
private:

/*******************************************************************************/
/* private data                                                                */
/*******************************************************************************/
private:
};

#endif
