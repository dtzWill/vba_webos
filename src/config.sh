#Note that I am using 2010q1 toolchain here, fwiw
export PRE_FLAGS='-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp'
export PIXI_FLAGS='-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -DPIXI'

export WEBOS_PDK=/opt/PalmPDK
export WEBOS_SDK=/opt/PalmSDK

export CFLAGS="-O3 $PRE_FLAGS -ftree-vectorize -ffast-math -DARM_CORE -DFINAL_VERSION"
export CFLAGS="$CFLAGS -I$WEBOS_PDK/include -I$WEBOS_PDK/include/SDL"
export CFLAGS="$CFLAGS -flto"

#export CFLAGS="$CFLAGS -g -O0"

export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS

export LIBS="-L$WEBOS_PDK/device/lib"
export LIBS="$LIBS -lGLESv2 -lSDL -lSDL_ttf -lSDL_image -lpdl -lstdc++"
export LIBS="$LIBS -Wl,--allow-shlib-undefined"
export LIBS="$LIBS $WEBOS_PDK/arm-gcc/arm-none-linux-gnueabi/libc/usr/lib/libstdc++.a"
export LIBS="$LIBS -flto"

export PATH=$WEBOS_PDK/arm-gcc/bin:$PATH


arm-none-linux-gnueabi-gcc
./configure --disable-dev --disable-profiling --host=arm-none-linux-gnueabi
