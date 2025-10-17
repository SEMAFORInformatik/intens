
#if !defined(GUI_FACTORY_INCLUDED_H)
#define GUI_FACTORY_INCLUDED_H

#include <iostream>

#include "app/HardCopyListener.h"

#include "gui/GuiElement.h"
#include "gui/GuiNavigator.h"
#include "gui/DialogFileSelection.h"
#include "gui/SimpleAction.h"
#include "gui/GuiIndexMenu.h"

#include "plot/PSPlot.h"

class GuiManager;
class Timer;
class GuiLabel;
class GuiButton;
class GuiForm;
class GuiApplHeader;
class GuiFolder;
class GuiMenubar;
class GuiPulldownMenu;
class GuiPopupMenu;
class GuiMenuButton;
class GuiMenuToggle;
class GuiSeparator;
class GuiMessageLine;
class GuiScrolledText;
class GuiScrolledlist;
class GuiScrolledlistListener;
class GuiButtonListener;
class GuiToggleListener;
class GuiEvent;
class GuiFieldgroup;
class GuiOrientationContainer;
class GuiButtonbar;
class GuiButtonListener;
class GuiEventData;
class GuiEventLoopListener;
class DialogCopyright;
class GuiCycleButton;
class GuiTable;
class GuiTableItem;
class GuiList;
class GuiFilterList;
class GuiFilterListListener;
class GuiDataField;
class DialogConfirmation;
class DialogWorkClock;
class DialogWorkClockListener;
class ConfirmationListener;
class InformationListener;
class FileSelectListener;
class XferDataItem;
class GuiPrinterDialog;
class WorkerListener;
class DialogUnmapListener;
class UserPasswordListener;
class GuiPixmap;
class GuiIndex;
class GuiCycleDialog;
class Gui2dPlot;
class GuiQWTPlot;
class ListPlot;
class GuiNavigator;
class Plot2dMenuDescription;
class Simpel;
class GuiImage;
class JobController;
class ServerSocket;
class IntensServerSocket;
class ClientSocket;
class Stream;
class GuiThermo;
class GuiTimeTable;
class DialogProgressBar;

/** Class GuiFactory
  provides an interface for creating gui objects without
  specifying their concrete classes
*/
class GuiFactory
{
public:
  GuiFactory() { s_factory = this; }

  virtual GuiManager *createManager() = 0;

  bool haveFolder();
  bool haveList();
  bool haveTable();
  bool have3dPlot();
  virtual bool have2dPlot()=0;

