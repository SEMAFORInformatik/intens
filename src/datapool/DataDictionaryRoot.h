
#if !defined(__DP_DATA_DICTIONARY_ROOT__)
#define __DP_DATA_DICTIONARY_ROOT__

#include "datapool/DataDictionary.h"

/** Diese Klasse wird nur für das Root-Element des DataDictionary-Baums
    benötigt. Sie beinhaltet neben dem Pointer auf den DataPool die Funktionen
    zum Erstellen des DataDictionary-Baums.
 */
class DataDictionaryRoot : public DataDictionary
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/

public:
  /** Konstruktor
      @param datapool Pointer auf den Datapool.
      @param userattr Ein Objekt wird benötigt als Factory für weitere Objekte.
   */
  DataDictionaryRoot(DataPool *datapool, DataUserAttr *userattr );

  /** Destruktor
   */
  virtual ~DataDictionaryRoot();

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** Die Funktion liefert eine Referenz auf das DataPool-Objekt.
      @return Referenz auf den DataPool.
   */
  virtual DataPool &datapool() const;

  /** Die Funktion liefert einen Pointer auf den DataPool
      @return Pointer auf den DataPool.
   */
  DataPool *GetDataPool() const;

  /** Die Funktion erweitert das DataDictionary um ein Element am gewünschten Ort.
      @param struct_name Name der Struktur, in welche das neue Element eingebunden werden soll.
             Bleibt der Name leer, wird das Objekt in der Root-Struktur abgelegt.
      @param name Name des neuen Element.
      @param type Typ des neuen Elements.
      @param struct_def_name Name der gewünschten Strukturdefinition, welche als Variable
             eingebunden werden soll. Der Typ muss type_StructDefinition sein, sonst muss
             der Name leer bleiben.
      @param item_is_global true -> Es soll ein globales Element erstellt werden, welches
             über alle Cycles nur einmal vorkommt.
      @param item_is_internal true -> interne Variable
      @return Pointer auf das neu erstellte Objekt.
   */
  DataDictionary *AddToDictionary( const char *struct_name,
                                   const std::string &name,
                                   const DataType type,
                                   const char *struct_def_name = 0,
                                   bool item_is_global = false,
                                   bool item_is_internal = false );

  /** Die Funktion erweitert das DataDictionary um ein Element am gewünschten Ort.
      @param struct_dict Pointer auf das DataDictionary-Objekt der Struktur, in welche das
             neue Element eingebunden werden soll. Ist der Pointer null, wird das Objekt
             in der Root-Struktur abgelegt.
      @param name Name des neuen Element.
      @param type Typ des neuen Elements.
      @param struct_def_dict Pointer auf das DataDictionary-Objekt der gewünschten Strukturdefinition,
             welche als Variable eingebunden werden soll. Der Typ muss type_StructDefinition sein,
             sonst muss der Pointer null bleiben.
      @param item_is_global true -> Es soll ein globales Element erstellt werden, welches
             über alle Cycles nur einmal vorkommt.
      @param item_is_internal true -> interne Variable
      @return Pointer auf das neu erstellte Objekt.
   */
  DataDictionary *AddToDictionary( DataDictionary *struct_dict,
                                   const std::string &name,
                                   const DataType type,
                                   DataDictionary *struct_def_dict = 0,
                                   bool item_is_global = false,
                                   bool item_is_internal = false );

  /** Die Funktion sucht den Dictionary-Eintrag mit dem entsprechenden Namen.
      Der Name kann mehrstufig sein, getrennt durch jeweils einen Punkt.
      Wenn ein Name nicht existiert, wird als Resultat null geliefert.
      @param addr_name Name des gesuchten Objekts
      @return Pointer auf das gefundene DataDictionary-Objekt
   */
  DataDictionary *FindDictEntry( const char * addr_name );

  /** Die Funktion zeigt an, ob es sich um das Root-Item des Dictionary handelt.
      \return true => das Objekt ist das Root-Item.
   */
  virtual bool isRootNode() const { return true; }

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
protected:

/*=============================================================================*/
/* Member variables                                                            */
/*=============================================================================*/
protected:

  DataPool * m_DP;
};

#endif
/** \file */
