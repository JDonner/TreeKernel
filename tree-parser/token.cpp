#include "token.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, Token const& tok)
{
//   std::string const& tname = s_token_names[tok.type];
   os << "<'" << tok.text << "',";// << tname << ">";
   return os;
}
