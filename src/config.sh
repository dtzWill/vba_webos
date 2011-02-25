#Note that I am using 2010q1 toolchain here, fwiw
#export PRE_FLAGS='-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -DARM_CORE -DPALM_PRE'
export PIXI_FLAGS='-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -DPALM_PIXI'

export CFLAGS="-O3 $PIXI_FLAGS -ftree-vectorize -ffast-math -DFINAL_VERSION"
export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS

export LIBS='-L/usr/local/lib -L/usr/lib -lGLESv2 -lSDL_ttf -lSDL_image -lpdl -Wl,-rpath,/usr/local/lib /usr/lib/libstdc++.a'
export LDFLAGS='-Wl,--allow-shlib-undefined'
./configure --disable-dev --disable-profiling --with-sdl-prefix=/usr/local
