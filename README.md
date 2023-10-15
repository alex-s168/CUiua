# CUiua
Stack based array programming language heavily inspired 
by Uiua.

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

## 

## Usage
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
gcc program.c runtime.a -o program

# Run the program
./program
```