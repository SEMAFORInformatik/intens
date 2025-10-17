
#if !defined(HELP_TEXT_INCLUDED_H)
#define HELP_TEXT_INCLUDED_H

#include <string>
#include <vector>

#include "gui/GuiScrolledlistListener.h"
#include "gui/GuiButtonListener.h"
#include "app/HelpFile.h"

class HelpManager;
class QWidget;
class GuiForm;
class GuiDataField;
class GuiScrolledText;
class GuiScrolledlist;
class XferDataItem;

/** Help in Form eines ASCII-Files.
 * Abgeleitet von der Klasse HelpFile.
 * @short Help in Form eines ASCII-Files.
 */
class HelpText : public HelpFile, public GuiScrolledlistListener{
  //---------------------------------------------------------------------------//
  // Constructor / Destructor                                                  //
  //---------------------------------------------------------------------------//
 public:
  HelpText(HelpManager *helpManager);
  virtual ~HelpText();

  /** Implementierung der Methode start vom GuiScrolledlistListener */
  virtual bool start();
  /** Implementierung der Methode getNextItem vom GuiScrolledlistListener */
  virtual bool getNextItem( std::string &label );
  /** Implementierung der Methode activated vom GuiScrolledlistListener */
  virtual void activated( int index );
  /** Implementierung der Methode selected vom GuiScrolledlistListener */
  virtual void selected( int index, bool  );
  /** Implementierung der Methode selected vom GuiScrolledlistListener */
  virtual void setValue(int, const std::string&) {};

  //---------------------------------------------------------------------------//
  // protected classes                                                         //
  //---------------------------------------------------------------------------//
 protected:
  /** Die Klasse HelpChapter enthaelt alle Angaben eines Kapitels.
   * @short
   * @param m_Start Position des ersten Zeichens (ohne Titel).
   * @param m_End Position des letzten Zeichens.
   */
  class HelpChapter{
    public:
      HelpChapter(std::string title = "", long int start = 0, long int end = 0)
	: m_Title( title)
	, m_Start( start )
	, m_End( end ){
      }

      std::string	 m_Title;
      long int	 m_Start;
      long int	 m_End;
  };


  //---------------------------------------------------------------------------//
  // public member functions                                                   //
  //---------------------------------------------------------------------------//
 public:
 /**
   * Hier werden die Help-Buttons in der Form installiert und mit dem dem HelpFile
   * verbunden
   * Wird direkt vom Parser aufgerufen.
   */
  virtual void installHelpCallback();
 /** Das Anzeigen von HelpText wird gestartet.
   * @param keyName Leer
   */
  virtual void help(const std::string &helpKey = std::string(), const int chapterNo = 1);
  bool openHelpFile();
  void closeHelpFile();
  /** Das Objekt wird nach Titel, Kapitel und HelpKeys abgesucht
   */
  bool inspectHelpText();

  void next();
  void previous();
  void cancel();
  void create();

  //---------------------------------------------------------------------------//
  // private member functions                                                   //
  //---------------------------------------------------------------------------//
 private:
  void createButtonbar();
#ifdef HAVE_QT
  void createScrolledList( QWidget* parent );
  void createScrolledText( QWidget* parent );
#else
  void createScrolledList( void* parent );
  void createScrolledText( void* parent );
#endif
  void createHelpDialog();
  HelpChapter *installChapter();
  void loadChapter(const HelpChapter &chapter);
  void helpDialog(int chapterNo);
  void initializeHelpDialog();


  //---------------------------------------------------------------------------//
  // private member classes                                                    //
  //---------------------------------------------------------------------------//

 private:
  class HelpNextListener : public GuiButtonListener{
   public:
     HelpNextListener(HelpText *helpFile): m_HelpFile( helpFile ){}
     virtual ~HelpNextListener() {}

     virtual void ButtonPressed(){m_HelpFile->next();}
    JobAction* getAction(){ return 0; }
   private:
     HelpText *m_HelpFile;

  };

  class HelpPreviousListener : public GuiButtonListener{
   public:
     HelpPreviousListener( HelpText *helpFile ): m_HelpFile( helpFile ){}
     virtual ~HelpPreviousListener() {}

     virtual void ButtonPressed(){m_HelpFile->previous();}
    JobAction* getAction(){ return 0; }
  private:
    HelpText *m_HelpFile;
  };

  class HelpCancelListener : public GuiButtonListener{
   public:
     HelpCancelListener( HelpText *helpFile ): m_HelpFile( helpFile ){}
     virtual ~HelpCancelListener() {}

     virtual void ButtonPressed(){m_HelpFile->cancel();};
    JobAction* getAction(){ return 0; }
   private:
     HelpText *m_HelpFile;
  };

  //---------------------------------------------------------------------------//
  // private member data                                                       //
  //---------------------------------------------------------------------------//
 private:
  typedef std::vector<HelpChapter>      HelpChapterList;

  std::ifstream  	  m_File;
#ifdef HAVE_QT
  GuiForm*	       m_DialogWidget;
  GuiDataField*    m_ChapterWidget;
  GuiScrolledText* m_TextWidget;
  GuiScrolledlist* m_ListWidget;
#else
  Widget	  m_DialogWidget;
  Widget	  m_ChapterWidget;
  Widget	  m_TextWidget;
  Widget	  m_ListWidget;
#endif
  bool            m_IsPoppedUp;
  HelpChapterList m_ChapterList;
  HelpChapterList::iterator m_chapterIter;
  int		  m_Selected;
  XferDataItem* m_labelXfer;

  HelpNextListener      m_NextListener;
  HelpPreviousListener  m_PreviousListener;
  HelpCancelListener    m_CancelListener;

  static const char     s_TITLE_CHAR;
  static const char     s_CHAPTER_CHAR;
  static const char     s_KEYWORD_CHAR;
};

#endif
