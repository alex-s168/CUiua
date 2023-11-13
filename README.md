# CUiua
Stack based array programming language heavily inspired 
by Uiua.

You can find the documentation [here](https://supercraftalex.github.io/CUiua-doc/).

Recommended font: [DejaVu Sans Mono for CUiua](https://github.com/SuperCraftAlex/DejaVu-for-CUiua)

The CUiua [formatter](https://github.com/SuperCraftAlex/CUiua-Formatter) is highly recommended

## Example code
### Find the lowest number in an array
```
⟼Min (.⍏⊏⊢)
```
or
```
⟼Min ((↧)/)
```

### Sorted array of random numbers
```
[(⚂)20⍥]⍏⊏
```

### Function that removes a fragment from an array
(you could also just use the "fragment" operator)
```
⟼FRAGMENT (⊂.⊢⇡(;⊤)∵ : ⇌⊢1+⇡(;⊤)∵ ⊕ (≡⊻¬)∵ [,⧻,⧻-(⊤):⍥] ⊂ ⊚ ⊏)
```

## Usage
Just execute a CUiua program:
```sh
./exec.sh program.cua
```

Manually:
```sh
# Build the transpiler
make
# This creates a cuiua executable

# Build the runtime library
./compilert.sh
# This creates a runtime.a file (static library)

# Compile a CUiua program
cat program.cua | ./cuiua > program.c

# Compile the CUiua program with the runtime library
gcc program.c runtime.a -lm -o program
# (or use any other C compiler like tcc or clang)

# Run the program
./program
```
