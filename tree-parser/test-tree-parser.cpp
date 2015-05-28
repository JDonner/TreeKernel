#include "tree_parser.h"


string g_pretty =
"(ROOT"
"  (S"
"    (NP (NNP Never))"
"    (VP (VBZ has)"
"      (NP"
"        (NP (DT the) (NN term))"
"        (SBAR"
"          (S (`` ``)"
"            (S"
"              (VP (VBG massaging)"
"                (NP (DT the) (NNS media))))"
"            ('' '')"
"            (VP (VBD seemed)"
"              (ADJP (RB so) (JJ accurate)))))))"
"    (. .)))"
   ;

string g_simple =
   "(tag elt)";

string g_flat =
   "(a b c d)";

string g_small =
   "(a b (c (d (e f))))";


string g_real =
   "(ROOT (S (VP (MD must) (VP (VB have) (NP (NN java)))) (. .)))";

void lex_only(string tree_text, string tree_name)
{
   cout << "lexing..." << tree_name << endl;
   TreeLexer lexer(tree_text);
   Token tok = lexer.next_token();
   int x = 0;
   while (tok.type != TreeLexer::EOF_TYPE and x < 10) {
      cout << "[" << tok.text << "] ";
      tok = lexer.next_token();
      ++x;
   }
   cout << endl;
}

void test_tree(string tree_text, string tree_name)
{
   cout << '\n' << tree_name << endl;
   //   lex_only(tree_text, tree_name);
   TreeLexer lexer(tree_text);
   TreeParser parser(lexer);
   Tree const* t = 0;
   try {
      t = parser.match_and_eat_tree();
   }
   catch (string& ex) {
      cout << "failed parse: " << ex << endl;
   }

   if (t) {
//      cout << "printing in order:" << endl;
//      t->print_inorder(cout);
      cout << "printing nice:" << endl;
      t->print_nice(cout, 0);
   }
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;
//   test_tree(g_simple, "test simple");
   test_tree(g_flat, "test flat");
   test_tree(g_small, "test small");
   test_tree(g_real, "test real");
   test_tree(g_pretty, "test pretty");

   return 0;
}
