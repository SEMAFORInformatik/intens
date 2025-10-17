//#ifdef _WIN32 // nur auf Windows Platform

#ifdef HAVE_QT
#include <qfont.h>
#endif

#include "app/IntensInterface.h"
#include "app/DataPoolIntens.h"
#include "app/UserAttr.h"
#include "datapool/DataReference.h"
#include "datapool/DataIntegerValue.h"
// #include "datapool/DataVector.h"
#include "xfer/XferDataItem.h"
#include "xfer/XferDataItemIndex.h"


#include "streamer/Stream.h"
#include "streamer/StreamManager.h"
#include "streamer/StructStreamParameter.h"
#include "operator/MatlabProcess.h"
#include "operator/MatlabInitProcess.h"
#include "utils/Date.h"
#include "utils/gettext.h"


#include "gui/GuiFactory.h"
#include "gui/GuiScrolledText.h"
#ifdef HAVE_QT
#include "gui/qt/QtMultiFontString.h"
#include "gui/qt/QtIconManager.h"
#include "gui/qt/GuiQtManager.h"
#endif

DataReference* IntensInterface_Impl::getNewDataReference(std::string const& name) {
  return DataPoolIntens::getDataReference(name);
}

DataReference* IntensInterface_Impl::getNewDataReference(DataReference const* dref, std::string const& name) {
  return DataPoolIntens::getDataReference(dref, name);
}

DataReference* IntensInterface_Impl::getNewDataReference(DataReference& dref) {
  return DataPool::newDataReference( dref );
}
// DataReference* IntensInterface_Impl::getNewDataReference(Root& root) {
//   DataReference::StructIterator *sit = dynamic_cast<DataReference::StructIterator*>(&root);
//   assert( sit );
//   return sit->NewDataReference();
// }

std::string IntensInterface_Impl::getClassName( DataReference* dref ) {
  if( dref == 0 )
    return "";
  if( dref->getDataType()==DataDictionary::type_StructVariable ){
    UserAttr *attr = dynamic_cast<UserAttr*>(dref->getUserAttr());
    if( attr != 0 ){
      return attr->Classname();
    }
  }
  return "";
}

std::string IntensInterface_Impl::getNodeName( DataReference* dref ) {
  if( dref == 0 )
    return "";
  if( dref->getDataType()==DataDictionary::type_StructVariable ){
    return dref->nodeName();
  }
  return "";
}

int IntensInterface_Impl::getMaxOccurs( DataReference* dref ) {
  if( dref == 0 )
    return -1;
  if( dref->getDataType()==DataDictionary::type_StructVariable ){
    UserAttr *attr = dynamic_cast<UserAttr*>(dref->getUserAttr());
    if( attr != 0 ){
      return attr->MaxOccurs();
    }
  }
  return -1;
}

XferDataItem*  IntensInterface_Impl::getNewXferDataItem( DataReference *dref ) {
  return new XferDataItem( dref );
}

void IntensInterface_Impl::FreeXferDataItem( XferDataItem* xfer ) {
  if ( xfer ) delete xfer;
}

XferDataItemIndex* IntensInterface_Impl::getNewXferDataItemIndex(XferDataItem* xfer, int idx, int lowerbound) {
  XferDataItemIndex *xdii = xfer -> newDataItemIndex(idx);
  if (lowerbound >= 0)
    xdii -> setLowerbound( lowerbound );
  return xdii;
}

int   IntensInterface_Impl::XferDataItemLastSize(XferDataItem* xfer ) {
  if (xfer)
    return xfer->getLastSize();
  return -1;
}
void  IntensInterface_Impl::XferDataItemSetIndexName(XferDataItem* xfer, int level, const char* name ) {
  XferDataItemIndex* xferIndex = xfer->newDataItemIndex(level);
  xferIndex->setIndexName(name, "");

}
void IntensInterface_Impl::XferDataItemSetDimensionIndizes(XferDataItem* xfer) {
  if (xfer)
    xfer->setDimensionIndizes();
}
void IntensInterface_Impl::XferDataItemSetIndex(XferDataItem* xfer, const char* idx, int level) {
  if (xfer)
    xfer->setIndex( idx, level );
}
bool IntensInterface_Impl::XferDataItemGetValue(XferDataItem* xfer, std::string& value ) {
  if (xfer)
    return xfer->getValue( value );
  return false;
}



