autoreconf --install --symlink

#Note that I am using 2010q1 toolchain here, fwiw
export PRE_FLAGS='-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp'
export PIXI_FLAGS='-mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -DPIXI'

export EXTRA_CFLAGS="-O3 -ggdb $PRE_FLAGS -ftree-vectorize -ffast-math -DARM_CORE -DFINAL_VERSION"
export CFLAGS="$EXTRA_CFLAGS $CFLAGS"
export CXXFLAGS="$EXTRA_CFLAGS $CXXFLAGS"
export LIBS="$LIBS -lGLESv2 -lSDL_ttf -lSDL_image -lpdl"

./configure --disable-dev --disable-profiling "$@"
