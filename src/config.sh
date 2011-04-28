autoreconf --install --symlink

#Note that I am using 2010q1 toolchain here, fwiw
export PRE_FLAGS='-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp'
export PIXI_FLAGS='-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -DPIXI'

export CFLAGS="-O3 $PRE_FLAGS -ftree-vectorize -ffast-math -DARM_CORE -DFINAL_VERSION"
export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS
export LIBS='-L/usr/local/lib -L/usr/lib -lGLESv2 -lSDL_ttf -lSDL_image -lpdl -Wl,-rpath,/usr/local/lib /usr/lib/libstdc++.a'
./configure --disable-dev --disable-profiling --with-sdl-prefix=/usr/local
