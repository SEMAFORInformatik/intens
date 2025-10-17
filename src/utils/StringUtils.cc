
#include <regex>

#include "utils/StringUtils.h"
#include "utils/Debugger.h"

INIT_LOGGER();

std::regex xss_re("<[^\\w<>]*(?:[^<>\\\"'\\s]*:)?[^\\w<>]*(?:\\W*s\\W*c\\W*r\\W*i\\W*p\\W*t|\\W*f\\W*o\\W*r\\W*m|\\W*s\\W*t\\W*y\\W*l\\W*e|\\W*s\\W*v\\W*g|\\W*m\\W*a\\W*r\\W*q\\W*u\\W*e\\W*e|(?:\\W*l\\W*i\\W*n\\W*k|\\W*o\\W*b\\W*j\\W*e\\W*c\\W*t|\\W*e\\W*m\\W*b\\W*e\\W*d|\\W*a\\W*p\\W*p\\W*l\\W*e\\W*t|\\W*p\\W*a\\W*r\\W*a\\W*m|\\W*i?\\W*f\\W*r\\W*a\\W*m\\W*e|\\W*b\\W*a\\W*s\\W*e|\\W*b\\W*o\\W*d\\W*y|\\W*m\\W*e\\W*t\\W*a|\\W*i\\W*m\\W*a?\\W*g\\W*e?|\\W*v\\W*i\\W*d\\W*e\\W*o|\\W*a\\W*u\\W*d\\W*i\\W*o|\\W*b\\W*i\\W*n\\W*d\\W*i\\W*n\\W*g\\W*s|\\W*s\\W*e\\W*t|\\W*i\\W*s\\W*i\\W*n\\W*d\\W*e\\W*x|\\W*a\\W*n\\W*i\\W*m\\W*a\\W*t\\W*e)[^>\\w])|(?:<\\w[\\s\\S]*[\\s\\0\\/]|['\\\"])(?:formaction|style|background|src|lowsrc|ping|on(?:d(?:e(?:vice(?:(?:orienta|mo)tion|proximity|found|light)|livery(?:success|error)|activate)|r(?:ag(?:e(?:n(?:ter|d)|xit)|(?:gestur|leav)e|start|drop|over)?|op)|i(?:s(?:c(?:hargingtimechange|onnect(?:ing|ed))|abled)|aling)|ata(?:setc(?:omplete|hanged)|(?:availabl|chang)e|error)|urationchange|ownloading|blclick)|Moz(?:M(?:agnifyGesture(?:Update|Start)?|ouse(?:PixelScroll|Hittest))|S(?:wipeGesture(?:Update|Start|End)?|crolledAreaChanged)|(?:(?:Press)?TapGestur|BeforeResiz)e|EdgeUI(?:C(?:omplet|ancel)|Start)ed|RotateGesture(?:Update|Start)?|A(?:udioAvailable|fterPaint))|c(?:o(?:m(?:p(?:osition(?:update|start|end)|lete)|mand(?:update)?)|n(?:t(?:rolselect|extmenu)|nect(?:ing|ed))|py)|a(?:(?:llschang|ch)ed|nplay(?:through)?|rdstatechange)|h(?:(?:arging(?:time)?ch)?ange|ecking)|(?:fstate|ell)change|u(?:echange|t)|l(?:ick|ose))|m(?:o(?:z(?:pointerlock(?:change|error)|(?:orientation|time)change|fullscreen(?:change|error)|network(?:down|up)load)|use(?:(?:lea|mo)ve|o(?:ver|ut)|enter|wheel|down|up)|ve(?:start|end)?)|essage|ark)|s(?:t(?:a(?:t(?:uschanged|echange)|lled|rt)|k(?:sessione|comma)nd|op)|e(?:ek(?:complete|ing|ed)|(?:lec(?:tstar)?)?t|n(?:ding|t))|u(?:ccess|spend|bmit)|peech(?:start|end)|ound(?:start|end)|croll|how)|b(?:e(?:for(?:e(?:(?:scriptexecu|activa)te|u(?:nload|pdate)|p(?:aste|rint)|c(?:opy|ut)|editfocus)|deactivate)|gin(?:Event)?)|oun(?:dary|ce)|l(?:ocked|ur)|roadcast|usy)|a(?:n(?:imation(?:iteration|start|end)|tennastatechange)|fter(?:(?:scriptexecu|upda)te|print)|udio(?:process|start|end)|d(?:apteradded|dtrack)|ctivate|lerting|bort)|DOM(?:Node(?:Inserted(?:IntoDocument)?|Removed(?:FromDocument)?)|(?:CharacterData|Subtree)Modified|A(?:ttrModified|ctivate)|Focus(?:Out|In)|MouseScroll)|r(?:e(?:s(?:u(?:m(?:ing|e)|lt)|ize|et)|adystatechange|pea(?:tEven)?t|movetrack|trieving|ceived)|ow(?:s(?:inserted|delete)|e(?:nter|xit))|atechange)|p(?:op(?:up(?:hid(?:den|ing)|show(?:ing|n))|state)|a(?:ge(?:hide|show)|(?:st|us)e|int)|ro(?:pertychange|gress)|lay(?:ing)?)|t(?:ouch(?:(?:lea|mo)ve|en(?:ter|d)|cancel|start)|ime(?:update|out)|ransitionend|ext)|u(?:s(?:erproximity|sdreceived)|p(?:gradeneeded|dateready)|n(?:derflow|load))|f(?:o(?:rm(?:change|input)|cus(?:out|in)?)|i(?:lterchange|nish)|ailed)|l(?:o(?:ad(?:e(?:d(?:meta)?data|nd)|start)?|secapture)|evelchange|y)|g(?:amepad(?:(?:dis)?connected|button(?:down|up)|axismove)|et)|e(?:n(?:d(?:Event|ed)?|abled|ter)|rror(?:update)?|mptied|xit)|i(?:cc(?:cardlockerror|infochange)|n(?:coming|valid|put))|o(?:(?:(?:ff|n)lin|bsolet)e|verflow(?:changed)?|pen)|SVG(?:(?:Unl|L)oad|Resize|Scroll|Abort|Error|Zoom)|h(?:e(?:adphoneschange|l[dp])|ashchange|olding)|v(?:o(?:lum|ic)e|ersion)change|w(?:a(?:it|rn)ing|heel)|key(?:press|down|up)|(?:AppComman|Loa)d|no(?:update|match)|Request|zoom))[\\s\\0]*=");

