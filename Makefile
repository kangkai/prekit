# HOST_OS can be linux or windows
HOST_OS :=
CFLAGS := -m32 -O2

# depends on zlib
LDFLAGS := -lz

# build for windows depends on mingw32
ifeq ($(HOST_OS),windows)
	CC := i586-mingw32msvc-gcc
	CFLAGS += -Iusb/api -I/usr/i586-mingw32msvc/include/ddk -Izlib-1.2.3-lib/include
	LDFLAGS := -Lprebuilt/usb -lAdbWinApi -Lzlib-1.2.3-lib/lib -lz
endif

vpath %.c libzipfile

# fastboot
F_SRC := protocol.c engine.c fastboot.c parser.c
ifeq ($(HOST_OS),linux)
    F_SRC += usb_linux.c util_linux.c
endif
ifeq ($(HOST_OS),windows)
	F_SRC += usb_windows.c util_windows.c
endif
F_OBJ := $(F_SRC:.c=.o)

ifeq ($(HOST_OS),linux)
    # usbtest
    U_SRC := usbtest.c usb_linux.c
endif
ifeq ($(HOST_OS),windows)
	U_SRC := usbtest.c usb_windows.c
endif
U_OBJ := $(U_SRC:.c=.o)

# libzipfile
ZIP_SRC := \
    centraldir.c \
    zipfile.c
ZIP_OBJ := $(ZIP_SRC:.c=.o)

ifeq ($(HOST_OS),)
help:
	@echo "make HOST_OS=[linux|windows]   build for linux or windows"
	@echo "make clean                     remove all generated .o files"
	@echo "make distclean                 remove all generated files"
endif

all: fastboot usbtest

ifeq ($(HOST_OS),linux)
fastboot: $(F_OBJ) $(ZIP_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

usbtest: $(U_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
endif
ifeq ($(HOST_OS),windows)
fastboot: $(F_OBJ) $(ZIP_OBJ)
	$(CC) $(CFLAGS) -o $@.exe $^ $(LDFLAGS)

usbtest: $(U_OBJ)
	$(CC) $(CFLAGS) -o $@.exe $^ $(LDFLAGS)
endif


clean:
	@echo "rm *.o"
	@rm -rf *.o

distclean: clean
	@ echo "rm fastboot usbtest"
	@rm -rf fastboot usbtest
	@ echo "rm fastboot.exe usbtest.exe"
	@rm -rf fastboot.exe usbtest.exe

.PHONY: clean distclean all