  virtual Timer              *createTimer( int interval, bool restart = false ) = 0;
  virtual void                createSimpleAction( SimpleAction::Object *obj ) = 0;
  /** Diese Funktion erstellt ein neues Form-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
      @param name Name des neu zu erstellenden GuiForm-Elements.
      @return Pointer auf das neue GuiForm-Element.
  */
  virtual GuiForm            *createForm( const std::string& name ) = 0;
  virtual GuiApplHeader      *createApplHeader( GuiElement *parent ) = 0;
  virtual GuiFolder          *createFolder( GuiElement *parent, const std::string & ) = 0;
  virtual GuiIndexMenu       *createIndexMenu( GuiElement *parent, GuiIndexMenu::HideFlag flag) = 0;
  virtual GuiMenubar         *createMenubar( GuiElement *parent ) = 0;
  virtual GuiPulldownMenu    *createPulldownMenu( GuiElement *parent, const std::string &name ) = 0;
  virtual GuiPopupMenu       *createPopupMenu( GuiElement *parent ) = 0;
  virtual GuiPopupMenu       *createNavigatorMenu( GuiElement *parent ) = 0;
  virtual GuiMenuButton      *createMenuButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event = 0, bool hide_disabled=false, const std::string& name="" ) = 0;
  virtual GuiMenuToggle      *createMenuToggle( GuiElement *parent, GuiToggleListener *listener, GuiEventData *event = 0 ) = 0;
  virtual GuiSeparator       *createSeparator( GuiElement *parent ) = 0;
  virtual GuiMessageLine     *createMessageLine( GuiElement *parent ) = 0;
  virtual GuiScrolledText    *createScrolledText( GuiElement *parent, const std::string &name ) = 0;
  virtual GuiScrolledlist    *createScrolledlist( GuiElement *parent, GuiScrolledlistListener *listener) = 0;
  virtual GuiLabel           *createLabel( GuiElement *parent, GuiElement::Alignment a ) = 0;
  virtual GuiButton          *createButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event=0 ) = 0;
  virtual GuiDataField       *createToggle( GuiElement *parent ) = 0;
  /** Diese Funktion erstellt ein neues Fieldgroup-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual GuiFieldgroup      *createFieldgroup( GuiElement *parent, const std::string &name) = 0;
  virtual GuiFieldgroup      *createFieldgroup( GuiElement *parent, GuiElement *at, const std::string &name) = 0;
  virtual GuiFieldgroupLine  *createFieldgroupLine(GuiElement *parent) = 0;
  virtual GuiOrientationContainer *createVerticalContainer( GuiElement *parent ) = 0;
  virtual GuiOrientationContainer *createHorizontalContainer( GuiElement *parent ) = 0;
  virtual GuiButtonbar       *createButtonbar( GuiElement *parent ) = 0;
  virtual DialogCopyright    *createDialogCopyright() = 0;
  virtual GuiCycleButton     *createCycleButton( GuiElement *parent ) = 0;
  /** Diese Funktion erstellt ein neues Table-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual GuiTable           *createTable( GuiElement *parent, const std::string &name ) = 0;
  virtual GuiTableItem       *createTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align ) = 0;
  virtual GuiTableItem       *createTableDataItem( GuiElement *parent ) = 0;
  virtual GuiTableItem       *createTableComboBoxItem( GuiElement *parent, const std::string& datasetName ) = 0;
  /** Diese Funktion erstellt ein neues List-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual GuiList            *createList ( GuiElement *parent, const std::string &name ) = 0;
  virtual GuiFilterList      *createFilterList ( GuiElement *parent, GuiFilterListListener &listener,
						 const std::string &name ) = 0;
  virtual GuiPrinterDialog   *createPrinterDialog() = 0;
  virtual GuiNavigator       *createNavigator( const std::string &id, GuiNavigator::Type navType ) = 0;
  virtual DialogWorkClock    *createDialogWorkClock( GuiElement *element
						     , DialogWorkClockListener *listener
						     , const std::string &title
						     , const std::string &msg ) = 0;

  /** Diese Funktion öffnet einen Confirmation Dialog
      @param par parent GuiElement
      @param title title string
      @param message text string
      @param listener ConfirmationListener pointer
      @param buttonText override button text
      @param cancelBtn boolean if true third button will be visible
      @param defaultBtn set default button: 1 => Fist, 2 => Second, 3 => Third
      @return Pointer auf das neue GuiForm-Element.
  */
  virtual GuiElement::ButtonType showDialogConfirmation(GuiElement *par
                                                        , const std::string &title
                                                        , const std::string &message
                                                        , ConfirmationListener *listener = 0
                                                        , const std::map<GuiElement::ButtonType,
                                                        std::string>& buttonText =std::map<GuiElement::ButtonType, std::string>()
                                                        , bool cancelBtn=false
                                                        , int defaultBtn=1) = 0;

  virtual bool showDialogWarning(GuiElement *
                                 , const std::string &
                                 , const std::string &
                                 , InformationListener * = 0 ) = 0;

  virtual bool showDialogInformation(GuiElement *
				     , const std::string &
				     , const std::string &
				     , InformationListener * = 0 ) = 0;

  virtual std::string showDialogTextInput(GuiElement *
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
					 , bool &ok
					 , ConfirmationListener* = 0 ) = 0;

  virtual std::string showDialogTextInputExt(GuiElement *elem
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
					 , const std::map<GuiElement::ButtonType, std::string>& buttonText
					 , GuiElement::ButtonType &buttonPressed
					 , ConfirmationListener* = 0 ) = 0;

  virtual bool showDialogFileSelection( GuiElement *element
					, const std::string &title
					, const std::string &filter
					, const std::string &directory
					, FileSelectListener *listener
					, DialogFileSelection::Type type
					, const std::vector<HardCopyListener::FileFormat> * const formats = 0
					, DialogFileSelection::Mode mode = DialogFileSelection::AnyFile
          , const std::string &basename = std::string()) = 0;
  virtual void showDialogUserPassword( const std::string &, const std::string &
				       , UserPasswordListener *, DialogUnmapListener *unmap = 0
				       , std::string errorMessage="" ) = 0;

  /** Diese Funktion liefert anhand der Informationen in der DataReference
      das richtige GuiDataField-Objekt. Die DataReference wird dabei
      mit dem gelieferten Objekt korrekt verknuepft.
  */
  virtual GuiDataField    *createDataField  ( GuiElement *, XferDataItem *, int prec = 0 ) = 0;
  /** Diese Funktion liefert ein GuiPixmap-Object. Die DataReference wird dabei
      mit dem gelieferten Objekt korrekt verknuepft.
  */
  virtual GuiPixmap       *createPixmap  ( GuiElement *parent, XferDataItem *dataitem ) = 0;
  virtual GuiIndex        *createGuiIndex( GuiElement *parent, const std::string &name ) = 0;
  /** Diese Funktion liefert ein GuiIndex-Object mit dem gewuenschten Namen.
      Falls kein Index mit diesem Namen existiert, wird der Pointer 0 geliefert.
      Falls der gefundene Index bereits installiert ist, wird ein Clon desselbigen
      geliefert.
  */
  virtual GuiIndex        *getGuiIndex( const std::string &name ) = 0;

  /** Diese Funktion erstellt ein neues 2dPlot-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual Gui2dPlot* create2dPlot( const std::string &, bool isPlot2d = false) = 0;
  /** Diese Funktion erstellt ein neues 3dPlot-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual Gui3dPlot* create3dPlot( GuiElement *, const std::string & ) = 0;

  // Plot staff
  /** Diese Funktion erstellt ein neues ListPlot-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  virtual ListPlot*        createListPlot( GuiElement *parent, const std::string &name ) = 0;
  virtual PSPlot*          createPSPlot( PSPlot::PSStream *stream ) = 0;

  virtual Plot2dMenuDescription *getPlot2dMenuDescription() = 0;


  virtual Simpel *newSimpelPlot( GuiElement *, const std::string & ) = 0;
  virtual Simpel *getSimpelPlot( const std::string & ) = 0;

  virtual GuiImage  *createImage( GuiElement *, const std::string &, int )=0;
  virtual GuiImage  *createLinePlot( GuiElement *, const std::string & )=0;
  virtual GuiThermo  *createThermo( GuiElement *, const std::string & )=0;
  virtual GuiTimeTable *createTimeTable( GuiElement *, const std::string & )=0;

  virtual void attachEventLoopListener( GuiEventLoopListener *lsnr ) = 0;
  virtual GuiCycleDialog  *createCycleDialog() = 0;

  /** Diese Funktion liefert das GuiScrolledText-Object für STD_WINDOW.
   */
  virtual GuiScrolledText *getStandardWindow() = 0;

  /** Diese Funktion liefert das GuiScrolledText-Object für LOG_WINDOW.
   */
  virtual GuiScrolledText *getLogWindow() = 0;
  virtual GuiScrolledText *getHtmlWindow() = 0;

  /** Diese Funktion liefert das TopLevel Element (=> GuiManager::Instance())
   */
  GuiElement        *getTopLevelElement();
  static GuiFactory *Instance(){ return s_factory;}

  void update( GuiElement::UpdateReason, bool omitAfterUpdateForms=false );

  virtual void installDialogsWaitCursor(GuiDialog* diag_modal=0) = 0;
  virtual void removeDialogsWaitCursor(GuiDialog* diag_modal=0) = 0;
  virtual DialogProgressBar *getDialogProgressBar() = 0;
  virtual GuiElement *createProgressBar( GuiElement *, const std::string & ) = 0;

  virtual ServerSocket *createServerSocket( IntensServerSocket *server, int port ) = 0;
  virtual ClientSocket *createClientSocket( const std::string &header
					    , Stream *out_stream
					    , Stream *in_stream )  = 0;
  virtual bool doCopy(Stream *out_stream, GuiElement* elem)  = 0;
  virtual bool doPaste(Stream *in_stream)  = 0;
  virtual char getDelimiter()  = 0;

  /** Replace old GuiElement with new GuiElemnt, including clones
   */
  virtual bool replace( GuiElement *old_el, GuiElement *new_el ) = 0;

 private:
  static GuiFactory * s_factory;
};
#endif
