
#if !defined(__DP_DATA_DIMENSION__)
#define __DP_DATA_DIMENSION__

#include "datapool/DataDebugger.h"
#include "datapool/DataContainer.h"

class DataItem;
class DataPool;

/** Die DataDimension-Objekte verwalten eine Liste von DataContainer-Objekten,
    welche DataElement-Objekte oder wiederum DataDimension-Objekte sein können.
    Sie werden vom DataItem-Objekt verwaltet.
 */
class DataDimension : public DataContainer
{
  friend class DataItem;

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Der Konstruktor fügt eine neue Dimension im Datenbaum ein.
      @param item DataItem der betroffenen Container.
      @param data Container, welcher um eine Dimension erweitert wird
   */
  DataDimension( DataItem &item ,DataContainer *data );

  /** Destruktor
   */
  virtual ~DataDimension();

protected:
  /** Konstruktor zum Kopieren
      @param nDim Dimensionsnummer
      @param size Grösse des Vektors
   */
  DataDimension( int nDim, int size );

private:
  /** Copy-Konstruktor ist nicht implementiert
      \param ref
      \note Der Copy-Konstruktor steht nicht zur Verfügung
   */
  DataDimension( const DataDimension &ref );

  /** Zuweisungsoperator ist nicht implementiert
      \param ref
      \return
      \note Der Zuweisungsoperator steht nicht zur Verfügung
   */
  DataDimension& operator=( const DataDimension &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /// Debug-Funktion
  BUG_DECL_SHOW_COUNT("DataDimension");

  /** Die Funktion zeigt den Container-Typ an.
      @return Containertyp
   */
  virtual ContainerType getContainerType() const {return ContainerIsDimension; }

  /** Die Funktion liefert den this-Pointer als DataDimension.
      @return Pointer auf das DataDimension-Objekt
   */
  virtual DataDimension *getDataDimension(){ return this; }

  /** Die Funktion liefert den this-Pointer als DataDimension.
      @return Pointer auf das DataDimension-Objekt
   */
  virtual const

/** Beschreibung der Funktion getDataDimension
    \return
    \todo Die Beschreibung der Funktion getDataDimension fehlt
 */
DataDimension *getDataDimension() const { return this; }

  /** Die Funktion teilt mit, dass es sich um ein DataDimension-Objekt handelt.
      @return true
   */
  virtual bool isDataDimension() const { return true; }

  /** Die Funktion setzt den Container data an die Position 0 des Vektors.
      Diese Position muss zu diesem Zeitpunkt leer sein !!
      @param data Pointer auf den zu installierenden Container
      @param inx Index
   */
  void setContainer( DataContainer *data, int inx=0 );

  /** Die Funktion verlegt den Container an der Position inx_from zur
      Position inx_to.
      @param inx_from Von-Index
      @param inx_to Ziel-Index
      @param data Container für die frei werdende Position inx_from
   */
  void moveContainer( int inx_from, int inx_to, DataContainer *data );

  /** Die Funktion gibt die Anzahl der möglichen Elemente im Container an.
      (Es kann auch Löcher d.h. Null-Pointers im Array haben).
      @return Anzahl der Elemente
   */
  virtual int getNumberOfElements() const;

  /** Die Funktion liefert die Anzahl der Dimensionen im Container.
      @return Anzahl Dimensionen
   */
  virtual int getNumberOfDimensions() const;

  /** Die Funktion liefert die Anzahl der Elemente einer durch den
      Parameter bestimmten Dimension.
      @param dim_num Nummer der gewünschten Dimension
      @return Anzahl der Elemente
   */
  virtual int getDataDimensionSizeOf( int dim_num ) const;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param incl_attr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, bool incl_attr ) const;

