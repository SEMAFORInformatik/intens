
#include <algorithm>
#include <QApplication>
#include <QRegularExpression>
#if QT_VERSION >= 0x060000
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QBoxLayout>
#include <limits>

#include "gui/qt/QtDialogCompare.h"

#include "gui/qt/GuiQtForm.h"
#include "gui/qt/GuiQtFolder.h"
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/GuiQwtPlot.h"
#include "gui/qt/GuiQtTableDataItem.h"
#include "gui/GuiFactory.h"
#include "gui/GuiDialog.h"
#include "gui/GuiList.h"
#include "gui/qt/GuiQtTable.h"
#include "gui/Gui2dPlot.h"
#include "gui/GuiTableLine.h"
#include "gui/GuiPlotDataItem.h"
#include "gui/GuiFieldgroup.h"
#include "gui/GuiFieldgroupLine.h"
#include "gui/GuiDataField.h"
#include "gui/GuiOrientationContainer.h"
#include "gui/GuiLabel.h"
#include "gui/GuiButton.h"
#include "gui/GuiButtonbar.h"
#include "gui/GuiScrolledText.h"
#include "gui/GuiIndex.h"
#include "gui/GuiStretch.h"
#include "datapool/DataVector.h"
#include "datapool/DataItem.h"
#include "datapool/DataIntegerValue.h"
#include "datapool/DataRealValue.h"
#include "datapool/DataStringValue.h"
#include "xfer/Scale.h"
  #include "xfer/XferDataItem.h"
#include "utils/gettext.h"
#include "utils/Date.h"
#include "utils/JsonUtils.h"
#include "utils/StringUtils.h"

INIT_LOGGER();

QtDialogCompare*  QtDialogCompare::s_compareDialog = 0;
std::vector<GuiElement*> delList;

/* --------------------------------------------------------------------------- */
/* createAllDataItemIndexesOnLastLevel --                                      */
/* --------------------------------------------------------------------------- */

static void createAllDataItemIndexesOnLastLevel(XferDataItem* xfer) {
  int numIdx = xfer->getNumIndexes();
  int numDims = xfer->getNumDimensions();
   int lastLevelFirstIndexNum = xfer->getFirstIndexNum( xfer->getNumberOfLevels()-1 );
   BUG_DEBUG( "lastLevelFirstIndexNum["<<lastLevelFirstIndexNum<<"]   numDims["<<numDims
	     <<"]   numIdx["<<numIdx<<"]  VN["<<xfer->getFullName(true)<<"]");
  if (lastLevelFirstIndexNum == -1 ||
      (numIdx - lastLevelFirstIndexNum) < numDims) {
    int cnt = lastLevelFirstIndexNum == -1 ? numDims : numIdx - lastLevelFirstIndexNum;
    for (int ii=0; ii<cnt; ++ii) {
      xfer->newDataItemIndex( xfer->getNumberOfLevels()-1 );
      xfer->setDimensionIndizes();
    }
  }

}

/* --------------------------------------------------------------------------- */
/* getDataVectorCharacteristic --                                              */
/* --------------------------------------------------------------------------- */

static bool getDataVectorCharacteristic(XferDataItem* xfer, double& min, double& max, double& percentFactor) {
    DataVector vector;
    double dVal;
    int iVal;
    bool ret;
    min = std::numeric_limits<double>::max();
    max = -std::numeric_limits<double>::max();
    int id[10] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    percentFactor = 0;

    // only real or integer can be processed
    if ( xfer->getDataType() != DataDictionary::type_Real &&
	 xfer->getDataType() != DataDictionary::type_Integer ) return false;

    if (xfer->getNumIndexes() == 1) {
      xfer->Data()->getDataVector( vector, 1, id );
    } else {
      xfer->Data()->getDataVector( vector, 2, id );
    }
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      if ( xfer->getDataType() == DataDictionary::type_Real) {
	ret = (*it)->getValue(dVal);
    } else {
	ret = (*it)->getValue(iVal);
	dVal = iVal;
      }

      if ( ret) {
	if (dVal < min) min = dVal;
	if (dVal > max) max = dVal;
      } else {
	BUG_INFO("invalid data request, xfer: " << xfer->getFullName(true) << "  val: " << dVal);
      }
    }
    double dz = std::max( fabs(min), fabs(max) );
    if (dz != 0)
      percentFactor = 100./dz;

    return min != max; // different check
}

/* --------------------------------------------------------------------------- */
/* hasDataVectorDifferences --                                                 */
/* --------------------------------------------------------------------------- */

bool hasDataVectorDifferences(XferDataItem* xfer) {
  double min, max, percentFactor;
  return getDataVectorCharacteristic(xfer, min, max, percentFactor);
}

void QtDialogCompare::CloseButtonListener::ButtonPressed() {
  if( m_dialog != 0 ) m_dialog->unmanage();
}

QtDialogCompare::QtDialogCompare( const std::string& name )
  : GuiQtForm(0, false, name)
  ,  m_dialog( 0 )
  , m_scalarTab( 0 )
  , m_vectorTab( 0 )
  , m_matrixTab( 0 )
  , m_othersTab( 0 )
  , m_maxScalarCols( 0 )
{
  setMenuInstalled();
}

QtDialogCompare::~QtDialogCompare() {
  destroy();
}

/* --------------------------------------------------------------------------- */
/* Instance --                                                                 */
/* --------------------------------------------------------------------------- */
QtDialogCompare& QtDialogCompare::Instance() {
  if (s_compareDialog == 0) {
    s_compareDialog = new QtDialogCompare("COMPARE_DIALOG");
  }
  return *s_compareDialog;
}

/* --------------------------------------------------------------------------- */
/* myWidget --                                                                 */
/* --------------------------------------------------------------------------- */
QWidget* QtDialogCompare::myWidget() {
  if (m_dialog)
    return m_dialog->getQtElement()->myWidget();
  return 0;
}

