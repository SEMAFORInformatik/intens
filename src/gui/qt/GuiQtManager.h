

#if !defined(GUI_QT_MANAGER_INCLUDED_H)
#define GUI_QT_MANAGER_INCLUDED_H

#include <QColor>
#include <QTimer>
#include <QStringList>

#include <set>

#include "gui/qt/GuiQtElement.h"
#include "gui/qt/GuiQtDialog.h"
#include "gui/GuiManager.h"
#include "job/JobFunction.h"
#include "job/JobStarter.h"

class QApplication;
class QCoreApplication;
class QWidget;
class QSettings;
class MainWidget;
class GuiQtForm;
class SmartPluginInterface;

class GuiQtManager : public GuiQtElement, public GuiManager, public GuiQtDialog
{
  Q_OBJECT
private:
  GuiQtManager();
public:
  virtual ~GuiQtManager();

public:
  static GuiQtManager &Instance();
  static GuiQtManager *createManager();

/*=============================================================================*/
/* pure virtual form GuiQtElement                                              */
/*=============================================================================*/
  virtual GuiElement::ElementType Type() { return GuiElement::type_Manager; }
  virtual void create();
  virtual void manage();
  virtual void getSize( int &, int & );
  virtual QWidget* myWidget();
  virtual void serializeXML(std::ostream &os, bool recursive = false);

/*=============================================================================*/
/* pure virtual form GuiManager                                                */
/*=============================================================================*/
  virtual void setMainTitle( const std::string &title );
  virtual void getDisplayName( std::string &title );
  virtual void update( GuiElement::UpdateReason );
  /** change indicator */
  virtual bool hasChanged(TransactionNumber trans, XferDataItem* xfer=0, bool show=false);
  virtual void updateLater( GuiElement::UpdateReason );

  virtual void initApplication( int *argc, char **argv, const std::string & );
  virtual void installQuitApplication( QuitApplication *quit ){}
  virtual void createApplication();
  virtual bool mapApplication();
  virtual bool runApplication();
  virtual bool closeApplication();
  virtual void showStartupBannerMessage(const std::string& message);
  virtual void writeSettings();
  void runJobAfterUpdateFormsFunction( JobElement::CallReason reason );

  virtual bool replace( GuiElement *old_el, GuiElement *new_el );

  virtual TransactionNumber LastGuiUpdate();
  virtual TransactionNumber LastWebUpdate();
  virtual void setWebUpdateTimestamp();
  virtual void overrideLastUpdate(TransactionNumber trans);
  virtual void resetOverrideLastUpdate();
  virtual void setUpdateAlways();

  /** Diese Funktion liefert den Pointer auf das gewuenschte Plugin. Falls kein
      Plugin mit diesem Namen existiert, wird der Pointer 0 geliefert.
   */
  Plugin *getPlugin( const std::string & );

  /** Diese Funktion hängt an das gewünschte PulldownMenu alle noch nicht
       installierten Forms zum Oeffnen der Fenster.
      @param menu bereits vorhandenes Menu für attach().
  */
  void createFormMenu( GuiPulldownMenu *menu );

  /** get list of names of available stylesheets
      @param bAbsolutPath short (basename) or absolute path name.
   */
  virtual std::set<std::string> getAvailableStylesheets(bool bAbsolutPath=false);
  /** set stylesheet to application
   */
  virtual std::string setStylesheetName(const std::string& stylesheet);

  virtual void initErrorBox();
  virtual std::ostream *getErrorBoxStream();
  virtual void showErrorBox( GuiElement *el );
  virtual void dispatchPendingEvents();

  virtual GuiElement * getElement();
  virtual GuiDialog  * getDialog();
  virtual GuiElement::Orientation getDialogExpandPolicy();

  /** load plugin through QPluginLoader
   */
  SmartPluginInterface* loadPlugin(const std::string& plugin_name);

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:

  /** Diese Funktion liefert das Widget des Main Form.
   */
  QDialog* Toplevel();
  virtual QDialog* getDialogWidget() { return Toplevel(); }
  virtual std::string getDialogName() { return "MainWindow"; }
  QApplication* QApp();

  static QSettings* Settings()  { return m_settings; }

  static void insertToWaitCursorList( QObject* o );
  static void removeWaitCursorsFromList();

