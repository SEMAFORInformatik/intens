DATAPOOL
  STRUCT {NS}BaseUnit {
    STRING {SCALAR, EDITABLE}
      name {LABEL="Name", FUNC={NS}BaseUnit_name_func},
      ui_name {LABEL="UNIT"}
    ;
    REAL {SCALAR, EDITABLE}
      factor {LABEL="Factor"}
    , shift {LABEL="Shift"}
    ;
    INTEGER {SCALAR, EDITABLE}
      use_divide {LABEL="Divide" , TOGGLE}
    , use_set {LABEL="Use" , TOGGLE}
    ;
  };
  STRUCT {NS}AppBaseUnit : {NS}BaseUnit {
    {NS}BaseUnit derived;
  };
  {NS}AppBaseUnit
    {NS}appBaseUnit;

  STRUCT {NS}AppBaseUnitLibrary {
    INTEGER {EDITABLE, SCALAR}
      use_brackets {
        TOGGLE
      , FUNC={NS}unit_use_brackets_func
      , LABEL=_("Use Brackets")
      }
    , unit_btn {
        BUTTON
      , LABEL=_("New")
      , FUNC={NS}unit_btn_func
      }
    , derived_unit_btn {
        BUTTON
      , LABEL=_("New")
      , FUNC={NS}derived_unit_btn_func
      }
    ;
  }
  ;
  {NS}AppBaseUnitLibrary
    {NS}appBaseUnitLibrary;

  STRING {SCALAR} {NS}filename;
  INTEGER {NS}unit_size;
  INTEGER {NS}unit_selected;
END DATAPOOL;

UI_MANAGER
  INDEX {NS}current_idx;
  INDEX {NS}derived_current_idx;
  LIST
    {NS}unit_list {
      _("Application Base Units"),
      TABLESIZE=20,
      FUNC={NS}appBaseUnit_func}(
        LABEL({NS}appBaseUnit.name) {NS}appBaseUnit[*].name
      , LABEL({NS}appBaseUnit.ui_name) {NS}appBaseUnit[*].ui_name
      , LABEL({NS}appBaseUnit.factor) {NS}appBaseUnit[*].factor:10
      , LABEL({NS}appBaseUnit.use_divide) {NS}appBaseUnit[*].use_divide
      , LABEL({NS}appBaseUnit.use_set) {NS}appBaseUnit[*].use_set
    );
  LIST
    {NS}unit_derived_list {
      _("Derived Units"),
      TABLESIZE=10,
      FUNC={NS}appBaseUnit_derived_func}(
        LABEL({NS}appBaseUnit.derived.name) {NS}appBaseUnit[{NS}current_idx].derived[*].name
      , LABEL({NS}appBaseUnit.derived.ui_name) {NS}appBaseUnit[{NS}current_idx].derived[*].ui_name
      , LABEL({NS}appBaseUnit.derived.factor) {NS}appBaseUnit[{NS}current_idx].derived[*].factor:10
      , LABEL({NS}appBaseUnit.derived.shift) {NS}appBaseUnit[{NS}current_idx].derived[*].shift:10
      , LABEL({NS}appBaseUnit.derived.use_divide) {NS}appBaseUnit[{NS}current_idx].derived[*].use_divide
      , LABEL({NS}appBaseUnit.derived.use_set) {NS}appBaseUnit[{NS}current_idx].derived[*].use_set
    );
  FIELDGROUP
    {NS}unit_common_fg {_("Common Properties")}(
      LABEL({NS}appBaseUnitLibrary.use_brackets) {NS}appBaseUnitLibrary.use_brackets
    );
  FIELDGROUP
    {NS}unit_fg {_("Base Unit Properties")}(
      LABEL({NS}appBaseUnit.name) {NS}appBaseUnit[{NS}current_idx].name
    , LABEL({NS}appBaseUnit.ui_name) {NS}appBaseUnit[{NS}current_idx].ui_name
    , LABEL({NS}appBaseUnit.factor) {NS}appBaseUnit[{NS}current_idx].factor:10
    , LABEL({NS}appBaseUnit.use_divide) {NS}appBaseUnit[{NS}current_idx].use_divide
    , VOID(0, 5)
    , SEPARATOR
    , VOID(0, 5)
    , LABEL({NS}appBaseUnit.use_set) {NS}appBaseUnit[{NS}current_idx].use_set
    , VOID(0, 10)
    , {NS}appBaseUnitLibrary.unit_btn {COLSPAN=2}
    )
  ;

  FIELDGROUP
    {NS}unit_derived_fg {_("Derived Unit Properties")}(
      LABEL({NS}appBaseUnit.derived.name) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].name
    , LABEL({NS}appBaseUnit.derived.ui_name) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].ui_name
    , LABEL({NS}appBaseUnit.derived.factor) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].factor:10
    , LABEL({NS}appBaseUnit.derived.shift) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].shift:10
    , LABEL({NS}appBaseUnit.derived.use_divide) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].use_divide
    , VOID(0, 5)
    , SEPARATOR
    , VOID(0, 5)
    , LABEL({NS}appBaseUnit.derived.use_set) {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].use_set
    , VOID(0, 10)
    , {NS}appBaseUnitLibrary.derived_unit_btn {COLSPAN=2}
    )
  ;

  FORM
    {NS}unit_manager_form
      {_("Unit Manager"),
    FUNC={NS}unit_manager_form_func,
    HIDDEN,
    APP_MODAL,
    HIDE_CYCLE}
    (
      ({NS}unit_list,
      ({NS}unit_fg,
      ({NS}unit_derived_list, {NS}unit_derived_fg))),
      {NS}unit_common_fg
    )
  ;
