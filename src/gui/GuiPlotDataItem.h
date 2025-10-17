
#ifndef GUI_PLOTDATAITEM_INCLUDED_H
#define GUI_PLOTDATAITEM_INCLUDED_H

/**
   Die Klasse 'GuiPlotDataItem' kapselt die Informationen fuer eine Plot-Achse.
   Die Informationen enthalten Angaben ueber die Skalierungsfaktor, den Namen und eine
   Refernenz auf ein DataItem. Ueber die Anzahl der Wildcards kann die zu verwendende
   Dimension des DataItems festgelegt weden. Die DataItems koennen, unabhaengig von
   den Wildcards, zusammen mit Indizes verwendet werden.
   @author Copyright (C) 1999  SEMAFOR Informatik & Energie AG, Basel, Switzerland
   @version $Id: GuiPlotDataItem.h,v 1.24 2006/02/07 17:35:31 tar Exp $
*/
#include <map>
#include <jsoncpp/json/value.h>

#include "datapool/DataPoolDefinition.h"
#include "xfer/XferDataItem.h"

class Stream;
class Annotation;
class MarkerLine;
class Scale;

class GuiPlotDataItem {

  //====================================================//
  // Constructor                                        //
  //====================================================//
 public:
  GuiPlotDataItem( XferDataItem *dataitem=0, int numwildcards=0 );
  ~GuiPlotDataItem();
  GuiPlotDataItem( const GuiPlotDataItem & );
 private:
  GuiPlotDataItem &operator=( const GuiPlotDataItem & );
  //====================================================//
  // public definitions                                 //
  //====================================================//
 public:
  typedef std::map<double, std::string> AnnotationLabelsMap;

  //====================================================//
  // private definitions                                //
  //====================================================//
 private:
  //====================================================//
  // public functions                                   //
  //====================================================//
 public:
  bool isAnnotationLabelsUpdated( TransactionNumber trans ) const;
  bool getAnnotationLabelsMap( AnnotationLabelsMap &annoMap );
  bool setAnnotationTitle( const std::string &title );
  bool getAnnotationTitle( std::string &title );
  bool setAnnotationAngle( double angle );
  double getAnnotationAngle();
  bool setAnnotationLabels( XferDataItem *dataitem );
  bool setAnnotationValues( XferDataItem *dataitem, Scale *scale );
  bool setDataReference( DataReference * );
  void setDataItem( XferDataItem * );
  DataReference *Data() { return m_dataitem ? m_dataitem->Data() : 0; }
  XferDataItem *XferData() { return m_dataitem; }
  UserAttr *Attr() { return m_dataitem->getUserAttr(); }
  XferDataItemIndex *newDataItemIndex( int );
  XferDataItemIndex *newDataItemIndex();
  int getNumOfWildcards();
  XferDataItemIndex *getDataItemIndexWildcard( int );
  XferDataItemIndex *getDataItemIndex( const std::string& name );
  int getNumDimensions();
  const std::string getName() const;
  const std::string getUniqueName(int xAxisIndex, int yAxisIndex, bool cyclePlotMode) const;
  bool isValid()const;
  bool isDataItemUpdated( TransactionNumber trans )const;
  virtual bool acceptIndex( const std::string &, int );
  virtual void setIndex( const std::string &name, int inx );
  bool getValue( double & )const;
  void setLabel( const std::string &label ) { m_label = label; }
  void setLabel( XferDataItem* labelXfer );
  void setLabels( const std::vector<std::string> &labelVec )  { m_labelVector = labelVec; }
  void setValueLabelXferIndex( int );
  std::string getLabel() const;
  void setLabelStream( const std::string &label );
  std::vector<std::string> getLabels() const;
  void setUnit( const std::string &unit ) { m_unit = unit; }
  std::string getUnit();
  void setUnitStream( const std::string &unit );
  void getLabelAndUnit( std::string &label, bool annotation = false );
  void setScale( Scale *scale ) { m_scale = scale; }
  double getScaleFactor();
  enum eLineStyle{ LINEAR, STEP, DISCRETE, BAR };
  void setLineStyle( eLineStyle style ) { m_linestyle = style; }
  eLineStyle getLineStyle() { return m_linestyle; }
  void setMinRange( double minRange ) { m_minRange = minRange; }
  double getMinRange();// { return m_minRange; }
  void setMinXferRange( XferDataItem *xferMinX );
  void setMaxRange( double maxRange ) { m_maxRange = maxRange; }
  void setMaxXferRange( XferDataItem *xferMaxX );
  double getMaxRange();// { return m_maxRange; }
  void setDefaultRange( bool state ) { m_defaultRange = state; }
  bool getDefaultRange() { return m_defaultRange; }

  bool isMarkerLineUpdated( TransactionNumber trans ) const;
  bool setMarkerLineLabels( XferDataItem *dataitem );
  bool setMarkerLineXAxis( XferDataItem *dataitem, Scale *scale );
  XferDataItem* getMarkerLineXAxisXferData();
  double getMarkerLineXAxisScaleFactor();

private:
  std::string convertToString( Stream *s ) const;
  Annotation *           m_annotation;
  double                 m_annotationAngle;
  MarkerLine *           m_markerLine;
  XferDataItem          *m_dataitem;
  Scale                 *m_scale;
  std::string            m_label;
  XferDataItem          *m_labelXfer;
  std::vector<std::string> m_labelVector;
  Stream                *m_labelStream;
  std::string            m_unit;
  Stream                *m_unitStream;
  eLineStyle             m_linestyle;
  double                 m_minRange;
  double                 m_maxRange;
  bool                   m_defaultRange;
  XferDataItem          *m_xferScaleMin;
  XferDataItem          *m_xferScaleMax;
};

#endif
