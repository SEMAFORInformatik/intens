
#if !defined(QT_DIALOG_FILESELECTION_H)
#define QT_DIALOG_FILESELECTION_H

#include <string>
#include <vector>

#include <QString>
#include <QFileDialog>

#include "gui/DialogFileSelection.h"
#include "gui/qt/GuiQtDialog.h"
#include "app/HardCopyListener.h"

class GuiElement;
class FileSelectListener;
class GuiEventLoopListener;
class QWidget;

class QtDialogFileSelection
  : public DialogFileSelection
  , public GuiQtDialog
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  QtDialogFileSelection();
public:
  virtual ~QtDialogFileSelection() {}
  virtual QDialog* getDialogWidget() { return NULL; }
  virtual std::string getDialogName() { return "QtDialogFileSelection"; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  static bool showDialog( GuiElement *element
			  , const std::string &title
			  , const std::string &filter
			  , const std::string &directory
			  , FileSelectListener *listener
			  , Type type
			  , const std::vector<HardCopyListener::FileFormat> * const formats
			  , Mode mode );

  static QString getDirectoryName ( const QString & startWith = QString()
				   , QWidget * parent = 0
				   , const QString & caption = QString()
				   , QFileDialog::Options options = QFileDialog::Options() );
  static QString getSaveFileName ( const QString & startWith = QString()
				   , const QString & filter = QString()
				   , QWidget * parent = 0
				   , const QString & caption = QString()
				   , QString * selectedFilter = 0
				   , QFileDialog::Options options = QFileDialog::Options() );

  static QString getOpenFileName ( const QString & startWith = QString()
				   , const QString & filter = QString()
				   , QWidget * parent = 0
				   , const QString & caption = QString()
				   , QString * selectedFilter = 0
				   , QFileDialog::Options options = QFileDialog::Options() );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:
  static QString createFilter( const std::vector<HardCopyListener::FileFormat> *formats );
  static std::string getFileformatAndSuffix( const std::string selectedFile, HardCopyListener::FileFormat &ff );
  void installEventLoop();
  void unmanage();
/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiEventLoopListener   *m_eventcontrol;
  std::string             m_title;
  std::string             m_filter;
  std::string             m_directory;
  bool                    m_no_file_exist;

};


#endif
