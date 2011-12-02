#include "node.h"
#include <cassert>
#include <iostream>
#include <sstream>

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

using namespace std;


bool Node::is_preterminal() const
{
   return children_.size() == 1 and children_[0].is_text();
}

int Node::productions_cmp(Node const& one, Node const& two)
{
   for (Node::Children::const_iterator i1 = one.children_.begin(), e1 = one.children_.end(),
           i2 = two.children_.begin(), e2 = two.children_.end();
        i1 != e1 and i2 != e2; ++i1, ++i2) {
      int cmp = Child::production_component_cmp(*i1, *i2);
      if (cmp)
         return cmp;
   }
   return one.children_.size() - two.children_.size();
}

bool Node::productions_equal(Node const* one, Node const* two)
{
//   cout << "nodes=?" << one->id_string() << "; " << two->id_string();
   // hoping for short-circuit speed, obviously
   bool equal = one->children_.size() == two->children_.size() and
      productions_cmp(*one, *two) == 0;
//   cout << equal << endl;
   return equal;
}

bool Node::Child::production_component_is_less(Node::Child const& one,
                                               Node::Child const& two)
{
   if (one.is_text() and two.is_text()) {
      return one.text() < two.text();
   }
   else if (one.is_text() != two.is_text()) {
      return one.is_text() and not two.is_text();
   }
   else {
      // both full nodes
      return productions_cmp(*one.node(), *two.node()) < 0;
   }
}

std::string Node::productions() const
{
   if (is_preterminal()) {
      return children_[0].text();
   }
   else {
      ostringstream oss;
      for (Node::Children::const_iterator it = children_.begin(), end = children_.end();
           it != end; ++it) {
         assert((*it).is_node());
         oss << (*it).node()->tag_ << " ";
      }
      return oss.str();
   }
}

string Node::id_string() const
{
   ostringstream oss;
   oss << this->tag_ << ":" << this->productions();
   return oss.str();
}
//
Node::Nodes Node::children() const
{
   Nodes childs;
   for (Children::const_iterator it = children_.begin(), end = children_.end();
        it != end; ++it) {
      if (it->is_node()) {
         assert(it->node());
         childs.push_back(it->node());
      }
      else {
         assert(false);
      }
   }
   return childs;
}

bool Node::production_is_less(Node const* one, Node const* two)
{
   for (Node::Children::const_iterator i1 = one->children_.begin(), e1 = one->children_.end(),
           i2 = two->children_.begin(), e2 = two->children_.end();
        i1 != e1 and i2 != e2; ++i1, ++i2) {
      int cmp = Child::production_component_cmp(*i1, *i2);
      return cmp < 0;
   }
   return ((int)one->children_.size() - (int)two->children_.size()) < 0;
}


void Node::pretty_print(std::ostream& os, int level) const
{
   os << std::endl;
   indent(os, level);
   os << '(' << this->tag_;
   for (Children::const_iterator it = this->child_links().begin(), end = this->child_links().end();
        it != end; ++it) {
      os << ' ';
      if (it->is_text())
         os << it->text();
      else
         it->node()->pretty_print(os, level+1);
   }

   os << ')';
   if (level == 0)
      os << std::endl;
}

void Node::indent(std::ostream& os, int level)
{
   for (int i = 0; i < level; ++i)
      os << "  ";
}
