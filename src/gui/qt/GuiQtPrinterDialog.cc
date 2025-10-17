
#include <QWidget>
#include <QPrinter>
#include <QSettings>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QApplication>

#include "gui/qt/GuiQtFactory.h"
#include "gui/qt/GuiQtSeparator.h"
#include "gui/qt/GuiQtComboBox.h"
#include "gui/qt/GuiQtManager.h"
#include "gui/qt/GuiQtPrinterDialog.h"
#include "gui/qt/QtHardCopyTree.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtIconManager.h"

#include "app/DataPoolIntens.h"
#include "app/HardCopyListener.h"
#include "app/ReportGen.h"
#include "app/DataSet.h"
#include "gui/GuiFactory.h"
#include "gui/GuiForm.h"
#include "gui/GuiScrolledText.h"
#include "utils/gettext.h"
#include "utils/StringUtils.h"

GuiQtPrinterDialog *GuiQtPrinterDialog::s_instance = 0;

//std::string s_export_dataset = "@PrintDialogExportItemSet";
std::string s_export_dataset = "devel_state_set";
std::string s_export_datavar = "@PrintDialogExportComboboxValue";

static const char *print_xpm[]={
"32 32 12 1",
"a c #ffffff",
"h c #ffff00",
"c c #ffffff",
"f c #dcdcdc",
"b c #c0c0c0",
"j c #a0a0a4",
"e c #808080",
"g c #808000",
"d c #585858",
"i c #00ff00",
"# c #000000",
". c None",
"................................",
"................................",
"...........###..................",
"..........#abb###...............",
".........#aabbbbb###............",
".........#ddaaabbbbb###.........",
"........#ddddddaaabbbbb###......",
".......#deffddddddaaabbbbb###...",
"......#deaaabbbddddddaaabbbbb###",
".....#deaaaaaaabbbddddddaaabbbb#",
"....#deaaabbbaaaa#ddedddfggaaad#",
"...#deaaaaaaaaaa#ddeeeeafgggfdd#",
"..#deaaabbbaaaa#ddeeeeabbbbgfdd#",
".#deeefaaaaaaa#ddeeeeabbhhbbadd#",
"#aabbbeeefaaa#ddeeeeabbbbbbaddd#",
"#bbaaabbbeee#ddeeeeabbiibbadddd#",
"#bbbbbaaabbbeeeeeeabbbbbbaddddd#",
"#bjbbbbbbaaabbbbeabbbbbbadddddd#",
"#bjjjjbbbbbbaaaeabbbbbbaddddddd#",
"#bjaaajjjbbbbbbaaabbbbadddddddd#",
"#bbbbbaaajjjbbbbbbaaaaddddddddd#",
"#bjbbbbbbaaajjjbbbbbbddddddddd#.",
"#bjjjjbbbbbbaaajjjbbbdddddddd#..",
"#bjaaajjjbbbbbbjaajjbddddddd#...",
"#bbbbbaaajjjbbbjbbaabdddddd#....",
"###bbbbbbaaajjjjbbbbbddddd#.....",
"...###bbbbbbaaajbbbbbdddd#......",
"......###bbbbbbjbbbbbddd#.......",
".........###bbbbbbbbbdd#........",
"............###bbbbbbd#.........",
"...............###bbb#..........",
"..................###..........."};


#if 0
#include <cups/cups.h>
#include <glib.h>