MatlabProcess* IntensInterface_Impl::getNewMatlabProcess(std::string const &name) {
  return new MatlabProcess( name );
}

void IntensInterface_Impl::FreeMatlabProcess(MatlabProcess*  mp) {
  if (mp) delete mp;
}

int IntensInterface_Impl::MatlabProcessAddInputStream( MatlabProcess*  mp, Stream *s , const std::string &fifo ) {
  return mp == 0 ? Process::O_STREAM_UNDECLARED : mp->addInputStream(s, fifo);
}
int IntensInterface_Impl::MatlabProcessAddOutputStream( MatlabProcess*  mp, Stream *s ) {
  return mp == 0 ? Process::O_STREAM_UNDECLARED : mp->addOutputStream(s);
}

bool IntensInterface_Impl::MatlabProcessStart(MatlabProcess*  mp) {
  return mp == 0 ? false : mp->start();
}
std::string  IntensInterface_Impl::MatlabProcessGetErrorMessage(MatlabProcess*  mp) {
  return mp == 0 ? "" : mp->getErrorMessage();
}
int IntensInterface_Impl::MatlabProcessGetExitStatus(MatlabProcess*  mp) {
  return mp == 0 ? -1 : mp->getExitStatus();
}
bool IntensInterface_Impl::MatlabProcessHasTerminated(MatlabProcess*  mp) {
  return mp == 0 ? false : mp->hasTerminated();
}
void IntensInterface_Impl::InputChannelEventWrite(InputChannelEvent &event, std::ostringstream& os) {
#ifdef HAVE_QT
  event.write( os );
#else
  assert(false);
#endif
}


Stream* IntensInterface_Impl::getNewStream(bool check) {
  return StreamManager::Instance().newStream(check);
}

StreamParameter* IntensInterface_Impl::StreamAddMatrixParameter( Stream* stream, XferDataItem *ditem
				       , int length, int prec, Scale *scale
      , bool thousand_sep, char delimiter, bool mandatory ) {
  return stream == 0 ? NULL : stream->addMatrixParameter(ditem, length, prec, scale, thousand_sep, delimiter, mandatory );
}
StreamParameter* IntensInterface_Impl::StreamAddParameter( Stream* stream, XferDataItem *ditem
						     , int length, int prec, Scale *scale
						     , bool thousand_sep
						     , bool  mandatory
						     , bool isDatasetText ) {
  return stream == 0 ? NULL : stream->addParameter(ditem, length, prec, scale, thousand_sep, mandatory, isDatasetText);
}
void IntensInterface_Impl::StructStreamParameterKeepIndex(StreamParameter* sp) {
  dynamic_cast< StructStreamParameter *>(sp) -> keepIndex();
}



DataSet* IntensInterface_Impl::getDataSet(std::string vn) {
  return DataPoolIntens::Instance().getDataSet( vn );
}
DataSetItemList* IntensInterface_Impl::getDataSetItemList(std::string vn) {
  DataSet *ds = DataPoolIntens::Instance().getDataSet( vn );
  if (ds)
    return &(ds->getDataSetItemList());
  return NULL;
}
int IntensInterface_Impl::DataSetGetInputPosition(DataSet* ds, const std::string &value ) {
  return ds == 0 ? -1 : ds->getInputPosition( value );
}

void IntensInterface_Impl::BeginDataPoolTransaction(TransactionOwner *owner) {
  DataPoolIntens::Instance().BeginDataPoolTransaction( owner );
}

void IntensInterface_Impl::CommitDataPoolTransaction( TransactionOwner *owner) {
  DataPoolIntens::Instance().CommitDataPoolTransaction( owner );
}

void IntensInterface_Impl::RollbackDataPoolTransaction(TransactionOwner *owner) {
  DataPoolIntens::Instance().RollbackDataPoolTransaction( owner );
}

TransactionNumber IntensInterface_Impl::CurrentDataPoolTransactionNumber() {
  return DataPoolIntens::CurrentTransaction();
}

bool IntensInterface_Impl::IsDataItemUpdated( DataReference* dref, TransactionNumber t ){
  return dref == 0 ? false : dref->isDataItemUpdated( DataReference::ValueUpdated, t );
}

bool IntensInterface_Impl::EraseElements( DataReference* dref, int idx ) {
  return dref == 0? true: dref->EraseElements(idx);
}

