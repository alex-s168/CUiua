mkdir -p runtime/build
for file in runtime/*.c
do
  gcc -c -Werror "$file" -o runtime/build/"$(basename -- "$file").o"
done
ar rcs runtime.a runtime/build/*