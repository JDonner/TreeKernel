#ifndef TREE_LEXER_H
#define TREE_LEXER_H

/***
 * Excerpted from "Language Implementation Patterns",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material,
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose.
 * Visit http://www.pragmaticprogrammer.com/titles/tpdsl for more book information.
***/

#include <cassert>
#include <cctype>
#include <iosfwd>
#include <sstream>
#include <string>

#include "lexer.h"


class TreeLexer : public Lexer
{
public:
   enum {
      ERROR_TYPE =-1,
      // EOS_TYPE defined in Lexer
      NAME       = 1,
      LPAREN     = 2,
      RPAREN     = 3,
      UNKNOWN
   };

   TreeLexer(std::string const& input)
   : Lexer(input)
   {
//      cout << "lexer, starting:" << this->c << endl;
   }

   bool isLETTER() {
      return isalpha(c) or c == '.' or c == '\'' or c == '`';
   }

//   virtual std::string const& getTokenName(int x) const { return Lexer::s_token_names[x]; }

   virtual Token next_token() {
//      cout << "lexer::next_token c:[" << this->c << "]" << endl;
      while ( this->c != Lexer::EOS ) {
         if (isspace(this->c)) {
            this->WS();
            continue;
         }

         switch ( this->c ) {
         case '(' : consume(); return Token(LPAREN, "(");
         case ')' : consume(); return Token(RPAREN, ")");
         default:
           if ( isLETTER() ) {
               return name();
           } else {
               std::ostringstream oss;
               oss << "invalid character: [" << this->c << "]";
               throw oss.str();
           }
         }
      }
      return Token(EOS_TYPE, "<EOS>");
   }

   /** NAME : LETTER+ ; // NAME is sequence of >=1 letter */
   // cleans up rest of name, detected by an alpha char.
   // Like '(', but it takes more characters
   Token name() {
      std::ostringstream oss;
      do {
         oss << this->c; letter();
      }
      while ( isLETTER() );
      Token tok(NAME, oss.str());
      return tok;
   }

   /** LETTER   : 'a'..'z'|'A'..'Z'; // define what a letter is (\w) */
   void letter() {
      if (isLETTER())
         consume();
      else {
         std::ostringstream oss;
         oss << "expecting LETTER; found [" << this->c << "]";
         throw oss.str();
      }
   }

   /** WS : (' '|'\t'|'\n'|'\r')* ; // ignore any whitespace */
   void WS() {
      while ( isspace(this->c) )
         consume();
   }

   static std::string s_token_names[];
};

#endif // TREE_LEXER_H
