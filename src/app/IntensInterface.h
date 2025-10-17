
#ifndef INTENS_INTERFACE_H
#define INTENS_INTERFACE_H

#include <string>
#include "gui/GuiElement.h" // wegen UpdateReason Definition
#include "datapool/DataVector.h" // wegen DataVector Definition
#include "datapool/DataContainer.h" // wegen DataContainer::SizeList Definition
#include "datapool/DataDictionary.h" // wegen  DataDictionary::DataType Definition
#include "app/DataSet.h" // wegen  DataSetItemList Definition


class DataReference;
class XferDataItem;
class XferDataItemIndex;
class MatlabProcess;
class InputChannelEvent;
class Stream;
class StreamParameter;
class Scale;
class GuiElement;
class GuiButton;
class GuiButtonListener;
class ConfirmationListener;
class InformationListener;
class QPixmap;
class QColor;
class QFont;

typedef long    TransactionNumber;    // Transactions of Datapool

class IntensInterface {
 public:
  virtual DataReference* getNewDataReference(std::string const&) = 0;
  virtual DataReference* getNewDataReference(DataReference const*, std::string const&) = 0;
  virtual DataReference* getNewDataReference(DataReference& dref) = 0;
  //  virtual DataReference* getNewDataReference(Root& sit) = 0;
  virtual std::string getClassName( DataReference* dref ) = 0;
  virtual std::string getNodeName( DataReference* dref ) = 0;
  virtual int getMaxOccurs( DataReference* dref ) = 0;

  virtual XferDataItem*      getNewXferDataItem( DataReference *dref=0 ) = 0;
  virtual void               FreeXferDataItem( XferDataItem* xfer ) = 0;
  virtual XferDataItemIndex* getNewXferDataItemIndex(XferDataItem* xfer, int idx, int lowerbound=-1) = 0;
  virtual int                XferDataItemLastSize(XferDataItem* xfer ) = 0;
  virtual void               XferDataItemSetIndexName(XferDataItem* xfer, int level, const char* name ) = 0;
  virtual void               XferDataItemSetDimensionIndizes(XferDataItem* xfer) = 0;
  virtual void               XferDataItemSetIndex(XferDataItem* xfer, const char*, int ) = 0;
  virtual bool               XferDataItemGetValue(XferDataItem* xfer, std::string& value ) = 0;

  virtual MatlabProcess* getNewMatlabProcess(std::string const &name) = 0;
  virtual void           FreeMatlabProcess(MatlabProcess*  mp) = 0;
  virtual int            MatlabProcessAddInputStream( MatlabProcess*  mp, Stream *s , const std::string &fifo="" ) = 0;
  virtual int            MatlabProcessAddOutputStream( MatlabProcess*  mp, Stream *s ) = 0;
  virtual bool           MatlabProcessStart(MatlabProcess*  mp) = 0;
  virtual std::string    MatlabProcessGetErrorMessage(MatlabProcess*  mp) = 0;
  virtual int            MatlabProcessGetExitStatus(MatlabProcess*  mp) = 0;
  virtual bool           MatlabProcessHasTerminated(MatlabProcess*  mp) = 0;
  virtual void           InputChannelEventWrite(InputChannelEvent &event, std::ostringstream& os) = 0;

  virtual Stream*  getNewStream(bool check) = 0;
  virtual StreamParameter* StreamAddMatrixParameter( Stream* stream, XferDataItem *ditem
				       , int length, int prec, Scale *scale
				       , bool thousand_sep, char delimiter, bool mandatory ) = 0;
  virtual StreamParameter* StreamAddParameter( Stream* stream, XferDataItem *ditem
					 , int length, int prec, Scale *scale
					 , bool thousand_sep
					 , bool  mandatory
					 , bool isDatasetText ) = 0;
  virtual void StructStreamParameterKeepIndex(StreamParameter* sp) = 0;

  virtual DataSet* getDataSet(std::string vn) = 0;
  virtual DataSetItemList* getDataSetItemList(std::string vn) = 0;
  virtual int DataSetGetInputPosition(DataSet* ds, const std::string &value ) = 0;

  virtual void BeginDataPoolTransaction(TransactionOwner *owner) = 0;
  virtual void CommitDataPoolTransaction(TransactionOwner *owner) = 0;
  virtual void RollbackDataPoolTransaction(TransactionOwner *owner) = 0;