/* --------------------------------------------------------------------------- */
/* destroy --                                                                  */
/* --------------------------------------------------------------------------- */
bool QtDialogCompare::destroy() {
  std::vector<GuiElement*>::iterator it = delList.begin();
  for (;it != delList.end();++it)
    delete (*it);
  delList.clear();

  ItemDataVector::iterator mit;
  for (mit = m_xferScalarNodes.begin(); mit != m_xferScalarNodes.end();++mit)
    delete mit->xfer;
  for (mit = m_xferVectorNodes.begin(); mit != m_xferVectorNodes.end();++mit)
    delete mit->xfer;
  for (mit = m_xferMatrixNodes.begin(); mit != m_xferMatrixNodes.end();++mit)
    delete mit->xfer;
  for (mit = m_xferOthersNodes.begin(); mit != m_xferOthersNodes.end();++mit)
    delete mit->xfer;
  m_xferScalarNodes.clear();
  m_xferVectorNodes.clear();
  m_xferMatrixNodes.clear();
  m_xferOthersNodes.clear();
  delete m_dialog;
  delete m_scalarTab;
  delete m_vectorTab;
  delete m_matrixTab;
  delete m_othersTab;
  for (std::vector<Gui2dPlot*>::iterator it = m_plot2dVector.begin(); it != m_plot2dVector.end();++it)
    delete *it;
  m_plot2dVector.clear();
  m_dialog = 0;
  m_scalarTab = 0;
  m_vectorTab = 0;
  m_matrixTab = 0;
  m_othersTab = 0;
  m_header.clear();
  m_itemIgnoreList.clear();
  m_maxScalarCols = 0;
  return true;
}

/* --------------------------------------------------------------------------- */
/* manage --                                                                   */
/* --------------------------------------------------------------------------- */
void QtDialogCompare::manage() {
  unmanage();
  destroy(); // force recreate
  if (!m_dialog) {
    create();
  }
  if (m_dialog) {
    m_dialog->setShown( true ); // gui update reason
    myWidget()->show();
    m_dialog->update(GuiElement::reason_Always);
  }
}

/* --------------------------------------------------------------------------- */
/* map --                                                                      */
/* --------------------------------------------------------------------------- */
void QtDialogCompare::map() {
  manage();
}

/* --------------------------------------------------------------------------- */
/* unmanage --                                                                 */
/* --------------------------------------------------------------------------- */
void QtDialogCompare::unmanage() {
  if (m_dialog) {
    myWidget()->hide();
  }
}

/* --------------------------------------------------------------------------- */
/* unmap --                                                                    */
/* --------------------------------------------------------------------------- */
void QtDialogCompare::unmap() {
  unmanage();
}

/* --------------------------------------------------------------------------- */
/* create --                                                                   */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::create() {
  BUG_DEBUG( "QtDialogCompare::create");
  std::vector<XferDataItem*> xferNodes;
  std::vector<std::string> itemLabelList;
  int id[1] = { -1 };
  std::string s;
  std::string vnItem        = compose("%1.%2", StructVarname, ItemVarname);
  std::string vnItemLabel   = compose("%1.%2", StructVarname, ItemLabelVarname);
  std::string vnIgnoreLabel = compose("%1.%2", StructVarname, ItemIgnoreVarname);
  std::string vnHeader      = compose("%1.%2", StructVarname, HeaderVarname);

  // read varnames from dataItems and create XferDataItems
  DataReference *refItem = DataPoolIntens::Instance().getDataReference( vnItem );
  if ( refItem != 0 ) {
    DataVector vector;
    refItem->getDataVector( vector, 1, id );
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      (*it)->getValue(s);
      DataReference *dref = DataPoolIntens::Instance().getDataReference( s );
      if (!dref) {
	GuiFactory::Instance()->getLogWindow()->writeText( compose("%1 : item '%2' contains invalid varname '%3'\n", DateAndTime(), vnItem, s));
	delete dref;
	continue;  // next
      }
      const DataItem* ditem= dynamic_cast<const DataItem*>(dref->getItem());
      if ( !ditem || ditem && !ditem->isAttributeSet( DATAisDifferent ) ) {
	std::string msg(compose(_("item '%1' has no different data"), s));
	GuiFactory::Instance()->getLogWindow()->writeText(compose("%1 : %2\n", DateAndTime(), msg));
        BUG_INFO(msg);
	delete dref;
	continue;  // next
      }

      // Bei einem Vektor/Matrix interessiert uns nur ein skalarer Wert
      if ( s[s.size()-1] == ']' ) {  // immer Skalar!!!!

	// read index list from string 's'
	std::size_t posA = s.find_last_of('[');
	std::istringstream is(s.substr(posA+1, s.size()-posA-2));
	int iNum=0;
	char c;
	int indx[10];
	while(is >> indx[iNum] ) { ++iNum; is >> c; }

	BUG_DEBUG("posA: " << posA << "  istr: " << s.substr(posA+1, s.size()-posA-2));

	XferDataItem *x = new XferDataItem( dref);
	// create all indexes on last level
	createAllDataItemIndexesOnLastLevel(x);

	// set wildcard option on first index
	int lastLevelFirstIndexNum = x->getFirstIndexNum( x->getNumberOfLevels()-1 );
	XferDataItemIndex* index = x->getDataItemIndex(lastLevelFirstIndexNum);
	index->setWildcard();
	index->setLowerbound(0);


	int numDims = x->getNumDimensions();
	if ( iNum >=  numDims ) {
	  BUG_INFO(compose("item '%1' has less dimensions '%2'",s, numDims));
	  //// delete x;
	  ///	  continue;
	} else

	while(iNum > 0) {
	  BUG_DEBUG("lastLevelFirstIndexNum: " << lastLevelFirstIndexNum
		    << "  iNum:" <<iNum
		    << "  indx[iNum]:" << indx[iNum-1]);
	  x->getNumDimensions();
	  x->setIndex( lastLevelFirstIndexNum+iNum, indx[iNum-1]); //
	  --iNum;
	}
	x->setDimensionIndizes();

	if ( !hasDataVectorDifferences(x) ) {
	  BUG_INFO(compose("vector item '%1' has no different scalar data",s));
	  delete x;
	  continue;
	}
	m_xferScalarNodes.push_back( ItemData("", x) );
	BUG_DEBUG("add new special Scalar Node DataItem named: " << x->getFullName(true));
      } else {
	xferNodes.push_back( new XferDataItem( dref ) );
	BUG_DEBUG("add new Scalar Node DataItem named: " << dref->fullName(true));
      }
    }
    delete refItem;
  }

  // read varnames from itemLabel and create a itemLabelList (std::vector<std::string>)
  DataReference *refItemLabel = DataPoolIntens::Instance().getDataReference( vnItemLabel );
  if ( refItemLabel != 0 ) {
    DataVector vector;
    refItemLabel->getDataVector( vector, 1, id );
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      if ((*it)->getValue(s))
	itemLabelList.push_back( s);
    }
    delete refItemLabel;
  }

  // read varnames from itemIgnore and create a itemIgnoreList (std::vector<std::string>)
  DataReference *refItemIgnore = DataPoolIntens::Instance().getDataReference( vnIgnoreLabel );
  if ( refItemIgnore != 0 ) {
    DataVector vector;
    refItemIgnore->getDataVector( vector, 1, id );
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      if ((*it)->getValue(s))
	m_itemIgnoreList.push_back( s);
    }
    delete refItemIgnore;
  }

  // read varnames from header and create a m_header (std::vector<std::string>)
  m_header.clear();
  DataReference *refHeader = DataPoolIntens::Instance().getDataReference( vnHeader );
  if ( refHeader != 0 ) {
    DataVector vector;
    refHeader->getDataVector( vector, 1, id );
    for (DataVector::iterator it = vector.begin();it != vector.end(); ++it) {
      if ((*it)->getValue(s))
	m_header.push_back( s);
    }
    if (m_header.size() == 0) {
      // use as default names of Cycles
      DataPoolIntens& dpi = DataPoolIntens::Instance();
      for (int c=0; c < dpi.numCycles(); ++c) {
	if (dpi.getCycleName(c,s))
	  m_header.push_back( s );
      }

    }
    delete refHeader;
  }

  // call main create
  if (xferNodes.size() || m_xferScalarNodes.size()) {
    build(xferNodes, itemLabelList);
  } else {
    std::string msg("No different data found");
    GuiFactory::Instance()->getLogWindow()->writeText( compose("%1 : %2\n", DateAndTime(), msg));
    GuiFactory::Instance()->showDialogWarning( 0, _("Information"), msg, 0 );
  }
}