  /** Die Funktion liefert die logische Grösse über alle Dimensionen.
      Es werden dabei die vorhanden Indexlisten verwendet.
      Nur dort, wo Wildcards gesetzt sind, wird die Dimensionsgrösse grösser als 1.
      @param dimsize Resultat-Liste der jeweiligen Dimensionsgrössen.
      @param dim Die Nummer der aktuellen Dimension
      @param index aktuelles DataInx-Objekt
      @return Grösse der Dimension
   */
  virtual int getAllDataDimensionSize( DataContainer::SizeList &dimsize, int dim, DataInx &index ) const;

  /** Die Funktion liefert die logische grösse der Dimension.
      @param incl_itemattr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
      @return logische grösse der Dimension
   */
  virtual int getValidDimensionSize( bool incl_itemattr=false ) const;

  /** Diese Funktion erstellt eine Kopie des Containers und liefert einen
      Pointer darauf.
      @return Pointer auf den neu erstellten Container
   */
  virtual DataContainer *clone() const;

  /** Diese Funktion überträgt die dynamischen Modifikationen des DataDictionary in den
      Datapool.
      @param datapool
      @param pDict
      @param AddOrDelete
      @param GlobalFlag
      @param FirstCycle
      @param pFirstCycleItem
   */
  virtual void DataDictionaryModified( DataPool &datapool
                                     , DataDictionary *pDict
                                     , bool AddOrDelete
                                     , bool GlobalFlag
                                     , bool FirstCycle
                                     , DataItem **pFirstCycleItem );

  /** Die Funktion teilt mit, ob gültige Datenelemente vorhanden sind.
      \return true => es sind gültige Datenelemente vorhanden
   */
  virtual bool hasValidElements( bool incl_itemattr ) const;

  /** Die Funktion teilt mit, ob gültige serialisierbare Datenelemente vorhanden sind.
      \param flags Flags für die Prüfung
      \return serialisierbare Elemente sind vorhanden
   */
  virtual bool hasSerializableValidElements( const SerializableMask flags ) const;

  /** Beschreibung der Funktion isValid
      \param x
      \return
      \todo Die Beschreibung der Funktion isValid fehlt
   */
  bool isValid( int x ) const;

  /** Die Funktion erstellt eine exakte Kopie des Containers und liefert den
      Pointer darauf.
      @param datapool Referenz auf den DataPool
      @return Pointer auf neuen Container
  */
  virtual DataContainer *copyContainer( DataPool &datapool ) const;

  /** Die Funktion löscht alle Werte im Struktur-Baum. Alle vorhanden DataItem's
      und ihre Attribute bleiben erhalten.
      \param incl_itemattr true => Die Item-Attribute werden auch gelöscht
      \return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus clearContainer( bool incl_itemattr );

  /** Die Funktion löscht alle Werte, Strukturen und DataItem's im Struktur-Baum.
      Die Attribute der DataItem's gehen dabei verloren.
      @return Status der durchgeführten Aktion.
  */
  virtual UpdateStatus eraseContainer();

  /** Der als Parameter übergebene Container wird am Ende des Dimensionsvektors
      angehängt. Die Funktion wird nur für das Erweitern der Resultatstruktur
      durch das Compare-Feature.
      @param cont Pointer auf das anzuhängende DataContainer-Objekt
   */
  void appendContainer( DataContainer *cont );

  /** Die Funktion fügt am gewünschten Ort (index) das DataContainer-Objekt bei. Bei Bedarf
      wird der Datenvektor erweitert. Ein eventuell bereits vorhandenes Element an der
      indexierten Stelle wird vorgängig gelöscht. ACHTUNG: Die Funktion prüft, ob es sich
      um den erwarteten Container-Typ handelt. DataElement-Objekte dürfen nur in der
      ersten Dimension eingefügt werden.
      @param index Index in den DatenVektor
      @param cont Pointer auf das einzufügende DataContainer-Objekt
   */
  void setContainer( int index, DataContainer *cont );

