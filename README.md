# COIL
`coil` expands C & C++ function declarations into definitions using tree-sitter, mostly as a little project to familiarize myself _with_ tree-sitter.

Make sure you run `git clone --recursive` to clone the tree-sitter submodules.  Otherwise you'll have to do the following later:
``` bash
git submodule init
git submodule update

```

## USAGE

You _really_ should be using `coil` via a wrapper for your text-editor.
- [Emacs](https://github.com/natsu-anon/.emacs.d/blob/master/coil-mode/coil-mode.el) I like this & I swear I'll make it its own repo soon.
- [Neovim](https://github.com/natsu-anon/nvim/blob/master/lua/coil/init.lua) I don't like this but I'll make it its own repo anyways.  Neovim's Lua API bindings just make me sad compared to Emacs-lisp.

```
USAGE:

    coil [options] [funciton definition(s) or byte-range to parse]

OPTIONS:
    -o <origin>       = Parse origin file.  Use alongside -p for C++.  If unused,
                        coil parses non-optional arguments for C functions.
    -i                = Also include the appropriate header (ONLY WORKS WITH -o)
    -p                = Treat origin file as C++ instead of C (ONLY WORKS WITH -o).
                        Selectively parse a C++ file by passing in a bit range.
    -c <destination>  = Write to destination file instead of printing to stdout.
    -h                = Prints usage information
```
