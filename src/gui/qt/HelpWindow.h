
#if !defined(HELP_WINDOW_INCLUDED_H)
#define HELP_WINDOW_INCLUDED_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#if QT_VERSION < 0x050000
#include <QHttp>
#endif
#include <QFile>

class QAction;
class QComboBox;
class QTextBrowser;
class QUrl;

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    HelpWindow(const QString& home_,  const QString& path, QWidget *parent = 0);

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    void setupFileActions();
    bool load(const QString &f);
    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

private slots:
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

private:
  QAction *actionBackward,
          *actionForward;

    QComboBox *comboStyle;
    QComboBox *comboSize;

    QToolBar *tb;
    QString fileName;
    QTextBrowser *textEdit;
};

#endif
