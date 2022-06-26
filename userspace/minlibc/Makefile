include ../Makefile.variables

AUTOTOOLS_ROOT := $(shell pwd)/autotools
AUTOTOOLS_PREFIX := $(AUTOTOOLS_ROOT)/prefix
AUTOTOOLS_PATH := export PATH=$(AUTOTOOLS_PREFIX)/bin/:$$PATH

NEWLIB := newlib-4.1.0
NEWLIB_OUTPUT := $(shell pwd)/build
NEWLIB_TARGET := $(NEWLIB_OUTPUT)/i686-minos/lib/libc.a

all: $(NEWLIB_TARGET)

$(AUTOTOOLS_ROOT)/autoconf.gz:
	mkdir -p "$(AUTOTOOLS_ROOT)" &&\
	cd "$(AUTOTOOLS_ROOT)" &&\
	curl -o autoconf.gz https://ftp.gnu.org/gnu/autoconf/autoconf-2.65.tar.gz &&\
	tar -xzf autoconf.gz &&\
	mkdir -p "$(AUTOTOOLS_PREFIX)" &&\
	mkdir -p autoconf-build &&\
	cd autoconf-build && ../autoconf-2.65/configure --prefix="$(AUTOTOOLS_PREFIX)" && make && make install

$(AUTOTOOLS_ROOT)/automake.gz:
	mkdir -p "$(AUTOTOOLS_ROOT)" &&\
	cd "$(AUTOTOOLS_ROOT)" &&\
	curl -o automake.gz https://ftp.gnu.org/gnu/automake/automake-1.11.tar.gz &&\
	tar -xzf automake.gz &&\
	mkdir -p "$(AUTOTOOLS_PREFIX)" &&\
	mkdir -p automake-build &&\
	$(AUTOTOOLS_PATH) && cd automake-build && ../automake-1.11/configure --prefix="$(AUTOTOOLS_PREFIX)" && make && make install

$(NEWLIB_TARGET): $(AUTOTOOLS_ROOT)/autoconf.gz $(AUTOTOOLS_ROOT)/automake.gz
	$(AUTOTOOLS_PATH) && cd $(NEWLIB)/newlib/libc/sys && autoconf
	$(AUTOTOOLS_PATH) && cd $(NEWLIB)/newlib/libc/sys/minos && autoreconf

	# Setup symlinks to trick newlib into working
	CURRENT_DIR=$$(pwd) &&\
	cd $(PREFIX_DIR)/bin && rm -f i686-minos-* &&\
	ln i686-elf-ar i686-minos-ar &&\
	ln i686-elf-as i686-minos-as &&\
	ln i686-elf-gcc i686-minos-gcc &&\
	ln i686-elf-gcc i686-minos-cc &&\
	ln i686-elf-ranlib i686-minos-ranlib &&\
	$(AUTOTOOLS_PATH) &&\
	cd $$CURRENT_DIR &&\
	$(PREFIX_PATH) &&\
	which i686-minos-gcc &&\
	mkdir -p newlib-build && \
	cd newlib-build &&\
	../$(NEWLIB)/configure --prefix=/ --target=i686-minos &&\
	make all &&\
	make DESTDIR=$(NEWLIB_OUTPUT) install

clean:
	rm -rf "$(AUTOTOOLS_ROOT)"
	rm -rf newlib-build
	rm -rf build
