include ../Makefile.variables

OUTPUT_PREFIX := $(shell pwd)/prefix
AUTOMAKE_OUTPUT := "$(OUTPUT_PREFIX)/bin/automake"
AUTOCONF_OUTPUT := "$(OUTPUT_PREFIX)/bin/autoconf"
NEWLIB := newlib-4.1.0
NEWLIB_OUTPUT := "$(shell pwd)/build"

all: $(AUTOMAKE_OUTPUT) $(AUTOCONF_OUTPUT)

$(AUTOMAKE_OUTPUT):
	# Download
	curl -o automake.gz https://ftp.gnu.org/gnu/automake/automake-1.11.tar.gz
	tar -xzf automake.gz

	# Build
	mkdir -p $(OUTPUT_PREFIX)
	mkdir -p automake-build
	cd automake-build && ../automake-1.11/configure --prefix="$(OUTPUT_PREFIX)" && make && make install

$(AUTOCONF_OUTPUT):
	# Download
	curl -o autoconf.gz https://ftp.gnu.org/gnu/autoconf/autoconf-2.65.tar.gz
	tar -xzf autoconf.gz

	# Build
	mkdir -p $(OUTPUT_PREFIX)
	mkdir -p autoconf-build
	cd autoconf-build && ../autoconf-2.65/configure --prefix="$(OUTPUT_PREFIX)" && make && make install

newlib:
	cd $(NEWLIB)/newlib/libc/sys && $(AUTOCONF_OUTPUT)

	# Setup symlinks to trick newlib into working
	cd prefix/bin && rm -f i686-minos-* &&\
	ln ../../$(PREFIX_DIR)/i686-elf-ar i686-minos-ar &&\
	ln ../../$(PREFIX_DIR)/i686-elf-as i686-minos-as &&\
	ln ../../$(PREFIX_DIR)/i686-elf-gcc i686-minos-gcc &&\
	ln ../../$(PREFIX_DIR)/i686-elf-ranlib i686-minos-ranlib &&\
	export PATH="$(OUTPUT_PREFIX)/bin":$$PATH &&\
	cd ../../ &&\
	which i686-minos-gcc &&\
	mkdir -p newlib-build && \
	cd newlib-build &&\
	../$(NEWLIB)/configure --prefix="$(OUTPUT_PREFIX)" --target=i686-minos &&\
	make all &&\
	make DESTDIR=$(NEWLIB_OUTPUT) install

clean:
	rm -rf automake-1.11
	rm -rf autoconf-2.65
	rm -rf automake-build
	rm -rf automake-conf
	rm -rf newlib-build
	rm -rf prefix