# HOST_OS can be linux|darwin|windows, only linux support currently
HOST_OS := linux
CFLAGS := -m32 -Iinclude

# depends on zlib
LDFLAGS := -lz

vpath %.c libzipfile

# fastboot
F_SRC := protocol.c engine.c fastboot.c
ifeq ($(HOST_OS),linux)
    F_SRC += usb_linux.c util_linux.c
endif
F_OBJ := $(F_SRC:.c=.o)

ifeq ($(HOST_OS),linux)
    # usbtest
    U_SRC := usbtest.c usb_linux.c
    U_OBJ := $(U_SRC:.c=.o)
endif

# libzipfile
ZIP_SRC := \
    centraldir.c \
    zipfile.c
ZIP_OBJ := $(ZIP_SRC:.c=.o)

all: fastboot

fastboot: $(F_OBJ) $(ZIP_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


ifeq ($(HOST_OS),linux)
usbtest: $(U_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
endif

clean:
	@echo "rm *.o"
	@rm -rf *.o

distclean: clean
	@ echo "rm fastboot usbtest"
	@rm -rf fastboot usbtest

.PHONY: clean distclean all
