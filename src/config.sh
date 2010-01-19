#Note that I am using 2009q3 toolchain here, fwiw
export CFLAGS='-O3 -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -ffast-math -fsingle-precision-constant -funroll-loops -DARM_CORE'
export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS
export LIBS='-L/usr/local/lib -L/usr/lib -lGLESv2 -lSDL_ttf -lpdl -Wl,-rpath,/usr/local/lib'
./configure --disable-dev --disable-profiling