/* --------------------------------------------------------------------------- */
/* setHeaders --                                                               */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::setHeaders(const std::vector<std::string>& columnLabels) {
  m_header = columnLabels;
}

/* --------------------------------------------------------------------------- */
/* exec --                                                                     */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::exec() {
  BUG_DEBUG("QtDialogCompare::exec");

  // je nach dialog expandPolicy wird evtl. die maximale Höhe oder Breite gesetzt
  QSize hs =  m_dialog->getDialogWidget()->sizeHint();
#if QT_VERSION >= 0x060000
  QRect maxDesktop= QGuiApplication::primaryScreen()->availableGeometry();
#else
  QRect maxDesktop= QApplication::desktop()->availableGeometry();
#endif
  GuiElement::Orientation dialog_exand_policy =  m_dialog->getDialogExpandPolicy();
  if (!(dialog_exand_policy & orient_Horizontal)) {
    BUG_DEBUG("Set Maximum Dialog Width: " << hs.width());
    m_dialog->getDialogWidget()->setMaximumWidth(hs.width());
  }
  if (!(dialog_exand_policy & orient_Vertical)) {
    BUG_DEBUG("Set Maximum Dialog Height: " << hs.height());
    m_dialog->getDialogWidget()->setMaximumHeight(hs.height());
  }

  // minimale Grösse setzen
  m_dialog->getDialogWidget()->setMinimumHeight(maxDesktop.height() < hs.height() ? maxDesktop.height() : hs.height());
  m_dialog->getDialogWidget()->setMinimumWidth(maxDesktop.width() < hs.width() ? maxDesktop.width() : hs.width());


  m_dialog->setShown( true ); // gui update reason
  m_dialog->getDialogWidget()->showNormal();
  m_dialog->getElement()->update(GuiElement::reason_Always);
  m_dialog->getDialogWidget()->exec();
}

/* --------------------------------------------------------------------------- */
/* addStructItem --                                                            */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::addStructItem(XferDataItem* xferNode, int index, const ItemData& itemInfo) {
  BUG_DEBUG("QtDialogCompare::addStructItem nodeName: " << xferNode->getName()
		 << " index: " <<index);

  XferDataItem* xferNodeNew = new XferDataItem( *xferNode );
  // neuer index wert wird gesetzt
  if (index >= 0) {
    int lastIndexNum = xferNode->getFirstIndexNum( xferNode->getNumberOfLevels()-1 );
    xferNodeNew->setIndex(lastIndexNum, index );
  }

  // alle struct member hinzufügen
  DataReference::StructIterator si;
  for(si =xferNodeNew->Data()->begin(); si != xferNodeNew->Data()->end(); ++si) {

    DataReference *refItem = si.NewDataReference();
    const DataItem* ditem= dynamic_cast<const DataItem*>(refItem->getItem());
    if ( !ditem || ditem && !ditem->isAttributeSet( DATAisDifferent ) ) {
      BUG_INFO( compose("struct item '%1' has no different data", refItem->fullName(true)) );
      // nur DATAisDifferent interessieren uns => continue
      delete refItem;
      continue;
    }
    XferDataItem *xfer = new XferDataItem( refItem );
    addNode(xfer, itemInfo);
  }
}

/* --------------------------------------------------------------------------- */
/* addStructNode --                                                            */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::addStructNode(XferDataItem* xferNode, const ItemData& itemInfo) {
  BUG_DEBUG( "QtDialogCompare::addStructNode  nodeName: " << xferNode->getName());

  // ist eine Struct DataReference
  if (xferNode->getDataType() == DataDictionary::type_StructVariable) {
    DataContainer::SizeList dims;
    int numDims = xferNode->getAllDataDimensionSize(dims);
    int lastIndexNum = xferNode->getFirstIndexNum( xferNode->getNumberOfLevels()-1 );
    // finden ein index auf dem letzten Level und haben die Dimension
    if (numDims == 1 && lastIndexNum != -1) {
      for (int i=0; i < dims[0]; ++i) {
	std::ostringstream os;
	os << "["<<i<<"]";
	ItemData ii(itemInfo, xferNode);
	ii.branch.push_back( os.str() );
	addStructItem(xferNode, i, ii); //itemInfo);
      }
    } else if (numDims==1) {
      // skalar struct und kein index
      addStructItem(xferNode, -1, itemInfo);
    }
  }
}

