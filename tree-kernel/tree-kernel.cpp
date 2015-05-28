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

#include "node.h"
#include "tree-kernel.h"

#include <map>
#include <cassert>
#include <iostream>
#include "sentence.h"

using namespace std;

typedef std::pair<Node const*, Node const*> NodePair;
typedef std::vector<NodePair> NodePairs;

typedef std::map<NodePair, double> NodePairsDeltaTable;

static
double preterminal_leaves_value(bool include_leaves, double lambda)
{
   return include_leaves ? lambda * 2 : lambda;
}

static
double& delta_ref_at(NodePairsDeltaTable& delta_table,
                     Node const* n1, Node const* n2)
{
   return delta_table[make_pair(n1, n2)];
}


/// Memo-ized dynamic programming
static
double get_delta(NodePairsDeltaTable& delta_table,
                 Node const* n1, Node const* n2,
                 int sigma,
                 bool include_leaves,
                 // use a lambda = 1.0 to give it no effect
                 double lambda)
{
   assert((sigma == 0 or sigma == 1) or
          !"sigma isn't tunable, it's got to be 0 or 1; it's a choice of algorithm");
   double& ref_delta = delta_ref_at(delta_table, n1, n2);
   if (ref_delta == 0.0) {
      // needs calculating - all nodes we'll see are > 0.
      if (n1->is_preterminal() and n2->is_preterminal()) {
         // We're treating the tree purely structurally, or by
         // part-of-speech by doing this...
         // wait a minute - 'include_leaves' is done ahead of time..
         ref_delta = preterminal_leaves_value(include_leaves, lambda);
      }
      else {
         ref_delta = lambda;
         Node::Nodes n1_children = n1->children();
         Node::Nodes n2_children = n2->children();
         // non-pre-terminals
         for (Node::Nodes::const_iterator
                 it1 = n1_children.begin(), end1 = n1_children.end(),
                 it2 = n2_children.begin(), end2 = n2_children.end();
              it1 != end1; ++it1, ++it2) {
            ref_delta *= sigma + get_delta(delta_table, *it1, *it2,
                                           sigma, include_leaves, lambda);
         }
      }
   }
   return ref_delta;
}


/// Pick matches out of sparse cross-product
static
NodePairs find_non_zero_delta_pairs(
   Sentence const& t1, Sentence const& t2,
   // Starts filling this in, too, since it's iterating across everything already
   NodePairsDeltaTable& node_pair_deltas,
   bool include_leaves,
   double decay_lambda)
{
   // These want to be topologically sorted, but it's too expensive,
   // as we do them pair-by-pair; we can't pre-do it.
   NodePairs node_pairs;

   // Sorted so that the same-production nodes are together.
   Sentence::Nodes::const_iterator i1 = t1.grouped_nodes().begin(),
      end1 = t1.grouped_nodes().end();
   Sentence::Nodes::const_iterator i2 = t2.grouped_nodes().begin(),
      end2 = t2.grouped_nodes().end();

   while (i1 != end1 and i2 != end2) {
      int cmp = Node::productions_cmp(**i1, **i2);
      if (0 < cmp) {
         ++i2;
      }
      else if (cmp < 0) {
         ++i1;
      }
      // they're equal; the interesting part
      else {
         Sentence::Nodes::const_iterator run2_start = i2;
         Node const* n1 = *i1;
         Node const* n2 = *i2;
         assert(n1);
         assert(n2);
         // run along the 'runs'
         while (i2 != end2 and Node::productions_equal(*i1, *i2)) {
            assert(*i1);
            assert(*i2);
            node_pairs.push_back(make_pair(*i1, *i2));

            // Fill in table of pre-terminals while we're here
            if ((*i1)->is_preterminal() and (*i2)->is_preterminal())
               delta_ref_at(node_pair_deltas, *i1, *i2) =
                  preterminal_leaves_value(include_leaves, decay_lambda);
            else
               delta_ref_at(node_pair_deltas, *i1, *i2) = 0.0;

            ++i2;
         }
         i2 = run2_start;
         ++i1;
      }
   }

   // Would sort anti-topologically but it'd have to be done for each
   // /pair/ and so can't be done ahead of time. Although, if we
   // sorted them topo-wise /per tree/ ahead of time, maybe it'd
   // make the pairwise topo-sort fast enough to be worth it. Hmmm.
   // Still,
   return node_pairs;
}

/// See Alessandro Moschitti, "Making Tree Kernels Practical for
///   Natural Language Learning" (2006)
/// With sigma = 0, this calculates the subtree (ST) kernel (always
///   includes all the way down to the leaves)
/// With sigma = 1, calculates SSTs - includes every connected fragment
///   of the tree, non-leaf nodes included.
double kernel_value(Sentence const& t1, Sentence const& t2,
                    bool want_sst_not_st, bool include_leaves,
                    // use decay_lambda = 1.0 for effectively no lambda
                    double decay_lambda)
{
   int sigma = want_sst_not_st ? 1 : 0;
   NodePairsDeltaTable delta_table;
   NodePairs node_pairs = find_non_zero_delta_pairs(
      t1, t2,
      // for some opportunistic premature optimization.
      delta_table, include_leaves, decay_lambda);

   double kernel = 0.0;
   for (NodePairs::const_iterator it = node_pairs.begin(), end = node_pairs.end();
        it != end; ++it) {
      Node const* n1 = it->first;
      Node const* n2 = it->second;
      double delta = get_delta(delta_table, n1, n2, sigma, include_leaves, decay_lambda);
      kernel += delta;
   }

   return kernel;
}
