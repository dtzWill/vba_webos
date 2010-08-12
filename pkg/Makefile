#Build various ipk's
.PHONY: cat_beta cat preware pkg update

all: cat_beta cat preware

#Make sure the binary is up-to-date
pkg_base/vba: ../src/src/sdl/VisualBoyAdvance
	cp $< $@
	cp $< ../binaries/armv7/vba

update: pkg_base/vba

cat_beta:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.beta

cat:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.official

preware:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance

#Build a pkg with the specified 'ID'
pkg: update
	@if [ -z $(ID) ]; then \
		echo "Don't run this target directly!"; \
		exit 1; \
	fi
	cp pkg_base pkg_build -r
	sed -i s/@APPID@/$(ID)/ pkg_build/appinfo.json
	palm-package pkg_build
	rm -rf pkg_build
	mkdir -p ipk
	mv *.ipk ipk
