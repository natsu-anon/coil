# NOTE: compile with -s to get it REAL small for release
CC := clang
CFLAGS := -Wall -Wextra -ggdb3 -O2
ifdef TARGET
	TARGET_OPT := -target $(TARGET)
else
	TARGET_OPT :=
endif
CROSS_DIR := cross_compiled
TREE-SITTER_INCLUDES = -I tree-sitter/lib/include -I tree-sitter/lib/src
SRC := $(wildcard src/*.c)
TREE_SITTER_OBJ := ts_obj/tree_sitter.o ts_obj/c_parser.o ts_obj/cpp_parser.o ts_obj/cpp_scanner.o
OBJ := $(patsubst src/%.c,obj/%.o,$(SRC)) $(TREE_SITTER_OBJ)

PREFIX ?= /usr/local/

.PHONY: default all test clean clean_ts clean_obj compile_commnds obj_dir ts_obj_dir install uninstall batchxc

default: $(OBJ)
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -o coil $^ $(TARGET_OPT)

cross: $(OBJ) | $(CROSS_DIR)
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -o $(CROSS_DIR)/coil_$(TARGET) $^ $(TARGET_OPT)

ts_obj/tree_sitter.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -c tree-sitter/lib/src/lib.c -o $@ $(TARGET_OPT)

ts_obj/%_parser.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLDUES) -c tree-sitter-$*/src/parser.c -o $@ $(TARGET_OPT)

ts_obj/cpp_scanner.o: | ts_obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLDUES) -c tree-sitter-cpp/src/scanner.c -o $@ $(TARGET_OPT)

obj/coil.o : | obj_dir
	$(CC) $(CFLAGS) $(TREE-SITTER_INCLUDES) -c src/coil.c -o $@ $(TARGET_OPT)

obj/%.o: src/%.c | obj_dir
	$(CC) $(CFLAGS) -c $^ -o $@ $(TARGET_OPT)

ts_obj_dir:
	@mkdir -p ts_obj

obj_dir:
	@mkdir -p obj

$(CROSS_DIR):
	@mkdir -p $(CROSS_DIR)

clean:
	rm -rf obj/*.o coil coil.exe NUL src/NUL

clean-objs:
	rm -rf obj/*

clean-ts:
	rm -rf ts_obj/*

clean-emacs:
	rm -rf *~ src/*~

install: coil
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

# macos is freaking out about my FILE forward dec >:^(
# AND I CANT INCLUDE stdio.h EITHER! WHY?
batchxc:
	make clean-objs clean-ts cross CC="zig cc" TARGET=x86_64-windows-gnu
	make clean-objs clean-ts cross CC="zig cc" TARGET=x86_64-linux-musl
	make clean-objs clean-ts cross CC="zig cc" TARGET=aarch64-windows-gnu
	make clean-objs clean-ts cross CC="zig cc" TARGET=aarch64-linux-gnu