MENU
  {NS}unit_list (
    FUNC {NS}unit_list_delete_func = "Delete Unit"
  ),
  {NS}unit_derived_list (
    FUNC {NS}unit_derived_list_delete_func = "Delete Unit"
  )
;
END UI_MANAGER;

STREAMER
  unit_manager_stream {JSON} ({NS}appBaseUnit);
END STREAMER;

OPERATOR
  FILESTREAM
    {NS}unit_manager_fs = unit_manager_stream{"Unit Manager Filestream"};

END OPERATOR;

FUNCTIONS
  FUNC
    {NS}appBaseUnit_func{
      IF (REASON_SELECT || REASON_ACTIVATE) {
        {NS}current_idx = INDEX;
        {NS}derived_current_idx = 0;
        SELECT_LIST({NS}unit_derived_list, 0);
        RUN({NS}unit_use_brackets_func);
      }
    }
  ;
  FUNC
    {NS}appBaseUnit_derived_func{
      IF (REASON_SELECT || REASON_ACTIVATE) {
        {NS}derived_current_idx = INDEX;
        RUN({NS}unit_use_brackets_func);
      }
    }
  ;
  FUNC
    {NS}unit_manager_form_func{
      INTEGER i;
      IF (REASON_OPEN) {
        {NS}filename = RESOURCE("APPHOME") + "/etc/application_unit.json";
        OPEN({NS}unit_manager_fs, {NS}filename);

        // use flags
        i = 0;
        WHILE(VALID({NS}appBaseUnit[i])){
          IF (!VALID({NS}appBaseUnit[i].use_set)){
            {NS}appBaseUnit[i].use_set = 1;
          }
          i++;
        }

        // set defaults
        IF (VALID({NS}appBaseUnit[0]) &&
           {NS}appBaseUnit[0].name != {NS}appBaseUnit[0].ui_name) {
          {NS}appBaseUnitLibrary.use_brackets = 1;
        }
        {NS}current_idx = 0;
        {NS}derived_current_idx = 0;
        SELECT_LIST({NS}unit_list, 0);
        SELECT_LIST({NS}unit_derived_list, 0);
        RUN({NS}unit_use_brackets_func);
      }
      IF (REASON_CLOSE) {
        SAVE({NS}unit_manager_fs, {NS}filename);
      }
    }
  ;
  FUNC
    {NS}unit_btn_func{
      SIZE({NS}appBaseUnit[*], {NS}unit_size);
      IF ({NS}unit_size==0) {
        {NS}current_idx = 0;
      }ELSE{
        {NS}current_idx = {NS}unit_size;
      }
      RUN({NS}unit_use_brackets_func);
      {NS}appBaseUnit[{NS}current_idx].use_set = 1;
    }
  ;
  FUNC
    {NS}derived_unit_btn_func {
      SIZE({NS}appBaseUnit[{NS}current_idx].derived[*], {NS}unit_size);
      IF ({NS}unit_size==0) {
        {NS}derived_current_idx = 0;
      }ELSE{
        {NS}derived_current_idx = {NS}unit_size;
      }
      RUN({NS}unit_use_brackets_func);
      {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].use_set = 1;
    }
  ;
  FUNC
    {NS}unit_list_delete_func{
      GET_SELECTION({NS}unit_list, {NS}unit_selected);
      CLEAR({NS}appBaseUnit[{NS}unit_selected]);
      PACK({NS}appBaseUnit[*]);
      {NS}derived_current_idx = 0;
    }
  ;
  FUNC
    {NS}unit_derived_list_delete_func{
      GET_SELECTION({NS}unit_derived_list, {NS}unit_selected);
      CLEAR({NS}appBaseUnit[{NS}current_idx].derived[{NS}unit_selected]);
      PACK({NS}appBaseUnit[{NS}current_idx].derived[*]);
      {NS}derived_current_idx = 0;
    }
  ;
  FUNC
    {NS}unit_use_brackets_func{
      IF ({NS}appBaseUnitLibrary.use_brackets != 1){
        ENABLE({NS}appBaseUnit[{NS}current_idx].ui_name);
        ENABLE({NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].ui_name);
      }ELSE{
        DISABLE({NS}appBaseUnit[{NS}current_idx].ui_name);
        DISABLE({NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].ui_name);
      }
    }
  ;
  FUNC
    {NS}BaseUnit_name_func{
      IF ({NS}appBaseUnitLibrary.use_brackets != 1) { RETURN; }
      IF (NODE(THIS) == "derived"){
        {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].ui_name = COMPOSE("[%1]", {NS}appBaseUnit[{NS}current_idx].derived[{NS}derived_current_idx].name);
      }ELSE{
        {NS}appBaseUnit[{NS}current_idx].ui_name = COMPOSE("[%1]", {NS}appBaseUnit[{NS}current_idx].name);
      }
    }
  ;
END FUNCTIONS;
