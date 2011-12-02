#ifndef SENTENCE_H
#define SENTENCE_H

/*
   BSD 2-clause license ("Simplified BSD License" or "FreeBSD License")

  Copyright 2011 Jeff Donner. All rights reserved.

  Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Jeff Donner ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Jeff Donner OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation
are those of the authors and should not be interpreted as representing
official policies, either expressed or implied of Jeff Donner.
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
