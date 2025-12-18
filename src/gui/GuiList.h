/*
                          GuiList.h  -  description
                             -------------------
    begin                : Wed May 23 2000
    copyright            : (C) 2000 by SEMAFOR Informatik & Energie AG
    email                : tar@semafor.ch

 $Id: GuiList.h,v 1.10 2006/10/25 13:08:59 amg Exp $
                       All Rights Reserved.

 SEMAFOR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY
 OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 PURPOSE, OR NON-INFRINGEMENT. SEMAFOR SHALL NOT BE LIABLE FOR ANY
 DAMAGES SUFFERED BY USERS AS A RESULT OF USING, MODIFYING OR
 DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
                                                                               
*/

#if !defined(GUI_LIST_INCLUDED_H)
#define GUI_LIST_INCLUDED_H


#include "gui/GuiElement.h"
#include "gui/GuiIndexListener.h"
#include "job/JobStarter.h"
#include "job/JobFunction.h"

class XferDataItem;
class XferDataParameter;
class Scale;
class JobFunction;
class Stream;

class GuiList
{
  /*=============================================================================*/
  /* Constructor / Destructor                                                    */
  /*=============================================================================*/
 public:
  GuiList();
  virtual ~GuiList();
  GuiList(const GuiList&);
  
  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
 public:
  bool withIndex(){
    return m_withIndex;
  }
  void withIndex( int width );
  int  tableSize(){ return m_tableSize; }
  bool sortEnabled() { return m_sort_enabled; }
  int  getMaxRows();
  /** setzt den Titel des Widgets */
  void setTitle( const std::string &title, GuiElement::Alignment align );
  std::string getTitle() { return m_title; }
  GuiElement::Alignment getTitleAlignment() { return m_titleAlign; }
  
  virtual int sortOrder() = 0;
  virtual bool selectRow(int row, bool recursive=true) = 0;
  virtual bool selectRows(std::vector<int> rows, bool recursive=true) = 0;
  /** Abfrage nach den Indizes der selektierten Zeilen
     @param idxs Resultliste mit den Indizes
   */
  virtual bool getSelectedRows(std::vector<int>& idxs) = 0;
  virtual bool getSortCriteria(std::string& sortColumn) = 0;
  virtual void resetSortCriteria() = 0;
  virtual void clearSelection() = 0;
  virtual void  addColumn( const std::string &label
			     , XferDataItem *dataitem
			     , GuiElement::Alignment align
			     , int length
			     , int precision
			     , Scale *scale
			     , bool thousand_sep
			     , bool optional);
  virtual void  addColumn( XferDataItem *dataitemLabel
			     , XferDataItem *dataitem
			     , GuiElement::Alignment align
			     , int length
			     , int precision
			     , Scale *scale
			     , bool thousand_sep
			     , bool optional);
  virtual void addColumnToolTip(XferDataItem *item);
  virtual void addColumnColor(XferDataItem *item);
  virtual void setTableSize( int size );
  virtual void setEnableSort( bool ok );
  bool hasOptionalColumns() { return m_hasOptionalColumns; }
  bool hasModifiableLabels() { return m_modifiableLabels; }
  void setResizeable(bool b=true) { m_resizeable = b; }
  bool isResizeable() { return m_resizeable; }
  void setMultipleSelection() { m_multipleSelection = true; }
  bool isMultipleSelection() { return m_multipleSelection; }
  int getHorizontalHeaderLines() { return m_horzHeaderLines; }
  void setHorizontalHeaderLines(int c) { m_horzHeaderLines = c; }
  int getCountDecimalPlace() { return m_countDecimalPlace; }
  int getToolTipColumn() { return m_toolTipColumn; }
  int getColorColumn() { return m_colorColumn; }
  JobFunction* getFunction() const { return m_function; }

  virtual void setFunction( JobFunction *func );
  virtual Stream* getStream() = 0;
  virtual void serializeXML(std::ostream &os, bool recursive = false);
  virtual bool serializeJson(Json::Value& jsonObj, bool onlyUpdated = false);
#if HAVE_PROTOBUF
  virtual bool serializeProtobuf(in_proto::ElementList* eles, bool onlyUpdated = false);
#endif

  /** open config dialog **/
  virtual void openConfigDialog( const std::string name = std::string()) {}

