set(STATUS_FILES experimental.png
  inPreparation.png
  shared.png
  approved.png
  tendered.png
  tested.png
  obsolete.png
)

### make install ###
install(FILES @@{STATUS_FILES}
  DESTINATION bitmaps/icons/status
)