/* --------------------------------------------------------------------------- */
/* addNode --                                                                  */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::addNode(XferDataItem* xferNode, const ItemData& itemInfo, bool top) {
  BUG_DEBUG( "QtDialogCompare::addNode nodeName: " << xferNode->getName());

  // inside ignore list
  if (isInsideIgnoreList(xferNode)) return;

  int numDims = xferNode->getNumDimensions();
  if (numDims==0) {
    BUG_DEBUG( "RETURN no data");
    // return;
  }

  // struct node?
  if (xferNode->getDataType() == DataDictionary::type_StructVariable) {
    const ItemData& itemInfo2 = itemInfo;
    ItemData test(itemInfo, top ? "" : xferNode->getName());
    addStructNode(xferNode, ItemData(itemInfo, top ? "" : xferNode->getName()));
    return;
  }

  // create all indexes on last level
  createAllDataItemIndexesOnLastLevel(xferNode);

  // simple node
  DataContainer::SizeList dims;
  numDims = xferNode->getAllDataDimensionSize(dims);
  BUG_DEBUG( "numDims["<<numDims<<"]  VN["<<xferNode->getFullName(true)<<"]");
  // Dimensionen mit Grösse 1 werden ignoriert
  while (numDims > 1) {
    if (dims[numDims-1] == 1) {
      BUG_WARN("!!! decrease dimension size !!!");
      --numDims;
    } else
      break;
  }

  // set wildcards XferDataItemIndex on last level
  int numIdx = xferNode->getNumIndexes();
  int to = numDims == 1 ? numDims : numDims-1;
  for (int ii=0; ii<to; ++ii) {
    BUG_DEBUG("setWildcard idxNum: "<< ii << " level: " << xferNode->getNumberOfLevels());
    XferDataItemIndex *index = xferNode->getDataItemIndex( numDims-1 - ii );
    index->setWildcard();
    index->setLowerbound(0);
  }
  xferNode->setDimensionIndizes();

  switch(numDims) {
  case 1:
    if (m_maxScalarCols < xferNode->getSize())
      m_maxScalarCols = xferNode->getSize();
    BUG_DEBUG("add Scalar Node: "<<xferNode->getFullName(true));
    m_xferScalarNodes.push_back( ItemData(itemInfo, new XferDataItem(*xferNode)) );
    break;
  case 2:
    {
    BUG_DEBUG("add Vector Node: "<<xferNode->getFullName(true));
    m_xferVectorNodes.push_back( ItemData(itemInfo, new XferDataItem(*xferNode)) );
    }
    break;
  case 3:
    BUG_DEBUG("add Matrix Node: "<<xferNode->getFullName(true));
    m_xferMatrixNodes.push_back( ItemData(itemInfo, new XferDataItem(*xferNode)) );
    break;
  default:
    BUG_DEBUG("not implemented, item named '" << xferNode->getFullName(true)
	      << "' has dimension '"<< xferNode->getNumDimensions() <<"'");
    m_xferOthersNodes.push_back( ItemData(itemInfo, new XferDataItem(*xferNode)) );
    break;
  }
}

/* --------------------------------------------------------------------------- */
/* build --                                                                    */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::build(std::vector<XferDataItem*>& xferNodes, const std::vector<std::string>& nodeLabel) {
  BUG_DEBUG( "QtDialogCompare::build cntRootNodes: " << xferNodes.size());
  if (m_dialog)
    destroy();

  // Dialog
  m_dialog = dynamic_cast<GuiQtForm*>( GuiFactory::Instance()->createForm("") );
  std::string s( "CompareDialog" );
  m_dialog->setTitle( s );
  m_dialog->resetCycleButton();
  m_dialog->hasCloseButton( false );
  //  m_dialog->getElement()->getDialog()->setApplicationModal();

  // Statistik
  m_maxScalarCols = 0;
  std::vector<XferDataItem*>::const_iterator it = xferNodes.begin();
  std::vector<std::string>::const_iterator lit = nodeLabel.begin();
  for (; it != xferNodes.end(); ++it) {
    BUG_DEBUG("rootNode: " << (*it)->getFullName(true) << "   numWildcards: " << (*it)->getNumOfWildcards()
	      << "  lastLevelHasIndices: " << (*it)->lastLevelHasIndices());
    ItemData id( lit != nodeLabel.end() ? *lit : "", 0);
    addNode((*it), id, true);
    if (lit != nodeLabel.end()) ++lit;
  }
  BUG_DEBUG("Counts Scalar: " << m_xferScalarNodes.size() <<
	    "  Vector: " << m_xferVectorNodes.size() <<
	    "  Matrix: " << m_xferMatrixNodes.size() <<
	    "  Others: " << m_xferOthersNodes.size() );

  // HauptFolder
  GuiQtFolder* folder = dynamic_cast<GuiQtFolder*>( GuiFactory::Instance()->createFolder( m_dialog, "" ) );
  folder->setTabExpandable( true );

  //
  // SKALARs
  ItemDataVector subNodes;
  std::string label;
  int start = 0;
  if (m_xferScalarNodes.size()) {
    // Skalar Folder
    m_scalarTab = folder->addFolderPage( "Scalar", "" );
    GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer( m_scalarTab->getElement() );
    oc->getElement()->setPanedWindow( status_OFF );
    m_scalarTab->attach( oc->getElement() );

    start = 0;
#if 1  // alles zusammengefasst
      GuiElement* elem = createScalarTable(m_xferScalarNodes, label);
      oc->attach( elem );
      elem = createScalarPlot(m_xferScalarNodes, label);
      if (elem)
	oc->attach( elem );
#else // getrennt in einzelne fieldgroups
    while (extractSubNodes(m_xferScalarNodes, &start, subNodes, label) ) {
      // GuiFieldgroup* fg = createScalarFieldGroup(subNodes, label);
      // oc->attach( fg->getElement() );
      GuiElement* elem = createScalarTable(subNodes, label);
      oc->attach( elem );
      elem = createScalarPlot(subNodes, label);
      if (elem)
	oc->attach( elem );
    }

    // add Stretch
    GuiStretch *v = new GuiStretch( 10 );
    oc->attach( v );
#endif
  }

  //
  // VEKTOR
  if (m_xferVectorNodes.size()) {
    // Vektor Folder
    m_vectorTab = folder->addFolderPage( "Vector", "" );
    GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer( m_vectorTab->getElement() );
    oc->getElement()->setPanedWindow( status_OFF );
    m_vectorTab->attach( oc->getElement() );

    int start = 0;
    while (extractSubNodes(m_xferVectorNodes, &start, subNodes, label) ) {
      GuiElement* ge = createVectorTable(subNodes, label);
      oc->attach( ge );
      ge= createVectorPlot(subNodes, label);
      if (ge)
	oc->attach( ge );
    }

    // add Stretch
    // GuiStretch *v = new GuiStretch( 10 );
    // oc->attach( v );
  }

  //
  // MATRIX
  if (m_xferMatrixNodes.size()) {
    // Matrix Folder
    m_matrixTab = folder->addFolderPage( "Matrix", "" );
#if 1
    GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer( m_matrixTab->getElement() );
    // oc->getElement()->setPanedWindow( status_ON );
    m_matrixTab->attach( oc->getElement() );

    int start = 0;
    while (extractSubNodes(m_xferMatrixNodes, &start, subNodes, label) ) {
      GuiElement* ge = createMatrixTab(subNodes, label);
      oc->attach( ge );
    }
#else
    createMatrixTab(m_xferMatrixNodes);
#endif

    // add Stretch
    // GuiStretch *v = new GuiStretch( 10 );
    // oc->attach( v );
  }

  // Others / Fehler
  if (m_xferOthersNodes.size() ||
      !m_xferScalarNodes.size() && !m_xferVectorNodes.size() && !m_xferMatrixNodes.size() ) {
    m_othersTab = folder->addFolderPage( "Others", "" );
    createOthersTab(m_xferOthersNodes, "Leer");
  }
  m_dialog->attach( folder-> getElement() );

  // Buttonbar generieren
  GuiButtonbar* bar = GuiFactory::Instance()->createButtonbar( m_dialog );

  // Close Button generieren und an das Buttonbar anfuegen
  CloseButtonListener* closeButtonListener = new CloseButtonListener( m_dialog );
  GuiButton* button = GuiFactory::Instance() -> createButton( bar->getElement(),  closeButtonListener );
  button->setLabel( _("Close") );

  m_dialog->create();
  m_dialog->update(GuiElement::reason_Always);
}

