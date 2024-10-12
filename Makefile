# NOTE: compile with -s to get it REAL small for release
# NOTE: cpp parser is NOT behaving WELL
CC := clang
CFLAGS := -Wall -Wextra
TREE-SITTER_INCLUDES = -I tree-sitter/lib/include -I tree-sitter/lib/src
SRC := $(wildcard src/*.c)
TREE_SITTER_OBJ := ts_obj/tree_sitter.o ts_obj/c_parser.o ts_obj/cpp_parser.o ts_obj/cpp_scanner.o
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC)) $(TREE_SITTER_OBJ)
DEBUG_RELEASE := -ggdb3

.PHONY: default all test clean compile_commnds obj_dir ts_obj_dir

default: coil

all: coil

release: $(OBJ)
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -o coil $^ -s -O3


coil: $(OBJ)
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -o $@ $^ $(DEBUG_RELEASE)

ts_obj/tree_sitter.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -c tree-sitter/lib/src/lib.c -o $@

ts_obj/%_parser.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLDUES) -c tree-sitter-$*/src/parser.c -o $@

ts_obj/cpp_scanner.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLDUES) -c tree-sitter-cpp/src/scanner.c -o $@

obj/coil.o : | obj_dir
	$(CC) $(CFLAGS) -c src/coil.c -o $@ $(DEBUG_RELEASE)

obj/%.o: src/%.c | obj_dir
	$(CC) $(CFLAGS) -c $^ -o $@ $(DEBUG_RELEASE)

ts_obj_dir:
	mkdir -p ts_obj

obj_dir:
	mkdir -p obj

test: coil
	@echo ATTN: TESTS START
	${CURDIR}/coil "void foo(void);"
	${CURDIR}/coil -o test.h
	${CURDIR}/coil -p -o navigation_octree.hpp
	${CURDIR}/coil -p -o navigation_octree.hpp 1000
	${CURDIR}/coil -p -o navigation_octree.hpp 800 1000
	@echo ATTN: TESTS SUCCEEDED

# ${CURDIR}/coil "void *fn(int a); void *gn(int b);"
# ${CURDIR}/coil void fn\(int a\)\;
# ${CURDIR}/coil "void fn(void); void gn(void);"

clean:
	rm -rf obj/*.o *.exe NUL src/NUL

clean-ts:
	rm -rf ts_obj/*.o

clean-emacs:
	rm -rf *~ src/*~

compile_commands:
	make --always-make --dry-run all $(TREE-SITTER_OBJ)\
	| grep -w $(CC) \
	| grep -w '\-[co]' \
	| jq -nR '[inputs|{command:., directory:".", file: match("\\-c [\\w\\-/]+\\.c").string[3:], output: match("\\-o [\\w\\-/]+\\.o").string[3:]}]' > compile_commands.json
