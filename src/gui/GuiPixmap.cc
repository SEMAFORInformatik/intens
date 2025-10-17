
#include <string>
#include "gui/GuiPixmap.h"

GuiPixmap::GuiPixmap() {
}

// GuiPixmap::GuiPixmap( const GuiPixmap &field ) {}
GuiPixmap::~GuiPixmap() {
}

bool GuiPixmap::isTypeSvg(const std::string& data) {
  if (data.find("<svg ") >= 0 &&
      data.find("</svg>") != std::string::npos) {
    return true;
  }
  return false;
}
