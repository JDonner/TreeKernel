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
   s.root_->pretty_print(os, 0);
   os << "# grouped nodes: " << s.grouped_nodes_.size();
   return os;
}
