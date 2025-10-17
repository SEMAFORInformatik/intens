add_subdirectory(status)

set(ICONS_FILES variant.png)

### make install ###
install(FILES @@{ICONS_FILES}
  DESTINATION bitmaps/icons
)