#include <gnome-cups-printer.h>
#include <gnome-cups-util.h>
#include <gnome-cups-queue.h>
#include <gnome-cups-init.h>
#endif
void MyQPrintDialog::init() {
  QFont font = QPrintDialog::font();
  setFont( QtMultiFontString::getQFont( "@print@", font ) );

#if QT_VERSION >= 0x040300 && defined Q_OS_UNIX
  QGroupBox *grBox = findChild<QGroupBox *>();
  QGridLayout* grLayout = dynamic_cast<QGridLayout* >(grBox->layout());
  QBoxLayout* boxLayout = dynamic_cast<QBoxLayout* >(grBox->layout());
  assert(grLayout || boxLayout);

  // add own part
  setWindowTitle(_("Intens Print Dialog") );

  // add own row (label, combobox and button)
  m_combo = new QComboBox;
  m_expBtn= new QToolButton();
  m_expBtn->setText( _("Export") );
  m_expBtn->setToolTip( _("Export") );
#if QT_VERSION < 0x050000
  m_expBtn->setIcon(QIcon(print_xpm));
#else
  m_expBtn->setIcon(QIcon(QString::fromStdString((const char*) print_xpm)));
#endif
  m_expBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
  connect(m_expBtn, SIGNAL(clicked(bool)), SLOT(slot_exportFile(bool)));
  m_expLabel = new QLabel( _("Graphic Format:") );

  // add group box and layout
  QGridLayout* gbLayout = new QGridLayout();
  gbLayout->setSpacing(5);
  gbLayout->setContentsMargins(5,5,5,5);

  // preview button
  m_previewBtn= new QToolButton();
  m_previewBtn->setText( _("Preview") );
  m_previewBtn->setToolTip( _("Preview") );
  QPixmap previewIcon;
  if(QtIconManager::Instance().getPixmap( "preview", previewIcon )) {
    m_previewBtn->setIcon(QIcon(previewIcon));
    m_previewBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_previewBtn->setIconSize(previewIcon.size());
  }
  m_previewBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
  m_previewBtn->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed) );
  connect(m_previewBtn, SIGNAL(clicked(bool)), SLOT(slot_preview(bool)));

  m_gbWidget = new QGroupBox();
#if 1 //QT_VERSION < 0x050000
  m_gbWidget->setCheckable(true);
#endif
  m_gbWidget->setChecked(true);
  m_gbWidget->setTitle(_("Only used for file export or preview"));
  m_gbWidget->setLayout( gbLayout );
  gbLayout->addWidget( m_expLabel, 0, 0 );
  gbLayout->addWidget( m_combo, 0, 1 );
  gbLayout->addWidget( m_expBtn, 0, 2);
  gbLayout->addWidget( m_previewBtn, 1, 0, 1, 3);

  if (grLayout) {
    int row  = grLayout->rowCount();
    grLayout->addWidget( m_gbWidget, row++, 0, 1, -1);
  } else if (boxLayout) {
    boxLayout->addWidget( m_gbWidget );
  }

#if 1  // only used by SLES9 platform
  m_locationLbl = findChild<QLabel *>("location");
  if (m_locationLbl) {
    QComboBox* cb_priners = findChild<QComboBox *>("printers");
    if (cb_priners) {
      connect(cb_priners, SIGNAL(activated(const QString&)),
	      SLOT(slot_printerChanged(const QString&)));
      slot_printerChanged( cb_priners->currentText() );
    }
  }
#endif

#endif
}

#if 1  // only used by SLES9 platform
void MyQPrintDialog::slot_printerChanged(const QString& printer) {
  if (m_locationLbl) {
    // get location property from resource file
    if (m_locationLbl->text().isEmpty() &&
	GuiQtManager::Settings()) {
      QString s=GuiQtManager::Settings()->value
	( "PrinterLocation/"+printer, "").toString();
      m_locationLbl->setText(s);
    }
  }
}
#endif

void MyQPrintDialog::slot_exportFile(bool state) {
  QString of;
#if 1  // only used by SLES9 platform
  if ( printer() ) {
    std::string ext = getFileExportExtension();
#endif
    QString fileName = QFileDialog::getSaveFileName(this,
      QString::fromStdString( compose("document.%1", ext) ),
      QString::fromStdString( compose("%1 Documents (*.%2)", upper(ext), ext) ) );

    if ( fileName.isEmpty() ) return; //cancel
    of = printer()->outputFileName();
    printer()->setOutputFileName(fileName);
  }

  m_diag->setFileFormat( getFileFormatExport(true) );
  m_diag->doExecute(true, getFileFormatExport(false));
  printer()->setOutputFileName(of);
  close();
}

// ******************************************************
// slot_preview
// ******************************************************

void MyQPrintDialog::slot_preview(bool state) {
  m_diag->doPreview();
}

// ******************************************************
// setFileFormatList
// ******************************************************