  void setWaitingModalDialog(bool activate);
  bool isWaitingModalDialog();

  QWidget *getWaitingWidget();
  bool setWaitingWidget( bool set, QWidget *w );
  /* get active Window
     (exception, if active window is a QtDialogWorkClock return 0)
   */
  QWidget *getActiveWindow();
  void printSizeInfo(std::ostream& os, int intent, bool onlyMaxChilds=true);

/*=============================================================================*/
/* private class definition                                                    */
/*=============================================================================*/
private:
  //==============
  // Trigger
  //==============
  class Trigger : public JobStarter
  {
  public:
    Trigger( GuiQtManager *manager, JobFunction *f )
      : JobStarter( f )
      , m_manager( manager ){
    }
    virtual ~Trigger() {}
    virtual void backFromJobStarter( JobAction::JobResult jobResult );
  private:
    GuiQtManager *m_manager;
  };
/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
  QString getColorName( const QString &col );
  static QColor getSettingColor(QSettings& settings, const char* setting_name, const QString& default_color_name);
  static bool resFileExists( QString & );
  bool resFileNotExists();

  void popupStartupBanner();
  void destroyStartupBanner();

/*=============================================================================*/
/* public resources ( colors ... )                                             */
/*=============================================================================*/
 public:
  static bool loadResourceFile( const std::string &resfilename );
  static bool writeResourceFile();

  // get Resources
  static const QColor&  foregroundColor();
  static const QColor&  backgroundColor();
  static const QColor&  buttonTextColor();
  static const QColor&  buttonColor();
  static const QColor&  highlightColor();

  static const QColor&  editableForegroundColor();
  static const QColor&  editableBackgroundColor();

  static const QColor&  editableLForegroundColor();
  static const QColor&  editableLBackgroundColor();

  static const QColor&  optionalForegroundColor();
  static const QColor&  optionalBackgroundColor();

  static const QColor&  optionalLForegroundColor();
  static const QColor&  optionalLBackgroundColor();

  static const QColor&  readonlyForegroundColor();
  static const QColor&  readonlyBackgroundColor();

  static const QColor&  readonlyLForegroundColor();
  static const QColor&  readonlyLBackgroundColor();

  static const QColor&  lockedForegroundColor();
  static const QColor&  lockedBackgroundColor();

  static const QColor&  alarmForegroundColor();
  static const QColor&  alarmBackgroundColor();

  static const QColor&  warnForegroundColor();
  static const QColor&  warnBackgroundColor();

  static const QColor&  infoForegroundColor();
  static const QColor&  infoBackgroundColor();

  static const QColor&  helpForegroundColor();
  static const QColor&  helpBackgroundColor();

  static const QColor&  indexForegroundColor();
  static const QColor&  indexBackgroundColor();

  static const QColor&  imageLowerAlarmColor();
  static const QColor&  imageUpperAlarmColor();
  static const QColor&  imageHighlightColor();

  static const QColor  colorForegroundColor(int numColor);
  static const QColor  colorBackgroundColor(int numColor);
  static const QColor&  color1ForegroundColor();
  static const QColor&  color1BackgroundColor();
  static const QColor&  color2ForegroundColor();
  static const QColor&  color2BackgroundColor();
  static const QColor&  color3ForegroundColor();
  static const QColor&  color3BackgroundColor();
  static const QColor&  color4ForegroundColor();
  static const QColor&  color4BackgroundColor();
  static const QColor&  color5ForegroundColor();
  static const QColor&  color5BackgroundColor();
  static const QColor&  color6ForegroundColor();
  static const QColor&  color6BackgroundColor();
  static const QColor&  color7ForegroundColor();
  static const QColor&  color7BackgroundColor();
  static const QColor&  color8ForegroundColor();
  static const QColor&  color8BackgroundColor();
  static const QColor&  navItemBorderColor();
  static const QColor&  fieldgroupForegroundColor();
  static const QColor&  fieldgroupBackgroundColor();
  static const std::string& fieldgroupShadow();
  virtual const std::string& stylesheetName();
  static bool hasSettingFileActualVersion();
  static const int& navItemBorderWidth();
  static const int& listItemHeight();
  static bool isWindowsInDarkMode();

  static const QStringList& defaultColorPalette();

  QColor readResourceColor( const std::string &name );