bool IntensInterface_Impl::EraseAllElements( DataReference* dref ) {
  return dref == 0? true: dref->eraseAllElements();
}

void IntensInterface_Impl::SetDimensionIndizes(DataReference* dref, int NumIndizes, const int * Indizes) {
  dref->SetDimensionIndizes( NumIndizes, Indizes );
}

void IntensInterface_Impl::SetDimensionIndizes(DataReference* dref, int level, int NumIndizes, const int * Indizes) {
  dref->SetDimensionIndizes( level, NumIndizes, Indizes );
}

void IntensInterface_Impl::SetDimensionIndexOfLevel(DataReference* dref, int val, int level, int index) {
  dref->SetDimensionIndexOfLevel( val, level, index );
}

DataVector* IntensInterface_Impl::getNewDataVector() {
  return new DataVector();
}

void IntensInterface_Impl::FreeDataVector(DataVector* dvec) {
  if (dvec) delete dvec;
}

void IntensInterface_Impl::insertDataVector(DataVector* dvec, int value) {
  if (dvec)
    dvec->push_back( new DataIntegerValue( value ) );
}

bool IntensInterface_Impl::getDataVector(DataReference* dref, DataVector &vector) {
  return dref==0 ? false : dref->getDataVector( vector );
}

bool IntensInterface_Impl::setDataVector(DataReference* dref, const DataVector &vector) {
  return dref==0 ? false : dref->setDataVector( vector );
}

void IntensInterface_Impl::GetFullName(const DataReference* dref, std::string & Name, bool WithIndizes) const {
  Name.erase();
  if (dref)
    dref->getFullName(Name,WithIndizes);
}

std::string IntensInterface_Impl::GetFullName(const DataReference* dref, bool WithIndizes) const {
  return dref==0 ? "" : dref->fullName(WithIndizes);
}

bool IntensInterface_Impl::SetItemInvalid(DataReference* dref, int i1) {
  return dref==0 ? false : dref->SetItemInvalid( i1 );
}

bool IntensInterface_Impl::SetItemInvalid_PreInx(DataReference* dref) {
  return dref==0 ? false : dref->SetItemInvalid_PreInx();
}

bool IntensInterface_Impl::GetItemValid(DataReference* dref) {
  return dref==0 ? false : dref->GetItemValid();
}

bool IntensInterface_Impl::GetItemValid_PreInx(DataReference* dref) {
  return dref==0 ? false : dref->GetItemValid_PreInx();
}

int IntensInterface_Impl::GetNumberOfLevels(DataReference* dref) const {
  return dref==0 ? 0 : dref->GetNumberOfLevels();
}

int IntensInterface_Impl::GetAllDataDimensionSize(DataReference* dref, DataContainer::SizeList &dimsize) {
  dimsize.clear();
  return dref==0 ? 0 : dref->GetAllDataDimensionSize(dimsize);
}

int IntensInterface_Impl::GetDataDimensionSize(DataReference* dref) {
  return dref==0 ? 0 : dref->GetDataDimensionSize();
}

void IntensInterface_Impl::AssignDataItem(DataReference &Dest,  DataReference &Source) {
  Dest.AssignDataItem(Source);
}

void IntensInterface_Impl::AssignDataElement(DataReference &Dest,  DataReference &Source) {
  Dest.AssignDataElement(Source);
}

bool IntensInterface_Impl::RefValid(DataReference* dref) {
  return dref==0 ? false : dref->RefValid();
}

void IntensInterface_Impl::FreeDataReference(DataReference* dref) {
  if (dref)
    delete dref;
}

bool IntensInterface_Impl::IsEqual(const DataReference &dref1, const DataReference &dref2) {
  return dref1 == dref2;
}

bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, int val) {
  return dref==0 ? false : dref->SetValue( val );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, int val, int i1) {
  return dref==0 ? false : dref->SetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, int val, int i1, int i2) {
  return dref==0 ? false : dref->SetValue( val, i1, i2 );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, double val) {
  return dref==0 ? false : dref->SetValue( val );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, double val, int i1) {
  return dref==0 ? false : dref->SetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, double val, int i1, int i2) {
  return dref==0 ? false : dref->SetValue( val, i1, i2 );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, const std::string& val) {
  return dref==0 ? false : dref->SetValue( val );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, const std::string& val, int i1) {
  return dref==0 ? false : dref->SetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceSetValue(DataReference* dref, const std::string& val, int i1, int i2) {
  return dref==0 ? false : dref->SetValue( val, i1, i2 );
}


bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, int& val) {
  return dref==0 ? false : dref->GetValue( val );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, int& val, int i1) {
  return dref==0 ? false : dref->GetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, int& val, int i1, int i2) {
  return dref==0 ? false : dref->GetValue( val, i1, i2 );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, double& val) {
  return dref==0 ? false : dref->GetValue( val );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, double& val, int i1) {
  return dref==0 ? false : dref->GetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, double& val, int i1, int i2) {
  return dref==0 ? false : dref->GetValue( val, i1, i2 );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, std::string& val) {
  return dref==0 ? false : dref->GetValue( val );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, std::string& val, int i1) {
  return dref==0 ? false : dref->GetValue( val, i1 );
}
bool IntensInterface_Impl::DataReferenceGetValue(DataReference* dref, std::string& val, int i1, int i2) {
  return dref==0 ? false : dref->GetValue( val, i1, i2 );
}


DataDictionary::DataType IntensInterface_Impl::GetDataType(DataReference& dref) {
  return dref.getDataType();
}
DataDictionary* IntensInterface_Impl::AddToDictionary(const std::string &structName, const std::string &name
				,const DataDictionary::DataType type ) {
  return DataPoolIntens::addToDictionary( structName, name, type );
}




GuiElement* IntensInterface_Impl::findElement( std::string name) {
  return GuiElement::findElement( name );
}

void IntensInterface_Impl::GuiUpdate(GuiElement::UpdateReason reason) {
  GuiFactory::Instance()->update( reason );
}

void IntensInterface_Impl::MatlabInitProcessSetSilentMode() {
  MatlabInitProcess::Instance().setSilentMode();
}
void IntensInterface_Impl::MatlabInitProcessResetSilentMode() {
  MatlabInitProcess::Instance().resetSilentMode();
}
bool IntensInterface_Impl::MatlabInitProcessInitialized() {
  return MatlabInitProcess::Instance().initialized();
}
bool IntensInterface_Impl::MatlabInitProcessStart() {
  return MatlabInitProcess::Instance().start();
}
bool IntensInterface_Impl::MatlabInitProcessHasTerminated() {
  return MatlabInitProcess::Instance().hasTerminated();
}



void IntensInterface_Impl::printMessage( const std::string &text, GuiElement::MessageType type, int  delay ) {
  GuiManager::Instance().getElement()->printMessage( text, type, delay);
}

bool IntensInterface_Impl::showDialogConfirmation(GuiElement * elem
						  , const std::string &title
						  , const std::string &message
						  , ConfirmationListener *listener ) {
  GuiElement::ButtonType ret = GuiFactory::Instance()->showDialogConfirmation( elem, title, message, listener );
  return ret == GuiElement::button_Yes;
}
bool IntensInterface_Impl::showDialogWarning(GuiElement * elem
					    , const std::string &title
					    , const std::string &message
					    , InformationListener *listener ) {
  return GuiFactory::Instance()->showDialogWarning( elem, title, message, listener );
}

void IntensInterface_Impl::writeToLogWindow(const std::string str) {
  std::string msg = compose("%1 PLUGIN: %2\n", DateAndTime(), str);

  GuiFactory::Instance()->getLogWindow()->writeText( msg );
}
void IntensInterface_Impl::writeToStandardWindow(const std::string str) {
  GuiFactory::Instance()->getStandardWindow()->writeText( str );
}

GuiForm* IntensInterface_Impl::getGuiForm( const std::string &fname ) {
  return GuiElement::getForm( fname );
}

GuiButton* IntensInterface_Impl::createButton( GuiElement *parent, GuiButtonListener *listener) {
  return GuiFactory::Instance() -> createButton( parent, listener );
}

#if defined HAVE_QT && !defined HAVE_HEADLESS
bool IntensInterface_Impl::getPixmap( const std::string& iconname, QPixmap &pixmap, int width, int height ) {
  return QtIconManager::Instance().getPixmap( iconname, pixmap );
}

QFont IntensInterface_Impl::getQFont( const std::string &label, QFont &font ) {
  return QtMultiFontString::getQFont( label, font );
}

const QColor& IntensInterface_Impl::getBackgroundColor() {
  return GuiQtManager::backgroundColor();
}
#endif
