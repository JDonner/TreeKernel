CXXFLAGS=-Wall -Wextra -Wno-unused-variable
CXX=g++ -g
# tree-parser not included - ask jeffrey.donner@gmail.com if you really want it
INCLUDE=-I/usr/include -I../tree-parser
LIB_SOURCES=\
	node.cpp \
	sentence.cpp \
	tree-kernel.cpp
TEST_SOURCES=\
	test-tree-kernel.cpp
OBJDIR=.objs
LIB_OBJECTS=$(addprefix $(OBJDIR)/, $(LIB_SOURCES:.cpp=.o))
TEST_OBJECTS=$(addprefix $(OBJDIR)/, $(TEST_SOURCES:.cpp=.o))
LIBSDIR=../tree-parser
LIBS=tree_parser

TEST_TARGET=test-tree-kernel
LIB_TARGET=libtree_kernel.a

all: $(LIB_TARGET) $(TEST_TARGET)

$(OBJDIR):
	mkdir $(OBJDIR)

# $@ is the target (objs/foo.o), and $< is the input foo.cpp
$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $^

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(LIB_TARGET): $(LIB_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

# don't need this: RPATH=$(BOOST_LIBDIR)
$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(LIB_OBJECTS) $(TEST_OBJECTS) -L$(LIBSDIR) -l$(LIBS) -o $@

clean:
	rm -f $(LIB_TARGET)
	rm -f $(TEST_TARGET)
	rm -f $(LIB_OBJECTS) $(TEST_OBJECTS)
