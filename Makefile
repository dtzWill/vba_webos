
all: build_pre build_pixi

.PHONY: build_pre build_pixi
build_pre: binaries/armv7/vba
build_pixi: binaries/armv6/vba

binaries/%/vba: %_configure
	$(MAKE) -C $*
	mkdir -p binaries/$*
	cp $*/src/sdl/VisualBoyAdvance binaries/$*/vba

%_configure: %/Makefile
	mkdir -p $*
	cd $* && $*=1 ../src/config.sh

src/configure:
	cd src && ./autogen.sh

clean:
	-rm -rf pixi pre
