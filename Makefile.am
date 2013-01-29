SUBDIRS = libzipfile

bin_PROGRAMS = fastboot usbtest
fastboot_SOURCES = \
	protocol.c \
	engine.c \
	fastboot.c \
	fastboot.h \
	parser.c \
	parser.h \
	usb_linux.c \
	util_linux.c \
	usb.h

fastboot_LDADD = \
	$(top_builddir)/libzipfile/libzipfile.a \
	-lz

usbtest_SOURCES = \
	usbtest.c \
	usb_linux.c
