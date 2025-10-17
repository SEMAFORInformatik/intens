
#if !defined(GUI_QT_FOLDER_UNDO_ELEMENT_H)
#define GUI_QT_FOLDER_UNDO_ELEMENT_H

#include "gui/qt/GuiQtFolder.h"
#include "datapool/DataTTrailElement.h"

class GuiQtFolder;
class DataPool;


class GuiQtFolderUndoElement : public DataTTrailElement
{
/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum Action {
    SWITCH
  , MAP
  , UNMAP
  };

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFolderUndoElement( GuiQtFolder *fol, int new_page, int old_page, Action a );
  virtual ~GuiQtFolderUndoElement();
private:
  GuiQtFolderUndoElement();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual DataTTrailElement::Typ typ() const { return ACTION; }
  virtual bool isEqual( DataTTrailElement &el ) { return false; }
  virtual bool isEqual( DataItem *item ) { return false; }
  virtual bool restore( DataPool &dp );
  virtual void print( std::ostream &ostr ) const;
  virtual const std::string name() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private members                                                             */
/*=============================================================================*/
private:
  GuiQtFolder *m_folder;
  int          m_new_page;
  int          m_old_page;
  Action       m_action;

};


class GuiQtFolderUndo
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiQtFolderUndo( GuiQtFolder *fol, int new_page, int old_page );
  virtual ~GuiQtFolderUndo();
private:
  GuiQtFolderUndo();
  GuiQtFolderUndo( const GuiQtFolderUndo & );
public:
  void setAction( GuiQtFolderUndoElement::Action a ){ m_action = a; }
private:
  static bool  s_activ;

  bool         m_activated;
  GuiQtFolder *m_folder;
  int          m_new_page;
  int          m_old_page;
  GuiQtFolderUndoElement::Action m_action;
};

#endif
