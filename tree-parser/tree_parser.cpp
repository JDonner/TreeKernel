#include "tree_parser.h"
#include <string>
#include <sstream>

using namespace std;


std::string TreeParser::match_and_eat_name()
{
   Token tok = lookahead;
   consume();
   return tok.text;
}


Tree::Element TreeParser::match_and_eat_element()
{
   if (lookahead.type == TreeLexer::NAME) {
      Tree::Element name = Tree::Element(match_and_eat_name());
      return name;
   }
   else if (lookahead.type == TreeLexer::LPAREN) {
      Tree::Element tree = Tree::Element(match_and_eat_tree());
      return tree;
   }
   else {
      throw std::string("expecting tok or tree; found:") + "<<SOMETOK>>"; //Lexer.names_by_type[lookahead.type];
      // wtf gcc, requiring this
      // invalid, but will never happen
      return Tree::Element(0);
   }
}

void TreeParser::match_and_eat_elements()
{
   Tree::Element tok_or_tree = match_and_eat_element();
   current_open_tree().add_child(tok_or_tree);
   while (lookahead.type != TreeLexer::RPAREN) {
      tok_or_tree = match_and_eat_element();
      current_open_tree().add_child(tok_or_tree);
   }
}

Tree* TreeParser::match_and_eat_tree()
{
   match(TreeLexer::LPAREN);
   // open tree;
   Tree::TagType tag = match_and_eat_name();
   open_node(tag);
   // push onto open tree stack

   match_and_eat_elements();
   // append to open tree
   // close tree
   match(TreeLexer::RPAREN);
   return close_node();
}


void TreeParser::match(int token_type)
{
   if ( this->lookahead.type == token_type )
      consume();
   else
      throw std::string("expecting ") + "<<SOMETOKEN>>" /*+ lexer.get_token_name(x)*/ +
         "; found " + lookahead.text;
}
