#ifndef TREE_PARSER_H
#define TREE_PARSER_H

/***
 * Excerpted from "Language Implementation Patterns",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material,
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose.
 * Visit http://www.pragmaticprogrammer.com/titles/tpdsl for more book information.
***/

#include "tree_lexer.h"
#include "tree.h"

#include <vector>
#include <iostream>
using namespace std;


class TreeParser {
public:
   TreeParser(TreeLexer& input)
   : lexer(input)
   { consume(); }

   Tree* match_and_eat_tree();

private:
   Tree::Element match_and_eat_element();

   Tree& current_open_tree() {
      return *open_nodes.back();
   }

   void match_and_eat_elements();

   std::string match_and_eat_name();


   /// Upon a '(', start a new [sub]tree
   void open_node(Tree::TagType tag) {
      Tree* t = new Tree(tag);
      open_nodes.push_back(t);
   }

   /// Upon a ')', close most-open tree
   Tree* close_node() {
      Tree* t = open_nodes.back();
      open_nodes.pop_back();
      return t;
   }

   // 'advance' might be a better name
   void consume() {
      this->lookahead = lexer.next_token();
   }

   /// insist on, but discard, the next token
   void match(int token_type);

private:
   std::vector<Tree*> open_nodes;
   TreeLexer& lexer;
   Token lookahead;
};

#endif // TREE_PARSER_H
