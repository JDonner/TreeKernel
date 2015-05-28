#ifndef PARSER_H
#define PARSER_H

/***
 * Excerpted from "Language Implementation Patterns",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material,
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose.
 * Visit http://www.pragmaticprogrammer.com/titles/tpdsl for more book information.
***/

#include "token.h"
#include "lexer.h"


class Parser {
public:
   Parser(Lexer& lexer)
   : lexer(lexer)
   {
     consume();
   }
   /** If lookahead token type matches x, consume & return else error */
   void match(int x) {
      if ( lookahead.type == x )
         consume();
      else
         throw std::string("expecting ") + "<<SOMETOKEN>>" /*+ lexer.get_token_name(x)*/ +
            "; found " + lookahead.text;
   }
   void consume() {
      lookahead = lexer.next_token();
   }

protected:
   // source of tokens
   Lexer& lexer;
   // the current lookahead token
   Token lookahead;
};

#endif // PARSER_H
