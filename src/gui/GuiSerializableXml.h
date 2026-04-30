
// SPDX-FileCopyrightText: 2025 SEMAFOR Informatik & Energie AG, Basel
// SPDX-License-Identifier: Apache-2.0


#if !defined(GUISERIALIZABLE_H)
#define GUISERIALIZABLE_H

#include <iostream>

class GuiSerializableToXml {
public:
  virtual void serialize( std::ostream &os )=0;
};

#endif