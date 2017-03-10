
# Executable
BINS = amiitool
STATICLIBS = mbedtls

# Compilation flags
CFLAGS ?= -Wall -pedantic -O2 -fsigned-char
ALL_CFLAGS = -I $(PWD)/include -I $(MBEDTLS_DIR)/include $(CFLAGS)
LDFLAGS = -L $(MBEDTLS_DIR)/library -l mbedcrypto

# Commands
INSTALL = /usr/bin/install -D
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

# Directories
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

# mbed TLS libraries
MBEDTLS_DIR = $(PWD)/mbedtls
MBEDTLS_CONFIG = $(PWD)/configs/mbedtls.h
MBEDTLS_CFLAGS = -DMBEDTLS_CONFIG_FILE='\"$(MBEDTLS_CONFIG)\"' $(CFLAGS)

HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
LIBSOBJ := $(filter-out $(BINS:%=%.o),$(OBJECTS))

# Disable built-in wildcard rules
.SUFFIXES:

# Keep objects to speed up recompilation
.PRECIOUS: %.o

# Always execute Makefiles for static libraries
.PHONY: $(STATICLIBS)

# Default target: compile all programs
all: $(BINS)

%: %.o $(LIBSOBJ) $(STATICLIBS)
	$(CC) $(ALL_CFLAGS) $(LIBSOBJ) $< -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

# Static mbed TLS
mbedtls: $(MBEDTLS_CONFIG)
	"$(MAKE)" lib -C $(MBEDTLS_DIR) CFLAGS="$(MBEDTLS_CFLAGS)"

# Clean targets
clean: mostlyclean
	$(MAKE) -C $(MBEDTLS_DIR) clean

mostlyclean:
	$(RM) $(OBJECTS) $(BINS)

# Install
install: $(BINS:%=install_%)

install_%: %
	$(INSTALL_PROGRAM) $< $(DESTDIR)$(bindir)/$<

# Uninstall
uninstall: $(BINS:%=uninstall_%)

uninstall_%: %
	$(RM) $(DESTDIR)$(bindir)/$<
