
#if !defined(GUI_PRINTER_DIALOG_H)
#define GUI_PRINTER_DIALOG_H

#include <string>
#include <map>

#include "app/ReportGen.h"
#include "gui/HardCopyTree.h"
#include "gui/GuiEventData.h"

class GuiElement;
class GuiForm;
class HardCopyListener;
class GuiPulldownMenu;

class GuiPrinterDialog{
public:
  GuiPrinterDialog();
  virtual ~GuiPrinterDialog(){
    delete m_hardcopytree;
  }
  typedef std::map<std::string, HardCopyListener *>   HardCopyList;

  class MyEventData : public GuiEventData{
  public:
    MyEventData( ReportGen::Mode mode )
      : m_mode(mode){}
    ReportGen::Mode mode(){return m_mode;}
  private:
    ReportGen::Mode m_mode;
  };

public:
  virtual void initPrinterDialog() = 0;
  HardCopyFolder *getFolder( bool with_remaining = true );
  virtual void preview( HardCopyListener *hcl, GuiElement *parent ){ assert( false ); }
  virtual void createPrintObjectList( GuiElement *parent ) = 0;
  virtual void showDialog( HardCopyListener *, GuiElement *parent , GuiEventData *event, JobAction* jobAction=0 ) = 0;
  virtual void selectHardCopyObject( HardCopyListener * ) = 0;

  HardCopyListener *getHardCopy( const std::string & );
  void insertHardcopy( const std::string &, HardCopyListener * );
  void removeHardcopy( HardCopyListener * );

  void installRemainingHardCopies();
  void createPrintMenu( GuiPulldownMenu *, bool preview = false );

  void setHardCopy( const std::string & );

  const HardCopyList &hardcopylist(){ return m_hardcopylist; }

  void setCurrentListener( HardCopyListener * hcl ){ m_currentListener = hcl; }
  HardCopyListener *getCurrentListener(){ return m_currentListener; }
  void setTree( HardCopyTree *tree ){ m_hardcopytree = tree; }
  HardCopyTree *tree(){ return m_hardcopytree; }
protected:
  GuiForm *dialog(){ return m_dialog; }
  void setDialog( GuiForm *d ){ m_dialog = d; }
private:
  bool                  m_print_menu_with_remaining_entries;
  HardCopyList          m_hardcopylist;
  int                   m_data_index;
  GuiForm              *m_dialog;
  HardCopyListener     *m_currentListener;
  HardCopyTree         *m_hardcopytree;
};

#endif
