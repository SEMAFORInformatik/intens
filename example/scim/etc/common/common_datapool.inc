// **********************************************************************
// Common
// **********************************************************************
SET approval_set (
  _("Experimental")   = "experimental"
, _("In Preparation") = "inPreparation"
, _("Shared")         = "shared"
, _("Approved")       = "approved"
, _("Tested")         = "tested"
, _("Tendered")       = "tendered"
, _("Obsolete")       = "obsolete"
);
COLOR approval_color (
  "experimental"   = ("#FFCC99", "#000000")
, "inPreparation"  = ("#FF6666", "#FFFFFF")
, "shared"         = ("#FF3333", "#FFFFFF")
, "approved"       = ("#FDC500", "#000000")
, "tested"         = ("#3E8925", "#FFFFFF")
, "tendered"       = ("#006089", "#FFFFFF")
, "obsolete"       = ("#000000", "#FFFFFF")
);

SET applicab_set (
  _("Draft")      = "draft"
, _("Restricted") = "restricted"
, _("Free")       = "free"
, _("Preferred")  = "preferred"
, _("Outdated")   = "outdated"
);

INTEGER dbId;

INTEGER ommitQuit; // exit because login was canceled -> do nothing in QUIT

// used in g_*_load_from_dbcomp_func if set to "reload",
// the user is asked if the component should be reloaded
// from the database if it is loaded already but modified
STRING loadType{HIDDEN};

STRUCT Parent {
  INTEGER
    navIndex // index of parent component
  ;
  STRING
    name // name of parent component
  ;
};
STRUCT ComponentLibrary {
  Parent
    parent
  ;
  INTEGER
    saveCanceled
  ;
};

SET type_set("" = "");
STRUCT ElementStatus { // component, project or variant
  STRING  status {SCALAR};    // new, modified, notModified, error
  STRING  substatus {SCALAR}; // trivial, nontrivial
  STRING  approval;           // new approval status if changed
  STRING  text {SCALAR};      // i.E. error message
  STRING  name {SCALAR};      // element name
  STRING  type {SCALAR, SET=type_set};  // element type
  INTEGER rev {SCALAR};       // element revision
  INTEGER id {SCALAR};        // element id (empty for status new)
  INTEGER version {SCALAR};   // element version
  STRING  username {SCALAR};  // element ownername
  STRING  group {SCALAR};     // element group
};

ElementStatus
  elementStatus {SCALAR}, allStatus
;

// Data for Api-Gateway Request - Response

STRUCT Request {
  STRING {SCALAR} user;
  STRING {SCALAR} username;
};

STRUCT ResponseItem {
  STRING {SCALAR} name;
  INTEGER {SCALAR} index;
  STRING {SCALAR} message;
};

STRUCT Response {
  STRING {SCALAR} status;
  STRING message;
  ResponseItem item;
};

Request {SCALAR} api_gateway_request;
Response {SCALAR} api_gateway_response;
