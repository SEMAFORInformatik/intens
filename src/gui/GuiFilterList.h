
#if !defined(GUI_FILTER_LIST_INCLUDED_H)
#define GUI_FILTER_LIST_INCLUDED_H

#include "gui/GuiFilterListListener.h"
/**
 * Mit der Klasse GuiFilterList koennen Daten in einer Liste dargestellt, sortiert
 * und ausgewaehlt werden.
 * @short Ein Listenobjekt
 * @author Daniel Kessler
 */

class GuiElement;

class GuiFilterList
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiFilterList( GuiFilterListListener &listener );
  virtual ~GuiFilterList(){}

  static std::string& getSearchMask() { return s_search_mask; }

/*=============================================================================*/
/* public member functions of GuiFilterList                                    */
/*=============================================================================*/
  virtual int getSelection() = 0;
  virtual GuiElement* getElement() = 0;
  virtual int rebuild() = 0;
  virtual void clear() = 0;

 private:
   GuiFilterList( const GuiFilterList & );
   GuiFilterList &operator=( const GuiFilterList & );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  void getListConf();
  void getListItems( GuiFilterListListener::RowsVector &rows );


  int getRowSize() { return  m_rowsConf.size(); }
  int getColSize() { return  m_colsConf.size(); }
  int getVisibleColums() { return m_visibleCols; }
  GuiFilterListListener::IntVector& getRowsConf() { return m_rowsConf; }
  GuiFilterListListener::IntVector& getColsConf() { return m_colsConf; }
  GuiFilterListListener& getListener() { return m_listener; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiFilterListListener             &m_listener;
  GuiFilterListListener::IntVector   m_rowsConf;
  GuiFilterListListener::IntVector   m_colsConf;

  int                   m_visibleCols;

  static std::string s_search_mask;
};

#endif