/* --------------------------------------------------------------------------- */
/* createScalarFieldGroup --                                                          */
/* --------------------------------------------------------------------------- */

GuiFieldgroup* QtDialogCompare::createScalarFieldGroup(const ItemDataVector& xferNodes, const std::string& title) {

  // Fieldgroup generieren
  GuiFieldgroup* fieldgroup = GuiFactory::Instance() -> createFieldgroup( m_dialog, "");
  fieldgroup->setTitle(title);
  fieldgroup->getElement()->setFrame( GuiElement::status_ON );
  fieldgroup->setMargins( 5, 10);
  assert( fieldgroup != 0 );
  fieldgroup->setTitleAlignment( GuiElement::align_Center );

  // arrowbar konfigurieren
  const int MAX_COLUMNS = 10;
  if (m_maxScalarCols < MAX_COLUMNS) {
    // wir machen die Spaltenüberschrift selbst
    GuiFieldgroupLine *line = fieldgroup->addFieldgroupLine();
    for (int i=0; i <=m_maxScalarCols; ++i) {
      GuiLabel *label = GuiFactory::Instance() -> createLabel( line->getElement()
							       , GuiElement::align_Default );
      if (i) {
	std::string tStr = i-1 < m_header.size() ? m_header[i-1] : compose("Cycle2 %1", i);
	label->setTitle( tStr );
      } else
	label->setTitle( "" );
    }
  } else
    // max index range setzen
    fieldgroup->setTableIndexRange( 0, m_maxScalarCols-1 );

  // lässt den Index verschwinden
  fieldgroup->setTableStep( 0 );
  fieldgroup->setTableSize( std::min(m_maxScalarCols, MAX_COLUMNS) );

  // nun werden für jedes XferDataItem eine Line erzeugt
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    // only scalar results
    GuiFieldgroupLine *_line = fieldgroup->addFieldgroupLine();
    assert( _line != 0 );
    GuiElement *line = _line->getElement();

    GuiDataField* dataField = GuiFactory::Instance() -> createDataField( line, it->xfer);
    if (!dataField) continue;

    GuiLabel *label = GuiFactory::Instance() -> createLabel( line
							     , GuiElement::align_Default );
    label->setTitle( (it->label.size() ? (it->Label() +".") : "") + dataField->Attr()->Label(it->xfer->Data())
		     + (dataField->Attr()->Unit().size() ? compose(" %1", dataField->Attr()->Unit()) : "") );
    line->attach( dataField->getElement() );
  }

  // disable all
  //  fieldgroup->getElement()->disable();
  return fieldgroup;
}

/* --------------------------------------------------------------------------- */
/* createScalarTable --                                                        */
/* --------------------------------------------------------------------------- */

GuiElement* QtDialogCompare::createScalarTable(const ItemDataVector& xferNodes, const std::string& title) {

#if 1
  GuiQtTable* list = dynamic_cast<GuiQtTable*>( GuiFactory::Instance()->createTable( m_dialog, "") );
  list->getTable()->setTitle( title, GuiElement::align_Left );
  //  list->setTableOrientation( GuiElement::orient_Vertical );
#else
  GuiQtList* list = GuiFactory::Instance()->createList( m_dialog, "");
  list->setTitle( title, GuiElement::align_Center  );
#endif
  list->disable();

  // fix size
  GuiTableSize* tablesize = list->newTableSize();
  tablesize->setRange( 1, m_header.size() );
  tablesize->setSize( m_header.size() );
  BUG_DEBUG("TABLESIZE: "<<m_header.size());
  list->setTableColSize( tablesize );

  // loop
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    XferDataItem* newItem = new XferDataItem( *(*it).xfer );
    std::string s ( (it->label.size() ? (it->Label() +".") : "") + newItem->userAttr().Label(it->xfer->Data())
		    + (newItem->userAttr().Unit().size() ? compose(" %1", newItem->userAttr().Unit()) : "") );
    s = QString::fromStdString(s).remove(QRegularExpression("<[^>]+>")).toStdString(); // remove html tags
    BUG_DEBUG("createScalarTable: VN["<<newItem->getFullName(true)<<"] title["<<s<<"]");
#if 1
    GuiTableLine* tableline =list->addTableLine( s, GuiElement::align_Default );
    GuiTableItem* tableitem=
      tableline->addTableDataItem(newItem);
    double sf =  getScaleFactor(  newItem->userAttr().Unit() );
    dynamic_cast<GuiQtTableDataItem*>(tableitem)->setScalefactor( new Scale( sf, '*' ) );
    dynamic_cast<GuiQtTableDataItem*>(tableitem)->getDataField()->getElement()->disable();
#else
    list->addColumn(s, newItem, GuiElement::align_Default, 20, -1, 0, false, false);
#endif
  }

  return list;
}

