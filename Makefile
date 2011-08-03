
all: build_pre build_pixi

.PHONY: build_pre build_pixi

build_pre: binaries/armv7/vba
build_pixi: binaries/armv6/vba

binaries/%/vba: %/Makefile
	$(MAKE) -C $*
	mkdir -p binaries/$*
	cp $*/src/sdl/VisualBoyAdvance binaries/$*/vba

%/Makefile: src/configure
	-rm -rf $*
	mkdir -p $*
	cd $* && $*=1 ../src/config.sh

src/configure: src/configure.in
	cd src && ./autogen.sh

clean:
	-rm -rf armv7 armv6
