
#include "gui/GuiFactory.h"
#include "app/DataPoolIntens.h"
#include "gui/DialogCompare.h"

const std::string DialogCompare::StructVarname     = "CompareDialog";
const std::string DialogCompare::QuerySourceVarname      = "query_source";
const std::string DialogCompare::QueryDestinationVarname = "query_destination";
const std::string DialogCompare::ItemVarname       = "select_item";
const std::string DialogCompare::ItemLabelVarname  = "itemLabel";
const std::string DialogCompare::ItemIgnoreVarname = "itemIgnore";
const std::string DialogCompare::HeaderVarname     = "header";
const std::string DialogCompare::XAnnotationAxisVarname = "xAnnotation";
const std::string DialogCompare::XAnnotationValueVarname = "xAnnotationValue";
const std::string DialogCompare::XAnnotationLabelVarname = "xAnnotationLabel";

/*=============================================================================*/
/* Constructor / Destructor                                                    */
/*=============================================================================*/
DialogCompare::DialogCompare() {
}

DialogCompare:: ~DialogCompare() {
};


/* --------------------------------------------------------------------------- */
/* getLoopTitle  --                                                            */
/* --------------------------------------------------------------------------- */
// std::string DialogCompare::getLoopTitle() {
//   std::string s;
//   DataReference *ref = DataPoolIntens::Instance().getDataReference( "ProgressDialogLoopTitle" );
//   if ( ref != 0 ) {
//     ref->GetValue( s );
//     delete ref;
//   }
//   return s;
// };