void MyQPrintDialog::setFileFormatList(HardCopyListener* hcl) {
#if QT_VERSION >= 0x040300 && defined Q_OS_UNIX
  if (!m_combo)  return;
  m_combo->clear();
  // # 229 Postscript format is not supported anymore
  if (hcl->isExportPrintFormat(HardCopyListener::Postscript))
     m_combo->addItem( _("Postscript") );
  if (hcl->isExportPrintFormat(HardCopyListener::HPGL))
    m_combo->addItem( _("HPGL") );
  if (hcl->isExportPrintFormat(HardCopyListener::PDF))
    m_combo->addItem( _("PDF") );
  if (hcl->isExportPrintFormat(HardCopyListener::JPEG))
    m_combo->addItem( _("JPEG") );
  if (hcl->isExportPrintFormat(HardCopyListener::PNG))
    m_combo->addItem( _("PNG") );
  if (hcl->isExportPrintFormat(HardCopyListener::GIF))
    m_combo->addItem( _("GIF") );
  if (hcl->isExportPrintFormat(HardCopyListener::BMP))
    m_combo->addItem( _("BMP") );
  if (hcl->isExportPrintFormat(HardCopyListener::SVG))
    m_combo->addItem( _("SVG") );
  if (hcl->isExportPrintFormat(HardCopyListener::PPM))
    m_combo->addItem( _("PPM") );
  if (hcl->isExportPrintFormat(HardCopyListener::TIFF))
    m_combo->addItem( _("TIF") );
  if (hcl->isExportPrintFormat(HardCopyListener::XBM))
    m_combo->addItem( _("XBM") );
  if (hcl->isExportPrintFormat(HardCopyListener::XPM))
    m_combo->addItem( _("XPM") );

  if (hcl->isExportPrintFormat(HardCopyListener::ASCII) ||
      hcl->isExportPrintFormat(HardCopyListener::Text)  ||
      hcl->isExportPrintFormat(HardCopyListener::LaTeX)
      )
    m_combo->addItem( _("ASCII") );
  if (m_combo->count() == 0) { //hide
    m_gbWidget->hide();
    m_combo->hide();
    m_expBtn->hide();
    m_expLabel->hide();
    m_previewBtn->hide();
    QGroupBox *grBox = findChild<QGroupBox *>();
    QLayout* layout = dynamic_cast<QGridLayout* >(grBox->layout());
    if (layout) {
      layout->removeWidget(m_gbWidget);
      layout->removeWidget(m_combo);
      layout->removeWidget(m_expBtn);
      layout->removeWidget(m_expLabel);
      layout->removeWidget(m_previewBtn);
    }
  }
#endif
}

/// set or unset export file fieldgroup
void MyQPrintDialog::enableExportFile(bool enable) {
  if (m_gbWidget)
    m_gbWidget->setChecked(enable);
}

HardCopyListener::FileFormat MyQPrintDialog::getFileFormatExport(bool deep) {
  // export file if fieldgroup checked
  std::string ft = (m_combo) ?  m_combo->currentText().toLower().toStdString() : "";
  if ( m_gbWidget && m_gbWidget->isChecked() && ft.size()) {
    if (ft == "postscript")
      return HardCopyListener::Postscript;
    if (ft == "ps")
      return HardCopyListener::Postscript;
    if (ft == "hpgl")
      return HardCopyListener::HPGL;
    if (ft == "pdf")
      return HardCopyListener::PDF;
    if (ft == "jpeg")
      return HardCopyListener::JPEG;
    if (ft == "png")
      return HardCopyListener::PNG;
    if (ft == "gif")
      return HardCopyListener::GIF;
    if (ft == "ascii")
      return HardCopyListener::ASCII;

    if (ft == "svg")
      return HardCopyListener::SVG;
    if (ft == "bmp")
      return HardCopyListener::BMP;
    if (ft == "ppm")
      return HardCopyListener::PPM;
    if (ft == "tif")
      return HardCopyListener::TIFF;
    if (ft == "xbm")
      return HardCopyListener::XBM;
    if (ft == "xpm")
      return HardCopyListener::XPM;

    std::cerr << "ERROR:  Unknown FileFormatExport.\n";
    return HardCopyListener::NONE;
  } else if ( printer()) {
    // print file if fieldgroup is not checked
    QPrinter::OutputFormat of =  printer()->outputFormat();

#if QT_VERSION < 0x050000
    if (of == QPrinter::PostScriptFormat)
      return HardCopyListener::Postscript;
#endif
    if (of == QPrinter::PdfFormat)
      return HardCopyListener::PDF;
  }
  return HardCopyListener::Postscript;
}

std::string MyQPrintDialog::getFileExportType() {
  return m_combo ?  m_combo->currentText().toStdString() : "";
}

