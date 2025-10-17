
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <assert.h>
#include "xfer/XferConverter.h"
#include "utils/NumLim.h"

#include "cppunit/TestCase.h"
#include "cppunit/TestSuite.h"
#include "cppunit/TestCaller.h"
#include "cppunit/TextTestResult.h"
#include "xfer/Scale.h"

//ANFANG/////////////////////////ZUSAETZLICH VON INTENS
#include "gui/GuiElement.h"
#include "gui/GuiIndex.h"
#include "gui/GuiFactory.h"
#include "app/DataPoolIntens.h"
#include "app/DataSet.h"
#include "app/DataAlterSetDbModified.h"
#include "app/UserAttr.h"
#include "app/ItemAttr.h"
#include "app/AppData.h"
#include "app/LicMgr.h"
#include "app/App.h"
//#include "xml/XMLDocumentHandler.h"
class XMLAttributeMap {
public:
  XMLAttributeMap(){}
  virtual ~XMLAttributeMap(){}
  std::string operator[]( const std::string & ) const;
//   std::string &operator[]( const std::string &indx ){ return _attrmap[indx]; }
//   void dispAll(void);

private:
//   typedef std::map<std::string, std::string> AttrMap;
//   AttrMap _attrmap;
};

GuiElement*  GuiElement::findElement( const std::string & ) { assert(false); }
GuiFactory* GuiFactory::s_factory=NULL;
GuiManager& GuiManager::Instance() { assert( false ); };
LicMgr &LicMgr::Instance() { assert( false ); };
bool LicMgr::check( const std::string &feature, const std::string &version )const{ assert( false ); };
std::string LicMgr::getErrorMessage() const { assert( false ); };
const char* App::FEATURE_PACKAGE = "intens";

void GuiIndex::addIndexContainer( const std::string &, GuiIndex * ) { assert(false); }

DataPoolIntens& DataPoolIntens::Instance()  { assert(false); }
DataPool& DataPoolIntens::getDataPool() { assert(false); }
bool DataPoolIntens::checkTargetStreams( DataReference *ref, std::string& error_msg ) { assert(false); }
void DataPoolIntens::clearTargetStreams( DataReference *ref ) { assert(false); }
DataReference* DataPoolIntens::getDataReference( const std::string &name )  { assert(false); }
DataSet *DataPoolIntens::getDataSet( const std::string &name ){ assert(false); }

int DataSet::getOutputPosition( const std::string &value ){ assert(false); }
bool DataSet::getInputValue( std::string &val, int inx ){ assert(false); }
DataAlterSetDbModified::DataAlterSetDbModified( TransactionNumber trans, bool new_trans ){ assert(false); }
DataAlterSetDbModified::~DataAlterSetDbModified(){ assert(false); }
bool DataAlterSetDbModified::check( DataReference &ref ){ assert(false); }
bool DataAlterSetDbModified::alterItem( DataItem &item
                                      , UpdateStatus &status
                                      , DataInx &inx
                                      , bool final ){ assert(false); }
DataAlterSetDbModified::UpdateStatus DataAlterSetDbModified::alterData( DataElement &el ){ assert(false); }
bool DataAlterSetDbModified::inclusiveValidItemAttributes() const{ assert(false); }
void DataAlterSetDbModified::epilog( bool result ){ assert(false); }

const std::string &JobAction::Name() { assert(false); }

void ItemAttr::setDataBaseTimestamp( TransactionNumber ts )   { assert(false); }
TransactionNumber ItemAttr::DataBaseTimestamp() const { assert(false); }

std::string XMLAttributeMap::operator[]( const std::string & ) const { assert(false); }

//ENDE/////////////////////////ZUSAETZLICH VON INTENS

class ConverterTest: public CppUnit::TestCase {
public:
  void setUp(){
  }
  void tearDown(){
  }

  void testReadInfinity(){  // Do infinity test
    std::istringstream istr( "INF" );
    double d;

    RealConverter simplerealconv(0, -1, new Scale(), false, false );
    CPPUNIT_ASSERT( simplerealconv.read( istr, d, ' ' ) );
    CPPUNIT_ASSERT( !NumLim::isFinite(d) );
  }

