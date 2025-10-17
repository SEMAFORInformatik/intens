
#if !defined(IDENTIFIER_MANAGER_INCLUDED_H)
#define IDENTIFIER_MANAGER_INCLUDED_H

#include <map>
#include <string>

/** Der IdManager sammelt alle vom Parser erhaltenen Identifiers und garantiert,
    dass es nicht zu einer Mehrfachvergabe eines Identifiers kommt. Gleichzeit
    unterstützt er den Scanner beim Qualifizieren der einzelnen Tokens.
*/
class IdManager
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
private:
  IdManager() {}
  virtual ~IdManager() {}

/*=============================================================================*/
/* public definitions                                                          */
/*=============================================================================*/
public:
  enum IdentifierType
  { id_None = 0
    , id_DataVariable
    , id_DataStructure
    , id_DataSet
    , id_ColorSet
    , id_Tag
    , id_Form
    , id_Fieldgroup
    , id_Index
    , id_Folder
    , id_Foldergroup
    , id_Uniplot
    , id_Psplot
    , id_Listplot
    , id_3dPlot
    , id_Image
    , id_LinePlot
    , id_Thermo
    , id_2dPlot
    , id_QWTPlot
    , id_Textwindow
    , id_Navigator
    , id_MessageQueue
    , id_Stream
    , id_Socket
    , id_Timer
    , id_ProcessGroup
    , id_Filestream
    , id_Reportstream
    , id_Task
    , id_Function
    , id_DbFilter
    , id_DbTransaction
    , id_Menu
    , id_Process
    , id_ProgressBar
    , id_Usergroups
    , id_Plugin
    , id_List
    , id_Table
    , id_TimeTable
    , id_Python
  };

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  /** der IdManager ist ein Singleton-Objekt. Diese Funktion liefert die
      Referenz auf das Objekt.
  */
  static IdManager &Instance();

  /** Diese Funktion löscht den IdManager. Dies kann geschehen, sobald der Parser
      ihn nicht mehr benötigt.
  */
  static void remove();

  /** Mit dieser Funktion wird ein Identifier beim Manager gemeldet.
      @param id Name des Identifiers
      @param type Typ des Identifiers
      @return true -> Der neue Identifier wird akzeptiert.
  */
  bool registerId( const std::string &id, IdentifierType type );

  /** Mit dieser Funktion wird festgestellt, ob der Identifier bereits gemeldet
      wurde und wenn ja, von welchem Typ er ist.
      @param id Name des Identifiers
      @return Typ des Identifiers oder 0, falls nicht vorhanden
  */
  IdentifierType getType( const std::string &id );

/*=============================================================================*/
/* private member functions                                                    */
/*=============================================================================*/
private:

/*=============================================================================*/
/* private Data                                                                */
/*=============================================================================*/
private:

  typedef std::map<std::string, IdentifierType>  IdentifierList;

  static IdManager    *s_instance;
  IdentifierList       m_id_list;

};

#endif
