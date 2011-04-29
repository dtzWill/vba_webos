# Build various ipk's
.PHONY: cat_beta cat preware pkg update install-%

# Extra the version from appinfo.json
# Not hacky at all.
VERSION = $(shell sed -n -e 's/.*version\".*\"\(.*\)\",/\1/p' pkg_base/appinfo.json)

all: cat_beta cat preware

#Make sure the binary is up-to-date
../binaries/armv7/vba: ../src/src/sdl/VisualBoyAdvance
	mkdir -p ../binaries/armv7
	cp $< $@

update: ../binaries/armv7/vba

cat_beta:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.beta

cat:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.official

cat_pixi:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.official.pixi

preware_pixi:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.pixi

testing:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.testing

# Install with the testing ID
install:
	$(MAKE) testing
	-@palm-install -r org.webosinternals.visualboyadvance.testing
	palm-install ipk/org.webosinternals.visualboyadvance.testing_$(VERSION)_all.ipk
	palm-launch org.webosinternals.visualboyadvance.testing

#Build a pkg with the specified 'ID'
pkg: update
	@if [ -z $(ID) ]; then \
		echo "Don't run this target directly!"; \
		exit 1; \
	fi
	cp pkg_base pkg_build -r
	cp ../binaries/armv7/vba pkg_build/
	sed -i s/@APPID@/$(ID)/ pkg_build/appinfo.json
	palm-package --use-v1-format pkg_build
	rm -rf pkg_build
	mkdir -p ipk
	mv *.ipk ipk

clean:
	-rm -rf pkg_build
	-rm -rf ipk
