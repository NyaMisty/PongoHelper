cd "$(dirname $0)"

pushd capstone
IOS_CC="xcrun -sdk iphoneos gcc"
IOS_CFLAGS="-O2 -nostdlib -arch arm64"
IOS_LDFLAGS="-arch arm64"
IOS_ARCHS="arm64"
CC="$IOS_CC" LIBARCHS="$IOS_ARCHS" CFLAGS="$IOS_CFLAGS" LDFLAGS="$IOS_LDFLAGS" MACOS_UNIVERSAL=no CAPSTONE_BUILD_CORE_ONLY=yes CAPSTONE_SHARED=no CAPSTONE_STATIC=yes CAPSTONE_ARCHS="arm aarch64" CAPSTONE_USE_SYS_DYN_MEM=no CAPSTONE_DIET=no make
popd

#SOURCE="printf/printf.c tinyalloc/tinyalloc.c main.c"
SOURCE="printf/printf.c main.c"

INCLUDE="-I../include -Icapstone/include"
CC="xcrun -sdk iphoneos gcc"
CFLAGS="-Wno-int-conversion -arch arm64 -Xlinker -kext -nostdlib -Xlinker -fatal_warnings"
LDFLAGS="-static -Lcapstone/ -lcapstone"
$CC $SOURCE -o test_module $CFLAGS $INCLUDE $LDFLAGS -D_SECURE__STRING_H_ -O3

