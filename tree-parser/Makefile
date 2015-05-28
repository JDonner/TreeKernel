CXXFLAGS=-O2 -Wall -Wextra
CXX=g++
INCLUDE=-I/usr/include
LIB_SOURCES=\
	token.cpp \
	tree.cpp \
	tree_parser.cpp
TEST_SOURCES=test-tree-parser.cpp
OBJDIR=.objs
LIB_OBJECTS=$(addprefix $(OBJDIR)/, $(LIB_SOURCES:.cpp=.o))
TEST_OBJECTS=$(addprefix $(OBJDIR)/, $(TEST_SOURCES:.cpp=.o))

TEST_TARGET=test-tree-parser
LIB_TARGET=libtree_parser.a

all: $(LIB_TARGET) $(TEST_TARGET)

$(OBJDIR):
	mkdir $(OBJDIR)

# $@ is the target (objs/foo.o), and $< is the input foo.cpp
$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<  -o $@

$(LIB_TARGET): $(LIB_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(LIB_OBJECTS) $(TEST_OBJECTS) -o $@

clean:
	rm -f $(LIB_TARGET)
	rm -f $(TEST_TARGET)
	rm -f $(LIB_OBJECTS) $(TEST_OBJECTS)
