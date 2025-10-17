
#if !defined( GUI_QT_PRINTER_DIALOG )
#define GUI_QT_PRINTER_DIALOG

#include "job/JobAction.h"
#include "gui/GuiPrinterDialog.h"
#include <QPrinter>
#include <QPrintDialog>

class HardCopyListener;
class GuiElement;
class GuiEventData;
class QComboBox;
class QToolButton;
class QLabel;
class QGroupBox;

class GuiQtPrinterDialog
  : public GuiPrinterDialog
{
private:
  GuiQtPrinterDialog();
public:
  void doExecute(bool exp=false, HardCopyListener::FileFormat=HardCopyListener::NONE);
public:
  static GuiQtPrinterDialog &Instance();
  virtual void initPrinterDialog(){}
  virtual void showDialog( HardCopyListener *, GuiElement *parent, GuiEventData *event, JobAction* jobAction=0 );
  virtual void selectHardCopyObject( HardCopyListener * ){}
  virtual void createPrintObjectList( GuiElement *parent ){}
  void preview( HardCopyListener *hcl, GuiElement *parent, JobAction* jobAction=0 );

  double leftMargin()   { return 20; }
  double rightMargin()  { return 20; }
  double topMargin()    { return 20; }
  double bottomMargin() { return 20; }
  int PaperSize();
  int Orientation();
  HardCopyListener::FileFormat FileFormat();
  void setFileFormat(HardCopyListener::FileFormat ff) { m_fileFormat = ff; }
  void doPreview();
private:
  static GuiQtPrinterDialog *s_instance;
  QPrinter         *m_printer;
  HardCopyListener* m_hcl;
  HardCopyListener::FileFormat m_fileFormat;

};

class MyQPrintDialog : public QPrintDialog {
  Q_OBJECT
public:
  MyQPrintDialog(GuiQtPrinterDialog* diag, HardCopyListener* hcl, QPrinter* pr)
    : QPrintDialog(pr), m_diag(diag), m_combo(0), m_expBtn(0), m_expLabel(0)
    , m_previewBtn(0), m_gbWidget(0) {
    init();
    setFileFormatList( hcl );
  }
  std::string getFileExportType();
  std::string getFileExportExtension();
  HardCopyListener::FileFormat getFileFormatExport(bool deep);
  void setFileFormatList(HardCopyListener* hcl);
  void enableExportFile(bool enable);

private slots:
  void slot_exportFile(bool);
  void slot_preview(bool);
#if 1  // only used by SLES9 platform
public slots:
  void slot_printerChanged(const QString&);
#endif

  void init();
  private:
  GuiQtPrinterDialog  *m_diag;
  QComboBox           *m_combo;
  QToolButton         *m_expBtn;
  QLabel              *m_expLabel;
  QToolButton         *m_previewBtn;
  QGroupBox           *m_gbWidget;
#if 1  // only used by SLES9 platform
  QLabel              *m_locationLbl;
#endif
};

#endif
