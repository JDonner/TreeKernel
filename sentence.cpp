/* Sorry for the boilerplate - I understand you can be sued if you don't
   disclaim warranty.

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

#include "sentence.h"
#include <algorithm>
#include <ostream>
#include <iostream>
#include <cassert>


using namespace std;

Node* Sentence::copy_from(Tree const* tree)
{
   Node* node = new Node(tree->tag());
   grouped_nodes_.push_back(node);
   string production;
   if (not tree->children().empty()) {
      for (Tree::Elements::const_iterator it = tree->children().begin(),
              end = tree->children().end();
           it != end; ++it) {
         Tree::Element const& elt = *it;
         if (elt.is_text()) {
            node->add_child(elt.text());
         }
         else {
            Node* child = Sentence::copy_from(elt.tree());
            node->add_child(child);
         }
      }
   }
   return node;
}

Sentence::Sentence(Tree const* tree)
{
   root_ = copy_from(tree);

   group_nodes();
}

void Sentence::group_nodes()
{
   sort(grouped_nodes_.begin(), grouped_nodes_.end(),
        Node::production_is_less);
}

std::ostream& operator<<(std::ostream& os, Sentence const& s)
{
   os << "printing sentence:" << endl;
   s.root_->print_nice(os, 0);
   os << "# grouped nodes: " << s.grouped_nodes_.size();
   return os;
}