  virtual TransactionNumber CurrentDataPoolTransactionNumber() = 0;
  virtual bool IsDataItemUpdated( DataReference* dref, TransactionNumber t ) = 0;
  virtual bool EraseElements( DataReference* dref, int idx ) = 0;
  virtual bool EraseAllElements( DataReference* dref ) = 0;
  virtual void SetDimensionIndizes(DataReference* dref, int NumIndizes, const int * Indizes) = 0;
  virtual void SetDimensionIndizes(DataReference* dref, int level, int NumIndizes, const int * Indizes) = 0;
  virtual void SetDimensionIndexOfLevel(DataReference* dref, int val, int level, int index) = 0;
  virtual DataVector* getNewDataVector() = 0;
  virtual void FreeDataVector(DataVector* dvec) = 0;
  virtual void insertDataVector(DataVector* dvec, int value) = 0;
  virtual bool getDataVector(DataReference* dref, DataVector &vector) = 0;
  virtual bool setDataVector(DataReference* dref, const DataVector &vector) = 0;
  virtual void GetFullName(const DataReference* dref, std::string & Name, bool WithIndizes=false) const = 0;
  virtual std::string GetFullName(const DataReference* dref, bool WithIndizes=false) const = 0;
  virtual bool SetItemInvalid(DataReference* dref, int i1) = 0;
  virtual bool SetItemInvalid_PreInx(DataReference* dref) = 0;
  virtual bool GetItemValid(DataReference* dref) = 0;
  virtual bool GetItemValid_PreInx(DataReference* dref) = 0;
  virtual int GetNumberOfLevels(DataReference* dref) const = 0;
  virtual int GetAllDataDimensionSize(DataReference* dref, DataContainer::SizeList &dimsize) = 0;
  virtual int GetDataDimensionSize(DataReference* dref) = 0;
  virtual void AssignDataItem(DataReference &Dest,  DataReference &Source) = 0;
  virtual void AssignDataElement(DataReference &Dest,  DataReference &Source) = 0;
  virtual bool RefValid(DataReference* dref) = 0;
  virtual void FreeDataReference(DataReference* dref) = 0;
  virtual bool IsEqual(const DataReference &dref1,  const DataReference &dref2)  = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, int val) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, int val, int i1) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, int val, int i1, int i2) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, double val) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, double val, int i1) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, double val, int i1, int i2) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val, int i1) = 0;
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val, int i1, int i2) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, int& val) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, int& val, int i1) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, int& val, int i1, int i2) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, double& val) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, double& val, int i1) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, double& val, int i1, int i2) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val, int i1) = 0;
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val, int i1, int i2) = 0;
  virtual DataDictionary::DataType GetDataType(DataReference& dref) = 0;
  virtual DataDictionary* AddToDictionary(const std::string &structName, const std::string &name
					  ,const DataDictionary::DataType type ) = 0;

  virtual GuiElement* findElement( std::string ) = 0;
  virtual void GuiUpdate(GuiElement::UpdateReason) = 0;

  virtual void MatlabInitProcessSetSilentMode() = 0;
  virtual void MatlabInitProcessResetSilentMode() = 0;
  virtual bool MatlabInitProcessInitialized() = 0;
  virtual bool MatlabInitProcessStart() = 0;
  virtual bool MatlabInitProcessHasTerminated() = 0;


  virtual void printMessage( const std::string &, GuiElement::MessageType, int  delay = 2 ) = 0;
  virtual bool showDialogConfirmation(GuiElement *
				      , const std::string &title
				      , const std::string &message
				      , ConfirmationListener * = 0 ) = 0;
  virtual bool showDialogWarning(GuiElement *
				 , const std::string &title
				 , const std::string &message
				 , InformationListener * = 0 ) = 0;
  virtual void writeToLogWindow(const std::string str) = 0;
  virtual void writeToStandardWindow(const std::string str) = 0;


  /** Diese Funktion liefert den Pointer auf die gewuenschte Form. Falls keine
      Form mit diesem Namen existiert, wird der Pointer 0 geliefert.
   */
  virtual GuiForm*         getGuiForm( const std::string &fname ) = 0;
  virtual GuiButton*       createButton( GuiElement *parent, GuiButtonListener *listener) = 0; //, GuiEventData *event=0
