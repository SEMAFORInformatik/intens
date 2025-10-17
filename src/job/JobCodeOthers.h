
#if !defined(JOB_CODE_OTHERS_INCLUDED_H)
#define JOB_CODE_OTHERS_INCLUDED_H

#include "job/JobCodeExec.h"
#include "gui/GuiFolderGroup.h"

class JobEngine;
class GuiElement;
class GuiListenerController;
class GuiForm;
class GuiFieldgroup;
class Stream;
class XferDataItem;
class JobClientSocket;
class JobRequest;
class JobSubscribe;
class JobMsgQueuePublish;
class JobPluginRequest;
class JobStackDataPtr;

/** Update GuiElement
 */
class JobCodeUpdateGuiElement : public JobCodeExec
{
public:
  JobCodeUpdateGuiElement( GuiElement *el ): m_element( el ){}
  virtual ~JobCodeUpdateGuiElement(){}
  /** Diese Funktion ruft die Funktion update() eines GuiElements auf.
      Der Typ des GuiElements spielt keine Rolle.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_element;
};

/** Enable GuiElement
 */
class JobCodeEnableGuiElement : public JobCodeExec
{
public:
  JobCodeEnableGuiElement( GuiElement *el ): m_element( el ){}
  virtual ~JobCodeEnableGuiElement(){}
  /** Diese Funktion ruft die Funktion enable() eines GuiElements auf.
      Der Typ des GuiElements spielt keine Rolle.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_element;
};

/** Disable GuiElement
 */
class JobCodeDisableGuiElement : public JobCodeExec
{
public:
  JobCodeDisableGuiElement( GuiElement *el ): m_element( el ){}
  virtual ~JobCodeDisableGuiElement(){}
  /** Diese Funktion ruft die Funktion disable() eines GuiElements auf.
      Der Typ des GuiElements spielt keine Rolle.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_element;
};

/** Disable dragging for GuiElement
 */
class JobCodeDisableDragGuiElement : public JobCodeExec
{
public:
  JobCodeDisableDragGuiElement( GuiElement *el ): m_element( el ){}
  virtual ~JobCodeDisableDragGuiElement(){}
  /** Diese Funktion ruft die Funktion disable_drag() eines GuiElements auf.
      Der Typ des GuiElements spielt keine Rolle.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_element;
};


/** Set Stylesheet to GuiElement
 */
class JobCodeSetStylesheet : public JobCodeExec
{
public:
  JobCodeSetStylesheet( GuiElement *el ): m_element( el ){}
  virtual ~JobCodeSetStylesheet(){}
  /** Diese Funktion ruft die Funktion setStylesheet() eines GuiElements auf.
      Der Typ des GuiElements spielt keine Rolle.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_element;
};

/** Replace GuiElement
 */
class JobCodeReplaceGuiElement : public JobCodeExec
{
public:
  JobCodeReplaceGuiElement( GuiElement *old_el, GuiElement *new_el )
    : m_old_element( old_el )
    , m_new_element( new_el ) {}
  virtual ~JobCodeReplaceGuiElement(){}
  /** Diese Funktion ruft die Funktion replace() des GuiManagers auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_old_element;
  GuiElement   *m_new_element;
};

/** GuiElementMethod
 */
class JobCodeGuiElementMethod : public JobCodeExec
{
public:
  JobCodeGuiElementMethod(GuiElement *gui_el)
    : m_gui_element( gui_el ) {}
  virtual ~JobCodeGuiElementMethod(){}
  /** Diese Funktion ruft die Funktion method() des GuiElements auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement   *m_gui_element;
};

/** Set RANGE of GuiFieldgroup
 */
class JobCodeSetGuiFieldgroupRange : public JobCodeExec
{
public:
  JobCodeSetGuiFieldgroupRange( GuiFieldgroup *fg )
    : m_fieldgroup( fg ){}
  virtual ~JobCodeSetGuiFieldgroupRange(){}
  /** Diese Funktion set die Range einer Fieldgroup Table neu.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiFieldgroup *m_fieldgroup;
};

/** Mit einem Objekt dieser Klasse wird bei einem GuiListenerController
    die Funktion allow() aufgerufen. Dies bedeutet, dass anschliessend
    alle von diesem kontrollierten Listeners über das GUI aufrufbar sind.
 */
class JobCodeAllow : public JobCodeExec
{
public:
  JobCodeAllow( GuiListenerController *ctrl ): m_controller( ctrl ){}
  virtual ~JobCodeAllow(){}
  /** Diese Funktion ruft die Funktion allow() des GuiListenerController
      auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiListenerController   *m_controller;
};

/** Mit einem Objekt dieser Klasse wird bei einem GuiListenerController
    die Funktion disallow() aufgerufen. Dies bedeutet, dass anschliessend
    alle von diesem kontrollierten Listeners über das GUI nicht mehr
    aufrufbar sind.
 */
class JobCodeDisallow : public JobCodeExec
{
public:
  JobCodeDisallow( GuiListenerController *ctrl ): m_controller( ctrl ){}
  virtual ~JobCodeDisallow(){}
  /** Diese Funktion ruft die Funktion disallow() des GuiListenerController
      auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiListenerController   *m_controller;
};

class JobCodeSerializeForm : public JobCodeExec
{
public:
  JobCodeSerializeForm( GuiForm *form ): m_form( form ) {}
  virtual ~JobCodeSerializeForm(){}
  virtual OpStatus execute( JobEngine * );
  bool serializeForm();
private:
  GuiForm   *m_form;
};

class JobCodeSerializeGuiElement : public JobCodeExec
{
public:
 JobCodeSerializeGuiElement(GuiElement *elem, AppData::SerializeType type,
							bool readStack=false, const std::string &filename=std::string() )
   : m_guiElem( elem )
   , m_type( type )
   , m_readStack( readStack )
   , m_filename( filename )  {}
  virtual ~JobCodeSerializeGuiElement(){}
  virtual OpStatus execute( JobEngine * );
  bool serializeGuiElement();
private:
  GuiElement   *m_guiElem;
  AppData::SerializeType m_type;
  bool              m_readStack;
  const std::string m_filename;
};

class JobCodeSerializeElement : public JobCodeExec
{
public:
 JobCodeSerializeElement(GuiElement *elem, AppData::SerializeType type,
						 const std::string &filename=std::string() )
   : m_guiElem( elem )
   , m_type( type )
   , m_filename( filename )  {}
  virtual ~JobCodeSerializeElement(){}
  virtual OpStatus execute( JobEngine * );
  bool serializeGuiElement();
private:
  GuiElement   *m_guiElem;
  AppData::SerializeType m_type;
  const std::string m_filename;
};

class JobCodeWriteSettings : public JobCodeExec
{
public:
  JobCodeWriteSettings() {}
  virtual ~JobCodeWriteSettings(){}
  virtual OpStatus execute( JobEngine * );
};

/** Mit einem Objekt dieser Klasse wird irgend ein Map gemacht.
 */
class JobCodeMap : public JobCodeExec
{
public:
 JobCodeMap(XferDataItem *xfer, int nargs)
   : m_xfer(xfer), m_elem(0), m_nargs(nargs) {}
 JobCodeMap(GuiElement *elem, int nargs)
   : m_xfer(0), m_elem(elem), m_nargs(nargs) {}
  virtual ~JobCodeMap(){}
  /** Diese Funktion ruft die Funktion
      - manage() eines Forms
      - map() eines GuiElements
      - activatePage(...) eines Folders (bei nargs != -1)
      auf.
      Der Name des GuiElements ist der Werte von xfer oder
      das GuiElement ist *elem.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  XferDataItem   *m_xfer;
  GuiElement     *m_elem;
  int             m_nargs;
};

/** Mit einem Objekt dieser Klasse wird irgend ein Unmap gemacht.
 */
class JobCodeUnmap : public JobCodeExec
{
public:
  JobCodeUnmap(){}
  virtual ~JobCodeUnmap(){}
  /** Diese Funktion ruft die Funktion unmanage() oder unmap() eines GuiElements auf.
      Der Name des GuiElements ist der Wert des xfer auf dem Stack.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Mit einem Objekt dieser Klasse wird die Sichtbarkeit irgend
    eines GuiElements geprüft.
 */
class JobCodeVisible : public JobCodeExec
{
public:
  JobCodeVisible( XferDataItem *xfer ): m_xfer( xfer ){}
  virtual ~JobCodeVisible(){}
  /** Diese Funktion prüft, ob irgend ein GuiElement sichtbar ist.
      Der Name des GuiElements ist der Werte von xfer.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  XferDataItem   *m_xfer;
};

/** Mit einem Objekt dieser Klasse wird das current GuiForm gesetzt. Sie wird
    durch das WebApi aufgerufen. ("SET_CURRENT_FORM(dataReference);")
 */
class JobCodeSetCurrentForm : public JobCodeExec
{
public:
  JobCodeSetCurrentForm( XferDataItem *xfer ): m_xfer( xfer ){}
  virtual ~JobCodeSetCurrentForm(){}
  /** Diese Funktion setzt das Current Form.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  XferDataItem   *m_xfer;
};

/** Mit einem Objekt dieser Klasse wird die Editierbarkeit irgend
    eines GuiElements geprüft.
 */
class JobCodeEditable : public JobCodeExec
{
public:
  JobCodeEditable(GuiElement *elem): m_element(elem) {}
  virtual ~JobCodeEditable(){}
  /** Diese Funktion prüft, ob irgend ein GuiElement editierbar ist.
      Das GuiElement ist entweder angegeben oder ist der Wert des xfer auf dem Stack.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiElement *m_element;
};

/** Mit einem Objekt dieser Klasse wird eine GuiForm geoeffnet. Sie ruft
    die Funktion manage() auf.
 */
class JobCodeMapForm : public JobCodeExec
{
public:
  JobCodeMapForm( GuiForm *form ): m_form( form ){}
  virtual ~JobCodeMapForm(){}
  /** Diese Funktion ruft die Funktion manage() des GuiForm auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiForm   *m_form;
};

/** Mit einem Objekt dieser Klasse wird eine GuiForm geschlossen. Sie ruft
    die Funktion unmanage() auf.
 */
class JobCodeUnmapForm : public JobCodeExec
{
public:
  JobCodeUnmapForm( GuiForm *form ): m_form( form ){}
  virtual ~JobCodeUnmapForm(){}
  /** Diese Funktion ruft die Funktion unmanage() des GuiForm auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiForm   *m_form;
};

/** Mit einem Objekt dieser Klasse wird die Sichtbarkeit des
    GuiForm geprüft.
 */
class JobCodeVisibleForm : public JobCodeExec
{
public:
  JobCodeVisibleForm( GuiForm *form ): m_form( form ){}
  virtual ~JobCodeVisibleForm(){}
  /** Diese Funktion prüft, ob das GuiForm sichtbar ist.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  GuiForm   *m_form;
};

/** Mit einem Objekt dieser Klasse werden alle einer Foldergruppe
    zugehörigen Folders geoeffnet.
 */
class JobCodeMapFolderGroup : public JobCodeExec
{
public:
  JobCodeMapFolderGroup( const std::string &name,
			 GuiFolderGroup::OmitMap omit_map )
    : m_groupname( name )
    , m_omit_map( omit_map ){}
  virtual ~JobCodeMapFolderGroup(){}
  /** Diese Funktion ruft die Funktion activateFolderGroupByName() des
      GuiManager auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_groupname;
  GuiFolderGroup::OmitMap m_omit_map;
};

/** Mit einem Objekt dieser Klasse werden alle einer Foldergruppe
    zugehörigen Folders geschlossen bzw. versteckt.
 */
class JobCodeUnmapFolderGroup : public JobCodeExec
{
public:
  JobCodeUnmapFolderGroup( const std::string &name ): m_groupname( name ){}
  virtual ~JobCodeUnmapFolderGroup(){}
  /** Diese Funktion ruft die Funktion activateFolderGroupByName() des
      GuiManager auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_groupname;
};

/** Mit einem Objekt dieser Klasse wird die Sichtbarkeit der
    Foldergruppe geprüft.
 */
class JobCodeVisibleFolderGroup : public JobCodeExec
{
public:
  JobCodeVisibleFolderGroup( const std::string &name ): m_groupname( name ){}
  virtual ~JobCodeVisibleFolderGroup(){}
  /** Diese Funktion prüft, ob die Foldergruppe sichtbar ist.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_groupname;
};

/** Mit einem Objekt dieser Klasse werden wird das GuiElement geoeffnet.
 */
class JobCodeMapGuiElement : public JobCodeExec
{
public:
  JobCodeMapGuiElement( const std::string &name ): m_elemname( name ){}
  virtual ~JobCodeMapGuiElement(){}
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_elemname;
};

/** Mit einem Objekt dieser Klasse werden wird das GuiElementgeschlossen bzw. versteckt.
 */
class JobCodeUnmapGuiElement : public JobCodeExec
{
public:
  JobCodeUnmapGuiElement( const std::string &name ): m_elemname( name ){}
  virtual ~JobCodeUnmapGuiElement(){}
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_elemname;
};

/** Mit einem Objekt dieser Klasse werden wird ein TableLine einer Table wieder geoeffnet.
 */
class JobCodeMapTableLine : public JobCodeExec
{
public:
  JobCodeMapTableLine( const std::string &name ): m_tablename( name ){}
  virtual ~JobCodeMapTableLine(){}
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_tablename;
};

/** Mit einem Objekt dieser Klasse werden wird ein TableLine einer Table versteckt.
 */
class JobCodeUnmapTableLine : public JobCodeExec
{
public:
  JobCodeUnmapTableLine( const std::string &name ): m_tablename( name ){}
  virtual ~JobCodeUnmapTableLine(){}
  virtual OpStatus execute( JobEngine * );
private:
  std::string    m_tablename;
};

/** Mit einem Objekt dieser Klasse wird eine Zeile in einem GuiList selektiert.
 */
class JobCodeSelectRows : public JobCodeExec
{
public:
  JobCodeSelectRows(const std::string &guiElementId, bool dataIsInt)
    : m_guiElementId( guiElementId )
    , m_dataIsInt(dataIsInt) {}
  virtual ~JobCodeSelectRows(){}
  /** Diese Funktion ruft die Funktion executeElement auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
  /** Diese Funktion ruft die Funktion
      - selectRow oder selectRows (Liste, Tabelle)
      - setItemSelection (Navigator)
      auf.
      @param guiElementId Name der Liste, der Tabelle oder des Navigators.
      @param xfer XferDataItem mit den Indizes der zu selektierenden Zeilen.
      @param data_int JobStackDataPtr mit dem Index der zu selektierenden Zeile (Liste oder Tabelle ohne multiple selection).
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  static OpStatus executeElement(const std::string& guiElemendId,  XferDataItem *xfer, int row=0);
private:
  std::string   m_guiElementId;
  bool m_dataIsInt;
};

/** Mit einem Objekt dieser Klasse werden
   - die Indizes der selektierten Zeilen der Liste oder des Navigators
   - die Indizes der Reihen und Spalten der selektierten Zellen der Tabelle
   - die Koordinaten (x, y), den y-Achsen-Typ (1 oder 2) sowie das y-Achsen-Label der selektierten Punkte des Plot2Ds
   via XferDataItems in den Datapool geschrieben.
 .
 */
class JobCodeGetSelection : public JobCodeExec
{
public:
  JobCodeGetSelection(const std::string &guiElementId ) :m_guiElementId( guiElementId ) {}
  virtual ~JobCodeGetSelection(){}
  /** Siehe Beschreibung der Klasse oben.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_guiElementId;
};

/** Mit einem Objekt dieser Klasse werden
   - der Name der sortieren Spalte (Column) der Liste
   via XferDataItems in den Datapool geschrieben.
 .
 */
class JobCodeGetSortCriteria : public JobCodeExec
{
public:
  JobCodeGetSortCriteria(const std::string &guiElementId ) :m_guiElementId( guiElementId ) {}
  virtual ~JobCodeGetSortCriteria(){}
  /** Siehe Beschreibung der Klasse oben.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_guiElementId;
};

/** Mit einem Objekt dieser Klasse werden alle Selektionen der Liste, des Navigators oder der Tabelle abgewählt.
 */
class JobCodeClearSelection : public JobCodeExec
{
public:
  JobCodeClearSelection(const std::string &guiElementId ) :m_guiElementId( guiElementId ) {}
  virtual ~JobCodeClearSelection(){}
  /** Diese Funktion ruft die Funktion clearSelection() des GuiElements auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  std::string   m_guiElementId;
};

/** Mit einem Objekt dieser Klasse wird eine Clipboard-Copy Funktion ausgefuehrt.
 */
class JobCodeCopy : public JobCodeExec
{
public:
  JobCodeCopy(Stream *stream, GuiElement* elem)
    : m_stream(stream), m_element(elem) {}
  virtual ~JobCodeCopy(){}
  /** Diese Funktion kopiert den Inhalt eines Streams in das
      Clipboard.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @param stream Pointer auf den Output Stream Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  Stream   *m_stream;
  GuiElement *m_element;
};

/** Mit einem Objekt dieser Klasse wird eine Clipboard-Paste Funktion ausgefuehrt.
 */
class JobCodePaste : public JobCodeExec
{
public:
  JobCodePaste( Stream *stream ): m_stream( stream ){}
  virtual ~JobCodePaste(){}
  /** Diese Funktion liest die Clipboard aus und schreibt die
      Daten in den Stream.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @param stream Pointer auf den Input Stream Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  Stream   *m_stream;
};

/** Mit einem Objekt dieser Klasse nochmals die Parse Funktion ausgefuehrt.
 */
class JobCodeParse : public JobCodeExec
{
public:
  JobCodeParse(XferDataItem *xfer) : m_xfer(xfer) {}
  JobCodeParse(const std::string& fn) : m_xfer(0), m_filename(fn) {}
  virtual ~JobCodeParse(){}
  /** Diese Funktion liest die Clipboard aus und schreibt die
      Daten in den Stream.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @param stream Pointer auf den Input Stream Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  XferDataItem   *m_xfer;
  std::string     m_filename;
};

/** Mit einem Objekt dieser Klasse wird ein Confirmation-Dialog
    aufgerufen um interaktiv in den Ablauf eingreifen zu können.
 */
class JobCodeConfirm : public JobCodeExec
{
public:
 JobCodeConfirm(bool cancelBtn,
				const std::map<GuiElement::ButtonType, std::string>& buttonText)
	: m_cancelBtn(cancelBtn)
	, m_buttonText(buttonText)
	{}
  virtual ~JobCodeConfirm() {}
  /** Diese Funktion ruft den Confirmation-Dialog auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );

  bool m_cancelBtn;
  std::map<GuiElement::ButtonType, std::string> m_buttonText;
};

/** Mit einem Objekt dieser Klasse wird ein GetText-Dialog
    aufgerufen um interaktiv in den Ablauf eingreifen zu können.
 */
class JobCodeGetText : public JobCodeExec
{
public:
  JobCodeGetText() {}
  virtual ~JobCodeGetText() {}
  /** Diese Funktion ruft den GetText-Dialog auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Mit einem Objekt dieser Klasse wird ein Message-Dialog
    aufgerufen um den Ablauf zu unterbrechen um eine Meldung
    zu zeigen.
 */
class JobCodeMessageBox : public JobCodeExec
{
public:
  JobCodeMessageBox(bool with_title=false){ m_with_title = with_title; }
  virtual ~JobCodeMessageBox(){}
  /** Diese Funktion ruft den Message-Dialog auf.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
private:
  bool m_with_title;
};

/** Mit einem Objekt dieser Klasse wird die Message gesetzt, welche
    nach dem Ende der Funktion in der MessageLine erscheinen soll.
 */
class JobCodeMessage : public JobCodeExec
{
public:
  JobCodeMessage(){}
  virtual ~JobCodeMessage(){}
  /** Diese Funktion setzt den Wert für die MessageLine.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** Mit einem Objekt dieser Klasse wird die Message gesetzt, welche
    nach dem Ende der Funktion in der MessageLine erscheinen soll.
 */
class JobCodeSetMessage : public JobCodeExec
{
public:
  JobCodeSetMessage(){}
  virtual ~JobCodeSetMessage(){}
  /** Diese Funktion setzt den Wert für die MessageLine.
      @param eng Pointer auf das aufrufende JobEngine Objekt.
      @return Status des laufenden Jobs (siehe JobElement.h)
  */
  virtual OpStatus execute( JobEngine * );
};

/** returns the file name of the associated file stream
 */
class JobCodeGetFileName : public JobCodeExec
{
public:
  JobCodeGetFileName(){}
  virtual ~JobCodeGetFileName(){}
  virtual OpStatus execute( JobEngine * );
};

/** returns the dir name of the associated file stream
 */
class JobCodeGetDirName : public JobCodeExec
{
public:
  JobCodeGetDirName(){}
  virtual ~JobCodeGetDirName(){}
  virtual OpStatus execute( JobEngine * );
};
/** returns the base name of the associated file stream
 */

class JobCodeGetBaseName : public JobCodeExec
{
public:
  JobCodeGetBaseName(){}
  virtual ~JobCodeGetBaseName(){}
  virtual OpStatus execute( JobEngine * );
};

/** sends stream over TCP/IP
 */
class JobCodeSend : public JobCodeExec
{
public:
  JobCodeSend( JobClientSocket *job )
    : m_job( job ) {
  }
  virtual ~JobCodeSend(){}
  virtual OpStatus execute( JobEngine * );

private:
  JobClientSocket *m_job;
};

/** send a request to a message queue
 */
class JobCodeRequest : public JobCodeExec
{
public:
  JobCodeRequest( JobRequest* job )
    : m_job( job ) {
  }
  virtual ~JobCodeRequest(){}
  virtual OpStatus execute( JobEngine * );

private:
  JobRequest *m_job;
};

/** subscribes to a plugin
 */
class JobCodeSubscribe : public JobCodeExec
{
public:
  JobCodeSubscribe( JobSubscribe* job )
    : m_job( job ) {
  }
  virtual ~JobCodeSubscribe(){}
  virtual OpStatus execute( JobEngine * );

private:
  JobSubscribe *m_job;
};

/** publish to a message queue
 */
class JobCodePublish : public JobCodeExec
{
public:
  JobCodePublish( JobMsgQueuePublish *job )
    : m_job( job ) {
  }
  virtual ~JobCodePublish(){}
  virtual OpStatus execute( JobEngine * );

private:
  JobMsgQueuePublish *m_job;
};

/** set hostname of a messagequeue
 */
class JobCodeSetMessageQueueHost : public JobCodeExec
{
 public:
  JobCodeSetMessageQueueHost(){}
  virtual ~JobCodeSetMessageQueueHost(){}
  virtual OpStatus execute( JobEngine * );
};

/** sends stream over TCP/IP
 */
class JobCodePrintLog : public JobCodeExec
{
public:
  JobCodePrintLog(){
  }
  virtual ~JobCodePrintLog(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Current Iso Time zurueck
 */
class JobCodeIsoTime: public JobCodeExec
{
public:
  JobCodeIsoTime(){}
  virtual ~JobCodeIsoTime(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Current Iso Date zurueck
 */
class JobCodeIsoDate: public JobCodeExec
{
public:
  JobCodeIsoDate(){}
  virtual ~JobCodeIsoDate(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    Current Iso DateTime zurueck
 */
class JobCodeIsoDateTime: public JobCodeExec
{
public:
  JobCodeIsoDateTime(){}
  virtual ~JobCodeIsoDateTime(){}
  virtual OpStatus execute( JobEngine * );
};

/** Die Code-Funktion dieser Klasse gibt den
    formatierten Text zurueck
 */
class JobCodeCompose: public JobCodeExec
{
public:
  JobCodeCompose(int numArgs)
  : m_numArgs(numArgs) {}
  virtual ~JobCodeCompose(){}
  virtual OpStatus execute( JobEngine * );
private:
  int m_numArgs;
};

#endif
