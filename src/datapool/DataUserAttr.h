
#if !defined(__DP_USER_ATTR__)
#define __DP_USER_ATTR__

#include "datapool/DataPool.h"

class DataDictionary;

/** Jedes DataDictionary-Objekt erhält ein eigenes DataUserAttr-Objekt, welches
    vom Benutzer des DataPool kontrolliert wird. Der DataPool interessiert sich
    nicht für diese Daten. Jedem Item können auf diese Weise eigene Eigenschaften
    zugeordnet werden.<br>
    Der Benutzer leitet seine eigene Attribut-Klasse von DataUserAttr ab und
    implementiert nach seinen Bedürfnissen. Wenn er beim Erstellen des DataPool
    keine eigene Klasse liefert, wird die DefaultDataUserAttr-Klasse verwendet.
 */
class DataUserAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

// Wir verwenden zwei Konstruktoren. Der eine ohne Parameter sollte WIRKLICH
// NICHTS machen. Er dient nur dem Zweck, temporaere Objekte zu erstellen.
// Diese werden gebraucht um die Funktion NewClass aufzurufen (die nur aus
// einem new besteht).
// Der zweite Constructor hat die Pflicht, das Objekt korrekt zu initialisieren
// (so wie vom Benutzer gewuenscht).
public:

  /** Beschreibung der Funktion DataUserAttr
      \todo Die Beschreibung der Funktion DataUserAttr fehlt
   */
  DataUserAttr();

  /** Beschreibung der Funktion DataUserAttr
      \param pDict
      \todo Die Beschreibung der Funktion DataUserAttr fehlt
   */
  DataUserAttr( DataDictionary *pDict );

  /** Beschreibung der Funktion ~DataUserAttr
      \return
      \todo Die Beschreibung der Funktion ~DataUserAttr fehlt
   */
  virtual ~DataUserAttr();

protected:

  /** Der Copy-Konstruktor ist nicht implementiert
      \param ref
   */
  DataUserAttr( const DataUserAttr &ref ); // unused

  /** Der Zuweisungsoperator ist nicht implementiert
      \param ref
      \return
   */
  DataUserAttr& operator=( const DataUserAttr &ref ); // unused

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Diese Funktion erstellt ein neues UserAttribute-Objekt.
      @param dict Optionaler Pointer auf das DataDictionary-Objekt
      @return Pointer auf das neue DataUserAttr-Objekt
   */
  virtual DataUserAttr *NewClass( DataDictionary *dict = 0 ) const = 0;

  /** Die Funktion erstellt eine Kopie des Attribut-Objekts.
      @return Pointer auf das neue DataUserAttr-Objekt
   */
  virtual DataUserAttr *copyClass() const = 0;

  /** Die Funktion liefert die Bit-Mask mit Applikationsattributen.
      @return Bit-Mask
   */
  virtual DATAAttributeMask getAttributes() const { return 0; }

  /** Für eine optionale Rundung für dem Schreiben eines Werts in ein DataElement
      kann die Anzahl der Nachkommastellen definiert werden.
   */
  void setRundung( int rnd );

  /** Die Funktion liefert die Anzahl der Nachkommastellen für eine optionale
      Rundung vor dem Schreiben eines Werts oder für einen Vergleich.
   */
  int getRundung() const;

  /** Beschreibung der Funktion setSortKey
      \return
      \todo Die Beschreibung der Funktion setSortKey fehlt
   */
  void setSortKey();

  /** Beschreibung der Funktion isSortKey
      \return
      \todo Die Beschreibung der Funktion isSortKey fehlt
   */
  bool isSortKey();

  /** Die Funktion zeigt anhand der Information im Parameter mask an, ab die Daten
      im DataItem serialisierbar sind.
      \param flags Flags für die Prüfung
      \return true => Daten sind serialisierbar
  */
  virtual bool isSerializable( const SerializableMask flags ) const = 0;

  /** Beschreibung der Funktion IsScalar
      \return
      \todo Die Beschreibung der Funktion IsScalar fehlt
   */
  virtual bool IsScalar() const { return false; }

  /** write
      \param ostr
      \attrs
      \todo Die Beschreibung der Funktion write fehlt
   */
  virtual void write( std::ostream &ostr, const std::vector<std::string> &attrs ) const {}

  /** Diese Funktion wird beim erstellen eines DataItems aufgerufen. Sie hat die
      Möglichkeit, an den Attributen des DataItems Korrekturen vorzunehmen.
      @param item Referenz auf das DataItem-Objekt
   */
  virtual void fixupAttributes( DataItem &item ) = 0;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
private:
  int     m_rundung;
  bool    m_sortkey;
};


/** Die DefaultDataUserAttr-Klasse wird verwendet, wenn der Benutzer des Datapools
    keine eigene Implementation der DataUserAttr-Klasse dem DataDictionary
    als Objekt-Faktory übergeben hat.
 */
class DefaultDataUserAttr : public DataUserAttr
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  /** Konstruktor
   */
  DefaultDataUserAttr();

  /** Konstruktor
      \param pDict
      \todo Die Beschreibung der Funktion DefaultDataUserAttr fehlt
   */
  DefaultDataUserAttr( DataDictionary * pDict );

  /** Copy-Konstruktor
      \param attr
      \todo Die Beschreibung der Funktion DefaultDataUserAttr fehlt
   */
  DefaultDataUserAttr( const DefaultDataUserAttr &attr );

  /** Destruktor
   */
  virtual ~DefaultDataUserAttr();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion erstellt ein neues UserAttribute-Objekt.
      \param pDict
      @return Pointer auf das neue DataUserAttr-Objekt
  */
 virtual DataUserAttr *NewClass( DataDictionary *pDict ) const;

  /** Die Funktion erstellt eine Kopie des Attribut-Objekts.
      @return Pointer auf das neue DataUserAttr-Objekt
   */
  virtual DataUserAttr * copyClass() const;

  /** Die Funktion zeigt anhand der Information im Parameter mask an, ab die Daten
      im DataItem serialisierbar sind.
      \param flags Flags für die Prüfung
      \return true => Daten sind serialisierbar
  */
  virtual bool isSerializable( const SerializableMask flags ) const { return true; }

  /** Diese Funktion wird beim erstellen eines DataItems aufgerufen. Sie hat die
      Möglichkeit, an den Attributen des DataItems Korrekturen vorzunehmen.
      @param item Referenz auf das DataItem-Objekt
   */
  virtual void fixupAttributes( DataItem &item );
};

#endif
/** \file */