std::string MyQPrintDialog::getFileExportExtension() {
  std::string ft = m_combo ?  m_combo->currentText().toLower().toStdString() : "";
  if (ft == "postscript")
    return "ps";
  if (ft == "jpeg")
    return "jpg";
  if (ft == "tiff")
    return "tif";
  if (ft == "ascii")
    return "txt";
  return ::lower(ft);
}

// ******************************************************
// Instance
// ******************************************************
GuiQtPrinterDialog &GuiQtPrinterDialog::Instance(){
  if( s_instance == 0 ){
    s_instance = new GuiQtPrinterDialog();
  }
  return *s_instance;
}

GuiQtPrinterDialog::GuiQtPrinterDialog() : m_printer(0), m_hcl(0),  m_fileFormat(HardCopyListener::PDF) {
  setTree( new QtHardCopyTree() );
}

// ******************************************************
// showDialog
// ******************************************************
void GuiQtPrinterDialog::showDialog( HardCopyListener *hcl, GuiElement *parent, GuiEventData *event, JobAction* jobAction ){
  assert( event != 0 );
  if( hcl==0 )
    hcl = getCurrentListener();
  m_hcl = hcl;
  MyEventData *myevent = static_cast<MyEventData*>(event);
  // get a dialog !!!! FIX IT
  if( !parent )
    parent = GuiElement::getMainForm()->getElement();
  switch( myevent->mode() ){
  case ReportGen::PREVIEW :
    // Wir sind zwar nicht völlig falsch hier,
    // brauchen aber keinen PrinterDialog
    preview( hcl, parent, jobAction );
    return;
    break;
  case ReportGen::SAVE :
    // Wir sind völlig falsch hier !!
    hcl->saveFile( parent );
    return;
    break;
  case ReportGen::PRINT :
    break;
  default :
    assert( false );
    break;
  }

  // show dialog Returns true on OK, false on CANCEL
  bool grabButton( false );
  GuiEventLoopListener *loopcontrol = new GuiEventLoopListener( grabButton );
  GuiManager::Instance().attachEventLoopListener( loopcontrol );

  // first time we create a qprinter
  if (!m_printer)  {
    m_printer = new QPrinter(QPrinter::ScreenResolution);
  }

  MyQPrintDialog dialog(this, hcl, m_printer);//, parent->getQtElement()->myWidget());
#ifndef _WIN32
  if (dialog.printer())
    dialog.slot_printerChanged(dialog.printer()->printerName());
#endif

  // dialog.exec() clears activeWindow
  QWidget *activeWindow = GuiQtManager::Instance().getActiveWindow();

  if (dialog.exec()) {
    HardCopyListener::FileFormat ff = m_printer->outputFormat() ==  QPrinter::PdfFormat ?
      HardCopyListener::PDF : HardCopyListener::Postscript;
    dialog.enableExportFile(false);  // wichtig um spaeter das export format eindeutig festzustellen
    m_fileFormat =dialog.getFileFormatExport(true);
    doExecute(false, ff); //dialog.getFileFormatExport(true));
  }
  // restore activeWindow
  if ( activeWindow)
    activeWindow->activateWindow();
  loopcontrol->goAway();
}

/* --------------------------------------------------------------------------- */
/* doExecute --                                                                */
/* --------------------------------------------------------------------------- */

