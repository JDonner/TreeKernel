#include "tree.h"
// for strcmp
#include <cstring>
#include <algorithm>

using namespace std;

char const* Tree::s_tags_to_indent[] = {
   // must be kept sorted
",",
"."
":",
"ADJP",
"CC",
"NP",
"PP",
"S",
"SBAR",
"VP"
};


static bool text_is_less(char const* one, char const* two)
{
   return strcmp(one, two) < 0;
}

bool Tree::should_indent_tag(char const* tag)
{
   return binary_search (s_tags_to_indent,
                         s_tags_to_indent + sizeof(Tree::s_tags_to_indent) / sizeof(Tree::s_tags_to_indent[0]),
                         tag, text_is_less);
}


//Tree::IndentTagsIniter(char const* strings, unsigned n_strings) {
//   sort(s_tags_to_indent, s_tags_to_indent + n_strings, text_is_less);
//}

//Tree::IndentTagsIniter init(Tree::s_tags_to_indent,
//                            sizeof(Tree::s_tags_to_indent) / sizeof(Tree::s_tags_to_indent[0]));

void Tree::print_inorder(std::ostream& os) const
{
   os << this->tag() << ": ";
   for (Elements::const_iterator it = this->children().begin(), end = this->children().end();
        it != end; ++it) {
      if (it->is_text())
         os << it->text() << " ";
      else
         it->tree()->print_inorder(os);
   }
   os << std::endl;
}


void Tree::print_nice(std::ostream& os, int level) const
{
   if (should_indent_tag(this->tag().c_str())) {
      os << std::endl;
      indent(os, level);
   }
   os << '(' << this->tag();
   for (Elements::const_iterator it = this->children().begin(), end = this->children().end();
        it != end; ++it) {
      os << ' ';
      if (it->is_text())
         os << it->text();
      else
         it->tree()->print_nice(os, level+1);
   }

   os << ')';
   if (level == 0)
      os << std::endl;
}