  void testMultiDimensionScale(){
    std::string varname = "testdata";
    DataPool *datapool = DataPool::Instance( );
    bool isGlobal = true;
    datapool->AddToDictionary( "", varname
			       , DataDictionary::type_Real
			       , 0, isGlobal );

    DataReference *dref = datapool->newDataReference( varname.c_str() );
    XferDataItem *xfer = new XferDataItem( dref );
    XferDataItemIndex *indx = xfer->newDataItemIndex();
    xfer->setDimensionIndizes();
    xfer->setIndex(0,0);
    xfer->setValue(1.23);
    double d;
    CPPUNIT_ASSERT( xfer->getValue( d ) == true );
    CPPUNIT_ASSERT( d==1.23 );

  }

  void testBugFixes(){ // added for bugfixing
    double d = -0.25249972;
    RealConverter rc(10, -1, new Scale(), false, false );
    std::ostringstream ostr;
    rc.write( d, ostr );
    assert( ostr.str() == "-0.2524997" );
    //    assert( ostr.str() == "-2.525e-01" );

    d = -0.00025249972;
    ostr.str("");
    rc.write( d, ostr );
    assert( ostr.str() == "-2.525e-04" );

    d = -3.45249992;
    ostr.str("");
    rc.write( d, ostr );
    assert( ostr.str() == "-3.4524999" );

    RealConverter rco(-10, -1, new Scale(), false, false );
    ostr.str("");
    rco.write( d, ostr );
    assert( ostr.str() == "-3.4524999" );

    RealConverter rc2(0,-1, 0, false, false );
    d = 7000.05;
    ostr.str("");
    rc2.write( d, ostr );
    assert( ostr.str() == "7000.05" );

    d = 70000.4;
    ostr.str("");
    rc2.write( d, ostr );
    assert( ostr.str() == "70000.4" );

    d = 0.000000200;
    ostr.str("");
    rc2.write( d, ostr );
    assert( ostr.str() == "0.0000002" );

    d = 70001.00004;
    ostr.str("");
    rc2.write( d, ostr );
    assert( ostr.str() == "70001.0" );

    d = 3;
    RealConverter rcDot(-10, -1, new Scale(), false, false );
    rcDot.setAppendDot();
    ostr.str("");
    rcDot.write( d, ostr );
    assert( ostr.str() == "3.        " );

    d = 1234567890;
    ostr.str("");
    rcDot.write( d, ostr );
    assert( ostr.str() == "1.2346e+09" );


  }

  void testPrecision(){ // do some precision tests
    double d;
    RealConverter precrealconv(6, 1, new Scale(), false, false );
    std::istringstream istr2( "5" );
    assert( precrealconv.read( istr2, d, ' ' ) );
    std::ostringstream ostr;
    assert( precrealconv.write( d, ostr ) );
    assert( ostr.str() == "   5.0" );
    ostr.str("");
    std::istringstream istr3( "10" );

    assert( precrealconv.read( istr3, d, ' ' ) );
    assert( precrealconv.write( d, ostr ) );
    assert( ostr.str() == "  10.0" );
    ostr.str("");

    assert( precrealconv.write( 15555,ostr ) );
    assert( ostr.str() == "2.e+04") ;
    ostr.str("");

    precrealconv.write( 0.0001,ostr );
    assert( ostr.str() == "   0.0") ;
    ostr.str("");

    RealConverter precrealconv2(5, -1, new Scale(), false, false );
    precrealconv2.write( 99066.222855,ostr );
    assert( ostr.str() == "1e+05") ;
  }

  void testScaleConversion(){ // do some scale conversion tests
    double d;
    std::ostringstream ostr;
    RealConverter scalerealconv(0, 0, new Scale( 1000, '*' ), false, false );
    scalerealconv.write( 1, ostr );
    //   cout << ostr.str() << endl;
    assert( ostr.str()=="1000" );
    ostr.str("");
    scalerealconv.write( 0.001, ostr );
    //   cout << ostr.str() << endl;
    assert( ostr.str()=="1" );

    std::istringstream istr( ostr.str());
    scalerealconv.read( istr, d, ' ' );
    //   cout << d << endl;
    assert( d==0.001 );
    ostr.str("");
    scalerealconv.write( 1.0001, ostr );
    //   cout << ostr.str() << endl;
    assert( ostr.str() == "1000" );
  }

