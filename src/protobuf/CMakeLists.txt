
add_library(intens_protobuf OBJECT
    Plot2D.proto
    Plot3D.proto
    Buttonbar.proto
    Button.proto
    CycleButton.proto
    ComboBox.proto
    Container.proto
    FieldGroup.proto
    Folder.proto
    Form.proto
    Header.proto
    Index.proto
    IndexMenu.proto
    Label.proto
    List.proto
    Menubar.proto
    MenuButton.proto
    Messagebar.proto
    Navigator.proto
    Progressbar.proto
    Radio.proto
    Separator.proto
    Stretch.proto
    Table.proto
    Text.proto
    Datafield.proto
    Thermo.proto
    Toggle.proto
    Void.proto
    GuiElement.proto
    Message.proto)

set_property(TARGET intens_protobuf PROPERTY AUTOMOC OFF)

protobuf_generate(
    TARGET intens_protobuf
    LANGUAGE cpp)

pkg_check_modules(ABSL absl_log_internal_check_op)
target_link_libraries(intens_protobuf ${ABSL_LIBRARIES})
