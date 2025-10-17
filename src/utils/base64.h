#ifndef BASE64_H
#define BASE64_H

#include <string>

void base64encode(const unsigned char* input,size_t l,std::string& output, bool add_crlf);
bool base64decode(const std::string& input, unsigned char *output, size_t& sz);

#endif
