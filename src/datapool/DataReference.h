
#if !defined(__DP_DATA_REFERENCE__)
#define __DP_DATA_REFERENCE__

#include "utils/PointerBasis.h"
#include "datapool/DataGlobals.h"
#include "datapool/DataDebugger.h"
#include "datapool/DataDictionary.h"
#include "datapool/DataPool.h"
#include "datapool/DataContainer.h"
#include "datapool/DataAttributes.h"

#include <vector>
#include <string>

class DataPool;
class DataDictionary;
class DataContainer;
class DataElement;
class DataUserAttr;
class DataItemAttr;
class DataVector;
class DataMatrix;
class DataIndices;
class DataAlterFunctor;
class DataRefImplV2;
class DataIndexList;
class DataDiff;

/** Mit einem DataReference-Objekt wird auf die Daten im DataPool zugegriffen.
    @author Copyright (C) 2000  SEMAFOR Informatik & Energie AG, Basel, Switzerland
 */
class DataReference : public DataGlobals
                    , public PointerBasis<DataReference>
{
/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  typedef Pointer<DataReference> Ptr;

  enum { WildcardIndex /** Beschreibung fehlt */   = -1  };
  /** \enum
      \todo Die Beschreibung der Aufzaehlungswerte ist unvollstaendig
   */
   // Dieser Wert in der Indexliste definiert eine
                                  // Iteration. D.h. an dieser Stelle wird durch die
                                  // gesamte DataDimension gesucht. Fuer man koennt eine
                                  // solche Indezierung folgendermassen Darstellen:
                                  // A[1,5,*] wobei der '*' dann ein WildcardIndex ist.

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
protected:
  /** Der Constructor kann nicht direkt aufgerufen werden. Es müssen die
      Static-Funktionen newDataReference() verwendet werden.
      \param addr
   */
  DataReference( const char *addr = 0 );

  /** Der Copy-Constructor kann nicht direkt aufgerufen werden. Es müssen die
      Static-Funktionen newDataReference() verwendet werden.
      \param ref
      \param addr
   */
  DataReference( const DataReference &ref, const char *addr = 0 );

public:
  /** Destruktor
   */
  virtual ~DataReference();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  BUG_DECL_SHOW_COUNT("DataReference");

  /** Die Funktion liefert ein neues DataReference-Objekt. Die Constructors
      sind nicht public.
  */
  static DataReference *newDataReference( const char * addr = 0 );

  /** Die Funktion liefert ein neues DataReference-Objekt als Kopie des als
      Parameter übergebenen Objekts. Die Constructors sind nicht public.
  */
  static DataReference *newDataReference( const DataReference &ref, const char * addr = 0 );

  /** Die Funktion zeigt an, ob die im Objekt referenzierten Daten serialisierbar sind.
      \param flags Flags für für die Prüfung
      \return true => serialisierbar
   */
  bool isSerializable( const SerializableMask flags );

  /** Mit dieser Funktion wird für den Zugriff mit diesem Reference-Objekt
      die Cycle-Nummer gesetzt.
      \param cyclenum Cycle-Nummer
  */
  void setCycleNumber( int cyclenum );

  /** Die Cyclenummer wird initialisert. Das Reference-Objekt greift auf die
      global definierte Data-Cycle zu.
  */
  void initCycleNumber();

  /** Beschreibung der Funktion operator==
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator== fehlt
   */
  bool operator==( const DataReference &ref ) const;

  /** Beschreibung der Funktion operator!=
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator!= fehlt
   */
  bool operator!=( const DataReference &ref ) const;

  /** Beschreibung der Funktion operator=
      \param ref
      \return
      \todo Die Beschreibung der Funktion operator= fehlt
   */
  DataReference& operator=( const DataReference &ref );

  /** Die Funktion gibt an, ob die Referenz des Objekts gültig ist.
      @return true => Die Referenz ist gültig.
   */
  bool isValid() const;

  /** Beschreibung der Funktion RefValid
      \return
      \todo Die Beschreibung der Funktion RefValid fehlt
   */
  bool RefValid() const { return isValid(); }

  /** Die Funktion gibt an, ob die Referenz des Objekts ungültig ist.
      @return true => Die Referenz ist ungültig.
   */
  bool isntValid() const;

  /*****************************************************************************/
  /* Die folgenden Funktionen liefern Informationen über das DataDictionary.   */
  /* Zum einen bieten sie direkten Zugriff auf das Dictionary, zum anderen     */
  /* sind gewisse Wrapper hier, die den Zugriff vereinfachen.                  */
  /*****************************************************************************/

  /** Die Funktion teilt mit, ob der Pointer auf das DataDictionary-Objekt des
      referenzierten Items vorhanden ist.
  */
  bool hasDictionary() const;

  /** Die Funktion setzt das DataDictionary-Objekt neu.
      @param dict Pointer auf das neue DataDictionary-Objekt
  */
  void setDictionary( DataDictionary *dict );

  /** Die Funktion liefert eine Referenz auf das DataDictionary-Objekt des
      referenzierten Items. Die Referenz muss gültig sein.
   */
  DataDictionary &dictionary() const;

  /** Beschreibung der Funktion GetDict
      \return
      \todo Die Beschreibung der Funktion GetDict fehlt
   */
  DataDictionary *GetDict() const;

  /** Die Funktion liefert einen Pointer auf das UserAttr-Objekt des referenzierten
      Items, falls die Referenz gültig ist.
   */
  DataUserAttr *getUserAttr() const;

  /** Die Funktion liefert eine Referenz auf das UserAttr-Objekt des referenzierten
      Items. Die Referenz muss gültig sein.
   */
  DataUserAttr &userAttr() const;

  /** Die Funktion liefert die kumulierten AttributeBits über alle Datenstufen.
   */
  DataAttributes getUserDataAttributes() const;

  /** Die Funktion liefert den Datentyp des referenzierten Items.
   */
  DataDictionary::DataType getDataType() const;

  /** Diese Funktion prüft, ob die als Parameter übergebene DataReference auf ein
      Element desselben Datentyps zeigt.
      \param ref Pointer auf DataReference-Objekt
      \return true => gleicher Datentyp
   */
  bool isSameDataType( DataReference *ref ) const;

  /** Die Funktion liefert den DbAttr Namen dieses Nodes.
      @return DbAttr
   */
  std::string dbAttr() const;

  /** Die Funktion liefert den einfachen Namen dieses Nodes.
      @return Name
   */
  std::string nodeName() const;

  /** Die Funktion liefert den einfachen Namen dieses Nodes im Parameter name.
      @param name für das Resultat
   */
  void getNodeName( std::string &name ) const;

  /** Die Funktion liefert den statischen Namen dieses Nodes. Der Statische
      Name besteht aus dem Nodenamen und dem Strukturnamen
      (Nicht der Struktur-Variablen).
   */
  std::string staticName() const;

  /** Die Funktion liefert den statischen Namen dieses Nodes im Parameter name.
      Der Statische Name besteht aus dem Nodenamen und dem Strukturnamen
      (Nicht der Struktur-Variablen).
      @param name für das Resultat
   */
  void getStaticName( std::string &name ) const;

  /** Die Funktion liefert den kompletten qualifizierten Namen dieses Nodes.
      Wenn der Param WithIndizes true ist, so werden die einzelnen Indizes
      auch angegeben. Ein Name kann zum Beispiel wie folgt aussehen:
      G.H.M1 oder G[2,7].H[3].M1[2,2,2]
      @param with_indizes true Indexliste auch ausgeben
      @return Voller Name mit vorhandenen Struktur-Items.
   */
  std::string fullName( bool with_indizes=false ) const;

  /** Die Funktion liefert den kompletten qualifizierten Namen dieses Nodes
      im Parameter name. Wenn der Param WithIndizes true ist, so werden die
      einzelnen Indizes auch angegeben. Ein Name kann zum Beispiel wie folgt aussehen:
      G.H.M1 oder G[2,7].H[3].M1[2,2,2]
      @param name Output: Voller Name mit vorhandenen Struktur-Items.
      @param with_indizes true Indexliste auch ausgeben
   */
  void getFullName( std::string &name, bool with_indizes=false ) const;

  /** Die Funktion schreibt den qualifizierten Namen in den Output-Stream
      (siehe getFullName() ).
      @param ostr Outputstream für die Ausgabe.
      @param with_indizes true Indexliste auch ausgeben
   */
  void printFullName( std::ostream &ostr, bool with_indizes ) const;

  /** Diese Funktion schreibt die als Parameter übergebene Indexliste in den
      Output-Stream.
      \todo Die Beschreibung der Parameter fehlt
   */
  static void printIndexList( std::ostream &ostr, int NumIndizes, const int * Indizes );

  /** Diese Funktion übergibt als Resultat die Indexlist als String zurück.
      \todo Die Beschreibung der Parameter fehlt
   */
  static std::string getIndexList( int NumIndizes, const int * Indizes );

  /*****************************************************************************/
  /* DataItem-Attribute                                                        */
  /* Jedem DataItem ist ein Attribut-Block zugeordnet, der benutzerspezifische */
  /* Attribute beherbergt. Dieser Attribut-Block wird nur alloziert, wenn er   */
  /* gebraucht wird.                                                           */
  /* Eine Veraenderung des Attribut-Blockes zieht kein Nachfuehren des Update- */
  /* Timestamps nach sich.                                                     */
  /*****************************************************************************/

  /** Die Funktion liefert einen Pointer auf das Attribut-Objekt des DataItems.
      Diese Funktion kann erst aufgerufen werden, wenn der Datapool
      alloziert worden ist.
      @return Pointer auf das Attribut-Objekt
   */
  const DataItemAttr *getDataItemAttr();

  /** Die Funktion liefert eine Referenz auf das Attribut-Objekt des DataItems
      für Updates. Das Basis-DataItem wird im Transaction-Trail festgehalten.
      Das Objekt muss gültig sein. Falls noch kein Element existiert wird eines
      angelegt.
      @return Referenz auf das Attribut-Objekt
  */
  DataItemAttr &getDataItemAttrForUpdates();

  /** Die Funktion verändert die Attribute-Bits der gewünschten DataItem-Variablen.
      Wildcards sind auf allen Ebenen möglich.
      @param set_mask Bit-Maske zum Setzen von Attribute-Bits.
      @param reset_mask Bit-Maske zum Löschen von Attribute-Bits.
   */
  void setDataItemAttr( DATAAttributeMask set_mask, DATAAttributeMask reset_mask );

  /** Die Funktion verändert alle Attribut-Bits der gewünschten DataItem-Variablen
      und deren indizierten Elemente. Falls keine Indizes vorhanden sind, werden nur
      die Attribute der DataItem-Objekt verändert. (siehe setDataItemAttr().)
      Wildcards sind auf allen Ebenen möglich.
      @param set_mask Bit-Maske zum Setzen von Attribute-Bits.
      @param reset_mask Bit-Maske zum Löschen von Attribute-Bits.
   */
  void setAttributes( DATAAttributeMask set_mask, DATAAttributeMask reset_mask );

  /** getAttributes
      \todo Die Beschreibung der Funktion fehlt
   */
  DATAAttributeMask getAttributes( TransactionNumber t );

  /*****************************************************************************/
  /* Management der DimensionsIndizes                                          */
  /* Die Adressierung eines Datums erfolgt via sogenannte DimensionsIndizes.   */
  /* Im eigentlichen bestehn diese aus einem Integerarray. Dieser Array wird   */
  /* folgendermassen interpretiert:                                            */
  /* - Fuer jede geschachtelte Struktur gibt es einen Abschnitt.               */
  /* - Ein Abschnitt besteht aus:                                              */
  /*      + Relativer Index des Items auf einem Level.                         */
  /*      + Anzahl Indizes die folgen                                          */
  /*      + Indizes (in der oben bestimmt Anzahl)                              */
  /* - Die Membervariable m_NextIndex enthaelt den Index des naechsten frein   */
  /*   Platzes in m_Indizes.                                                   */
  /* - Die Membervariable m_LastItemIndex enthaelt den Index des letzten       */
  /*   Abschnitts (den Index auf den realtiven ItemIndex).                     */
  /*****************************************************************************/

  /** Die Funktion löscht die DimensionsIndizes fuer das aktuelle (letzte) Item
      in der Indexliste.
   */
  void ClearDimensionIndizes();

  /** Die Funktion setzt die Dimension-Indizes fuer das aktuelle Item auf dem
      letzten adressierten Level.
      Beispiel: A[2].B[3,5] kann geändert werden in A[2].B[6,7,8].
      @param num_inx Anzahl der Indizes in der Liste indizes.
      @param inx Indexliste.
   */
  void SetDimensionIndizes( int num_inx, const int *inx );

  /** Die Funktion setzt die Dimension-Indizes fuer irgend eine Dimension
      Beispiel: A[2].B[3,5] kann geändert werden in A[4,6,7].B[3,5].
      Der Parameter level definiert die Ebene, die verändert werden soll.
      Im obigen Beispiel müsste level = 0 sein.
      @param level Ebene in der Struktur
      @param num_inx Anzahl der Indizes in der Liste indizes.
      @param inx Indexliste.
   */
  void SetDimensionIndizes( int level, int num_inx, const int *inx );

  /** Die Funktion kopiert die Indexwerte des DataIndexList-Objekts
      auf die gewünschte Datenebene.
      @param indexlist
   */
  void SetDimensionIndizes( int level, const DataIndexList &indexlist );

  /** Die Funktion gibt eine Referenz auf dasDataIndexList-Objekt der
      gewünschten Datenebene.
      @param level
      @return Referenz auf DataIndexList-Objekt
   */
  const DataIndexList &getDimensionIndizes( int level ) const;

  /** Die Funktion gibt die Anzahl Levels der Adressierung an. Dies entspricht
      der Anzahl verschachtelter Strukturen!!! Um das ganze absolut klar zu machen:
      GetNumberOfLevels bezieht sich auf die gewünschte Indexierung in der DataReference
      und nicht auf die aktuell vorhandene Indexierung im Datapool.
      @return Anzahl der Ebenen in der Index-Struktur.
   */
  int GetNumberOfLevels() const;

  /** Die Funktion liefert die Anzahl der Dimensionsindizes auf der angegebenen Ebene.
      @param level Ebene der Indexierung (-1 bedeutet höchste vorhandene Ebene)
      @return Anzahl der vorhandnen Dimensionen
   */
  int GetNumberOfDimensionIndizes( int level=-1 ) const;

  /** Die Funktion liefert den Wert der mit Parameter index adressierten Dimension
      auf der gewünschten Struktur-Ebene.
      Beispiel: A[3,4,5].B[33,44,55].C[333,444,555]
      - GetDimensionIndexOfLevel(1,2) = 55
      - GetDimensionIndexOfLevel(0,0) = 3
      @param level Ebene der Indexierung (-1 bedeutet höchste vorhandene Ebene)
      @param index Index auf die gewünschte Dimension.
      @return Indexierung auf der gewünschten Dimension
   */
  int GetDimensionIndexOfLevel( int level, int index ) const;

  /** Die Funktion ändert den Index der adressierten Dimension auf der gewünschten
      Struktur-Ebene. Die Adressierung funktioniert identisch wie in der Funktion
      getDimensionIndexOfLevel().
      Wenn die Indexierung ausserhalb des allozierten Bereiches liegt, so wird
      dieser automatisch erweitert. Die neu entstandenen Positionen in der Liste
      werden mit Null gefüllt. Das folgende Beispiel zeigt dies:
      Ausgangslage : A[2].B[2,3].C[4]
      Aufruf       : SetDimensionIndexOfLevel(99,1,4)
      Resultat     : A[2].B[2,3,0,0,99].C[4]
      @param val Neuer Wert des addressierten Index.
      @param level Ebene der Indexierung (-1 bedeutet höchste vorhandene Ebene)
      @param index Index auf die gewünschte Dimension.
   */
  void SetDimensionIndexOfLevel( int val, int level, int index );

  /** Beschreibung der Funktion SetDimensionIndex
      \param index
      \return
      \todo Die Beschreibung der Funktion SetDimensionIndex fehlt
   */
  void SetDimensionIndex( int index );

  /** Diese Funktion entfernt die höchste Struktur-Ebene inkl. Indizes und reduziert
      somit die Referenz um eine Ebene.
      Beispiel: aus A[2,3,5].B[2,8,6,5] wird A[2,3,5].
      @return true -> Reduktion war erfolgreich.
   */
  bool ConvertToParentReference();

  /*****************************************************************************/
  /* Management der DimensionsIndizes (fortgesetzt)                            */
  /* Die folgenden Funktionen analysieren die Dimensionsindizes eines einzigen */
  /* Abschnittes. Da sie nur auf eine einzige Dimension beschraenkt sind, und  */
  /* nicht an eine DataReference gebunden sind, sind sie alle statisch defi-   */
  /* niert.                                                                    */
  /*****************************************************************************/

  /** Die Funktion sucht die erste Wildcard in der Liste und gibt dessen Index zurück.
   */
  static int FirstWildcardIndex( int NumIndizes, const int *Indizes );

  /** Die Funktion sucht die erste Wildcard in der Liste und gibt dessen Index zurück.
      Die Anzahl der Indizes muss an der zweiten Position der Liste stehen. Erst
      anschliessend beginnt die eigentliche Liste.
   */
  static int FirstWildcardIndex( const int *Indizes );

  /** Die Funktion sucht die letzte Wildcard in der Liste und gibt dessen Index zurück.
   */
  static int LastWildcardIndex( int NumIndizes, const int *Indizes );

  /** Die Funktion sucht die letzte Wildcard in der Liste und gibt dessen Index zurück.
      Die Anzahl der Indizes muss an der zweiten Position der Liste stehen. Erst
      anschliessend beginnt die eigentliche Liste.
   */
  static int LastWildcardIndex( const int *Indizes );

  /** Die Funktion liefert die Anzahl der Wildcards in der Liste.
   */
  static int NumberOfWildcardIndex( int NumIndizes, const int *Indizes );

  /** Die Funktion liefert die Anzahl der Wildcards in der Liste.
      Die Anzahl der Indizes muss an der zweiten Position der Liste stehen. Erst
      anschliessend beginnt die eigentliche Liste.
   */
  static int NumberOfWildcardIndex( const int *Indizes );

  /** Die Funktion zeigt an, ob in der Liste Wildcards vorhanden sind.
   */
  static bool WildcardInIndexList( int NumIndizes, const int *Indizes );

  /** Die Funktion zeigt an, ob in der Liste Wildcards vorhanden sind.
      Die Anzahl der Indizes muss an der zweiten Position der Liste stehen. Erst
      anschliessend beginnt die eigentliche Liste.
   */
  static bool WildcardInIndexList( const int *Indizes );

  /** Die Funktion zeigt an, ob ausschliesslich Wildcards in der Liste vorhanden sind.
   */
  static bool IndexListIsOnlyWildcards( int NumIndizes, const int *Indizes );

  /** Die Funktion zeigt an, ob ausschliesslich Wildcards in der Liste vorhanden sind.
      Die Anzahl der Indizes muss an der zweiten Position der Liste stehen. Erst
      anschliessend beginnt die eigentliche Liste.
   */
  static bool IndexListIsOnlyWildcards( const int *Indizes );

  /** Diese Funktion vergleicht zwei Index-Abschnitte. Sie gibt true zurueck,
      wenn sie das gleiche Element adressieren. Dies kann jedoch etwas Trickreich
      zum entdecken sein, wenn unterschiedlich viele Indizes vorhanden sind und
      Nullen substituiert werden können.
   */
  static bool IndexListsAreEqual( const int *Inx1, const int *Inx2 );

  /** Diese Funktion zeigt an, ob in der internen Indexliste Wildcards existieren,
      welche nicht auf dem letzten Level sind.
      Beispiel: A[*].B[1,*]
      @return true -> Es hat solche Wildcards.
   */
  bool hasWildcardsNotLastLevel();

  /** Diese Funktion zeigt an, ob in den internen Indexlisten Wildcards existieren.
        @return true -> Es hat Wildcards.
   */
  bool hasWildcards() const;

  /** Die Funktion liefert die Anzahl der Wildcards in der internen Liste
      der Indizes.
      @return Anzahl der vorhandenen Wildcards.
  */
  int numberOfWildcards() const;

  /*****************************************************************************/
  /* Get/Set Value                                                             */
  /* Die folgenden Funktionen implementieren den Zugriff auf ein Datum. Es gibt*/
  /* identische Get - und Set - Funktionen fuer jeden Datentyp und fuer 0,1    */
  /* 2, variablen und keinen Dimensionen. Dies macht 5 Get und 5 Set-Funktionen*/
  /* pro Datentyp. Ueber 4 Datentypen macht dies 40 Funktionen.                */
  /* Alle Funktionen ausser denen mit den Postfix PreInx setzen die Dimensions-*/
  /* Indizes des letzten Items neu. Diejenigen mit dem Suffix PreInx verwenden */
  /* die zuletzt verwendete Adressierung nochmals.                             */
  /* Saemtliche Funktionen werden via GetElementPtr implementiert.             */
  /*****************************************************************************/

  /**************************/
  /* DataVector Funktionen  */
  /**************************/

  /** Die Funktion schreibt die indizierten Werte in den Vektor vector und zeigt
      an, ob Werte gefunden wurden.
   */
  bool getDataVector( DataVector &vector, int num_inx, int *inx );

  /** Beschreibung der Funktion getDataVector
      \param vector
      \return
      \todo Die Beschreibung der Funktion getDataVector fehlt
   */
  bool getDataVector( DataVector &vector );

  /** Die Funktion schreibt die Werte val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool setDataVector( const DataVector &vector, int num_inx, int *inx );

  /** Beschreibung der Funktion setDataVector
      \param vector
      \return
      \todo Die Beschreibung der Funktion setDataVector fehlt
   */
  bool setDataVector( const DataVector &vector );

  /**************************/
  /* DataMatrix Funktionen  */
  /**************************/

  /** Die Funktion schreibt die indizierten Werte in die Matrix matrix und zeigt
      an, ob Werte gefunden wurden.
   */
  bool getDataMatrix( DataMatrix &matrix, int num_inx, int *inx );

  /** Beschreibung der Funktion getDataMatrix
      \param matrix
      \return
      \todo Die Beschreibung der Funktion getDataMatrix fehlt
   */
  bool getDataMatrix( DataMatrix &matrix );

  /** Die Funktion schreibt die Werte val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool setDataMatrix( const DataMatrix &matrix, int num_inx, int *inx );

  /** Beschreibung der Funktion setDataMatrix
      \param matrix
      \return
      \todo Die Beschreibung der Funktion setDataMatrix fehlt
   */
  bool setDataMatrix( const DataMatrix &matrix );

  /**********************/
  /* Integer Funktionen */
  /**********************/

  /** Die Funktion schreibt den indizierten Wert in die Variable val und zeigt
      an, ob ein Wert gefunden wurde.
   */
  bool GetValue( int &val, int num_inx, int *inx );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( int &val, int i1, int i2 );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i1
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( int &val, int i1=0 );

  /** Beschreibung der Funktion GetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion GetValue_PreInx fehlt
   */
  bool GetValue_PreInx( int &val );

  /** Die Funktion schreibt den Wert val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool SetValue( const int val, int num_inx, int *inx );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const int val, int i1, int i2 );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const int val, int i=0 );

  /** Beschreibung der Funktion SetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion SetValue_PreInx fehlt
   */
  bool SetValue_PreInx( const int val );

  /**********************/
  /* Real Funktionen    */
  /**********************/

  /** Die Funktion schreibt den indizierten Wert in die Variable val und zeigt
      an, ob ein Wert gefunden wurde.
   */
  bool GetValue( double &val, int num_inx, int *inx );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( double &val, int i1, int i2 );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( double &val,int i=0 );

  /** Beschreibung der Funktion GetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion GetValue_PreInx fehlt
   */
  bool GetValue_PreInx( double &val );

  /** Die Funktion schreibt den Wert val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool SetValue( const double val, int num_inx, int *inx );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const double val, int i1, int i2 );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const double val, int i=0 );

  /** Beschreibung der Funktion SetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion SetValue_PreInx fehlt
   */
  bool SetValue_PreInx( const double val );

  /**********************/
  /* String Funktionen  */
  /**********************/

  /** Die Funktion schreibt den indizierten Wert in die Variable val und zeigt
      an, ob ein Wert gefunden wurde.
   */
  bool GetValue( std::string &val, int num_inx, int *inx );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( std::string &val, int i1, int i2 );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( std::string &val, int i=0 );

  /** Beschreibung der Funktion GetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion GetValue_PreInx fehlt
   */
  bool GetValue_PreInx( std::string &val );

  /** Die Funktion schreibt den Wert val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool SetValue( const std::string &val, int num_inx, int *inx );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const std::string &val, int i1, int i2 );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const std::string &val, int i=0 );

  /** Beschreibung der Funktion SetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion SetValue_PreInx fehlt
   */
  bool SetValue_PreInx( const std::string &val );

  /**********************/
  /* Complex Funktionen */
  /**********************/

  /** Die Funktion schreibt den indizierten Wert in die Variable val und zeigt
      an, ob ein Wert gefunden wurde.
   */
  bool GetValue( dComplex &val, int num_inx, int *inx );

  /** Beschreibung der Funktion GetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion GetValue fehlt
   */
  bool GetValue( dComplex &val, int i=0 );

  /** Beschreibung der Funktion GetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion GetValue_PreInx fehlt
   */
  bool GetValue_PreInx( dComplex &val );

  /** Die Funktion schreibt den Wert val an den indizierten Ort und zeigt an, ob
      die Aktion erfolgreich war.
   */
  bool SetValue( const dComplex &val, int num_inx, int *inx );

  /** Beschreibung der Funktion SetValue
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion SetValue fehlt
   */
  bool SetValue( const dComplex &val, int i=0 );

  /** Beschreibung der Funktion SetValue_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion SetValue_PreInx fehlt
   */
  bool SetValue_PreInx( const dComplex &val );

  /*****************************************************************************/
  /* Validitaet eines Wertes                                                   */
  /* Die folgenden Funktionen manipulieren den Valid-Status eines Wertes. Die  */
  /* Indexierungen funktionieren Analog den Get/Set-Funktionen.                */
  /* Anmerkung: Es gibt keine Funktion, die ein uengueltiges Element wieder    */
  /* gueltig macht. Eine solche Operation macht in meinen Augen keinen Sinn!!  */
  /*****************************************************************************/

  /** Die Funktion teilt mit, ob der indizierte Wert gültig ist.
   */
  bool GetItemValid( int num_inx, int *inx );

  /** Beschreibung der Funktion GetItemValid
      \param i
      \return
      \todo Die Beschreibung der Funktion GetItemValid fehlt
   */
  bool GetItemValid( int i=0 );

  /** Beschreibung der Funktion GetItemValid_PreInx
      \return
      \todo Die Beschreibung der Funktion GetItemValid_PreInx fehlt
   */
  bool GetItemValid_PreInx();

  /** Die Funktion setzt eine DataElement auf Invalid. Sie gibt true zuerueck,
      wenn die Operation erfolgreich war. Achtung: Bei Struktur-Variablen ist
      der Aufruf wirkunslos. Es wird false zurückgegeben.
   */
  bool SetItemInvalid( int num_inx, int *inx );

  /** Beschreibung der Funktion SetItemInvalid
      \param i
      \return
      \todo Die Beschreibung der Funktion SetItemInvalid fehlt
   */
  bool SetItemInvalid( int i=0 );

  /** Beschreibung der Funktion SetItemInvalid_PreInx
      \return
      \todo Die Beschreibung der Funktion SetItemInvalid_PreInx fehlt
   */
  bool SetItemInvalid_PreInx();

  /*****************************************************************************/
  /* Locked-Status eines Wertes                                                */
  /* Die folgenden Funktionen manipulieren den Locked-Status eines Wertes.     */
  /* Jedem Wert im Datapool ist ein LockedBit zugeordnet. Wenn dieses Bit      */
  /* gesetzt ist, so werden alle veraenderungen an diesem Wert ignoriert ohne  */
  /* dass ein Fehler gemeldet wird. Der Aufrufer merkt also absolut nichts,    */
  /* davon, dass ein Item gelocket worden ist.                                 */
  /* Achtung: Das Lockbit har keinen Einfluss auf Struktur-Variablen. Es       */
  /*          wird also kein Locking auf die gesamte Substruktur gemacht.      */
  /*****************************************************************************/

  /** Die Funktion zeigt an, ob das Lockbit eines DataElements gesetzt ist. Wenn
      ein ungültiger Wert adressiert worden ist, gibt die Funktion false zurück.
      \todo Die Beschreibung der Parameter fehlt
   */
  bool GetItemLocked( int num_inx, int *inx );

  /** Beschreibung der Funktion GetItemLocked
      \param i
      \return
      \todo Die Beschreibung der Funktion GetItemLocked fehlt
   */
  bool GetItemLocked( int i=0 );

  /** Beschreibung der Funktion GetItemLocked_PreInx
      \return
      \todo Die Beschreibung der Funktion GetItemLocked_PreInx fehlt
   */
  bool GetItemLocked_PreInx();

  /** Die Funktion setzt das Locked-Bit. Das gesetzte Bit verhindert, dass ein
      DataElement verändert werden kann. Die Funktion gibt true zurück, wenn
      die Operation erfolgreich war.
      \todo Die Beschreibung der Parameter fehlt
   */
  bool SetItemLocked( bool val, int num_inx, int *inx );

  /** Beschreibung der Funktion SetItemLocked
      \param val
      \param i
      \return
      \todo Die Beschreibung der Funktion SetItemLocked fehlt
   */
  bool SetItemLocked( bool val, int i=0 );

  /** Beschreibung der Funktion SetItemLocked_PreInx
      \param val
      \return
      \todo Die Beschreibung der Funktion SetItemLocked_PreInx fehlt
   */
  bool SetItemLocked_PreInx( bool val );

  /*****************************************************************************/
  /* Timestamp-Handling                                                        */
  /* Jedes Element enthaelt einen Timestamp wobei das Wort Timestamp nicht     */
  /* korrekt ist, da es eigentlich ein Transaktions-Zaehler ist. Jedesmal wenn */
  /* eine Applikation die Funktion NewTrasaction aufruft, wird dieser Zaehler  */
  /* um ein erhoeht. Eine Transaktion besteht also zwischen 2 Aufrufen von     */
  /* dieser Funktion. Jeder Transaktion ist eine eindeutige Nummer zugeordnet. */
  /* Die folgenden Funktionen beantworten die Frage, ob eine Wert seit einer   */
  /* bestimmten oder innerhalb der laufenden Transaktion veraendert worden ist.*/
  /*****************************************************************************/

  /** Die Funktion zeigt an, ob ein Dataitem relativ zu einer Transaktionsnummer
      verändert wurde. Das Dataitem ist verändert, sobald ein untergeordneter Wert
      oder Attribut in einem Container verändert oder gelöscht wurde.
      @param status Art des zu vergleichenden Timestamps
      @param t Transaktionsnummer
      @return true -> Das DataItem wurde verändert
   */
  bool isDataItemUpdated( UpdateStatus status, TransactionNumber t );

  /** Die Funktion zeigt an, ob ein Dataitem relativ zu der aktuellen Transaktionsnummer
      verändert wurde. Das Dataitem ist verändert, sobald ein untergeordneter Wert
      oder Attribut in einem Container verändert oder gelöscht wurde.
      @param status Art des zu vergleichenden Timestamps
      @return true -> Das DataItem wurde verändert
   */
  bool isDataItemUpdated( UpdateStatus status );

  /** Die Funktion liefert die TransactionNumber des letzten Updates des Dataitems.
      @param status Art des zu vergleichenden Timestamps
      @return Timestamp des letzten Updates
   */
  TransactionNumber getDataItemUpdated( UpdateStatus status );

  /** Die Funktion liefert die TransactionNumber des letzten Value-Updates des Dataitems.
      @return Timestamp des letzten Updates
   */
  TransactionNumber getDataItemValueUpdated();

  /** Diese Funktion liefert den Timestamp des letzten TTrail Rollbacks
      auf diesem DataItem.
      @return Timestamp des letzten Rollbacks
   */
  TransactionNumber getDataItemLastRollback();

  /** Die Funktion setzt auf der letzten Ebene die Indexlist neu und zeigt an,
      ob das DataElement relativ zu einer gegebenen Transaktionsnummer verändert
      worden ist. Eine Veränderung wird wie folgt definiert:
      Der Node wurde innerhalb oder nach der gegebenen Transkationsnummer verändert.
      @param status Art des zu vergleichenden Timestamps
      @param t Timestamp
      @param num_inx Anzahl der Indizes in der Liste indizes.
      @param inx Indexliste.
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated( UpdateStatus status, TransactionNumber t, int num_inx, int *inx );

  /** (siehe isDataContainerUpdated() mit Indexliste)
      @param status Art des zu vergleichenden Timestamps
      @param t Timestamp
      @param i Index in der letzten Dimension.
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated( UpdateStatus status, TransactionNumber t, int i=0 );

  /** (siehe isDataContainerUpdated() mit Indexliste).
      Der Zugriff erfolgt mit den bereits vorhandenen Indizes.
      @param status Art des zu vergleichenden Timestamps
      @param t Timestamp
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated_PreInx( UpdateStatus status, TransactionNumber t );

  /** Die Funktion setzt auf der letzten Ebene die Indexlist neu und zeigt an,
      ob das DataElement relativ zu der aktuellen Transaktionsnummer verändert
      worden ist. Eine Veränderung wird wie folgt definiert:
      Der Node wurde innerhalb oder nach der gegebenen Transkationsnummer verändert.
      @param status Art des zu vergleichenden Timestamps
      @param num_inx Anzahl der Indizes in der Liste indizes.
      @param inx Indexliste.
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated( UpdateStatus status, int num_inx, int *inx );

  /** (siehe isDataContainerUpdated() mit Indexliste)
      @param status Art des zu vergleichenden Timestamps
      @param i Index in der letzten Dimension.
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated( UpdateStatus status, int i=0 );

  /** (siehe isDataContainerUpdated() mit Indexliste).
      Der Zugriff erfolgt mit den bereits vorhandenen Indizes.
      @param status Art des zu vergleichenden Timestamps
      @return true -> Container wurde verändert
   */
  bool isDataContainerUpdated_PreInx( UpdateStatus status );

  /** Die Funktion liefert die TransactionNumber des letzten Updates des DataContainers.
      @param status Art des zu vergleichenden Timestamps
      @return Timestamp des letzten Updates
   */
  TransactionNumber getDataContainerUpdated( UpdateStatus status );

  /** Die Funktion geht durch alle referenzierten Elemente und liefert den grössten
      gefundenen Timestamp. Falls auf dem letzten Level nicht indiziert ist, wird
      der Timestamp des DataItems verwendet und nicht das erste Element mit Index 0.
      @param status Art des zu vergleichenden Timestamps
      @return Timestamp des letzten Updates
  */
  TransactionNumber getDataTimestamp( UpdateStatus status );

  /** Die Funktion markiert alle gewünschten Elemente einer Variablen oder Strukturvariablen
      mit einem neuen Timestamp. Es sind Wildcards an beliebiger Stelle möglich.<br>
      Falls keine Indizes auf dem letzten Level vorhanden sind, werden alle Elemente markiert.
      Falls keine Wildcards vorhanden sind, wird genau ein Element markiert.
      Sind Wildcards vorhanden, so werden alle Elemente in diesem Bereich markiert.<br>
      Die Funktion gibt als Resultat true zurück, wenn mindestens ein Element
      markiert wurde.
      \param set true => der Timestamp wird auf den aktuellen Wert gesetzt,<br>
                 false => der Timestamp wir auf 0 gesetzt.
      \param status Art des Updates
      \retval true mindestens ein Timestamp wurde mutiert
   */
  bool setDataTimestamp( bool set, UpdateStatus status );

  /** Die Funktion verändert das Stylesheet Attribute der gewünschten DataItem-Variablen
      und deren indizierten Elemente. Falls keine Indizes vorhanden sind, werden nur
      die Attribute der DataItem-Objekt verändert. (siehe setDataItemAttr().)
      Wildcards sind auf allen Ebenen möglich.
      @param set_mask Bit-Maske zum Setzen von Attribute-Bits.
      @param reset_mask Bit-Maske zum Löschen von Attribute-Bits.
   */
  bool setStylesheet(const std::string& stylesheet);
  /** Diese Funktion liefert das der DataReference hinterlegte Stylesheet zurück
   */
  const std::string getStylesheet(TransactionNumber t, bool& changed);
  /** Diese Funktion liefert sofern verfügbar den Media Type der Daten zurück
   */
  const std::string getMediaType();

  /*****************************************************************************/
  /* Loesch-Funktionen                                                         */
  /*****************************************************************************/

  /** Die Funktion löscht Elemente in der Datenmatrix. Sie kann ausschliesslich
      auf dem letzten Level arbeiten. Sie kann keine Strukturvariablen löschen.
      Es ist auch moeglich Wildcards beim Loeschen zu verwenden.
      Beispiel: A[*], A[1,*] oder A[*,2]
      Achtung: A[*] loescht NICHT alles. z.B. In einem 3-Dimensionalen Array
      ist A[*] identisch mit A[0,0,*]. Wenn wirklich alles gelöscht werden soll,
      so muss A[*,*,*] angegeben werden. Die Funktion liefert true, wenn etwas
      gelöscht worden ist.
   */
  bool EraseElements( int num_inx, int *inx );

  /** Beschreibung der Funktion EraseElements
      \param i
      \return
      \todo Die Beschreibung der Funktion EraseElements fehlt
   */
  bool EraseElements( int i=0 );

  /** Beschreibung der Funktion EraseElements
      \param i1
      \param i2
      \return
      \todo Die Beschreibung der Funktion EraseElements fehlt
   */
  bool EraseElements( int i1, int i2 );

  /** Die Funktion arbeitet wie EraseElements(), verwendet jedoch die bereits
      intern vorhandene Indexliste.
   */
  bool EraseElements_PreInx();

  /** Die Funktion löscht alle Elemente des letzten adressierten Levels.
      Danach wird ein leeres, ungültiges Element erstellt.
      Diese Funktion ist identisch mit einem Aufruf von EreaseElements() mit
      einer unendlichen Anzahl Wildcards in der Indexliste. Die Funktion liefert
      true, wenn Elemente gelöscht wurden.
      Achtung: Die gegenwärtige Implementation liefert true, wenn mindestens ein
      gueltiges Element vorhanden ist, oder wenn eine Dimensionsmatrix vorhanden ist.
      Im zweiten Fall wird nicht geprüft, ob in der Matrix gültige Werte vorhanden
      waren.
      @param incl_attr true => Die Item-Attribute werden auch gelöscht
      @return true => Es wurden Elemente ungültig gemacht.
   */
  bool eraseAllElements( bool incl_attr=false );

  /** Die Funktion macht alle Elemente auf dem letzten Level ungültig, unabhängig
      davon, ob Indizes vorhanden sind (siehe auch clearDataItem() ).
      Es dürfen auf verschiedenen Levels Wildcards vorhanden sein.
      @return true => Es wurden Elemente ungültig gemacht.
   */
  bool clearAllElements();

  /** Die Funktion macht alle durch die Indexliste referenzierten Elemente ungültig.
      Wenn keine Indizes auf dem letzten Level vorhanden sind, werden dort alle
      Elemente ungültig gemacht (wie clearAllElements() ).
      Es dürfen auf verschiedenen Levels Wildcards vorhanden sein.
      @param incl_attr true => Die Item-Attribute werden auch gelöscht
      @return true => Es wurden Elemente ungültig gemacht.
   */
  bool clearDataItem( bool incl_attr=false );

  /** Die Funktion komprimiert Arrays oder Matrizen, indem ungültige Elemente
      oder Arrays entfernt werden.
      \todo Die Beschreibung der Parameter fehlt
   */
  bool packDataItem( bool packRow );

  /*****************************************************************************/
  /* Analyse-Funktionen eines Dataitems.                                       */
  /* Die folgenden Funktion analysieren die Dimensionmatrix eines Items. Sie   */
  /* geben Auskunft ueber die Anzahl Dimensionen, deren Groesse und uber die   */
  /* Anzahl DataElemente innerhalb des DataItems                               */
  /*****************************************************************************/

  /** Die Funktion liefert die Anzahl der physisch vorhandenen Dimensionen auf
      der letzten Ebene der Adresse.
      @return Anzahl Dimensionen
  */
  int GetNumberOfDataDimensions();

  /** Die Funktion liefert die Anzahl der Elemente einer durch den
      Parameter bestimmten Dimension.
      @param dim_num Nummer der gewünschten Dimension
      @return Anzahl der Elemente
   */
  int GetDataDimensionSize( int dim_num=0 );

  /** Die Funktion liefert einen Array mit den gültigen Grössen aller vorhandenen
      Dimensionen. Es werden dabei die vorhanden Indexlisten verwendet.
      Nur dort, wo Wildcards gesetzt sind, wird die Dimensionsgrösse grösser als 1.
      @param dimsize Liste mit allen Dimensionsgrössen
      @return Grösse des Arrays dimsize
  */
  int getAllDataDimensionSize_PreInx( DataContainer::SizeList &dimsize );

  /** Die Funktion liefert einen Array mit den Grössen aller vorhandenen Dimensionen.
      Sie funktioniert ähnlich wie GetMaxDataDimensionSize().
      @param dimsize Liste mit allen Dimensionsgrössen
      @param incl_attr ungültige DataItem's mit gültigen Attributen werden berücksichtigt
  */
  int GetAllDataDimensionSize( DataContainer::SizeList &dimsize, bool incl_attr=false );

  /** Beschreibung der Funktion sizeOfDataItem
      \param dimsize
      \return
      \todo Die Beschreibung der Funktion sizeOfDataItem fehlt
   */
  int sizeOfDataItem( DataContainer::SizeList &dimsize ){ return GetAllDataDimensionSize( dimsize ); }

  /** Beschreibung der Funktion GetDataDimensionSizeOfLevel_PreInx
      \param level
      \param pos
      \return
      \todo Die Beschreibung der Funktion GetDataDimensionSizeOfLevel_PreInx fehlt
   */
  int GetDataDimensionSizeOfLevel_PreInx( int level, int pos );

/*******************************************************************************/
/* Assignment-Funktionen.                                                      */
/* Die Assigment-Funktionen gibt es in zwei auspreagungen, die aehnlich aber   */
/* nicht identisch sind.                                                       */
/* - AssignDataItem                                                            */
/*   Diese Funktion weist die gesamte Dimensionsmatrix eine Dataelementes einem*/
/*   anderen zu. Dies beinhaltet auch saemtliche verschachtelte Strukturen etc.*/
/*   Die Adressierung des letzten Elementes dieser Refrenz bleibt desshalb ohne*/
/*   Wirkung auf das Assigment. Es ist also egal, ob die Datareference mit "A",*/
/*   "A[7]" oder "A[3,2,4,88]" erstellt wurde. Es wird immer nur "A" referen-  */
/*   ziert. Selbstverstaendlich nicht egal ist die Adressierung der andreren   */
/*   Indizes (also B[7].A ist nicht gleich B[6].A).                            */
/* - AssignDataElement                                                         */
/*   Diese Funktion ist gedacht, einzelne Werte zu uebertragen. Sie führt eine */
/*   Adressierung bis zum einzelnen Element durch. Dies wird dann zugewiesen.  */
/*   Ein Spezielafall existiert, wenn das Element eine Struktur-Variable ist.  */
/*   In diesem Fall funktionert AssignDataElement analog AssignDataItem.       */
/*   Achtung: Damit dies Funktioert muessen BEIDE DataRefernzen korrekt        */
/*            indizeirt worden sein. Man muss annehmen, dass auf BEIDE mit den */
/*            PreInx-Funktionen zugegriffen wird.                              */
/*                                                                             */
/* Generell gilt, dass die beiden Adressierten Referenzen auf den gleichen     */
/* Datentyp deuten muessen (gilt insbesondere für Strukturen).                 */
/*******************************************************************************/

  /** Die Funktion weist das erhaltene DataItem dem aktuell adressierten
      DataItem zu. Die als Parameter übergebene DataReference muss denselben
      Typ haben.
      @param source Referenz auf die zuzuweisenden Daten
   */
  void AssignDataItem( DataReference &source );

  /** Die Funktion weist alle DataItems eines DataStructElements den korres-
      pondierenden DataItems eines anderen DataStructElements zu.
      @param source Referenz auf die zuzuweisenden Daten
   */
  void AssignCorrespondingDataItem( DataReference &source );

  /** Die Funktion weist (Copy) das erhaltene DataElement dem aktuell adressierten
      DataElement zu.
      @param source Referenz auf die zuzuweisenden Daten
   */
  void AssignDataElement( DataReference &source );


  bool writeXML( std::ostream &os
               , std::vector<std::string> & attrs
               , int level
               , bool debug );

  /** Die Funktion schreibt das Element im JSON-Format in den Output-Stream.
      @param ostr Output-Stream
      @param level
      @param indent
      @param flags Flags für für die Prüfung von 'serializable'
      @return true -> erfolgreich
   */
  bool writeJSON( std::ostream &os,
                  int level,
                  int indent,
                  const SerializableMask flags,
                  bool with_endl=true);

  /** Diese Funktion wird nicht mehr benötigt
      \deprecated Sie kann bei Gelegenheit  eliminiert werden. (26.1.2016/bh)
  */
  /* bool writeData( std::vector<DataDiff*>& resultData */
  /*               , const DataUserAttr::HideFlags hideFlags ) { assert( false ); return false; } */

  /** Die Funktion schreibt das referenzierte DataItem im XML-Format in den Outputstream.
      (internal debug-function)
      @param ostr Outputstream
      @param i Anzahl Zeichen einrücken
   */
  void writeDP( std::ostream &ostr, int i=0 );

  /** Die Funktion druckt das DataItem komplett (d.h. die geamte
      Matrix). Wenn withDimensions auch noch gesetzt ist, so werden
      auch die Coarsetables (DataDimesnions) ausgedruckt.
      Dies ist eine Debugfunktion!!!
   */
  void PrintItem( bool withDimensions, std::ostream &sout );

  /** Die Funktion sorgt dafür, dass für alle referenzierten Daten die Funktion
      alterData() des Funktors ausgeführt wird.
      @param func Functor zum Bearbeiten der gewünschten Daten
      @param cycle optionale Cyclenummer
      @return true => es wurden Elemente verändert (updates).
   */
  bool alterData( DataAlterFunctor &func, int cycle=-1 );

  /** Die Funktion sorgt dafür, dass für alle referenzierten Daten die Funktion
      alterData() des Funktors ausgeführt wird. Als Parameter werden die Indizes
      auf der letzten Ebene als Parameter geliefert. Diese werden nur für diesen
      Zugriff verwendet.
      @param func Functor zum Bearbeiten der gewünschten Daten
      @param num_inx Anzahl der Indizes
      @param inx Liste der Indizes
      @return true => es wurden Elemente verändert (updates).
   */
  bool alterData( DataAlterFunctor &func, int num_inx, int *inx );

  /** Die Funktion vergleicht zwei durch die beiden DataReference-Objekte bestimmten
      DataElement-Objekte und legt das Ergebnis im eigenen DataItem-Objekt ab.
      Die zu vergleichenden, wie auch das eigene DataItem, müssen denselben DataType
      sowie, bei einer Struktur, dieselbe Strukturdefinition haben. Die Indexliste
      darf keine Wildcards haben.
      @param ref_links Referenz auf das linke zu vergleichende Objekt
      @param ref_rechts Referenz auf das rechte zu vergleichende Objekt
   */
  void compareElements( DataReference &ref_links, DataReference &ref_rechts );

  /** Die Funktion vergleicht ein Liste von durch das DataReference-Objekt bestimmten
      DataElement-Objekte und legt das Ergebnis im eigenen DataItem-Objekt ab.
      Die Indexliste muss eine Wildcard auf der letzten Datenebene haben.
      Die zu vergleichenden, wie auch das eigene DataItem, müssen denselben DataType
      sowie, bei einer Struktur, dieselbe Strukturdefinition haben.
      Mit der Option cycles=true darf keine Wildcard vorhanden sein. In diesem Fall wird
      das entsprechende Element über alle vorhandenen Cycles verglichen.
      @param ref Referenz auf die zu vergleichenden Objekte
      @param cycles true -> Vergleich über alle Cycles ohne Wildcard
   */
  void compareElements( DataReference &ref, bool cycles=false );

  /** Die Funktion liefert den Pointer auf das referenzierte DataItem.
      @return Pointer auf das DataItem
   */
  const DataItem *getItem();

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:
  /** Die Funktion kopiert die Indizes aus der übergebenen DataReference.
   */
  void copyIndices( const DataReference &ref );

  /** Die Funktion liefert die Referenz auf das Indices-Objekt.
   */
  DataIndices &indices() const;

  /** Die Funktion liefert eine Referenz auf das DataPool-Objekt.
      @return Referenz auf den DataPool.
   */
  DataPool &datapool() const;

  /** Die Funktion liefert eine Referenz auf das referenzierte Basis-DataItem.
      @param item_index Index in die Basis-Struktur des Data-Dictionaries.
      @param for_updates true => Das DataItem wird, falls eine Transaktion
             offen ist, im TTrail gesichert.
      @param cycle Cyclenummer
      @return Referenz des DataItems.
   */
  DataItem &getDataItem( int item_index, bool for_updates, int cycle=-1 );

  /** Die Funktion liefert den Pointer auf das referenzierte DataItem.
      Diese Funktion ist nur für Funktionsaufrufe, welche nicht auf
      const umgestellt sind, aber dem Objekt nicht antun!
      @return Pointer auf das DataItem
   */
  DataItem *getItemNoUpdate();

  /** Die Funktion liefert den Pointer auf das referenzierte DataElement.
      @return Pointer auf das DataElement-Objekt
   */
  const DataElement *getElement();

  /** Die Funktion liefert den Pointer auf das referenzierte DataElement und im
      Parameter den Pointer des zugehörigen DataItems.
      @param item Referenz auf den Pointer auf das DataItem.
      @return Pointer auf das DataElement-Objekt
   */
  const DataElement *getElement( const DataItem *&item );

  /** Die Funktion liefert den Pointer auf das referenzierte DataElement. Die Indizes
      auf der letzten Ebene werden für diesen Zugriff als Parameter geliefert. Nach
      dem Zugriff wird wieder der alte Zustand der Indizes hergestellt.
      @param num_inx Anzahl der Indizes in der Liste indizes.
      @param inx Indexliste.
      @return Pointer auf das DataElement-Objekt
   */
  const DataElement *getElement( int num_inx, int *inx );

  /** Die Funktion liefert das referenzierte DataItem-Objekt und löscht vorgängig
      alle Daten als Vorbereitung für das Compare-Feature.
      @return Initialisiertes Resultat-Dataitem
   */
  DataItem *getCompareResultItem();

/*******************************************************************************/
/* Funktionen für die Bearbeitung einer Addresse mit Indizes                   */
/*******************************************************************************/

  /** Die Funktion analysiert die angegebene Adresse und setzt die Indizes neu.
      Wenn die Adresse gueltig ist, wird m_RefValid gesetzt sonst reset.
      Eine Adresse hat folgende Form:<BR>
\Text{
\<Addr\>
  : \<Name\> \<Indexlist\>
  | \<Name\> \<Indexlist\>.\<Addr\>
  ;
\<Indexlist\>
  : \<empty\>
  | [\<Indizes\>]
  ;
\<Indizes\>
  : \<integer\>
  | \<Indizes\> , \<integer\>
  ;}
      Achtung:<BR>
      Indizes sind einfache positive Zahlen, ohne Vorzeichen, Exponenten oder Nachkommazahlen.
      @param addr Adresse einer Datenreferenz
      @return true => ok, keine Fehler.
  */
  bool ParseSymbolicAdress( const char * addr );

  /** Die Funktion wird von ParseSymbolicAdress() aufgerufen und übernimmt
      die eigentliche Arbeit. Sie wird auch rekursiv aufgerufen.
      \param Addr
      \param pDict
      \return true => ok
      \Draft
   */
  bool ParseAddr(const char * & Addr,DataDictionary * pDict=0);

  /** Die Funktion wird von ParseAddr() aufgerufen und übernimmt das Lesen einer Indexliste.
      \param Addr
      \return true => ok
   */
  bool ParseIndexList(const char * & Addr);

  /** Die Funktion fügt ein neues Item in den laufenden Index ein. Die Anzahl der
      DimensionsIndizes wird auf 0 gesetzt
      \param dict Pointer auf das Dictionary-Objekt des hinzufügenden Items.
   */
  void addItemToIndizes( DataDictionary *dict );

  /** Die Funktion fügt einen weiteren Dimensionsindex zum aktuellen Item hinzu.
      \param Index Wert des Index
   */
  void addDimensionToIndizes( int Index );

  /** Diese Funktion macht einen Check ob das Objekt logisch korrekt aufgesetzt ist.
      Sie führt eine Serie von Assert-Anweisungen aus.
   */
  void AssertReference() const;

/*******************************************************************************/
/* StructIterator                                                              */
/* Die Unterklasse StructIterator dient dazu bei einer Strukturvariablen die   */
/* einzelnen  Member der Struktur aufzuzählen. Dies erlaubt es, generisch zu   */
/* programmieren. Einer Funktion muss nur noch eine initialisierte DataReferenz*/
/* auf die Strukturvariable mitgegeben werden, und diese kann dann selbstaendig*/
/* auf die einzelnen Member zugrifen (um sie z.B. in eine Datenbank zu fuellen.*/
/*                                                                             */
/* Die Idee der Implementierung ist angelehnt an die Implementation von STL-   */
/* Iteratoren.                                                                 */
/* Folgende Funktionen sind vorhanden:                                         */
/* DataReference::begin()           -> gibt den Startiterator wieder           */
/* DataReference::end()             -> gibt den Enditerator wieder             */
/* StructIterator::operator++       -> switch den Iterator eins weiter         */
/* StructIterator::operator=        -> Zuweisung                               */
/* StructIterator::operator==       -> Vergleich                               */
/* StructIterator::GetDict()        -> Dictpointer von Iterator                */
/* StructIterator::GetAttr()        -> UserAttr-Pointer von Iterator           */
/* StructIterator::GetDataType()    -> Datentyp von Iterator                   */
/* StructIterator::NewDataReference -> Konstruiert eine neue Datareference     */
/*                                                                             */
/* Anwendung:                                                                  */
/* Das folgende Codeschnipsel listet alle Member einer Struktur-Variablen auf: */
/* DataReference::StructIterator si;                                           */
/* for(si = dref.begin(); si != dref.end(); ++si)                              */
/*    cout << si.GetDict()->GetName();                                         */
/*                                                                             */
/*******************************************************************************/
public:
  class StructIterator {
  public:
    // Constructor / Destructor
    StructIterator( const DataReference * pRef=0, const DataDictionary * pDict=0 );
    StructIterator( const StructIterator &s );
    virtual ~StructIterator();

  public:  // Operators
    StructIterator &operator=(const StructIterator &s);
    bool operator==(const StructIterator & Other) const;
    bool operator!=(const StructIterator & Other) const;
    StructIterator &operator++(int);
    StructIterator& operator++();

  public:
    /** Die Funktion erstellt ein neues DataReference-Objekt auf das aktuelle
        Dataitem des Iterators.
    */
    DataReference *NewDataReference() const;

    /** Die Funktion liefert den Dictionary-Eintrag fuer das DataItem der aktuellen
        Position des Iterators.
    */
    const DataDictionary *GetDict() const;

    /** Die Funktion liefert den Poiner auf das UserAttr des DataItems der aktuellen
        Position des Iterators.
    */
    DataUserAttr *GetAttr()  const;

    /** Die Funktion liefert den Datentyp des DataItems der aktuellen Position
        des Iterators.
    */
    DataDictionary::DataType GetDataType() const;

  protected: // member variables
    const DataReference   * m_pRef;       // Pointer auf die "umgebende" Referenz
    const DataDictionary  * m_pDict;      // Dict-Pointer. Sozusagen die aktuelle Position
  };


  /** Diese Funktion liefert den initialen Iterator. Sie nimmt an, dass das
      aktuelle Element der Datareference eine Struktur-Variable ist.
      Der Begin-Iterator ist ein gueltiger Iterator. D.h. man kann aus
      ihm einen Namen etc herausholen.
  */
  StructIterator begin() const;

  /** Diese Funktion liefert den Enditerator. Sie nimmt an, dass das
      aktuelle Element der Datareference eine Struktur-Variable ist.
      Der End-Iterator ist kein gueltiger Iterator (man kann sich vorstellen,
      dass er eins hinter dem letzten gueltigen Wert angibt.
   */
  StructIterator end() const;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int              m_cyclenum;              // Cyclenummer 0-relativ oder -1
  bool             m_ref_valid;             // Referenz ist gueltig
  DataDictionary  *m_dict;                  // Pointer auf das Dictionary
  DataIndices     *m_indices;               // Indexliste

  BUG_DECLARE_COUNT;
};

#endif
/** \file
    Die Datei enthält die Definition der Klasse <b>DataReference</b>.
*/
