export LIBS='-L/usr/local/lib -L/usr/lib -Wl,-rpath,/usr/local/lib /usr/lib/libstdc++.a'

./autogen.sh --with-sdl-prefix=/usr/local --host=arm-linux-gnueabi
