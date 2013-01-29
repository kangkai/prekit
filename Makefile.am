EXTRA_DIST = \
	usb_linux.c \
	util_linux.c \
	usb_windows.c \
	util_windows.c \
	zlib-1.2.3-lib \
	prebuilt \
	usb

SUBDIRS = libzipfile

# introduced for libtool
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = fastboot usbtest

# usb_os.c and util_os.c are links to platform specific source files
fastboot_SOURCES = \
	protocol.c \
	engine.c \
	fastboot.c \
	fastboot.h \
	parser.c \
	parser.h \
	usb_os.c \
	util_os.c \
	usb.h

AM_CFLAGS = \
	@USB_INCLUDE@

fastboot_LDADD = \
	$(top_builddir)/libzipfile/libzipfile.la \
	@ZLIB_LIBS@ \
	@USB_LIBS@

usbtest_SOURCES = \
	usbtest.c \
	usb_os.c

usbtest_LDADD = \
	@USB_LIBS@
