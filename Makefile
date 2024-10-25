# NOTE: compile with -s to get it REAL small for release
# NOTE: cpp parser is NOT behaving WELL
CC := clang
CFLAGS := -Wall -Wextra -ggdb3 -O2
TREE-SITTER_INCLUDES = -I tree-sitter/lib/include -I tree-sitter/lib/src
SRC := $(wildcard src/*.c)
TREE_SITTER_OBJ := ts_obj/tree_sitter.o ts_obj/c_parser.o ts_obj/cpp_parser.o ts_obj/cpp_scanner.o
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC)) $(TREE_SITTER_OBJ)

PREFIX ?= /usr/local/

.PHONY: default all test clean compile_commnds obj_dir ts_obj_dir install

default: coil

all: coil

coil: $(OBJ)
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -o $@ $^

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
	@mkdir -p ts_obj

obj_dir:
	@mkdir -p obj

clean:
	rm -rf obj/*.o coil coil.exe NUL src/NUL

clean-ts:
	rm -rf ts_obj/*.o

clean-emacs:
	rm -rf *~ src/*~

install: all
	@echo "installing coil to $(DESTDIR)$(PREFIX)bin..."
	@mkdir -p $(DESTDIR)$(PREFIX)bin
	@cp -p coil $(DESTIDR)$(PREFIX)bin/coil
	@chmod 755 $(DESTDIR)$(PREFIX)bin/coil
	@echo "Done!"

uninstall:
	@echo "uninstalling coil to $(DESTDIR)$(PREFIX)bin..."
	@rm -rfv $(DESTDIR)$(PREFIX)bin/coil
	@echo "Done!"

compile_commands:
	@make --always-make --dry-run all $(TREE-SITTER_OBJ)\
	| grep -w $(CC) \
	| grep -w '\-[co]' \
	| jq -nR '[inputs|{command:., directory:".", file: match("\\-c [\\w\\-/]+\\.c").string[3:], output: match("\\-o [\\w\\-/]+\\.o").string[3:]}]' > compile_commands.json