/* --------------------------------------------------------------------------- */
/* getScaleFactor --                                                           */
/* --------------------------------------------------------------------------- */

double QtDialogCompare::getScaleFactor(const std::string& units) {
  std::string unit( units );
  trim(unit, "[]");

  if (unit == "mm") {
    return 1e3;
  }
  if (unit == "%" || unit == "cm") {
    return 1e2;
  }
  if (unit == "km/h") {
    return 3.6;
  }
  if (unit == "1/(km/h)") {
    return 1./3.6;
  }
  if (unit == "1/(km/h)²") {
    return 1./3.6/3.6;
  }
  if (unit == "kN/(km/h)") {
    return 1e-3/3.6;
  }
  if (unit == "kN/(km/h)²") {
    return 1e-3/3.6/3.6;
  }
  if (unit == "kN" || unit == "kN/m²" || unit == "kV" || unit == "kW" || unit == "km" || unit == "ton" ) {
    return  1e-3;
  }
  if (unit == "mH" || unit == "mΩ"  || unit == "Ω/km"  || unit == "‰") {
    return 1e3;
  }
  if (unit == "T" || unit == "V" || unit == "‰t" || unit == "deg" || unit == "W"  || unit == "Nm" || unit == "kg" ) {
    return 1.;
  }
  if (unit == "1/min") {
    return 60.;
  }
  if (unit == "t/(km/h)") {
    return 1e-3/3.6;
  }
  if (unit == "t/(km/h)²") {
    return 1e-3/3.6/3.6;
  }
  if (unit != "") {
    std::cerr << "unknown unit: ["<<unit<<"]\n";
  }

  return 1;
}

/* --------------------------------------------------------------------------- */
/* createScalarPlot --                                                         */
/* --------------------------------------------------------------------------- */

GuiElement* QtDialogCompare::createScalarPlot(const ItemDataVector& xferNodes, const std::string& title) {
  Gui2dPlot* plot2d = GuiFactory::Instance()->create2dPlot("", true);
  plot2d->setFooterText( title );
  plot2d->setYPlotStyle( Gui2dPlot::BAR, Gui2dPlot::Y1AXIS );
  dynamic_cast<GuiQWTPlot*>(plot2d)->setBarChartOrientation(Qt::Horizontal);
  plot2d->setBarStyleOption_PlotItemGrouped();
  plot2d->withAnnotationOption( true );
  plot2d->showAnnotationLabels( true );
  plot2d->setLabel( _("Percent [%]"),  Gui2dPlot::XAXIS );

  // annotation : get data
  std::string vnXAnnotationAxis = compose("%1.%2", StructVarname, XAnnotationAxisVarname);
  std::string vnXAnnotationVal = compose("%1.%2", StructVarname, XAnnotationValueVarname);
  std::string vnXAnnotationLbl = compose("%1.%2", StructVarname, XAnnotationLabelVarname);
  DataReference *refXAnnoAxis = DataPoolIntens::Instance().getDataReference( vnXAnnotationAxis );
  DataReference *refXAnnoVal =  DataPoolIntens::Instance().getDataReference( vnXAnnotationVal );
  DataReference *refXAnnoLbl =  DataPoolIntens::Instance().getDataReference( vnXAnnotationLbl );
  XferDataItem* xAxisXfer = new XferDataItem( refXAnnoAxis );
  XferDataItem* xAxisValueXfer = new XferDataItem( refXAnnoVal );
  XferDataItem* xAxisLabelXfer = new XferDataItem( refXAnnoLbl );
  DataVector vectorAnnoAxis, vectorAnnoLbl, vectorAnnoVal;
  int id[1] = { -1  };
  refXAnnoAxis->getDataVector( vectorAnnoAxis, 1, id );
  refXAnnoLbl->getDataVector( vectorAnnoLbl, 1, id );
  refXAnnoVal->getDataVector( vectorAnnoVal, 1, id );
  vectorAnnoAxis.clearVector();
  vectorAnnoLbl.clearVector();
  vectorAnnoVal.clearVector();

  // add a dummy x item ( addDummyItem() )
  GuiPlotDataItem* xplotaxis;
  Gui2dPlot::PlotItem* plotitem = 0;
  XferDataItem* newItem = 0;

  if( (xplotaxis = new GuiPlotDataItem( xAxisXfer, 1) ) == 0 )
    std::cout << _("XAxis Undeclared identifier.\n");
  if( xAxisLabelXfer->getNumOfWildcards() > 1 ){
    std::cout << _("Too many WILDCARDS.\n");
  }
  Gui2dPlot::PlotItem* xplotitem = plot2d->addDataItem(xplotaxis, 0 );
  xplotitem->setAxisType( Gui2dPlot::XAXIS );
  xplotaxis->setScale(  new Scale( 1., '*' ) );
  if( ( plotitem = plot2d->addDataItem( xplotaxis,  xplotitem ) ) == 0 )
    std::cout << _("Undeclared identifier.\n");
  xplotitem = plotitem;
  xplotaxis->setAnnotationValues( xAxisValueXfer, new Scale( 1., '*' ) );
  xplotaxis->setAnnotationLabels( xAxisLabelXfer );
  //  xplotaxis->setAnnotationAngle( -45 );

  // loop, create y items
  double relMin = 100.;
  int maxDim = 0;
  ItemDataVector::const_reverse_iterator it = xferNodes.rbegin();
  for (; it != xferNodes.rend(); ++it) {

    // only real  or integer can be processed
    if ( (*it).xfer->getDataType() != DataDictionary::type_Real &&
	 (*it).xfer->getDataType() != DataDictionary::type_Integer ) continue;

    newItem = new XferDataItem( *(*it).xfer );
    std::string s ( (it->label.size() ? (it->Label() +".") : "") + newItem->userAttr().Label(it->xfer->Data()) );
    s = QtMultiFontString::getQString( s ).toStdString();

    // create plotitem
    GuiPlotDataItem* plotaxis;
    if( (plotaxis = new GuiPlotDataItem( newItem,1 )) == 0 )
      std::cout << _("Undeclared identifier.\n") ;

    if( newItem->getNumOfWildcards() > 1+1 ){ // maybe + + if wildcardIndex
      std::cout << _("Too many WILDCARDS.\n");
    }
    if( ( plotitem = plot2d->addDataItem( plotaxis, xplotitem ) ) == 0 )
      std::cout << _("Undeclared identifier.\n");

    // get max data dimension
    DataContainer::SizeList dims;
    int nd = newItem->getAllDataDimensionSize(dims);
    if (nd)
      maxDim = std::max(maxDim, dims[0] );

    // get min/max
    double min, max, percentFactor;
    bool diff = getDataVectorCharacteristic(newItem, min, max, percentFactor);
    if (max < 0.)
      percentFactor *= -1;
    relMin = std::min(relMin, min*percentFactor);
    BUG_DEBUG("createScalarPlot VN["<<newItem->getFullName(true)<<"] Min["<<min<<"] Max["<<max<<"] pf["<<percentFactor
	      <<"] Different["<<diff<<"] numIdx["<<newItem->getNumIndexes()<<"]  relativeMin["<<relMin<<"]");
    plotitem->setAxisType( Gui2dPlot::Y1AXIS );
    plotaxis->setLabel( s );
    plotaxis->setLabels( m_header );
    plotaxis->setScale( new Scale( percentFactor, '*' ) );
    GuiIndex::registerIndexContainer( plot2d->getElement() );

    // annotation: collect value
    DataValue::Ptr ptr = new DataStringValue( s );
    vectorAnnoLbl.appendValue( ptr );
    ptr = new DataRealValue( (it-xferNodes.rbegin()) );
    vectorAnnoVal.appendValue( ptr );
  }
  plot2d->setPlotAxisScale( 10 * floor(relMin/10), 100., Gui2dPlot::XAXIS );

  // annotation: setDataVector
  refXAnnoLbl->setDataVector( vectorAnnoLbl, 1, id );
  refXAnnoVal->setDataVector( vectorAnnoVal, 1, id );
  for (int i=0; i < maxDim; ++i) {
    DataValue::Ptr ptr = new DataRealValue( i );
    vectorAnnoAxis.appendValue( ptr );
  }
  refXAnnoAxis->setDataVector( vectorAnnoAxis, 1, id );

  if (newItem == 0) { // no displaying data
    delete plot2d;
    return 0;
  } else {
    m_plot2dVector.push_back(plot2d);
    return plot2d->getElement();
  }
}

