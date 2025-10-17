
#if !defined(GUI_SCROLLEDLIST_INCLUDED_H)
#define GUI_SCROLLEDLIST_INCLUDED_H

#include "gui/GuiScrolledlistListener.h"
#include "gui/GuiElement.h"

class GuiScrolledlist
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
public:
  GuiScrolledlist(GuiScrolledlistListener *listener)
    : m_listener(listener)
    , m_columnLength(20)
    , m_tableSize(10)
  {}

  virtual ~GuiScrolledlist() {
  }

  void setTitle(const std::string&);
  const std::string& getTitle() { return  m_title; }
  void setHeaderLabel(const std::string& label) { m_headerLabel = label; }
  const std::string& getHeaderLabel() { return  m_headerLabel; }
  void setColumnLength(int len) { m_columnLength = len; }
  int getColumnLength() { return  m_columnLength; }

/*=============================================================================*/
/* public member functions                                                     */
/*=============================================================================*/
public:
  virtual GuiElement *getElement() = 0;
  virtual int createSelectionList(bool sorted=false) = 0;
  virtual void setSelectedPosition( int pos ) = 0;
  virtual void setEditable(bool flag) = 0;
  void setTableSize(int size);
  bool serializeJson(Json::Value& jsonObj, bool onlyUpdated);
#if HAVE_PROTOBUF
  bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated);
#endif

  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);
protected:
  GuiScrolledlistListener* getListender(){ return m_listener; }
  int  tableSize(){ return m_tableSize; }

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
 private:
  GuiScrolledlistListener *m_listener;
  std::string m_title;
  std::string m_headerLabel;
  int         m_columnLength;
  int         m_tableSize;
};

#endif
