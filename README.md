A C++ implementation of tree kernels per Alessandro Moschitti's 2006
paper, "Making Tree Kernels Practical for Natural Language Learning"

This consists of two directories, tree-parser and tree-kernel itself.
tree-parser parses Penn Treebank forms of sentences as produced by
the [Standford Parser](http://nlp.stanford.edu/software/lex-parser.shtml),
[(example)](http://nlp.stanford.edu:8080/parser) among others. Eg:

     (ROOT
       (S
         (NP (PRP$ My) (NN dog))
         (ADVP (RB also))
         (VP (VBZ likes)
           (S
             (VP (VBG eating)
               (NP (NN sausage)))))
         (. .)))


This TreeKernel source is MIT licensed (see LICENSE), and commercially
useable. If you need a different one just ask.

Though one person has used this without an SVM, I intended to
integrate this into an SVM library, though I never got around to
it. The two libraries I had in mind were:
    libSVM - [github](https://github.com/cjlin1/libsvm),
    [home](http://www.csie.ntu.edu.tw/~cjlin/libsvm)
and:
    Dlib - [home](http://dlib.net/ml.html)

Feel free to integrate it yourself, I'd love to hear about it if you
do.

Building
--------

This is GNU Makefile, and GCC based, but nothing is OS-dependent so it
should be easily tweakable for other systems.

    cd tree-parser
    make

    cd ../tree-kernel
    make

TODO
----

There are several optimizations possible - templating out sigma,
making NodePairsDeltaTable an unordered_map, but most of all, storing
the Nodes in Sentence in a vector and using indices as pointers to
them (to save memory and cache). C bindings would be nice, too.

If you make any improvements I'll be happy to take them. Of course,
you can just do what you want with it.
