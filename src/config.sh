# Pull env vars for pdk
. pdk.rc

export CFLAGS="$CFLAGS -DPALM_PIXI"
export CXXFLAGS="$CXXFLAGS -DPALM_PIXI"

./autogen.sh --host=arm-none-linux-gnueabi