  virtual bool columnCellColor(int icol, int idx, std::string& foreground, std::string& background) = 0;

  virtual GuiElement *getElement() = 0;
  /** Diese Funktion ruft eine variante Methode auf.
      Die Argumente werden diese Methode übergeben.
      Das Result wird als Json-String zurückgeben.
  */
  virtual std::string variantMethod(const std::string& method,
                                    const Json::Value& jsonArgs,
                                    JobEngine *eng);

/*=============================================================================*/
/* protected member functions                                                  */
/*=============================================================================*/
 protected:
  bool isDataItemUpdated( TransactionNumber trans );
  
 private:
/*=============================================================================*/
/* private class Column                                                        */
/*=============================================================================*/
 
  class Column : public GuiIndexListener{
 public:
   Column( int column
	       ,const std::string &label
	       , XferDataItem *dataItem 
	       , const GuiElement::Alignment &align
	       , const int length
	       , const int precision
	       , Scale *scale
	       , const bool thousand_sep
	       , bool optional);
   Column( int column
	       , XferDataItem *dataItemLabel
	       , XferDataItem *dataItem 
	       , const GuiElement::Alignment &align
	       , const int length
	       , const int precision
	       , Scale *scale
	       , const bool thousand_sep
	       , bool optional);
   Column( const Column& );
   virtual ~Column();

    bool   installDataItem( XferDataItem *dataItem );
    int    getWidth() { return m_width; }
    bool   isInverted() { return m_isInverted; }
    bool   isOptional() { return m_optional; }
    void   setOptional(bool optional) { m_optional = optional; }
    const std::string &getLabel();
    const std::string getTooltip();
    GuiElement::Alignment getAlignment() { return m_alignment; }
    void setWidth( int width ) { m_width = width; }
    void setLabel( const std::string &label ) { m_label = label; }
    XferDataParameter *getParam() const { return m_param; }
    bool getDataType( DataDictionary::DataType &dataType );
    std::string getDataType();
    const int getRowsCount();
    //    QVariant cellValue( int idx );
    std::string cellFormattedValue( int idx );
    // bool cellColor( int idx, QColor& foreground, QColor& background );
    // GuiIndexListener
    virtual bool acceptIndex( const std::string &name, int inx );
    virtual void setIndex( const std::string &name, int inx );
    bool isDataItemUpdated( TransactionNumber trans );
 private:
    Column  &operator=( const Column &i );
    Column  *operator&();
    const Column  *operator&() const;

    XferDataParameter                 *m_param;
    std::string                        m_label;
    XferDataItem                      *m_xfer_label;
    GuiElement::Alignment              m_alignment;
    int                                m_column;
    int                                m_width;
    bool                               m_isInverted;
    bool                               m_optional;
  };

  // webApi aux func
  void pushArgSelected(const Json::Value& jsonAry);
  bool popArgSelected(int& x0, int& y0, int& x1, int& y1);

  /*=============================================================================*/
  /* public member functions                                                     */
  /*=============================================================================*/
 public:
  Column *col( int i ) const;

/*=============================================================================*/
/* protected data                                                              */
/*=============================================================================*/
 protected:
  typedef std::vector<Column *> ColsVector;
  ColsVector             m_cols;

/*=============================================================================*/
/* private data                                                                */
/*=============================================================================*/
 private:
  JobFunction         *m_function;

  std::string            m_title;
  GuiElement::Alignment  m_titleAlign;
  bool                 m_hasOptionalColumns;
  bool                 m_modifiableLabels;
  bool                 m_withIndex;
  bool                 m_resizeable;
  bool                 m_multipleSelection;
  int                  m_countDecimalPlace;
  int                  m_toolTipColumn;
  int                  m_colorColumn;
  int                  m_tableSize;
  int                  m_horzHeaderLines;
  bool                 m_sort_enabled;

  std::vector<int>     m_argSelected;
  friend class GuiQtTableViewBase;  // WebApi
};

class GuiListTrigger : public JobStarter{
public:
  GuiListTrigger( JobFunction *f )
    : JobStarter( f ){}
  virtual ~GuiListTrigger() {}
  virtual void backFromJobStarter( JobAction::JobResult rslt ) {
    if( rslt == JobAction::job_Ok ){
      GuiManager::Instance().update( GuiElement::reason_Process );
    }
  }
};

#endif
