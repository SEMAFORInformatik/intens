
#if !defined(__DP_DATADICTIONARY_H__INCLUDED__)
#define __DP_DATADICTIONARY_H__INCLUDED__

#include <string>
#include <string.h>
#include <vector>
#include <complex>
#include <assert.h>

#include "app/LSPItem.h"
#include "datapool/DataGlobals.h"
#include "datapool/DataDebugger.h"

class DataPool;
class DataDictionaryRoot;
class DataReference;
class DataUserAttr;

/** Der Baum aus DataDictionary-Objekten ergibt die logische Struktur des DataPool.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataDictionary : public DataGlobals, public LSPItem
{
/*=============================================================================*/
/* Datendefinitionen                                                           */
/*=============================================================================*/
public:
  enum
    { InvalidIndex /** Beschreibung fehlt */  = -1  // Ungueltiger Itemindex
    };
  /** \enum
      \todo Die Beschreibung der Aufzaehlungswerte ist unvollstaendig
   */


/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Dieser Konstruktur wird nur für das Erstellen des Root-Objekts benötigt.
      @param userattr Ein Objekt wird benötigt als Factory für weitere Objekte.
   */
  DataDictionary( DataUserAttr *userattr = 0 );

  /** Beschreibung der Funktion DataDictionary
      \param Name
      \param Type
      \param ItemIsGlobal
      \param ItemIsInternal
      \todo Die Beschreibung der Funktion DataDictionary fehlt
   */
  DataDictionary( const std::string & Name,
                  DataType Type,
                  bool ItemIsGlobal,
                  bool ItemIsInternal );

  /** Beschreibung der Funktion ~DataDictionary
      \return
      \todo Die Beschreibung der Funktion ~DataDictionary fehlt
   */
  virtual ~DataDictionary();