std::string trim(std::string& s, std::string add_chars) {
  size_t pos = s.find_last_not_of(" \n\r\t"+add_chars);
  if ( pos == std::string::npos ) { // only white space
    s = "";
    return s;
  }

  // right trim
  if ( pos+1 != s.size() ) {
    s.erase(pos+1);
  }

  // left trim
  pos = s.find_first_not_of(" \n\r\t"+add_chars);
  if ( pos != 0 ) {
    s.erase(0, pos);
  }
  return s;
}

void removeString(std::string& s, std::string rs) {
  size_t posB = 0;
  size_t posE = s.find(rs, posB);
  while(posE != std::string::npos) {
    s = s.replace(posE, rs.size(), "");
    posE = s.find(rs, posE);
  }
}

std::string removeString2(std::string s, std::string rs) {
  removeString(s, rs);
  return s;
}


void removeFontString(std::string& s) {
  const char FONT_SEPARATOR = '@';
  size_t posB = s.find(FONT_SEPARATOR, 0);
  size_t posE = s.find(FONT_SEPARATOR, posB+1);
  while(posB != std::string::npos && posE != std::string::npos) {
    s = s.replace(posB, posE-posB+1, "");
    posB = s.find(FONT_SEPARATOR, posB+1);
    posE = s.find(FONT_SEPARATOR, posB+1);
  }
}

bool toBool(const std::string& s) {
  return s != "0" && !s.empty() && lower(s) != "false";
}

std::vector<std::string> &split(const std::string &s,  const std::string& delim, std::vector<std::string> &elems) {
    std::string item;
    size_t posB = 0;
    size_t posE = s.find(delim, posB);
    while(posE != std::string::npos) {
      elems.push_back(s.substr(posB, posE-posB));
      posB = posE + delim.size();
      posE = s.find(delim, posB);
    };
    if (posB < s.size())
      elems.push_back(s.substr(posB));

    return elems;
}

std::vector<std::string> split(const std::string &s,  const std::string& delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

void join(const std::vector<std::string>& v, char c, std::string& s, int pos) {
   s.clear();
   for (std::vector<std::string>::const_iterator p = v.begin()+pos;
        p != v.end(); ++p) {
     s += *p;
     if (p != v.end() - 1)
       s += c;
   }
}

void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}

bool XSSValidation(std::string& input) {
  if (!getenv("XSS_OMIT_CHECK")) {
    std::smatch match;
    std::regex_search(input, match, xss_re);
    if (match.size()) {
      BUG_INFO("InvalidInput found: '" << input << "'");
      return false;
    }
    BUG_INFO("validInput found: '" << input << "'");
  }
  return true;
}