  /** Die Funktion schreibt das Element im XML-Format in den Output-Stream.
      @param ostr Output-Stream
      @param attrs String-Liste für die Attribute
      @param is_structitem
      @param level
      @param debug
      @param isScalar
      @return true -> erfolgreich
   */
   virtual bool writeXML( std::ostream &ostr
                        , std::vector<std::string> &attrs
                        , bool is_structitem
                        , int level
                        , bool debug
                        , bool isScalar );

  /** Die Funktion schreibt das Element im XML-Format in den Output-Stream.
      @param ostr Output-Stream
      @param attrs String-Liste für die Attribute
      @param containers
      @param is_structitem
      @param level
      @param debug
      @return true -> erfolgreich
   */
  virtual bool writeXML( std::ostream &ostr
                       , std::vector<std::string> &attrs
                       , ContainerList &containers
                       , bool is_structitem
                       , int level
                       , bool debug );

  /** Die Funktion schreibt das Element im JSON-Format in den Output-Stream.
      @param ostr Output-Stream
      @param is_structitem
      @param level
      @param indentation
      @param isScalar
      @param scale
      @param flags Flags für für die Prüfung von 'serializable'
      @return true -> erfolgreich
   */
  virtual bool writeJSON( std::ostream &ostr,
                          bool is_structitem,
                          int level,
                          int indentation,
                          bool isScalar,
                          double scale,
                          const SerializableMask flags );

  /** Die Funktion schreibt die Elemente des Containers im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
      @param index Index in einem Dimensionsvektor
   */
  virtual void writeDP( std::ostream &ostr, int i, int index ) const;

  /** Die Funktion ruft für ein oder mehrere indizierte Datenelemente (mit
      Wildcards) den Functor auf um die gewünschten Aktionen auszuführen.
      @param item Referenz auf das DataItem
      @param func Referenz auf den Functor für die Aktion
      @param inx  Referenz auf Index-Objekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus alterData( DataItem &item
                                , DataAlterFunctor &func
                                , DataInx &inx );


  /** Die Funktion liefert die Reference auf das indizierte DataElement. Falls
      noch nicht vohanden, wird das Element angelegt.
      @param item Referenz auf das DataItem
      @param inxlist Indexliste
      @param index aktueller Index
      @return Referenz auf das DataElement
  */
  virtual DataElement &getDataElement( DataItem &item,
                                       DataIndexList &inxlist,
                                       int index );

  /** Die Funktion liefert die Reference auf das indizierte DataElement. Falls
      noch nicht vohanden, wird das Element angelegt.
      @param item Referenz auf das DataItem
        @param inx Index
      @return Referenz auf das DataElement
  */
  DataElement &getDataElement( DataItem &item,
                               int inx );

  /** Die Funktion liefert die Reference auf das letzte indizierte DataContainer-Objekt.
      Falls noch nicht vohanden, wird das Objekt angelegt.
      @param item Referenz auf das DataItem
      @param inxlist Indexliste
      @param index aktuelle Indexnummer
      @param sz benötigte Grösse auf dem letzten Index
      @return Referenz auf das DataContainer-Objekt
  */
  virtual DataContainer &getDataContainer( DataItem &item,
                                           DataIndexList &inxlist,
                                           int index, int sz );

  /** Die Funktion vergleicht die Werte zweier DataContainer-Objekte. Abhängig vom
      CompareType manipuliert sie bei Bedarf die Daten des eigenen Objekts.
      @param cont Reference auf das zu vergleichende Objekt
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  virtual DataCompareResult compare( const DataContainer &cont, int fall, int max_faelle );

  /** Die Funktion wird von der DataDimension verwendet, um Strukturelemente mit
      gleichem SortKey zu vergleichen.
      @param dim DataDimension-Objekt für den Vergleich
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return DataCompareResult-Objekt
   */
  DataCompareResult compareSorted( const DataDimension &dim, int fall, int max_faelle );

  /** Die Funktion setzt für alle vorhandenen Elemente in der Struktur die entsprechenden
      Bits in der Attribut-Bitmaske.
      @param mask Maske mit den zu setzenden Attribut-Bits
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return true -> Die Bitmaske hat sich verändert
   */
  virtual bool setCompareAttributesOfAllElements( DATAAttributeMask mask, int fall, int max_faelle );

