#!/usr/bin/env python

import sys
import nltk

dog = """
(ROOT
  (S
    (NP (NN dog))
    (VP (VBP eat)
        (NP (NN dog)))))
"""


def main(args):
    t = nltk.Tree.parse(dog)
    for subt in t.subtrees():
        print subt


if __name__ == '__main__':
    main(sys.argv[1:])
