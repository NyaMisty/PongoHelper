cd "$(dirname $0)"
arm64-apple-ios12.0.0-clang main.c -o test_module -arch arm64 -Xlinker -kext -nostdlib -Xlinker -fatal_warnings -I../include/ -D_SECURE__STRING_H_ -O3 
