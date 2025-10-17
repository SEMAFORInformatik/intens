
#include "HelpWindow.h"
#include "printpreview.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
// #include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#if QT_VERSION < 0x060000
#include <QTextCodec>
#endif
#include <QTextBrowser>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>

#include <iostream>

#include "gui/qt/QtIconManager.h"

#ifdef Q_WS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

HelpWindow::HelpWindow(const QString& home_,  const QString& path, QWidget *parent)
    : QMainWindow(parent)
{

    textEdit = new QTextBrowser(this);

    setupFileActions();

    setCentralWidget(textEdit);
    textEdit->setFocus();
    setCurrentFileName(home_);
    //    setCurrentFileName(QString("./doc/manual/index.html"));

    connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));

    setWindowModified(textEdit->document()->isModified());

    QString initialFile = home_;//"./doc/manual/index.html";
    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
        initialFile = args.at(1);

    if (!load(initialFile)) {
      textEdit->clear();
      setCurrentFileName(QString());
    }
    resize(800, 600);
}

void HelpWindow::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}

void HelpWindow::setupFileActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(menu);

    QAction *a;

    a = new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("&Open..."), this);
    a->setShortcut(Qt::CTRL|Qt::Key_O);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
//     tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = new QAction(tr("Save &As..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    menu->addAction(a);
    menu->addSeparator();

    a = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("&Print..."), this);
    a->setShortcut(Qt::CTRL|Qt::Key_P);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
//     tb->addAction(a);
    menu->addAction(a);

    a = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("Print Preview..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    menu->addAction(a);

    a = new QAction(QIcon(rsrcPath + "/exportpdf.png"), tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL|Qt::Key_D);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
//     tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = new QAction(tr("&Quit"), this);
    a->setShortcut(Qt::CTRL|Qt::Key_Q);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    menu->addAction(a);

    QPixmap iconF, iconB;
    QtIconManager::Instance().getPixmap( "prev", iconB );
    actionBackward = new QAction(QIcon(iconB), tr("Backward"), this);
    connect(actionBackward, SIGNAL(triggered()), textEdit, SLOT(backward()));
    connect(textEdit, SIGNAL(backwardAvailable(bool)), actionBackward, SLOT(setEnabled(bool)));
    actionBackward->setEnabled( false );
    QtIconManager::Instance().getPixmap("next", iconF );
    actionForward = new QAction(QIcon(iconF), tr("Forward"), this);
    connect(actionForward, SIGNAL(triggered()), textEdit, SLOT(forward()));
    connect(textEdit, SIGNAL(forwardAvailable(bool)), actionForward, SLOT(setEnabled(bool)));
    actionForward->setEnabled( false );
    tb->addAction(actionBackward);
    tb->addAction(actionForward);
}

bool HelpWindow::load(const QString &f)
{
  std::cout << "load file ["<<f.toStdString()<<"]\n"<<std::flush;
  if (!QFile::exists(f))
    return false;
  QFile file(f);
  if (!file.open(QFile::ReadOnly))
    return false;

  QByteArray data = file.readAll();
  // simple assume utf8
  QString str = QString::fromUtf8(data);

  if (1||Qt::mightBeRichText(str)) {
    QStringList m_searchPath = textEdit->searchPaths();
    if ( !m_searchPath.contains( QFileInfo(file).absolutePath() ) ) {
      QStringList new_searchPath = m_searchPath << (QFileInfo(file).absolutePath());
      textEdit->setSearchPaths( new_searchPath );
    }
    textEdit->setHtml(str);
  } else {
    str = QString::fromLocal8Bit(data);
    textEdit->setPlainText(str);
  }

  setCurrentFileName(f);
  return true;
}

bool HelpWindow::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    if (fileName.startsWith(QLatin1String(":/")))
        return true;
    int ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    //    setDefaultButton(QMessageBox::Yes);
    //    setEscapeButton(QMessageBox::Cancel);
    if (ret == QMessageBox::Yes)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void HelpWindow::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
    setWindowModified(false);
}

void HelpWindow::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty())
        load(fn);
}

bool HelpWindow::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;
    QTextStream ts(&file);
#if QT_VERSION >= 0x060000
    ts.setEncoding(QStringConverter::Utf8);
#else
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
#endif
    ts << textEdit->document()->toHtml();
    textEdit->document()->setModified(false);
    return true;
}

bool HelpWindow::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    setCurrentFileName(fn);
    return fileSave();
}

void HelpWindow::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        textEdit->document()->print(&printer);
    }
    delete dlg;
#endif
}

void HelpWindow::filePrintPreview()
{
    PrintPreview *preview = new PrintPreview(textEdit->document(), this);
    preview->setWindowModality(Qt::WindowModal);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}

void HelpWindow::filePrintPdf()
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        textEdit->document()->print(&printer);
    }
#endif
}
