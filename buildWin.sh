#! /bin/bash
set -e
cd /storage/Programming/CloudVis

# Options
mainFile="program"
libs="-lm -lpthread -lopengl32 -lraylib"
# -ldl
localLibs=""
flags=""

echo building...
# Build
x86_64-w64-mingw32-gcc -g -std=gnu99 -c src/*.c $flags || rm -f *.o

echo linking...
# Link
x86_64-w64-mingw32-gcc -o bin/$mainFile *.o -s -std=gnu99 $libs -Llib $localLibs
rm -f *.o

echo updating resources...
# update resources
rm -rf bin/res
cp -r res/ bin/

echo running...
# Run
./bin/$mainFile.exe