  void testNoTrailingZeros(){ // test if trailing zeros are removed
    double d;
    std::ostringstream ostr;
    RealConverter realconv(9, -1, new Scale(), false, false );
    realconv.write( 4.180000001e-8, ostr );
    //std::cout << ostr.str() << std::endl;
    assert( ostr.str()==" 4.18e-08" );
    ostr.str("");
    realconv.write( 4.18000000001, ostr );
    //std::cout << ostr.str() << std::endl;
    assert( ostr.str()=="     4.18" );
    ostr.str("");
    realconv.write( 134.000001, ostr );
    //std::cout << ostr.str() << std::endl;
    assert( ostr.str()=="    134.0" );
    ostr.str("");
    realconv.write( 4.0000001, ostr );
    //cout << ostr.str() << endl;
    assert( ostr.str()=="      4.0" );
  }

  void testThousandSep(){  // Do thousand separator test
    double d;
    RealConverter curr(-20, 2, new Scale(), false, false );
    std::ostringstream ostr;
    curr.setThousandSep();
    curr.write( 2123000, ostr );
    //std::cout << "                    " << ostr.str() << std::endl;
    assert( ostr.str()=="2'123'000.00        " );

    std::istringstream istr( ostr.str());
    curr.read( istr, d, ' ' );
    assert( d==2123000 );
    ostr.str("");
    RealConverter rc(20, 2, new Scale(), false, false );
    rc.setThousandSep();
    rc.write( -2123000, ostr );
    assert( ostr.str()=="       -2'123'000.00" );
  }