protected:

  /** Beschreibung der Funktion DataDictionary
      \param special
      \todo Die Beschreibung der Funktion DataDictionary fehlt
   */
  DataDictionary( int special );

  /** Beschreibung der Funktion DataDictionary
      \param ref
      \todo Die Beschreibung der Funktion DataDictionary fehlt
   */
  DataDictionary( const DataDictionary &ref );

  /** Beschreibung der Funktion operator=
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator= fehlt
   */
  DataDictionary& operator=( const DataDictionary &ref );

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataDictionary");

  /* ------------------------------------------------------------------------- */
  /* DataDictionary Konstruktion.                                              */
  /* Die folgenden Funktionen dienen dem Aufbau des Data-Dictionaries. Sie     */
  /* arbeiten auf dem Level der Dictpointers und fuegen neue Elemente am       */
  /* aktuellen Node ein.                                                       */
  /* ------------------------------------------------------------------------- */

  /** Die Funktion fügt ein Item auf gleichem Level ein wie das aktuelle Item und gibt
      als Resultat den Pointer darauf zurueck.
      Im Falle des types type_StructVariable muss die Variable StructRef auf
      ein Item vom Typ type_StructDefinition gesetzt sein.
      Im Fehlerfall kommt null zurueck. Ein Fehler tritt auf, wenn der Name
      nicht eindeutig ist oder wenn das Memory nich alloziert werden kann.
      \param Name Name des neuen Items
      \param Type Datentyp des neuen items
      \param StructRef Pointer auf die Strukturdefinition bei Datentyp Strukturvariable.
      \param ItemIsGlobal true => das neue Item ist eine globale Variable und wird nicht
             pro Cycle separat geführt.
      \param ItemIsInternal
      \return Pointer auf das neu erstellte DataDictionary-Objekt
  */
  DataDictionary *AddNextItem( const std::string & Name,
                               const DataType Type,
                               DataDictionary * StructRef = 0,
                               bool ItemIsGlobal = false,
                               bool ItemIsInternal = false );

  /** Beschreibung der Funktion AddNextItem
      \param dict
      \return
      \todo Die Beschreibung der Funktion AddNextItem fehlt
   */
  DataDictionary *AddNextItem( const DataDictionary &dict );

  /** Die Funktion funktioniert aehnlich wie AddNextItem(). Das aktuelle Item muss
      vom Typ type_StructDefinition sein und das neue Item WIRD die Erste variable
      in dieser Struktur. Es wird ein Pointer auf das neue Item zurück gegeben.
      \param Name Name des neuen Items
      \param Type Datentyp des neuen items
      \param StructRef Pointer auf die Strukturdefinition bei Datentyp Strukturvariable.
      \param ItemIsGlobal true => das neue Item ist eine globale Variable und wird nicht
             pro Cycle separat geführt.
      \param ItemIsInternal
      \return Pointer auf das neu erstellte DataDictionary-Objekt
  */
  DataDictionary *AddFirstMember( const std::string & Name,
                                  const DataType Type,
                                  DataDictionary * StructRef = 0,
                                  bool ItemIsGlobal = false,
                                  bool ItemIsInternal = false );

  /** Beschreibung der Funktion AddFirstMember
      \param dict
      \return
      \todo Die Beschreibung der Funktion AddFirstMember fehlt
   */
  DataDictionary *AddFirstMember( const DataDictionary &dict );

  /* ------------------------------------------------------------------------- */
  /* Benutzer-Attribute.                                                       */
  /* Jedem Dictionary-Eintrag ist eine Instanze der UserAttribute zugeordnet.  */
  /* Der Datapool-Benutzer kann damit tun und lassen, was er will. Eigentlich  */
  /* interessiert uns dies ueberhaupt nicht.                                   */
  /* ------------------------------------------------------------------------- */

  /** Die Funktion liefert den Pointer auf das User-Attribute-Objekt.
      Der Aufrufer ist für den Inhalt des Objekts verantwortlich. Der DataPool
      hat keine Verwendung dafür. Das Objekt muss vorhanden sein.
  */
  DataUserAttr *GetAttr() const;

  /** Beschreibung der Funktion getDataUserAttr
      \return
      \todo Die Beschreibung der Funktion getDataUserAttr fehlt
   */
  const DataUserAttr *getDataUserAttr() const;

  /** Die Funktion installiert ein UserAttribute-Objekt. Als Muster dient
      das als Parameter übergebene Objekt .
  */
  void AllocAttr( DataUserAttr *pParentAttribute );

  /* ------------------------------------------------------------------------- */
  /* Namens-Funktionen                                                         */
  /* Jeder Dictionary-Node hat einen Namen. Wir unterscheiden 3 Faelle. Zum    */
  /* einen der simple Name, wie er dklariert wird und unabhaengig davon, wo    */
  /* er verwendet wird. Zum anderen gibt es noch den statischen Name. Dieser   */
  /* Besteht aus dem Simplen Namen plus allen uebergeordneten Namen. Dies sind */
  /* die Struktur-Definitionen fuer verschachtelte Items.                      */
  /* Zuletzt gibt es noch den dynamischen Namen. Dieser verwendet die Struktur-*/
  /* variable als uebergeordnete Instanz. Dies kann im Datadictionary jedoch   */
  /* nicht generiert werden, da das Datadictionary ein statisches Gebilde ist. */
  /* Die dynamischen Namen sind in der DataReference zu finden (wo sonst).     */
  /* ------------------------------------------------------------------------- */

  /** Die Funktion liefert den Namen des Dictionary-Eintrags als Resultat.
      @return Referenz auf den Namen
   */
  const std::string &getName() const;

  /** Die Funktion schreibt den Namen des Dictionary-Eintrags in den Parameter.
      @param name Referenz auf den string für das Resultat
   */
  void getName( std::string &name ) const;

  /** Die Funktion liefert den vollen statischen Namen des Items. Der statische Name
      ist der Name, wie er innerhalb einer Struktur definiert wurde (im Gegensatz
      zum Namen innerhalb einer Struktur-Variablen)
      @return String mit dem Namen
   */
  const std::string getStaticName() const;

  /** Die Funktion liefert den vollen statischen Namen des Items. Der statische Name
      ist der Name, wie er innerhalb einer Struktur definiert wurde (im Gegensatz
      zum Namen innerhalb einer Struktur-Variablen)
      @param name String mit dem Namen
   */
  void getStaticName( std::string &name ) const;

  /** Die Funktion zeigt an, ob es sich beim Parent-Item um das Root-Item des
      Dictionary handelt.
      \return true => das Parent-Item ist Root. Das Root-Item liefert immer false.
   */
  virtual bool isParentNodeRoot() const;

  /** Die Funktion zeigt an, ob es sich um das Root-Item des Dictionary handelt.
      \return true => das Objekt ist das Root-Item.
   */
  virtual bool isRootNode() const { return false; }

  /* ------------------------------------------------------------------------- */
  /* DatenTypen                                                                */
  /* Jeder Dictionary-Eintrag hat einen bestimmten Datentyp. Unter gewissen    */
  /* Umstaenden kann dieser auch nach der allokation geaendert werden.         */
  /* ------------------------------------------------------------------------- */

  /** Die Funktion liefert den Datentyp des Dictionary-Objekts.
      @return DataType (Datentyp des Dictionary-Objekts)
   */
  DataType getDataType() const;

  /** Die Funktion setzt den Datentyp neu. Dies ist nur moeglich, wenn der originale
      Datentyp und der neue ein BasicDataType sind.
      ACHTUNG: Diese Funktion DARF NUR aufgerufen werden, bevor der Datenpool alloziert
      wird (d.H. bevor NewCylce aufgerufen wird. Sonst ist diese Manipulation fatal,
      da das Dictionary und die Daten nicht mehr zusammen passen.
      @param typ Neuer Datentyp
   */
  void changeDataType( DataType typ );

  /* ------------------------------------------------------------------------- */
  /* Default-Dimensions-Groessen                                               */
  /* Normalerweise wird in einem DataItem die Dimensionsmatrix dynamisch ange- */
  /* legt. Dies geschieht mehr oder weniger auf der Basis, was gerade Benoetigt*/
  /* wird. Nun kann es aber sein, dass der Benutzer genau weiss, was er will.  */
  /* In diesem Falle kann er Defaultgroessen definieren. Beim Anlegen der      */
  /* Dimensionen werden dann diese verwendet. Damit wir ein kontinuierliches   */
  /* resize effektiv unterbunden. Implementiert ist das ganze mit einem Vektor */
  /* der fuer jede Dimension einen Index enthaelt. Beim Allozieren der Dimen-  */
  /* sion n wird der Wert an n-ter Stelle des Vektors fuer die Groesse verwen- */
  /* det.                                                                      */
  /* Achtung: Dimension 0 wird NIE verwendet. Per Definition ist Dimension das */
  /* DatenElement selbst und kann desshalb keine Ausdehnung haben!!!           */
  /* ------------------------------------------------------------------------- */

  /** Die Funktion liefert die Anzahl der Dimensionen, welche beim Setzen
      eines Elements mindestens angelegt werden sollen.
      \return Anzahl Default-Dimensionen
   */
  int getDefaultDimensions();

  /** Die Funktion liefert die Default-Dimension des Levels nDim. Beim Anlegen
      einer DataDimension wird der Vektor für die DataContainer mit dieser
      Default-Grösse angelegt. Dieser Mechanismus erspart ein häufiges Resizen.
      \param nDim Nummer der Dimension
      \return Grösse der Dimension
   */
  int GetDefaultDimension( int nDim );

  /**
   * unsetFolder
   * verhindert, dass die Struktur im Navigator
   * als Folder dargestellt wird.
   */
  void unsetFolder(); // DAS HAT HIER NICHTS VERLOREN !!!  BHO

  /** Beschreibung der Funktion getFolderFlag
      \return
      \todo Die Beschreibung der Funktion getFolderFlag fehlt
   */
  bool getFolderFlag() const; // DAS HAT HIER NICHTS VERLOREN !!!  BHO

  /**
   * SetDefaultDimension:
   *    Setzt die Default-Dimension des Leveln n. Die
   *    Groesse des Arrays (Anzahl Levels) wird bei Bedarf angepasst.
   *    Die Default-Dimension setzt die Groesse, auf die ein
   *    Item des Levels n defaultmaessig alloziert werden soll.
   *    Dieser Mechanismus erspart ein haeufiges Resizen.
   */
  void SetDefaultDimension( int nDim, int val );

  /** Diese Funktion setzt alle DefaultDimensions auf 0. Dies kann
      gebraucht werden, wenn ein Dictitem geloescht wird oder wenn
      die Applikation eine andere Direktive erhaelt (z.B. Deklaration oder
      ein explizites Resize (existiert sowas ueberhaupt???)
   */
  void ClearDefaultDimension();

  /* ------------------------------------------------------------------------- */
  /* ItemIndex                                                                 */
  /* Jedes Data - Item innerhalb einer Struktur-Definition hat einen Index.    */
  /* Dieser  wird im DataItemContainer verwendet um das Item schnell zu loka-  */
  /* lisieren. Da der gesamte Datapool in einer Superstruktur eingebettet ist, */
  /* bedeutet dies, das jedes DataItem einen ItemIndex braucht.                */
  /* Nondata-Items hingegen brauchen nur eine Dictionary-Eintrag. Dies sind    */
  /* momentan Struktur-Definitionen und geloescte Items.                       */
  /* ------------------------------------------------------------------------- */

  /**
   * GetItemIndex:
   *    Liefert den ItemIndex
   */
  inline int GetItemIndex(void) const {return m_ItemIndex; }

  /** Die Funktion setzt das 'Global'-Attribut einer Variablen.
      Das Attribut bewirkt, dass die angelegte Variable ausserhalb von der
      Cycle-Verwaltung nur einmal vorkommt. Bei Cycle-Operationen im Datapool
      wie NewCycle(), ClearCycle() und RemoveCycle() werden globale Variablen
      nicht angerührt.
      \param yes true => das Attribut wird gesetzt
   */
  inline void setItemGlobal( bool yes=true ) { m_ItemIsGlobal = yes; }

  /** Die Funktion zeigt an, ob die Variable als 'global' definiert worden ist.
      (siehe auch Funktion setItemGlobal() )
      \return true => die Variable ist global.
   */
  inline bool ItemIsGlobal() const { return m_ItemIsGlobal; }

  /** Die Funktion setzt das 'Internal'-Attribut einer Variablen.
      Das Attribut zeigt an, ob die angelegte Variable nur intern sichtbar ist.
      \param yes true => das Attribut wird gesetzt
   */
  inline void setItemInternal( bool yes=true ) { m_ItemIsInternal = yes; }

  /** Die Funktion zeigt an, ob die Variable als 'intern' definiert worden ist.
      (siehe auch Funktion setItemInternal() )
      \return true => die Variable ist nur intern sichtbar.
   */
  inline bool ItemIsInternal() const { return m_ItemIsInternal; }

  /** Die Funktion setzt das 'Protected'-Attribut einer Variablen.
      Das Attribut bewirkt, dass die angelegte Variable bei einem ClearCycle()
      nicht angerührt wird.
      \param yes true => das Attribut wird gesetzt
   */
  inline void setItemProtected( bool yes=true ) { m_ItemIsProtected = yes; }

  /** Die Funktion zeigt an, ob die Variable als 'protected' definiert worden ist.
      (siehe auch Funktion setItemProtected() )
      \return true => die Variable ist protected.
   */
  inline bool ItemIsProtected() const { return m_ItemIsProtected; }

  /** Die Funktion setzt das 'WithoutTTrail'-Attribut einer Variablen.
      Das Attribut bewirkt, dass bei einer Mutation innerhalb einer Transaction
      im TTrail kein Protokoll der Änderung gemacht wird.
   */
  inline void setItemWithoutTTrail() { m_ItemWithoutTTrail = true; }

  /** Die Funktion zeigt an, ob die Variable als 'withoutTTrail' definiert worden ist.
      (siehe auch Funktion setItemWithoutTTrail() )
      \return true => für die Variable wird kein TTrail-Protokoll geführt.
   */
  inline bool ItemIsWithoutTTrail() const { return m_ItemWithoutTTrail; }

  /** Die Funktion setzt den Pointer des Parent-Objekts.
      \param dict Pointer auf das Parent-DataDictionary-Objekt.
   */
  void setParent( DataDictionary *dict );

  /** Die Funktion liefert den Pointer auf das Parent-Objekt.
      \return Pointer auf das Parent-DataDictionary-Objekt.
   */
  DataDictionary *GetParent() const;

  /** Die Funktion setzt den Pointer das nächste Objekt in derselben Struktur.
      \param dict Pointer auf das nächste DataDictionary-Objekt.
   */
  void setNext( DataDictionary *dict );

  /** Die Funktion liefert den Pointer auf das nächste Objekt in derselben Struktur.
      \return Pointer auf das nächste DataDictionary-Objekt.
   */
  DataDictionary *getNext() const;

  /** Die Funktion zeigt an, ob eine Struktur-Node vorhanden ist.
      \return true => Node ist vorhanden
   */
  bool hasStructureNode() const;

  /** Die Funktion liefert den Pointer auf die Structure-Definition oder Strukture-Node.
      \return Pointer auf die Strukturdefinition
   */
  DataDictionary *getStructure() const;

  /** Die Funktion liefert die Referenz auf den Structure-Node. Dieser muss
      vorhanden sein.
   */
  DataDictionary &structure() const;

  /**
   * GetNumberOfNextNodes
   *    Zeahlt die Anzahl Nextnodes. Der Laufende Node wird
   *    ebenfalls dazugerechnet
   */
  int GetNumberOfNextNodes(void) const;

  /**
   * GetNumberOfNextNodes
   *    Gibt den hoechsten vergebenen Index zurueck. Bei einer leeren Liste
   *    Liste kann dies auch -1 sein
   */
  int GetHighestIndex(void) const;

  /**
   * GetLastEntry
   *    Holt den letzten Eintrag aus der Kette. Diese Funktion ist fuer vergessliche
   *    Leute gedacht, die den Pointer irgendwie verlocht haben.
   */
  DataDictionary * GetLastEntry(void) const;

  /** Die Funktion sucht auf der aktuellen Ebene das DataDictionary-Objekt mit dem
      entsprechenden Namen und gibt den Pointer darauf zurück, resp. den Null-Pointer,
      wenn der Name nicht vorkommt.
      @param pName Pointer auf den ersten Character des gesuchten Namens
      @param len Länge des Namens
      @return Pointer auf das DataDictionary-Objekt
   */
  DataDictionary *SearchByName( const char * pName, int len=0 ) const;

  /** Die Funktion sucht auf der aktuellen Ebene das DataDictionary-Objekt mit dem
      entsprechenden Namen und gibt den Pointer darauf zurück, resp. den Null-Pointer,
      wenn der Name nicht vorkommt.
      @param Name gesuchter Name
      @return Pointer auf das DataDictionary-Objekt
   */
  DataDictionary *SearchByName( const std::string &Name ) const;

  /** Die Funktion teilt mit, ob der angegebene Name in der aktuellen Ebene eindeutig ist,
      unter Verwendung von SearchByName().
      @param name Pointer auf den ersten Character des gesuchten Namens
      @param len Länge des Namens
      @return true -> Name ist noch nicht vorhanden
   */
  bool isNameUnique( const char *name, int len = 0 ) const;

  /** Die Funktion teilt mit, ob der angegebene Name in der aktuellen Ebene eindeutig ist,
      unter Verwendung von SearchByName().
      @param name gesuchter Name
      @return true -> Name ist noch nicht vorhanden
   */
  bool isNameUnique( const std::string &name ) const;

  /**
   * SearchByIndex
   *    Gibt einen Pointer auf das DataDictionary mit dem gewünschten
   *    ItemIndex oder 0 wenn es nicht vorkommt. Es wird nur auf dem
   *    Aktuellen Level gesucht (Der geamte Level).
   *
   */
  DataDictionary * SearchByIndex(int index) const;

  /**
   * IsValidType:
   *    Ueberprueft, ob der uebergeben Datentyp Gueltig ist
   */
  static inline bool IsValidType(DataType d)
    {return (d == type_Integer          ||
             d == type_Real             ||
             d == type_String           ||
             d == type_Complex          ||
             d == type_CharData         ||
             d == type_StructDefinition ||
             d == type_StructVariable     );
    }

  /**
   * IsDefinedType:
   *    Ueberprueft, ob der uebergeben Datentyp definiert ist
   */
  static inline bool IsDefinedType(DataType d)
    {return (IsValidType(d)     ||
             d == type_None     ||
             d == type_Bad        );
    }


  /**
   * IsBasicType:
   *    Gibt true zurueck, wenn der Typ einer der Grundlegenden
   *    Typen ist.
   */
  static inline bool IsBasicType(DataType d)
    {return (d == type_Integer          ||
             d == type_Real             ||
             d == type_String           ||
             d == type_Complex          ||
             d == type_CharData           );
    }

  /**
   * TypeNeedsAlloc:
   *    Gibt true zurueck, wenn der Typ memory verwendet im Datenpool
   */
  static inline bool TypeNeedsAlloc(DataType d)
    {return (IsBasicType(d)             ||
             d == type_StructVariable     );
    }

  /** Diese Funktion vergleicht zwei Dictionary-Objekte und teilt mit, ob
      ob sie identisch sind. Identisch heisst, dass sie den gleichen Typ
      und beim Typ Struktur-Variable den gleichen Strukturtyp haben.
      \param dict Referenz auf das zu vergleichende DataDictionary-Objekt
  */
  bool isSameDataType( DataDictionary &dict ) const;

  /** Diese Funktion vergleicht zwei Dictionary-Objekte und teilt mit, ob
      ob sie identisch sind und den gleichen Namen haben.
      (siehe isSameDataType() )
      \param dict Referenz auf das zu vergleichende DataDictionary-Objekt
  */
  bool isSameDataTypeAndName( DataDictionary &dict ) const;

  /** Die Funktion setzt den DatenType auf Invalid und löscht eventuell vorhandene
      abhängige Nodes. Dies ist eine Notmassnahme. Ein Versuch wieder
      aufzuraeumen, wenn etwas schief gelaufen ist. Diese Funktion
      sollte nur verwendet werden, wenn man entweder weiss, was man
      tut oder in einem Notfall.
      \deprecated Dies Funktion sollte nicht mehr verwendet werden
   */
  void SetDataTypeInvalid();

  /**
   * MarkNodeDeleted:
   *    Markiert den aktuellen Node als Geloescht. Dazu wird der Typ auf none
   *    gesetzt und die Struct-Entries geloescht.
   *    Achtung: Es darf NIEMALS eine Strukturdefinition auf diese Weise bahendelt
   *             werden.
   */
  void MarkNodeDeleted(void);

  /**
   * IsNodeDeleted
   *    Findet heraus, ob dies ein geloeschter Node ist
   */
  bool IsNodeDeleted(void) const;

  /** Die Funktion liefert einen Pointer auf den Datapool
   */
  DataPool *GetDataPool();

  /** Die Funktion liefert eine Referenz auf das DataPool-Objekt.
      @return Referenz auf den DataPool.
   */
  virtual DataPool &datapool() const;

  /** Die Funktion teilt mit, ob der Datenpool bereits alloziert worden ist.
      \return true => Datapool ist alloziert
   */
  bool DataPoolAllocated();

  /** Die Funktion teil mit, ob das Zeichen in einem Namen vorkommen darf.
      @param c Character
      @return true -> der Character ist erlaubt
   */
  static bool isValidNameChar(char c);

  /** Die Funktion teil mit, ob ein String aus gültigen Zeichen besteht und
      als Name zugelassen ist.
      @param pt Character-Array
      @param len Länge des Character-Arrays
      @return true -> der String ist ok
   */
  static bool isValidNameString( const char * pt, int len = 0 );

  /** Die Funktion teil mit, ob ein String aus gültigen Zeichen besteht und
      als Name zugelassen ist.
      @param name zu prüfender Name
      @return true -> der String ist ok
   */
  static bool isValidNameString( const std::string &name );

  /** Die Funktion macht Einen Ausdruck des DataDictionary auf die Standard-
      Ausgabe. Dies ist ein Debugging-Feature.
   */
  DataDictionary *PrintDataDictionary( int Level=0 );

  /** Die Funktion kopiert alle Items im DataDictionary dict und hängt sie an
      das bestehende DataDictionary an.
      @param dict R>eferenz auf DataDictionary
   */
  void copyDataDictionary( const DataDictionary &dict );

  /** Beschreibung der Funktion hasSortKey
      \return
      \todo Die Beschreibung der Funktion hasSortKey fehlt
   */
  bool hasSortKey() const;

  /** Beschreibung der Funktion isSortKey
      \return
      \todo Die Beschreibung der Funktion isSortKey fehlt
   */
  bool isSortKey() const;

  /** Die Funktion gibt den Inhalt des DataDictionary im XML-Format aus und
      ruft sich dabei rekursiv auf.
      @param ostr Outputstream
      @param i Anzahl einzurückende Leerstellen
      @return Pointer auf den nächsten Dictionary-Eintrag auf derselben Ebene.
   */
  DataDictionary *writeDD( std::ostream &ostr, int i );

  /** Die Funktion gibt den Inhalt des DataDictionary-Eintrags im XML-Format aus.
      @param ostr Outputstream
   */
  void writeDD( std::ostream &ostr );

  bool isInternalName() const;

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
protected:
  /** Die Funktion teil mit, ob ein Parent-Node vorhanden ist.
   */
  bool hasParentNode() const;

  /** Die Funktion liefert die Referenz auf den Parent-Node. Dieser muss
      vorhanden sein.
   */
  DataDictionary &parent() const;

  /** Die Funktion teil mit, ob ein nächster Node in der Kette vorhanden ist.
   */
  bool hasNextNode() const;

  /** Die Funktion liefert die Referenz auf den nächsten Node in der Kette.
      Dieser muss vorhanden sein.
   */
  DataDictionary &next() const;

  /** Die Funktion liefert den Pointer auf das erste Dictionary-Objekt einer Kette.
      \return Pointer auf das erste Objekt in der Kette
   */
  DataDictionary * GetFirstUserEntry() const;

  /**
   * GetRootDictionary:
   *    Liefert den Pointer des Root-Dictionaries
   */
  DataDictionary * GetRootDictionary(void);

  /**
   * SearchDeletedNode
   *    Die Funkion sucht in der Kette, ob ein geloeschter Node existiert, der
   *    wieder verwedndet werden kann. Ein Node kann verwendet werden wenn er:
   *    1) Type type_None hat
   *    2) Einen Namen, der laenger als 2 ist
   *    3) Die ersten 2 Buchstaben des Namens mit "$$" beginnen.
   */
  DataDictionary * SearchDeletedNode(void) const;

  /**
   * ReuseNode
   * Fuehre einen geloeschten Node wieder einem Sinnvollen Zweck zu. D.h. er
   * wird wieder verwendet.
   */
  void ReuseNode( const std::string & Name,
                  DataType Type,
                  DataDictionary * StructRef,
                  bool ItemIsGlobal,
                  bool ItemIsInternal );

  bool isInternalName( const std::string &name );

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
private:
  typedef std::vector<int> DimensionSizeList;

  std:: string      m_Name;               // Name des Items
  DataUserAttr    * m_pAttributes;        // User-Attribute
  DataDictionary  * m_pNext;              // Pointer auf naechstes Element
  DataDictionary  * m_pStruct;            // Pointer auf structur-Member
  DataDictionary  * m_pParentNode;        // Parent Node
  DataType          m_DatenType;          // Typ der Daten
  int               m_ItemIndex;          // Index dieses Items
  bool              m_ItemIsGlobal;       // Das Item ist nicht den Cycleswitches unterworfen
  bool              m_ItemIsInternal;     // Das item wird nur für interne Zwecke verwendet
  bool              m_ItemIsProtected;    // wird bei Clear-Cycle nicht angerührt
  bool              m_ItemWithoutTTrail;  // Kein TTrail Eintrag bei Transactions.
  bool              m_folderFlag;         // Navigator Option
  DimensionSizeList m_DefaultDimensions;  // Defaultgroesse der einzelnen Dimensionen

  BUG_DECLARE_COUNT;
};

#endif // !defined(DATADICTIONARY_H__INCLUDED_)
/** \file */
