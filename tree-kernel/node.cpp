#include "node.h"
#include <cassert>
#include <iostream>
#include <sstream>

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
