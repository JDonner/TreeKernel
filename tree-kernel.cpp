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

   //cout << "list1" << endl;
   //for (Sentence::Nodes::const_iterator i1 = t1.grouped_nodes().begin(),
   //        end1 = t1.grouped_nodes().end(); i1 != end1; ++i1) {
   //   cout << (*i1)->id_string() << endl;
   //}
   //cout << "list2" << endl;
   //for (Sentence::Nodes::const_iterator i2 = t2.grouped_nodes().begin(),
   //        end2 = t2.grouped_nodes().end(); i2 != end2; ++i2) {
   //   cout << (*i2)->id_string() << endl;
   //}

   // Sorted so that the 'shallowly-same' nodes are together.
   // What's shallowly-same mean?
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

/// See Alexandro Moschitti, "Making Tree Kernels Practical for
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
