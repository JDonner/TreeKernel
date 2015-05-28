#ifndef SENTENCE_H
#define SENTENCE_H

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

#include <vector>
#include <map>
#include <string>
#include <iosfwd>
#include <stdint.h>

#include "node.h"

#include "tree.h"

// The style of node this uses. I think it's Penn Treebank -
// whatever the Stanford Parser uses.

// Must have Java.
// (S
//   (VP (MD Must)
//       (VP (VB have)
//           (NP (NN java))))
//   (. .))

// Never has the term 'massaging the media' seemed so accurate.
// (S
//   (NP (NNP Never))
//   (VP (VBZ has)
//     (NP
//       (NP (DT the) (NN term))
//       (SBAR
//         (S ("" "")
//           (S
//             (VP (VBG massaging)
//               (NP (DT the) (NNS media)))) ('' '')
//           (VP (VBD seemed)
//             (ADJP (RB so) (JJ accurate))))))) (. .))

// Improvements:
// * Put Nodes in-place in a vector, and use indices as pointers.
// * Templatize that choice.

class Sentence
{
   friend std::ostream& operator<<(std::ostream& os, Sentence const& s);

public:
   Sentence(std::string const& sentence)
   : text_(sentence)
   , root_(parse(sentence))
   {}

   Sentence(Tree const* tree);

   Node* copy_from(Tree const* tree);

   typedef std::vector<Node*> Nodes;

   Nodes const& grouped_nodes() const { return grouped_nodes_; }

private:
   void group_nodes();

   // &&& we do parsing elsewhere, and use copy_from
   static Node const* parse(std::string const& text);

private:
   /// original text, for no good reason
   std::string text_;

   /// top of tree of nodes
   Node const* root_;

   /// Sorted, grouping same-production nodes. It's the grouped-ness
   /// that matters.
   Nodes grouped_nodes_;
};

std::ostream& operator<<(std::ostream& os, Sentence const& s);

#endif // SENTENCE_H
