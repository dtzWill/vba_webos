
all: build_armv6 build_armv7

build_%: %/.configured
	$(MAKE) -C $*
	mkdir -p binaries/$*
	cp $*/src/sdl/VisualBoyAdvance binaries/$*/vba

.PRECIOUS: armv6/.configured armv7/.configured
%/.configured: src/configure
	-rm $@
	-rm -rf $*
	mkdir -p $*
	cd $* && $*=1 ../src/config.sh
	touch $@

src/configure: src/configure.in
	cd src && ./autogen.sh

clean:
	-rm -rf armv7 armv6
