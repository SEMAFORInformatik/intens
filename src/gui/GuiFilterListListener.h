
#if !defined(GUI_FILTER_LIST_LISTENER_INCLUDED_H)
#define GUI_FILTER_LIST_LISTENER_INCLUDED_H

#include <vector>
#include <string>

class GuiFilterList;

/**
 * Die Klasse GuiFilterListListener ist die Schnittstelle zum
 * Listenobjekt GuiFilterList. Nur ueber diese Schnittstelle kann
 * kann das Listenobjekt angesprochen werden
 * @short Der Listener zum Listenobjekt GuiFilterList
 * @author Daniel Kessler
 */

class GuiFilterListListener
{
/*=============================================================================*/
/* Interface                                                                   */
/*=============================================================================*/
public:
  GuiFilterListListener():m_list(0), m_sortColumn(-1) {}

  typedef std::vector<std::string> StringsVector;
  /**
     Klasse welche die Ergebnisliste einer DB-Abrage beinhaltet
  */
  class RowsVector : public  std::vector<StringsVector> {
  public:
    /** Konstruktor */
    RowsVector() {}
    /** (Re-)initialisieren des Spaltentyp-Vektors
	WICHTIG: Erst nachdem interne Ergnisliste gesetzt wurde,
	diese Funktion aufrufen!! (wegen Spaltengroesse ...)
    */
    void initColumnTypeVector() { m_isNumeric.resize(size(), 0); }
    /** setzen der Spaltentyps (Numerisch, oder nicht Numerisch)*/
    void setColumnType(int nCol, bool bNumeric) { if (nCol < m_isNumeric.size()) m_isNumeric[nCol] = bNumeric; }
    /** Abfrage nach Spaltentyp (Ist Typ unbekannt, so ist er Nicht Numerisch) */
    bool isColumnTypeNumeric(int nCol) { return (nCol < m_isNumeric.size()) ? m_isNumeric[nCol] : 0; }

    /** setzen der Sortiering Parameter */
    static void setSortParams(int nCol, bool bNum) { s_sortColumn=nCol; s_sortNumeric=bNum; }
    /** groesser Sortierung */
    static bool greater( const StringsVector& svec1, const StringsVector& svec2 );
    /** kleiner Sortierung */
    static bool less( const StringsVector& svec1, const StringsVector& svec2 );

  private:
    std::vector<bool>        m_isNumeric;
    static int               s_sortColumn;
    static bool              s_sortNumeric;
  };

  typedef std::vector<int> IntVector;
  /**
   * Wird durch das Listenobjekt GuiFilterList aufgerufen.
   * rows.size() muss gleich rowLines.size() sein !
   * @param rows Muss mit dem Listeninhalt gefuellt werden. Leere Felder muessen
   * mit einem leeren String gefuellt werden. Pro Zeile ein StringVector.
   * @param rowLines Muss mit der jeweiligen Zeilenhoehe gefuellt werden.
   */
  virtual void getListItems( IntVector &rowLines, RowsVector &rows ) = 0;
  /**
   * Wird durch das Listenobjekt GuiFilterList aufgerufen.
   * @param lables Muss hier mit den Lables gefuellt werden. Leere Labels muessen
   * mit einem leeren String gefuellt werden. Kann leer bleiben, wenn die Labels noch nicht bekannt sind.
   * @return Null, wenn keine Labels angezeigt werden sollen.
   */
  virtual bool getLabels(StringsVector &labels) { return false; }
  /**
   * Wird durch das Listenobjekt GuiFilterList nur bei create() aufgerufen. Die Groesse des Vectors entspricht der
   * @param rowLines rowLines.size() muss der Anzahl sichtbarer Zeilen entsprechen.
   * Der Inhalt muss der jeweiligen Zeilenhoehe in Zeichen entsprechen.
   */
  virtual void initVisibleRows( IntVector &rowLines ) = 0;
  /** Wird durch GuiFilterList aufgerufen.
   * @return anzahl sichtbarer Zeilen.
   */
  virtual int getVisibleLines() = 0;
  /**
   * Wird durch das Listenobjekt GuiFilterList nur bei getListConf() aufgerufen.
   * @param cols cols.size() muss der Anzahl Kollonen entsprechen.
   * Der Inhalt muss der jeweiligen Spaltenbreite in Zeichen entsprechen.
   * @param vcols Muss der Anzahl sichtbarer Kollonen entsprechen.
   */
  virtual void getListColsConf( IntVector &cols, int &vcols ) = 0;
  /**
   * Wird durch das Listenobjekt GuiFilterList aufgerufen.
   * @param title Muss dem Titel entsprechen. Kann leer bleiben, wenn der Titel noch nicht bekannt ist.
   * @return Null, wenn kein Titel angezeigt werden soll.
   */
  virtual bool getTitle( std::string &title ) { return false; }
  /** Wird durch GuiFilterList aufgerufen.
   */
  virtual void selected( int row ) {}
  /** Wird durch GuiFilterList aufgerufen.
   */
  virtual void unselected( int row ) {}
  /** Wird durch GuiFilterList aufgerufen.
   */
  virtual void activated( int row ) {}
  /**
   * @return selectedRow
   */
  int getSelection();
  /**
   * Wird vom Konstruktor von GuiFilterList aufgerufen.
   * @param list Pointer auf GuiFilterList
   */
  void setMyList(GuiFilterList *list);
  void clearList();
  /**
   * @return Anzahl Zeilen
   */
  int rebuildList();
  void grabListFocus ();
  void enableList();
  void disableList();

  /** Setzen der Sortierungsspalte */
  void setSortColumn(int c) { m_sortColumn = c; }
  /** Abfrage der Sortierungsspalte (Spaltenindex negative => Sortierungsspalte ist nicht gesetzt)*/
  int  getSortColumn() { return m_sortColumn; }

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:
  GuiFilterList      *m_list;
  int                 m_sortColumn;
};

#endif
