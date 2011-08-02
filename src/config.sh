export LIBS='-L/usr/local/lib -L/usr/lib -Wl,-rpath,/usr/local/lib /usr/lib/libstdc++.a'

# Pull env vars for pdk
. pdk.rc

./autogen.sh --with-sdl-prefix=/usr/local --host=arm-none-linux-gnueabi