/* --------------------------------------------------------------------------- */
/* createVectorTable --                                                         */
/* --------------------------------------------------------------------------- */

GuiElement* QtDialogCompare::createVectorTable(const ItemDataVector& xferNodes, const std::string& title) {
  GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer(  m_vectorTab->getElement() );
  GuiQtFolder* folder = dynamic_cast<GuiQtFolder*>( GuiFactory::Instance()->createFolder(oc->getElement(), "" ));
  folder->setTabExpandable( true );
  oc->getElement()->setFrame( GuiElement::status_ON );
  oc->getElement()->setTitle( title );
  oc->attach( folder );

  // loop
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    GuiOrientationContainer* vectorChildTab = folder->addFolderPage( (*it).xfer->getName(), "" );
#if 1
    GuiTable* list = GuiFactory::Instance()->createTable( vectorChildTab->getElement(), "");
#else
    GuiList* list = GuiFactory::Instance()->createList( vectorChildTab->getElement(), "");
#endif
    list->setTitle( (it->label.size() ? (it->Label() +".") : "") + (*it).xfer->getName(), GuiElement::align_Center  );

    DataContainer::SizeList dims;
    int numdims = (*it).xfer->getAllDataDimensionSize(dims);
    for( int i = 0; i <  dims[0]; ++i ){
       XferDataItem* newItem = new XferDataItem( *(*it).xfer );
       newItem->setIndex( newItem->getNumIndexes()-2, i ); // vorletzte index auf loopIdx i

       BUG_DEBUG("createVectorTable newItem: " << newItem->getFullName(true));
       std::string tStr = i < m_header.size() ? m_header[i] : compose("Cycle %1", i);
#if 1
       GuiTableLine* tableline =list->addTableLine( tStr, GuiElement::align_Default );
       tableline->addTableDataItem(newItem);
#else
       list->addColumn( compose(tStr, (i+1)), newItem, GuiElement::align_Default, 20, -1, 0, false, false);
#endif
    }
    vectorChildTab->attach( list -> getElement() );
  }
  return oc-> getElement(); //folder-> getElement();
}

/* --------------------------------------------------------------------------- */
/* createVectorPlot --                                                         */
/* --------------------------------------------------------------------------- */

GuiElement* QtDialogCompare::createVectorPlot(const ItemDataVector& xferNodes, const std::string& title) {
  GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer(  m_vectorTab->getElement() );
  GuiQtFolder* folder = dynamic_cast<GuiQtFolder*>( GuiFactory::Instance()->createFolder(oc->getElement(), "" ));
  folder->setTabExpandable( true );
  oc->getElement()->setFrame( GuiElement::status_ON );
  oc->getElement()->setTitle( title );
  oc->attach( folder );

  // loop
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    GuiOrientationContainer* vectorChildTab = folder->addFolderPage( (*it).xfer->getName(), "" );

    // Plot erzeugen
    Gui2dPlot* plot2d = GuiFactory::Instance()->create2dPlot("", true);
    plot2d->setYPlotStyle( Gui2dPlot::PLOT, Gui2dPlot::Y1AXIS );
    plot2d->setLabel( title,  Gui2dPlot::Y1AXIS);

    // add a dummy x item ( addDummyItem() )
    GuiPlotDataItem* xplotaxis;
    Gui2dPlot::PlotItem* plotitem = 0;
    XferDataItem* newItem = 0;

    if( (xplotaxis = new GuiPlotDataItem()) == 0 )
      std::cout << _("XAxis Undeclared identifier.\n");
    Gui2dPlot::PlotItem* xplotitem = plot2d->addDataItem(0, 0 );
    xplotitem->setAxisType( Gui2dPlot::XAXIS );

    // loop cycle
    DataContainer::SizeList dims;
    int numdims = (*it).xfer->getAllDataDimensionSize(dims);
    for( int i = 0; i <  dims[0]; ++i ){
       XferDataItem* newItem = new XferDataItem( *(*it).xfer );
       newItem->setIndex( newItem->getNumIndexes()-2, i ); // vorletzte index auf i

       // create plotitem
       GuiPlotDataItem* plotaxis;
       if( (plotaxis = new GuiPlotDataItem( newItem,1 )) == 0 )
	 std::cout << _("Undeclared identifier.\n") ;

       if( newItem->getNumOfWildcards() > 1+1 ){ // maybe + + if wildcardIndex
	 std::cout << _("Too many WILDCARDS.\n");
       }
       if( ( plotitem = plot2d->addDataItem( plotaxis, 0/*xplotitem*/ ) ) == 0 )
	 std::cout << _("Undeclared identifier.\n");

       std::string tStr = i < m_header.size() ? m_header[i] : compose("Cycle %1", i);
       plotitem->setAxisType( Gui2dPlot::Y1AXIS );
       plotaxis->setLabel( tStr );
       plotaxis->setScale( new Scale( 1., '*' ) );
       GuiIndex::registerIndexContainer( plot2d->getElement() );
    }
    vectorChildTab->attach(plot2d -> getElement() );
  }
  return oc-> getElement();
}

