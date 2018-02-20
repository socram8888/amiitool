
# Project directory
PWD ?= "`pwd`"

# Executable
BINS = amiitool

# Compilation flags
CFLAGS ?= -fpic -fPIC -Wall -pedantic -O2 -fsigned-char
LDFLAGS = -L. -l nfc3d

# Commands
INSTALL = /usr/bin/install -D
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

# Directories
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

# mbed TLS libraries
MBEDTLS_CONFIG ?= $(realpath configs/mbedtls.h)
MBEDTLS_CFLAGS = -DMBEDTLS_CONFIG_FILE='\"$(MBEDTLS_CONFIG)\"' $(CFLAGS)

HEADERS := $(wildcard *.h) gitversion.h
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
LIBSOBJ := $(filter-out $(BINS:%=%.o),$(OBJECTS))

# Disable built-in wildcard rules
.SUFFIXES:

# Keep objects to speed up recompilation
.PRECIOUS: %.o

# Always execute Makefiles for mbedtls
.PHONY: mbedtls/library/libmbedtls.a

# Default target: compile all programs
all: libnfc3d.so $(BINS)

libnfc3d.so: $(LIBSOBJ) mbedtls/library/libmbedtls.a
	$(CC) -shared -I include -I mbedtls/include $(LIBSOBJ) -o $@ -L mbedtls/library -l mbedcrypto

%: %.o libnfc3d.so
	$(CC) -I include $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -I include -I mbedtls/include $(CFLAGS) -c $< -o $@

gitversion.h:
	echo "#define GIT_COMMIT_ID 0x`git rev-parse HEAD | head -c8`" > $(PWD)/gitversion.h
	echo "#define GIT_COMMIT_COUNT `git rev-list --count --all`" >> $(PWD)/gitversion.h

# Static mbed TLS
mbedtls/library/libmbedtls.a: $(MBEDTLS_CONFIG)
	"$(MAKE)" lib -C mbedtls/ CFLAGS="$(MBEDTLS_CFLAGS)"

# Clean targets
clean: mostlyclean
	$(MAKE) -C mbedtls/ clean

mostlyclean:
	$(RM) $(OBJECTS) $(BINS) gitversion.h

# Install
install: $(BINS:%=install_%)

install_%: %
	$(INSTALL_PROGRAM) $< $(DESTDIR)$(bindir)/$<

# Uninstall
uninstall: $(BINS:%=uninstall_%)

uninstall_%: %
	$(RM) $(DESTDIR)$(bindir)/$<