#if defined HAVE_QT && !defined HAVE_HEADLESS
  virtual bool             getPixmap( const std::string& iconname, QPixmap &pixmap, int width=-1, int height=-1 ) = 0;
  virtual QFont            getQFont( const std::string &label, QFont &font ) = 0;
  virtual const QColor&           getBackgroundColor() = 0;
#endif
};

class IntensInterface_Impl : public IntensInterface {
 public:
  virtual DataReference* getNewDataReference(std::string const&);
  virtual DataReference* getNewDataReference(DataReference const*, std::string const&);
  virtual DataReference* getNewDataReference(DataReference& dref);
  //  virtual DataReference* getNewDataReference(Root& sit);

  virtual std::string getClassName( DataReference* dref );
  virtual std::string getNodeName( DataReference* dref );
  virtual int getMaxOccurs( DataReference* dref );

  virtual XferDataItem*      getNewXferDataItem( DataReference *dref=0 );
  virtual void               FreeXferDataItem( XferDataItem* xfer );
  virtual XferDataItemIndex* getNewXferDataItemIndex(XferDataItem* xfer, int idx, int lowerbound=-1);
  virtual int                XferDataItemLastSize(XferDataItem* xfer );
  virtual void               XferDataItemSetIndexName(XferDataItem* xfer, int level, const char* name );
  virtual void               XferDataItemSetDimensionIndizes(XferDataItem* xfer);
  virtual void               XferDataItemSetIndex(XferDataItem* xfer, const char*, int );
  virtual bool               XferDataItemGetValue(XferDataItem* xfer, std::string& value );

  virtual MatlabProcess*  getNewMatlabProcess(std::string const &name);
  virtual void            FreeMatlabProcess(MatlabProcess* mp);
  virtual int             MatlabProcessAddInputStream( MatlabProcess*  mp, Stream *s , const std::string &fifo="" );
  virtual int             MatlabProcessAddOutputStream( MatlabProcess*  mp, Stream *s );
  virtual bool            MatlabProcessStart(MatlabProcess*  mp);
  virtual std::string     MatlabProcessGetErrorMessage(MatlabProcess*  mp);
  virtual int             MatlabProcessGetExitStatus(MatlabProcess*  mp);
  virtual bool            MatlabProcessHasTerminated(MatlabProcess*  mp);
  virtual void            InputChannelEventWrite(InputChannelEvent &event, std::ostringstream& os);

  virtual Stream*      getNewStream(bool check);
  virtual StreamParameter* StreamAddMatrixParameter( Stream* stream, XferDataItem *ditem
				       , int length, int prec, Scale *scale
				       , bool thousand_sep, char delimiter, bool mandatory );
  virtual StreamParameter* StreamAddParameter( Stream* stream, XferDataItem *ditem
					 , int length, int prec, Scale *scale
					 , bool thousand_sep
					 , bool  mandatory
					 , bool isDatasetText );
  virtual void StructStreamParameterKeepIndex(StreamParameter* sp);

  virtual DataSet* getDataSet(std::string vn);
  virtual DataSetItemList* getDataSetItemList(std::string vn);
  virtual int DataSetGetInputPosition(DataSet* ds, const std::string &value );

  virtual void BeginDataPoolTransaction(TransactionOwner *owner);
  virtual void CommitDataPoolTransaction(TransactionOwner *owner);
  virtual void RollbackDataPoolTransaction(TransactionOwner *owner);