  /** Die DataDimension wird markiert als Spezialfall für das Darstellen von Vergleichen
      zwischen Strukturen mit dem Compare-Feature.
   */
  virtual void setCompareDimension();

  /** Die Funktion zeigt an, ob es sich um eine 'Compare'-DataDimension handelt.
      ( siehe setCompareDimension() )
      @return true -> Es ist eine 'Compare'-DataDimension
   */
  virtual bool isCompareDimension() const;

  /** Beschreibung der Funktion setReserve
      \param reserve
      \return
      \todo Die Beschreibung der Funktion setReserve fehlt
   */
  void setReserve( int reserve );

  /** Die Funktion liefert eine Pythonobjekt mit allen im Container vorhandenen Daten.
      @return Pointer auf ein PyObject.
   */
  virtual void *getPython();

  /** Die Funktion schreibt alle passenden Werte des Pythonobjekts in den Container.
      @param obj Referenz auf das Pythonobjekt
      @return Status des durchgeführten Updates.
   */
  virtual UpdateStatus setPython( const PythonObject &obj );

  ///
  virtual UpdateStatus setPython( const PythonObject &obj, DataItem &item, int dims );

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Diese Funktion fuegt einen vom DataItem neu erstellten Container an der
      Position Index ein. Es darf noch kein Objekt an dieser Stelle vorhanden sein.
      Bei Bedarf werden die benoetigten Dimensionen auch noch eingeschoben.
      @param index Index in den Vektor im DataDimension-Objekt
      @param item Referenz auf das entsprechende DataItem-Objekt
   */
  void installNewContainer( int index, DataItem &item );

  /** Die Funktion setzt den Container auf die gewünschte Grösse. Ab einer
      gewissen Grösse wird zusätzlicher Platz im Vector reserviert.
      @param new_size benötigte Grösse (size) des Vektors.
   */
  void resizeContainer( int new_size );

  /** Die Funktion setzt den Container exakt auf die gewünschte Grösse.
      @param new_size benötigte Grösse (size) des Vektors.
   */
  void setSizeOfContainer( int new_size );

  /** Diese Funktion ueberprueft, ob der Daten-vektor des Containers
      die erforderliche Groesse hat.
      @param sz Grösse (size) des benötigten Vektors.
      @return true -> der Daten-Vektor ist gross genug
   */
  bool isBigEnough(int sz ) const;

  /** Die Funktion vergleicht alle DataElemente über alle Dimensionen.
      @param cont Referenz auf das zu vergleichende Objekt (muss ein DataDimension-Objekt sein)
      @param fall Nummer des Vergleichs für das Einfügen der Resultat-Werte
      @param max_faelle maximal mögliche Vergleichsfälle
      @return CompareResult-Wert
   */
  DataCompareResult compareDimensions( const DataContainer &cont, int fall, int max_faelle );

  /** Die Funktion liefert den Pointer auf den DataContainer an der mit Index inx
      angegebenen Position. Ist der Index zu gross, wird ein Null-Pointer geliefert.
      @param inx Index des gewünschten Objekts
      @return Pointer auf DataContainer-Objekt
   */
  DataContainer *container( int inx );

  /** Die Funktion liefert den const-Pointer auf den DataContainer an der mit Index inx
      angegebenen Position. Ist der Index zu gross, wird ein Null-Pointer geliefert.
      @param inx Index des gewünschten Objekts
      @return Pointer auf DataContainer-Objekt
   */
  const DataContainer *container( int inx ) const;

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  int               m_NumDimensions;    // Anzahl Dimensionen IM SUBAST
  ContainerList     m_Containers;
  int               m_number_of_resize;
  bool              m_is_compare_dimension;

  BUG_DECLARE_COUNT;
};

#endif
/** \file */