  void testRounding(){  // Do rounding tests
    double d;
    RealConverter curr(9, -1, new Scale(), false, false);
    std::ostringstream ostr;
    curr.write( 11799.999995, ostr );
    //     std::cout << ostr.str() << std::endl;
    assert( ostr.str()=="  11800.0" );
    ostr.str("");
    curr.write( 2.499995, ostr );
    //cout << ostr.str() << endl;
    assert( ostr.str()=="      2.5" );
    ostr.str("");
    curr.write( 9.999999, ostr );
    //std::cout << ostr.str() << std::endl;
    assert( ostr.str()=="     10.0" );

    ostr.str("");
    curr.write( 9.9999999999, ostr );
    //std::cout << ostr.str() << std::endl;
    assert( ostr.str()=="       10" );
  }
  void testDelimiter(char delimiter){ // do some delimiter tests

    std::ostringstream ostr;
    ostr << 5 << delimiter << 6 << delimiter << 7 << "\n";
    std::string str(ostr.str());
    std::ostringstream ostr_a;
    ostr_a << 5 << delimiter << "\n";
    std::string str_a(ostr_a.str());
    std::string str_b("5 6 7\n");

    std::string s;
    StringConverter strconv(0, false );
    std::istringstream istr(str);
    assert( strconv.read( istr, s, delimiter ) );
    assert( s == "5" );
    assert( strconv.read( istr, s, delimiter ) );
    assert( s == "6" );
    assert( strconv.read( istr, s, delimiter ) );
    assert( s == "7" );
    assert( istr.str().substr(istr.tellg()) == "\n" );
    assert(!strconv.read( istr, s, delimiter ) );
    assert( s == "" );
    assert( istr.str().substr(istr.tellg()) == "\n" );

    double d;
    RealConverter realconv(0, -1, new Scale(), false, false );
    std::istringstream istr2(str);
    assert( realconv.read( istr2, d, delimiter ) );
    assert( d == 5. );
    assert( realconv.read( istr2, d, delimiter ) );
    assert( d == 6. );
    assert( realconv.read( istr2, d, delimiter ) );
    assert( d == 7. );
    assert( istr2.str().substr(istr2.tellg()) == "\n" );
    assert(!realconv.read( istr2, d, delimiter ) );
    assert( isnan(d) );
    assert( istr2.str().substr(istr2.tellg()) == "\n" );

    std::istringstream istr2a(str_a);
    assert( realconv.read( istr2a, d, delimiter ) );
    assert( d == 5. );
    assert(!realconv.read( istr2a, d, delimiter ) );
    assert( isnan(d) );
    assert( istr2a.str().substr(istr2a.tellg()) == "\n" );

    std::istringstream istr2b(str_b);
    assert( realconv.read( istr2b, d, delimiter ) );
    assert( d == 5. );
    assert( realconv.read( istr2b, d, delimiter ) );
    assert( d == 6. );
    assert( realconv.read( istr2b, d, delimiter ) );
    assert( d == 7. );
    assert( istr2b.str().substr(istr2b.tellg()) == "\n" );
    assert(!realconv.read( istr2b, d, delimiter ) );
    assert( isnan(d) );
    assert( istr2b.str().substr(istr2b.tellg()) == "\n" );

    int i;
    IntConverter intconv(0, new Scale(), false );
    std::istringstream istr3(str);
    assert( intconv.read( istr3, i, delimiter ) );
    assert( i == 5 );
    assert( intconv.read( istr3, i, delimiter ) );
    assert( i == 6 );
    assert( intconv.read( istr3, i, delimiter ) );
    assert( i == 7 );
    assert( istr3.str().substr(istr3.tellg()) == "\n" );
    assert(!intconv.read( istr3, i, delimiter ) );
    assert( i == std::numeric_limits<int>::min() );
    assert( istr3.str().substr(istr3.tellg()) == "\n" );

    std::istringstream istr3a(str_a);
    assert( intconv.read( istr3a, i, delimiter ) );
    assert( i == 5 );
    assert( istr3a.str().substr(istr3a.tellg()) == "\n" );
    assert(!intconv.read( istr3a, i, delimiter ) );
    assert( i == std::numeric_limits<int>::min() );
    assert( istr3a.str().substr(istr3a.tellg()) == "\n" );

    std::istringstream istr3b(str_b);
    assert( intconv.read( istr3b, i, delimiter ) );
    assert( i == 5 );
    assert( intconv.read( istr3b, i, delimiter ) );
    assert( i == 6 );
    assert( intconv.read( istr3b, i, delimiter ) );
    assert( i == 7 );
    assert( istr3b.str().substr(istr3b.tellg()) == "\n" );
    assert(!intconv.read( istr3b, i, delimiter ) );
    assert( i == std::numeric_limits<int>::min() );
    assert( istr3b.str().substr(istr3b.tellg()) == "\n" );
  }
  void testDelimiter2Blank(){ // do some precision tests
    std::string str("5    6 7");
    char delimiter = ' ';

    std::istringstream istr( str );
    double d;
    RealConverter realconv(0, -1, new Scale(), false, false );
    assert( realconv.read( istr, d, delimiter ) );
    assert( d == 5 );
    assert( realconv.read( istr, d, delimiter ) );
std::cout << "===>  d["<<d<<"]\n";
    assert( d == 6 );
    assert( realconv.read( istr, d, delimiter ) );
    assert( d == 7 );

    std::istringstream istr2( str );
    int i;
    IntConverter intconv(0, new Scale(), false );
    assert( intconv.read( istr2, i, delimiter ) );
    assert( i == 5 );
    assert( intconv.read( istr2, i, delimiter ) );
    assert( i == 6 );
    assert( intconv.read( istr2, i, delimiter ) );
    assert( i == 7 );

  }
  void testDelimiterBlank(){ // do some delimiter tests
    testDelimiter(' ');
  }
  void testDelimiterTab(){ // do some delimiter tests
    testDelimiter('\t');
  }
  void testDelimiterSC(){ // do some delimiter tests
    testDelimiter(';');
  }
private:
};

int main( int argc, char ** argv ){
  CppUnit::TextTestResult result;
  CppUnit::TestSuite suite;
  /**/
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testBugFixes", &ConverterTest::testBugFixes ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testReadInfinity", &ConverterTest::testReadInfinity ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testPrecision", &ConverterTest::testPrecision ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testScaleConversion", &ConverterTest::testScaleConversion ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testThousandSep", &ConverterTest::testThousandSep ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testRounding"   , &ConverterTest::testRounding ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testNoTrailingZeros", &ConverterTest::testNoTrailingZeros ) );
			  /**/
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testDeliminiter2Blank", &ConverterTest::testDelimiter2Blank ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testDelimiterBlank", &ConverterTest::testDelimiterBlank ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testDelimiterTab", &ConverterTest::testDelimiterTab ) );
  suite.addTest( new CppUnit::TestCaller< ConverterTest >(
			  "testDelimiterSC", &ConverterTest::testDelimiterSC ) );
  // suite.addTest( new CppUnit::TestCaller< ConverterTest >(
  // 							  "testMultiDimensionScale", &ConverterTest::testMultiDimensionScale ) );
  suite.run(&result);
  std::cout << result << std::endl;
  return 0;
}
