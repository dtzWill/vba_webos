export LIBS='-L/usr/local/lib -L/usr/lib -Wl,-rpath,/usr/local/lib /usr/lib/libstdc++.a'
export LDFLAGS='-Wl,--allow-shlib-undefined'

./autogen.sh --with-sdl-prefix=/usr/local
