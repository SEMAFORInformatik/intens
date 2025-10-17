
#if !defined(GUI_QT_FACTORY_INCLUDED_H)
#define GUI_QT_FACTORY_INCLUDED_H

#include "gui/GuiFactory.h"
#include "gui/DialogFileSelection.h"

class GuiElement;
class GuiLabel;
class GuiButton;
class GuiForm;
class GuiFieldgroup;
class DialogCopyright;
class GuiCycleButton;
class GuiTable;
class GuiPulldownMenu;
class GuiIndex;
class GuiQWTPlot;
class DialogWorkClock;
class GuiProgressBar;

/** Class GuiQtFactory
    provides a class for creating qt gui objects
*/
class GuiQtFactory : public GuiFactory {
public:
  GuiQtFactory();

  virtual GuiManager *createManager();

  virtual bool have2dPlot(){ return true; }

  virtual void attachEventLoopListener( GuiEventLoopListener *lsnr );

  virtual Timer              *createTimer( int interval, bool restart = false );
  virtual void                createSimpleAction( SimpleAction::Object *obj );
  /** Diese Funktion erstellt ein neues Form-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
      @param name Name des neu zu erstellenden GuiForm-Elements.
      @return Pointer auf das neue GuiForm-Element.
  */
  virtual GuiForm         *createForm       ( const std::string& name );
  virtual GuiApplHeader   *createApplHeader ( GuiElement *parent );
  virtual GuiFolder       *createFolder( GuiElement *parent, const std::string & );
  virtual GuiIndexMenu    *createIndexMenu( GuiElement *parent, GuiIndexMenu::HideFlag flag);
  virtual GuiMenubar      *createMenubar    ( GuiElement *parent );
  virtual GuiPulldownMenu *createPulldownMenu( GuiElement *parent, const std::string &name );
  virtual GuiPopupMenu       *createPopupMenu( GuiElement *parent );
  virtual GuiPopupMenu       *createNavigatorMenu( GuiElement *parent );
  virtual GuiMenuButton      *createMenuButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event = 0, bool hide_disabled=false, const std::string& name="" );
  virtual GuiMenuToggle      *createMenuToggle( GuiElement *parent, GuiToggleListener *listener, GuiEventData *event = 0 );
  virtual GuiSeparator       *createSeparator( GuiElement *parent );
  virtual GuiMessageLine     *createMessageLine( GuiElement *parent );
  virtual GuiLabel        *createLabel      ( GuiElement *parent, GuiElement::Alignment a );
  virtual GuiButton       *createButton     ( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event);
  virtual GuiDataField    *createToggle( GuiElement *parent );
  virtual GuiFieldgroup   *createFieldgroup ( GuiElement *parent, const std::string &name );
  virtual GuiFieldgroup   *createFieldgroup ( GuiElement *parent, GuiElement *at, const std::string &name );
  virtual GuiFieldgroupLine *createFieldgroupLine(GuiElement *parent);
  virtual GuiOrientationContainer *createVerticalContainer( GuiElement *parent );
  virtual GuiOrientationContainer *createHorizontalContainer( GuiElement *parent );
  virtual GuiButtonbar    *createButtonbar  ( GuiElement *parent );
  virtual DialogCopyright *createDialogCopyright();
  virtual GuiCycleButton  *createCycleButton( GuiElement *parent );
  virtual GuiTable        *createTable( GuiElement *parent, const std::string &name );
  virtual GuiTableItem    *createTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align );
  virtual GuiTableItem    *createTableDataItem( GuiElement *parent );
  virtual GuiTableItem    *createTableComboBoxItem( GuiElement *parent, const std::string& datasetName );
  virtual GuiList         *createList ( GuiElement *parent, const std::string &name );
  virtual GuiFilterList      *createFilterList ( GuiElement *parent, GuiFilterListListener &listener,
						 const std::string &name );
  virtual GuiDataField    *createDataField  ( GuiElement *, XferDataItem *, int prec = 0 );
  virtual GuiScrolledText *createScrolledText( GuiElement *parent, const std::string &name );
  virtual GuiScrolledlist    *createScrolledlist( GuiElement *parent, GuiScrolledlistListener *listener);
  virtual GuiPrinterDialog   *createPrinterDialog();
  virtual GuiNavigator       *createNavigator( const std::string &id, GuiNavigator::Type navType );
  virtual DialogWorkClock    *createDialogWorkClock( GuiElement *element
						     , DialogWorkClockListener *listener
						     , const std::string &title
						     , const std::string &msg );

  virtual GuiElement::ButtonType showDialogConfirmation(GuiElement *
                                                        , const std::string &
                                                        , const std::string &
                                                        , ConfirmationListener * = 0
                                                        , const std::map<GuiElement::ButtonType
                                                        , std::string>& buttonText =std::map<GuiElement::ButtonType, std::string>()
                                                        , bool cancelBtn=false
                                                        , int defaultBtn=1);

  virtual bool showDialogInformation(GuiElement *
                                     , const std::string &
                                     , const std::string &
                                     , InformationListener * = 0 );

  virtual std::string showDialogTextInput(GuiElement *
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
					 , bool &ok
					 , ConfirmationListener* = 0 );

  virtual std::string showDialogTextInputExt(GuiElement *elem
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
           , const std::map<GuiElement::ButtonType, std::string>& buttonText
					 , GuiElement::ButtonType &buttonPressed
					 , ConfirmationListener* = 0 );

  virtual bool showDialogWarning(GuiElement *
                                 , const std::string &
                                 , const std::string &
                                 , InformationListener * = 0  );

  virtual bool showDialogFileSelection( GuiElement *element
					, const std::string &title
					, const std::string &filter
					, const std::string &directory
					, FileSelectListener *listener
					, DialogFileSelection::Type type
					, const std::vector<HardCopyListener::FileFormat> * const formats = 0
					, DialogFileSelection::Mode mode = DialogFileSelection::AnyFile
					, const std::string &basename = std::string() );
  virtual void showDialogUserPassword( const std::string &, const std::string &
				       , UserPasswordListener *, DialogUnmapListener *unmap = 0
				       , std::string errorMessage="" );

  virtual GuiPixmap       *createPixmap  ( GuiElement *parent, XferDataItem *dataitem );
  virtual GuiIndex        *createGuiIndex( GuiElement *parent, const std::string &name );
  virtual GuiIndex        *getGuiIndex( const std::string &name );
  virtual GuiCycleDialog  *createCycleDialog();

  virtual Gui2dPlot* create2dPlot( const std::string &, bool isPlot2d = false);
  virtual Gui3dPlot* create3dPlot( GuiElement *, const std::string & );

  // Plot staff
  virtual ListPlot        *createListPlot( GuiElement *parent, const std::string &name );
  virtual PSPlot*          createPSPlot(  PSPlot::PSStream *stream );

  virtual Plot2dMenuDescription *getPlot2dMenuDescription();

  virtual Simpel *newSimpelPlot( GuiElement *, const std::string & );
  virtual Simpel *getSimpelPlot( const std::string & );


  /** Diese Funktion erstellt ein neues Image-Objekt und liefert den Pointer.
      Ist bereits ein GuiElement mit diesem Namen bekannt, so wird als Resultat
      ein NULL-Pointer geliefert.
  */
  GuiImage *createImage( GuiElement *, const std::string &, int );
  GuiImage *createLinePlot( GuiElement *, const std::string & );

  GuiThermo *createThermo( GuiElement *, const std::string & );
  GuiTimeTable *createTimeTable( GuiElement *, const std::string & );

  /** Diese Funktion liefert das GuiScrolledText-Object für STD_WINDOW.
   */
  virtual GuiScrolledText *getStandardWindow();

  /** Diese Funktion liefert das GuiScrolledText-Object für LOG_WINDOW.
   */
  virtual GuiScrolledText *getLogWindow();
  virtual GuiScrolledText *getHtmlWindow();

  virtual void installDialogsWaitCursor(GuiDialog* diag_modal=0);
  virtual void removeDialogsWaitCursor(GuiDialog* diag_modal=0);
  virtual DialogProgressBar *getDialogProgressBar();
  virtual GuiElement *createProgressBar( GuiElement *, const std::string & );

  virtual ServerSocket *createServerSocket( IntensServerSocket *server, int port );
  virtual ClientSocket *createClientSocket( const std::string &header
					    , Stream *out_stream
					    , Stream *in_stream );
  virtual bool doCopy(Stream *out_stream, GuiElement* elem);
  virtual bool doPaste(Stream *in_stream);
  virtual char getDelimiter();
  virtual bool replace( GuiElement *old_el, GuiElement *new_el );

};
#endif
