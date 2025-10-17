
#if !defined(DIALOGCOMPARE_H)
#define DIALOGCOMPARE_H

#include <string>

class DialogCompare
{
/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
 public:
  DialogCompare();
  virtual ~DialogCompare();

 public:
    /** Base Varname of struct */
    static const std::string StructVarname;

    /** query source Varname */
    static const std::string QuerySourceVarname;
    /** query destination  Varname */
    static const std::string QueryDestinationVarname;

    /** select item Varname */
    static const std::string ItemVarname;
    /** override label of selected items Varname */
    static const std::string ItemLabelVarname;

    /** ignore items Varname */
    static const std::string ItemIgnoreVarname;

    /** Header Varname if not set, default Values are the Cycle names */
    static const std::string HeaderVarname;

    /** XAnnotation Varname only used by scalar plot */
    static const std::string XAnnotationAxisVarname;
    /** XAnnotation Value Varname only used by scalar plot */
    static const std::string XAnnotationValueVarname;
    /** XAnnotation Label Varname only used by scalar plot */
    static const std::string XAnnotationLabelVarname;
};

#endif
