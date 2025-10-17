
#if !defined(XFER_GENERICPARAMETER_INCLUDED_H)
#define XFER_GENERICPARAMETER_INCLUDED_H

#include <string>
#include <map>
#include <vector>

class XferDataItem;
class XferDataItemIndex;

class XferGenericParameter {
 public:
    double getDoubleValue( );
    std::string getStringValue( );
    std::vector<double> getDoubleVector( );
//    int getValue( std::vector<std::string> &v );
    void setValue( double v );
    void setValue( std::string &s );
    void setValue( const std::vector<double> &v );
//    void setValue( const std::vector<std::string> &v );

    /** returns an existing XferGenericParameter object
	or creates a new new one. Returns 0 if the variable
	is undeclared.
	@param name of variable
    */
    static XferGenericParameter *get(const char *name );

 private:
    bool checkItem( const char *paramName );
    XferGenericParameter( XferDataItem *x=0 );
    XferGenericParameter( const XferGenericParameter & );
    void operator=( const XferGenericParameter & );

    typedef std::map< std::string, XferGenericParameter *> XferParameterMap;
    static XferParameterMap xfermap;
    XferDataItem *xferItem;
    XferDataItemIndex *index;
};

#endif
