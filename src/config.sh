export CFLAGS='-O3 -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -ffast-math -fsingle-precision-constant -funroll-loops'
export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS
export LIBS='-L/usr/local/lib -L/usr/lib -lGLESv2 -lSDL_ttf -Wl,-rpath,/usr/local/lib'
./configure --disable-dev --disable-profiling
