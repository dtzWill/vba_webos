#Build various ipk's
.PHONY: cat_beta cat preware pkg update install-%

all: cat_beta cat preware

#Make sure the binary is up-to-date
../binaries/armv7/vba: ../src/src/sdl/VisualBoyAdvance
	cp $< $@

update: ../binaries/armv7/vba

cat_beta:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.beta

cat:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance.official

preware:
	$(MAKE) pkg ID=org.webosinternals.visualboyadvance

install-%:
	$(MAKE) $(subst install-,,$@)
	@palm-install -r org.webosinternals.visualboyadvance
	palm-install ipk/org.webosinternals.visualboyadvance_1.3.4_all.ipk
	palm-launch org.webosinternals.visualboyadvance

#Build a pkg with the specified 'ID'
pkg: update
	@if [ -z $(ID) ]; then \
		echo "Don't run this target directly!"; \
		exit 1; \
	fi
	cp pkg_base pkg_build -r
	cp ../binaries/armv7/vba pkg_build/
	sed -i s/@APPID@/$(ID)/ pkg_build/appinfo.json
	palm-package pkg_build
	rm -rf pkg_build
	mkdir -p ipk
	mv *.ipk ipk

clean:
	-rm -rf pkg_build
	-rm -rf ipk
