#ifndef LISTPLOT_H
#define LISTPLOT_H

#include "plot/RedisplayListener.h"
#include "app/HardCopyListener.h"
#include "gui/GuiElement.h"
#include "gui/FileSelectListener.h"

class ListGraph;

class ListPlot : public RedisplayListener,
		 public HardCopyListener {
 private:

 public:
  /// Konstruktor
  ListPlot( /*GuiElement *parent,*/ const std::string &name, int width = 148, int height = 105 )
    : m_saveListener( this ) {}
  /// Destruktor
  virtual ~ListPlot() {}

  virtual GuiElement* getElement() = 0;
#if defined HAVE_QT
    virtual QProcess* getQProcess() { assert(false); return NULL;};
#endif

  virtual void getSize(int &, int&)=0;

  /** Setzt den Plot-Titel
      @param title Titel-Text
   */
    virtual void setTitle( const std::string &title ) = 0;
  /** Plot-Titel
   */
  virtual const std::string& getTitle() const = 0;
  /** Setzt die Plot-Groesse
      @param width Breite des Plots
      @param height Hoehe des Plots
   */
    virtual void setSize( int width, int height ) = 0;
  /** Fügt eine neue (leere) ListGraph-Zeile ein
   */
    virtual void addHGraph() = 0;
  /** Fügt ein neues ListGraph-Objekt in der aktuellen Zeile ein
      @param name Name des ListGraph-Objektes
  */
    virtual ListGraph *addGraph( const std::string &name ) = 0;
  /** Definiert die Legende
      @param streamname Name des Streams
  */
    virtual void setCaptionStream( const std::string &streamname ) = 0;

    ///AMG neu hinzugefuegt!!!
  /** Liefert die Anzahl der Unterteilungen einer x-Achsenunterteilungen zurueck
      @return Anzahl der Unterteilungen einer x-Achsenunterteilung
   */
      virtual int getNSubDivX() = 0;
  /** Liefert die Anzahl der Unterteilungen einer y-Achsenunterteilungen zurueck
      @return Anzahl der Unterteilungen einer y-Achsenunterteilung
   */
      virtual int getNSubDivY() = 0;
  /** Liefert true zurueck, wenn eine gemeinsame y-Achsenunterteilung gewuenscht ist
   */
    virtual bool isSameYRange() = 0;
  /** Liefert den gr\"ossten, im MotifListPlot vorkommenden, y-Achsenwert
   */
    virtual int maxYRange() = 0;
  /** Liefert true zurueck, wenn alle Zyklen gemeinsam dargestellt werden sollen
   */
    virtual bool printAllCycles() = 0;

    virtual void updateConfigDialog( GuiElement::UpdateReason reason ) = 0;

  bool serializeJson(Json::Value &v, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);

/*=============================================================================*/
/* Definitions for Save-Function                                               */
/*=============================================================================*/
  virtual bool saveFile( GuiElement * );
  class SaveListener : public FileSelectListener
  {
  public:
    SaveListener( ListPlot *s ): m_listplot( s ) {}
    virtual void FileSelected( const std::string &filename
			       , const HardCopyListener::FileFormat format
			       , const std::string dir );
  private:
    ListPlot   *m_listplot;
  };

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  SaveListener                         m_saveListener;
};
#endif
