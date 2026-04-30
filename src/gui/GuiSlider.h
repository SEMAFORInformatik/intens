
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(GUI_SLIDER_INCLUDED_H)
#define GUI_SLIDER_INCLUDED_H

class GuiElement;

class GuiSlider{

public:
  virtual GuiElement * getElement() = 0;
};

#endif