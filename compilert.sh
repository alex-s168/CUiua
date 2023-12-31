set -e
set -o pipefail

rm -rf runtime/build
mkdir runtime/build
for file in runtime/*.c
do
  clang -xc -O3 -c -Werror "$file" -o runtime/build/"$(basename -- "$file").o" -fPIC
done
rm -f runtime.a
ar rcs runtime.a runtime/build/*

echo "built runtime"
