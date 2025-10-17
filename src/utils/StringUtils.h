
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>

template <class charT, class traits, class Allocator>
inline std::basic_string<charT, traits, Allocator>
upper(const std::basic_string<charT,traits, Allocator>& str) {
      std::basic_string<charT, traits, Allocator> newstr(str);
  for (size_t index = 0; index < str.length(); index++)
    if (islower(str[index]))
      newstr[index] = toupper(str[index]);
  return newstr;
}

template <class charT, class traits, class Allocator>
inline std::basic_string<charT, traits, Allocator>
lower(const std::basic_string<charT,traits, Allocator>& str) {
      std::basic_string<charT, traits, Allocator> newstr(str);
  for (size_t index = 0; index < str.length(); index++)
    if (isupper(str[index]))
      newstr[index] = tolower(str[index]);
  return newstr;
}

std::string trim(std::string& s, std::string add_chars="");
void removeString(std::string& s, std::string rs);
std::string removeString2(std::string s, std::string rs);
void removeFontString(std::string& s);
bool toBool(const std::string& s);

// split functions
std::vector<std::string> &split(const std::string &s,  const std::string& delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s,  const std::string& delim);
void join(const std::vector<std::string>& v, char c, std::string& s, int pos=0);

void replaceAll(std::string& source, const std::string& from, const std::string& to);

/** XSS (Cross-Site-Scripting) String Validation
 */
bool XSSValidation(std::string& s);
#endif
