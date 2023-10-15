set -e
set -o pipefail

make

./compilert.sh

cat "$1" | ./cuiua > out.c

gcc out.c runtime.a -o out -lm

./out