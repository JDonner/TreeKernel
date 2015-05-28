#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <set>
#include <ostream>


class Tree {
public:
   typedef std::string TagType;

public:
   Tree(std::string const& tag)
   : tag_(tag)
   {}

   struct Element {
      Element(Tree* tree)
      : tree_(tree)
      {}

      Element(std::string const& text)
      : tok_(text)
      , tree_(0)
      {}

      enum TreeType {
         eTree,
         eText,
         eUnknown
      };

      bool is_tree() const { return tree_ != 0; }
      bool is_text() const { return !is_tree(); }

      std::string const& text() const { return this->tok_; }
      Tree* tree() const { return this->tree_; }

   public:
      // wants to be a union, but std::string has a ctor
      std::string tok_;
      Tree* tree_;
   };
   typedef std::vector<Element> Elements;

   Elements const& children() const { return children_; }

   void add_child(Tree::Element const& child) {
      children_.push_back(child);
   }

   TagType const& tag() const { return this->tag_; }

   void print_inorder(std::ostream& os) const;

   // wait a minnit, I don't need to print it nice, it'll come across the wire
   // that way... -- no it doesn't.
   // break at every NP, PP, VP, S, :, ,, CC
   virtual void print_nice(std::ostream& os, int level) const;

private:
   static void indent(std::ostream& os, int level) {
      for (int i = 0; i < level; ++i)
         os << "  ";
   }

   static bool should_indent_tag(char const* tag);

private:
   TagType tag_;
   Elements children_;
   static char const* s_tags_to_indent[];
//   static std::set<char const*> s_set_tags_to_indent;
};

#endif // TREE_H
