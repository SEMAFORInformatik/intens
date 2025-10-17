#ifndef HEADLESS_GUI_FACTORY_H
#define HEADLESS_GUI_FACTORY_H

#include "gui/GuiFactory.h"

class HeadlessGuiManager;

class HeadlessGuiFactory : public GuiFactory
{
public:
  HeadlessGuiFactory();
  virtual ~HeadlessGuiFactory();

  GuiManager *createManager();

  bool have2dPlot();

  Timer              *createTimer( int interval, bool restart = false );
  void                createSimpleAction( SimpleAction::Object *obj );
  GuiForm            *createForm( const std::string& name );
  GuiApplHeader      *createApplHeader( GuiElement *parent );
  GuiFolder          *createFolder( GuiElement *parent, const std::string & );
  GuiIndexMenu       *createIndexMenu( GuiElement *parent, GuiIndexMenu::HideFlag flag);
  GuiMenubar         *createMenubar( GuiElement *parent );
  GuiPulldownMenu    *createPulldownMenu( GuiElement *parent, const std::string &name );
  GuiPopupMenu       *createPopupMenu( GuiElement *parent );
  GuiPopupMenu       *createNavigatorMenu( GuiElement *parent );
  GuiMenuButton      *createMenuButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event, bool hide_disabled=false, const std::string& name="" );
  GuiMenuToggle      *createMenuToggle( GuiElement *parent, GuiToggleListener *listener, GuiEventData *event );
  GuiSeparator       *createSeparator( GuiElement *parent );
  GuiMessageLine     *createMessageLine( GuiElement *parent );
  GuiScrolledText    *createScrolledText( GuiElement *parent, const std::string &name );
  GuiScrolledlist    *createScrolledlist( GuiElement *parent, GuiScrolledlistListener *listener);
  GuiLabel           *createLabel( GuiElement *parent, GuiElement::Alignment a );
  GuiButton          *createButton( GuiElement *parent, GuiButtonListener *listener, GuiEventData *event=0 );
  GuiDataField       *createToggle( GuiElement *parent );
  GuiFieldgroup      *createFieldgroup( GuiElement *parent, const std::string &name);
  GuiFieldgroup      *createFieldgroup( GuiElement *parent, GuiElement *at, const std::string &name);
  GuiFieldgroupLine  *createFieldgroupLine(GuiElement *parent);
  GuiOrientationContainer *createVerticalContainer( GuiElement *parent );
  GuiOrientationContainer *createHorizontalContainer( GuiElement *parent );
  GuiButtonbar       *createButtonbar( GuiElement *parent );
  DialogCopyright    *createDialogCopyright();
  GuiCycleButton     *createCycleButton( GuiElement *parent );
  GuiTable           *createTable( GuiElement *parent, const std::string &name );
  GuiTableItem       *createTableLabelItem( GuiElement *parent, const std::string &label, GuiElement::Alignment align );
  GuiTableItem       *createTableDataItem( GuiElement *parent );
  GuiTableItem       *createTableComboBoxItem( GuiElement *parent, const std::string& datasetName );
  GuiList            *createList ( GuiElement *parent, const std::string &name );
  GuiFilterList      *createFilterList ( GuiElement *parent, GuiFilterListListener &listener, 
						 const std::string &name );
  GuiPrinterDialog   *createPrinterDialog();
  GuiNavigator       *createNavigator( const std::string &id, GuiNavigator::Type navType );
  DialogWorkClock    *createDialogWorkClock( GuiElement *element
						     , DialogWorkClockListener *listener
						     , const std::string &title
						     , const std::string &msg );
  
  GuiElement::ButtonType showDialogConfirmation(GuiElement *
                                                 , const std::string &
                                                 , const std::string &
                                                 , ConfirmationListener *
                                                 , const std::map<GuiElement::ButtonType
                                                 , std::string>& buttonText =std::map<GuiElement::ButtonType, std::string>()
                                                 , bool cancelBtn=false
                                                 , int defaultBtn=1);

  bool showDialogWarning(GuiElement *
                                 , const std::string &
                                 , const std::string &
                                 , InformationListener * );

  bool showDialogInformation(GuiElement *
				     , const std::string &
				     , const std::string &
				     , InformationListener * );

  std::string showDialogTextInput(GuiElement *
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
					 , bool &ok
					 , ConfirmationListener* );

  std::string showDialogTextInputExt(GuiElement *elem
					 , const std::string &caption
					 , const std::string &message
					 , const std::string &label
           , const std::map<GuiElement::ButtonType, std::string>& buttonText
					 , GuiElement::ButtonType &buttonPressed
					 , ConfirmationListener* = 0 );


  bool showDialogFileSelection( GuiElement *element
					, const std::string &title
					, const std::string &filter
					, const std::string &directory
					, FileSelectListener *listener
					, DialogFileSelection::Type type
					, const std::vector<HardCopyListener::FileFormat> * const formats
					, DialogFileSelection::Mode mode = DialogFileSelection::AnyFile
          , const std::string &basename = std::string());
  void showDialogUserPassword( const std::string &, const std::string &
				       , UserPasswordListener *, DialogUnmapListener *unmap
				       , std::string errorMessage="" );

  GuiDataField    *createDataField  ( GuiElement *, XferDataItem *, int prec );
  GuiPixmap       *createPixmap  ( GuiElement *parent, XferDataItem *dataitem );
  GuiIndex        *createGuiIndex( GuiElement *parent, const std::string &name );
  GuiIndex        *getGuiIndex( const std::string &name );
  
  Gui2dPlot* create2dPlot( const std::string &, bool isPlot2d = false);
  Gui3dPlot* create3dPlot( GuiElement *, const std::string & );

  ListPlot*        createListPlot( GuiElement *parent, const std::string &name );
  PSPlot*          createPSPlot( PSPlot::PSStream *stream );

  Plot2dMenuDescription *getPlot2dMenuDescription();


  Simpel *newSimpelPlot( GuiElement *, const std::string & );
  Simpel *getSimpelPlot( const std::string & );

  GuiImage  *createImage( GuiElement *, const std::string &, int );
  GuiImage  *createLinePlot( GuiElement *, const std::string & );
  GuiThermo  *createThermo( GuiElement *, const std::string & );
  GuiTimeTable *createTimeTable( GuiElement *, const std::string & );

  void attachEventLoopListener( GuiEventLoopListener *lsnr );
  GuiCycleDialog  *createCycleDialog();

  GuiScrolledText *getStandardWindow();
  GuiScrolledText *getLogWindow();
  GuiScrolledText *getHtmlWindow();
  
  void installDialogsWaitCursor(GuiDialog* diag_modal=0);
  void removeDialogsWaitCursor(GuiDialog* diag_modal=0);
  DialogProgressBar *getDialogProgressBar();
  virtual GuiElement *createProgressBar( GuiElement *, const std::string & );

  ServerSocket *createServerSocket( IntensServerSocket *server, int port );
  ClientSocket *createClientSocket( const std::string &header
					    , Stream *out_stream 
					    , Stream *in_stream ) ;
  bool doCopy(Stream *out_stream, GuiElement* elem) ;
  bool doPaste( Stream *in_stream) ;
  char getDelimiter() ;

  bool replace( GuiElement *old_el, GuiElement *new_el );
};
#endif