  static bool stylesheetContains(const std::string qclass);
  static const int DefaultStretchFactor() { return s_defaultStretchFactor; }

protected:
  void timerEvent( QTimerEvent *event );
private slots:
  void slot_GuiUpdateEvent();
  void slot_desktop_workAreaResized(int);
  void slot_writeToDatapool(const std::string varnameData, const std::string& data, int maxlen);

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  static GuiQtManager  *s_instance;
  QCoreApplication     *m_qapp;
  QDialog              *m_toplevel;

  TransactionNumber     m_gui_transaction;
  TransactionNumber     m_web_transaction;

  std::vector<TransactionNumber>     m_override_gui_transactions;
  int                   m_gui_cycle;
  bool                  m_gui_full_update;
  bool                  m_with_gui;

  static std::set<QObject*>    s_waitCursorList;

  static QSettings*  m_settings;
  static const int s_defaultStretchFactor;


  // Qt basic colors (basic QApplication::palette() )
  static QColor m_foregroundColor;
  static QColor m_backgroundColor;
  static QColor m_buttonColor;
  static QColor m_buttonTextColor;
  static QColor m_baseColor;
  static QColor m_textColor;
  static QColor m_brightTextColor;
  static QColor m_highlightColor;
  static QColor m_highlightedTextColor;
  static QColor m_linkColor;
  static QColor m_linkVisitedColor;
  static QStringList m_defaultColorPalette;

  // other intens internal used colors
  static QColor m_editableForegroundColor;
  static QColor m_editableBackgroundColor;
  static QColor m_editableLForegroundColor;
  static QColor m_editableLBackgroundColor;
  static QColor m_optionalForegroundColor;
  static QColor m_optionalBackgroundColor;
  static QColor m_optionalLForegroundColor;
  static QColor m_optionalLBackgroundColor;
  static QColor m_readonlyForegroundColor;
  static QColor m_readonlyBackgroundColor;
  static QColor m_readonlyLForegroundColor;
  static QColor m_readonlyLBackgroundColor;
  static QColor m_lockedForegroundColor;
  static QColor m_lockedBackgroundColor;
  static QColor m_alarmForegroundColor;
  static QColor m_alarmBackgroundColor;
  static QColor m_warnForegroundColor;
  static QColor m_warnBackgroundColor;
  static QColor m_infoForegroundColor;
  static QColor m_infoBackgroundColor;
  static QColor m_helpForegroundColor;
  static QColor m_helpBackgroundColor;
  static QColor m_indexForegroundColor;
  static QColor m_imageLowerAlarmColor;
  static QColor m_imageUpperAlarmColor;
  static QColor m_imageHighlightColor;
  static QColor m_indexBackgroundColor;
  static QColor m_color1ForegroundColor;
  static QColor m_color1BackgroundColor;
  static QColor m_color2ForegroundColor;
  static QColor m_color2BackgroundColor;
  static QColor m_color3ForegroundColor;
  static QColor m_color3BackgroundColor;
  static QColor m_color4ForegroundColor;
  static QColor m_color4BackgroundColor;
  static QColor m_color5ForegroundColor;
  static QColor m_color5BackgroundColor;
  static QColor m_color6ForegroundColor;
  static QColor m_color6BackgroundColor;
  static QColor m_color7ForegroundColor;
  static QColor m_color7BackgroundColor;
  static QColor m_color8ForegroundColor;
  static QColor m_color8BackgroundColor;
  static QColor m_fieldgroupForegroundColor;
  static QColor m_fieldgroupBackgroundColor;
  static QColor m_navItemBorderColor;
  static int m_navItemBorderWidth;
  static int m_listItemHeight;
  static std::string m_fieldgroupShadow;
  static std::string m_stylesheetName;
  static std::string m_stylesheetApply;
  static std::string m_prevSettingFileVersion;

  static const std::string STYLESHEET_APPLY_INIT;
  static const std::string STYLESHEET_APPLY_SHOWN;
  static const std::string STYLESHEET_APPLY_BOTH;

private:
  QWidget *m_waitingWidget;
  bool     m_waitingModalDialog;
  QWidget *m_startup;
  int m_updateLater;
  int m_updateStylesheet;
  JobFunction     *m_jobAfterUpdateFormsFunction;
};

#endif