void GuiQtPrinterDialog::doExecute(bool exp, HardCopyListener::FileFormat fileformat) {
//  std::cerr << " \nexport["<<exp<<"] fileformat["<<fileformat<<"]  hcl["<<m_hcl<<"]  creator["<<m_printer.creator().toStdString()<<"] outputFileName["<<m_printer.outputFileName().toStdString()<<"]  docName["<<m_printer.docName().toStdString()<<"] printProgram["<<m_printer.printProgram().toStdString()<<"] printerName["<<m_printer.printerName().toStdString()<<"]\n";

  GuiElement* parent = GuiElement::getMainForm()->getElement();
  HardCopyListener::Orientation orientation = HardCopyListener::Portrait;
  ReportGen::Mode mode = ReportGen::PRINT;
  std::string printerName, fileName;
  HardCopyListener::PaperSize pageSize = HardCopyListener::A4;
  if (!exp) {
    fileformat = HardCopyListener::PDF;
#ifndef __MINGW32__
    fileformat = HardCopyListener::Postscript;
#endif
  }
  // get orientation
  orientation = m_printer->pageLayout().orientation() == QPageLayout::Portrait ?
    HardCopyListener::Portrait : HardCopyListener::Landscape;

// get output mode
  if( exp || m_printer->outputFileName().length() ) {
    mode = ReportGen::SAVE;
    fileName = m_printer->outputFileName().toStdString();
  }
  else
    printerName = m_printer->printerName().toStdString();

  // get page size
  switch( m_printer->pageLayout().pageSize().id() ){
    case QPageSize::A2 :
      pageSize = HardCopyListener::A2;
      break;
    case QPageSize::A3 :
      pageSize = HardCopyListener::A3;
      break;
    default :
      pageSize = HardCopyListener::A4;
      break;
  }

  switch ( m_hcl->getPrintType(fileformat) ) {
    case HardCopyListener::NOT_SUPPORTED:
      //      std::cerr << " Print Type not supported => Try Script.\n" << std::flush;
    case HardCopyListener::SCRIPT:
      //      std::cout << " Print Type script. fileformat["<<fileformat<<"]\n" << std::flush;
      // invoke ReportGen
      ReportGen::Instance().printDocument( mode
				       , m_hcl
				       , parent
				       , fileName
				       , printerName
				       , pageSize
				       , orientation
				       , m_printer->copyCount()
				       , fileformat
				       );
      break;
    case HardCopyListener::FILE_EXPORT:
//       if (fileformat == HardCopyListener::PDF || fileformat == HardCopyListener::Postscript) {
// 	m_hcl->print( &m_printer );
// 	return;
//       }
      m_hcl->write(fileName);
      break;
    case HardCopyListener::OWN_CONTROL:
      m_hcl->print( m_printer );
      break;
//    case HardCopyListener::NOT_SUPPORTED:
    default:
      GuiFactory::Instance()->getLogWindow()->writeText( _("Print Type not supported.") );
      break;
  }

}

// ******************************************************
// doPreview
// ******************************************************
void GuiQtPrinterDialog::doPreview() {
  GuiElement* parent = GuiElement::getMainForm()->getElement();
  preview( m_hcl, parent, NULL );
}

// ******************************************************
// preview
// ******************************************************
void GuiQtPrinterDialog::preview( HardCopyListener *hcl, GuiElement *parent, JobAction* jobAction ){
  //  QPrinter printer;
  ReportGen::Mode mode = ReportGen::PRINT;
  HardCopyListener::Orientation orientation = HardCopyListener::Portrait;
  std::string printerName, fileName;
  HardCopyListener::PaperSize pageSize = HardCopyListener::A4;
  int quantity(1);
  HardCopyListener::FileFormat format;
  double lm, rm, tm, bm;
  hcl->getDefaultSettings(pageSize, orientation, quantity, format, lm, rm, tm, bm);

#ifdef __MINGW32__
  setFileFormat( HardCopyListener::PDF );
#else
  setFileFormat( HardCopyListener::PDF );
#endif
  ReportGen::Instance().printDocument( ReportGen::PREVIEW
				       , hcl
				       , parent
				       , fileName
				       , printerName
				       , pageSize
				       , orientation
				       , quantity//printer.numCopies()
#if (defined __MINGW32__) || (defined __linux__)
				       , HardCopyListener::PDF
#else
				       , HardCopyListener::PDF
#endif
				       , jobAction
				       );
}


/* --------------------------------------------------------------------------- */
/* PaperSize --                                                                */
/* --------------------------------------------------------------------------- */

int GuiQtPrinterDialog::PaperSize(){
  //  return intParameter( m_field_papersize->Data(), -1 );
  int ps =  m_printer->pageLayout().pageSize().id();
  if (ps == QPageSize::A4)
    return HardCopyListener::A4;
  if (ps == QPageSize::A3)
    return HardCopyListener::A3;
  if (ps == QPageSize::A3)
    return HardCopyListener::A3;
  return HardCopyListener::Unscaled;

}

/* --------------------------------------------------------------------------- */
/* Orientation --                                                              */
/* --------------------------------------------------------------------------- */

int GuiQtPrinterDialog::Orientation(){
  //  return intParameter( m_field_orientation->Data(), -1 );
  return m_printer->pageLayout().orientation() == QPageLayout::Portrait ?
    HardCopyListener::Portrait : HardCopyListener::Landscape;
}

/* --------------------------------------------------------------------------- */
/* FileFormat --                                                              */
/* --------------------------------------------------------------------------- */

HardCopyListener::FileFormat GuiQtPrinterDialog::FileFormat(){
  return m_fileFormat;
}