  virtual TransactionNumber CurrentDataPoolTransactionNumber();
  virtual bool IsDataItemUpdated( DataReference* dref, TransactionNumber t );
  virtual bool EraseElements( DataReference* dref, int idx );
  virtual bool EraseAllElements( DataReference* dref );
  virtual void SetDimensionIndizes(DataReference* dref, int NumIndizes, const int * Indizes);
  virtual void SetDimensionIndizes(DataReference* dref, int level, int NumIndizes, const int * Indizes);
  virtual void SetDimensionIndexOfLevel(DataReference* dref, int val, int level, int index);
  virtual DataVector* getNewDataVector();
  virtual void FreeDataVector(DataVector* dvec);
  virtual void insertDataVector(DataVector* dvec, int value);
  virtual bool getDataVector(DataReference* dref, DataVector &vector);
  virtual bool setDataVector(DataReference* dref, const DataVector &vector);
  virtual void GetFullName(const DataReference* dref, std::string & Name, bool WithIndizes=false) const;
  virtual std::string GetFullName(const DataReference* dref, bool WithIndizes=false) const;
  virtual bool SetItemInvalid(DataReference* dref, int i1);
  virtual bool SetItemInvalid_PreInx(DataReference* dref);
  virtual bool GetItemValid(DataReference* dref);
  virtual bool GetItemValid_PreInx(DataReference* dref);
  virtual int GetNumberOfLevels(DataReference* dref) const;
  virtual int GetAllDataDimensionSize(DataReference* dref, DataContainer::SizeList &dimsize);
  virtual int GetDataDimensionSize(DataReference* dref);
  virtual void AssignDataItem(DataReference &Dest,  DataReference &Source);
  virtual void AssignDataElement(DataReference &Dest,  DataReference &Source);
  virtual bool RefValid(DataReference* dref);
  virtual void FreeDataReference(DataReference* dref);
  virtual bool IsEqual(const DataReference &dref1, const DataReference &dref2);
  virtual bool DataReferenceSetValue(DataReference* dref, int val);
  virtual bool DataReferenceSetValue(DataReference* dref, int val, int i1);
  virtual bool DataReferenceSetValue(DataReference* dref, int val, int i1, int i2);
  virtual bool DataReferenceSetValue(DataReference* dref, double val);
  virtual bool DataReferenceSetValue(DataReference* dref, double val, int i1);
  virtual bool DataReferenceSetValue(DataReference* dref, double val, int i1, int i2);
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val);
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val, int i1);
  virtual bool DataReferenceSetValue(DataReference* dref, const std::string& val, int i1, int i2);
  virtual bool DataReferenceGetValue(DataReference* dref, int& val);
  virtual bool DataReferenceGetValue(DataReference* dref, int& val, int i1);
  virtual bool DataReferenceGetValue(DataReference* dref, int& val, int i1, int i2);
  virtual bool DataReferenceGetValue(DataReference* dref, double& val);
  virtual bool DataReferenceGetValue(DataReference* dref, double& val, int i1);
  virtual bool DataReferenceGetValue(DataReference* dref, double& val, int i1, int i2);
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val);
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val, int i1);
  virtual bool DataReferenceGetValue(DataReference* dref, std::string& val, int i1, int i2);
  virtual DataDictionary::DataType GetDataType(DataReference& dref);
  virtual DataDictionary* AddToDictionary(const std::string &structName, const std::string &name
					  ,const DataDictionary::DataType type );

  virtual GuiElement* findElement( std::string );
  virtual void GuiUpdate(GuiElement::UpdateReason);

  virtual void MatlabInitProcessSetSilentMode();
  virtual void MatlabInitProcessResetSilentMode();
  virtual bool MatlabInitProcessInitialized();
  virtual bool MatlabInitProcessStart();
  virtual bool MatlabInitProcessHasTerminated();

  virtual void printMessage( const std::string &, GuiElement::MessageType, int  delay = 2 );
  virtual bool showDialogConfirmation(GuiElement *
				      , const std::string &title
				      , const std::string &message
				      , ConfirmationListener * = 0 );
  virtual bool showDialogWarning(GuiElement *
				 , const std::string &title
				 , const std::string &message
				 , InformationListener * = 0 );
  virtual void writeToLogWindow(const std::string str);
  virtual void writeToStandardWindow(const std::string str);

 /** Diese Funktion liefert den Pointer auf die gewuenschte Form. Falls keine
      Form mit diesem Namen existiert, wird der Pointer 0 geliefert.
   */
  virtual GuiForm*         getGuiForm( const std::string &fname );
  virtual GuiButton*       createButton( GuiElement *parent, GuiButtonListener *listener);
#if defined HAVE_QT && !defined HAVE_HEADLESS
  virtual bool             getPixmap( const std::string& iconname, QPixmap &pixmap, int width=-1, int height=-1 );
  virtual QFont            getQFont( const std::string &label, QFont &font );
  virtual const QColor&    getBackgroundColor();
#endif
};

#endif
