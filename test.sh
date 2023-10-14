set -e
set -o pipefail

# builds the transpiler
make

# invokes the transpiler
cat "test.cua" | ./cuiua > out.c

# builds the transpiled code with the runtime library into an executable
./buildrt.sh

# runs the executable
./out