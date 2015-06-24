/*
Copyright (c) 2011,2015 Jeff Donner

    Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

    The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string>
#include "tree_parser.h"
#include "tree-kernel.h"
#include "sentence.h"

using namespace std;

// g++ -g .objs/node.o .objs/sentence.o .objs/tree-kernel.o .objs/test-tree-kernel.o -L../tree-parser -ltree_parser -o test-tree-kernel

string g_real =
   "(ROOT (S (VP (MD Must) (VP (VB have) (NP (NN java)))) (. .)))";

// oversimple parse
string g_dog_eat_dog =
"(S"
"  (NN dog)"
"  (VBP eat)"
"  (NN dog))";

// oversimple parse
string g_dog_eat_fish =
"(S"
"  (NN dog)"
"  (VBP eat)"
"  (NN fish))";

string g_brought_a_cat =
" (VP (V brought)"
"     (NP (D a)"
"         (N cat)))"
   ;

Tree const* make_tree(string const& tree_text)
{
   TreeLexer lexer(tree_text);
   TreeParser parser(lexer);
   Tree const* t = 0;
   try {
      t = parser.match_and_eat_tree();
   }
   catch (string& ex) {
      cout << "failed parse: " << ex << endl;
   }

//   t->pretty_print(cout, 0);

   return t;
}

double kernel_value(string one, string two, bool want_sst_not_st)
{
   Tree const* t1 = make_tree(one);
   Tree const* t2 = make_tree(two);

   cout << "s1" << endl;
   Sentence s1(t1);
   cout << "s2" << endl;
   Sentence s2(t2);

   // sigma == 1 is SSTs (fragments)
   // sigma == 0 is STs (whole sub-trees, down to leaves)
   int sigma = want_sst_not_st ? 1 : 0;
   double value = kernel_value(s1, s2, sigma);
   return value;
}

void test_sst(string test_name, string one, string two, double expected)
{
   cout << test_name << endl;
   double value = kernel_value(one, two, true);
   if (value != expected) {
      cout << "failed - got: " << value << " instead of: " << expected << endl;
   }
   else
      cout << " ok" << endl;
}

void test_st(string test_name, string one, string two, double expected)
{
   cout << test_name << endl;
   double value = kernel_value(one, two, false);
   if (value != expected) {
      cout << "failed - got: " << value << " instead of: " << expected << endl;
   }
   else
      cout << " ok" << endl;
}

/* Yo - to test better, we need more sub-tree'd sentences.

   def generate_all_connected_subtrees(node, path-so-far):
       generate powerset of all children of node
       with the empty subset, add path-so-far as a complete subtree

       generate_all_connected_subtrees(child, path-so-far + child)

   def generate all subtrees(root):
       for node in all root:
           generate_all)connected_subtrees(node, null)

 */

int main()
{
   // SSTs are like STs, except that you can
   // 'break a single leg off of' a pre-terminal, and all off of
   // an ordinary non-terminal (because it's really just acting as
   // a child of its parent's rule). So if you include a production
   // you must include it all. Pre-terminals only /have/ one 'leg',
   // so they're included automatically.
   //
   // '17' is from the paper
   test_sst("brought a cat", g_brought_a_cat, g_brought_a_cat, 17);
   // hand-counted; number of non-terminal nodes
   test_st("brought a cat", g_brought_a_cat, g_brought_a_cat, 5);

   return 0;
}
