add_subdirectory(icons)

set(BITMAPS_FILES left.xpm
  logo.png
  new.png
  open.png
  remove.png
  right.xpm
  startup.png
  store.png
)

install(FILES ${BITMAPS_FILES}
  DESTINATION bitmaps
)