/* --------------------------------------------------------------------------- */
/* createMatrixTab --                                                          */
/* --------------------------------------------------------------------------- */

GuiElement* QtDialogCompare::createMatrixTab(const ItemDataVector& xferNodes, const std::string& title) {
  GuiOrientationContainer* oc = GuiFactory::Instance()->createVerticalContainer(  m_matrixTab->getElement() );
  GuiQtFolder* folder = dynamic_cast<GuiQtFolder*>( GuiFactory::Instance()->createFolder(oc->getElement(), "" ) );
  folder->setTabExpandable( true );
  oc->getElement()->setFrame( GuiElement::status_ON );
  oc->getElement()->setTitle( title );
  oc->attach( folder );

  // loop
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    DataContainer::SizeList dims;
    (*it).xfer->getAllDataDimensionSize(dims);

    int numIdx = (*it).xfer->getNumIndexes();
    for( int i = 0; i <  dims[0]; ++i ){

      // GuiOrientationContainer
      std::string tStr = i < m_header.size() ? m_header[i] : compose("Cycle %1", i);
      GuiOrientationContainer* matrixChildTab = folder->addFolderPage( compose("%1_%2",(*it).xfer->getName(), tStr), "" );

      // GuiTable Matrix Type
      GuiTable* table = GuiFactory::Instance()->createTable( matrixChildTab->getElement(), "");
      table->setTitle(  (*it).xfer->getName(), GuiElement::align_Left );
      table->setMatrix();

      XferDataItem* newItem = new XferDataItem( *(*it).xfer );
      newItem->setIndex( newItem->getNumIndexes()-3, i ); // vorvorletzte index auf loopIdx i

      GuiTableLine* tableline =table->addTableLine( "", GuiElement::align_Default );
      BUG_DEBUG("createMatrixTab newItem: " << newItem->getFullName(true));
      tableline->addTableDataItem(newItem);

      matrixChildTab->attach( table -> getElement() );
    }
  }
  return oc-> getElement();
}

/* --------------------------------------------------------------------------- */
/* createOthersTab --                                                          */
/* --------------------------------------------------------------------------- */

void QtDialogCompare::createOthersTab(const ItemDataVector& xferNodes, const std::string& title) {
  // Fieldgroup generieren
  static int FG_COUNTER = 0;
  std::ostringstream os;
  os << "createOthersTab";
  os << "_"<<++FG_COUNTER;
  GuiFieldgroup* fieldgroup = GuiFactory::Instance() -> createFieldgroup( m_dialog->getElement(), os.str());
  fieldgroup->setMargins( 5, 10);
  assert( fieldgroup != 0 );
  m_othersTab->attach( fieldgroup -> getElement() );
  fieldgroup->setTitleAlignment( GuiElement::align_Center );
  fieldgroup->setTitle( xferNodes.size() == 0 ? _("No items selected.") :
			_("Cannot handle items with more than 3 dimensions (and with no dimensions).") );
  // loop
  ItemDataVector::const_iterator it = xferNodes.begin();
  for (; it != xferNodes.end(); ++it) {

    // only scalar results
    GuiFieldgroupLine *_line = fieldgroup->addFieldgroupLine();
    assert( _line != 0 );
    GuiElement *line = _line->getElement();

    GuiLabel *label = GuiFactory::Instance() -> createLabel( line
							     , GuiElement::align_Default );
    label->setTitle( (it->label.size() ? (it->Label() +".") : "") + (*it).xfer->userAttr().Label((*it).xfer->Data())
		     + ((*it).xfer->userAttr().Unit().size() ? compose(" %1", (*it).xfer->userAttr().Unit()) : "") );
  }
}

/* --------------------------------------------------------------------------- */
/* update --                                                                   */
/* --------------------------------------------------------------------------- */
void QtDialogCompare::update(GuiElement::UpdateReason reason) {
  if( m_dialog != 0 && myWidget()->isVisible() ){
    BUG_DEBUG("QtDialogCompare::update  reason: " << reason);
    if (m_scalarTab)
      m_scalarTab->getElement()->update( reason );
    if (m_vectorTab)
      m_vectorTab->getElement()->update( reason );
    if (m_matrixTab)
      m_matrixTab->getElement()->update( reason );
    if (m_othersTab)
      m_othersTab->getElement()->update( reason );
    BUG_DEBUG("QtDialogCompare::update done");
  }
}

/* --------------------------------------------------------------------------- */
/* extractSubNodes --                                                          */
/* --------------------------------------------------------------------------- */

bool QtDialogCompare::extractSubNodes(ItemDataVector& allNodes, int* start,
					 ItemDataVector& subNodes, std::string& label) {
  subNodes.clear();
  label.clear();

  QtDialogCompare::ItemDataVector::iterator it = allNodes.begin() + *start;
  while (it != allNodes.end()) {
      if (label.size() == 0) {
	// neue SuchParameter
	label = (*it).label;
      }
      while (it != allNodes.end() && (*it).label == label) {
	subNodes.push_back( *it );
	++it;
      }
      if (subNodes.size()) {
	*start += subNodes.size();
	return true;
      }
    }


  return false;
}

/* --------------------------------------------------------------------------- */
/* isInsideIgnoreList --                                                       */
/* --------------------------------------------------------------------------- */
bool QtDialogCompare::isInsideIgnoreList(XferDataItem* node) {

  std::vector<std::string>::iterator it = m_itemIgnoreList.begin();
  std::string::size_type posA = 0;
  for (;it != m_itemIgnoreList.end(); ++it){
    posA = node->getFullName(false).rfind( *it );
    if (posA == node->getFullName(false).size() - (*it).size()) {
      BUG_DEBUG("is inside isInsideIgnoreList, Node " << node->getFullName(false));
      return true;
    }
  }
  return false;
}
