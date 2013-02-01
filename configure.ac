#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([fastboot], [0.8], [chengwei.yang@intel.com])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# init libtool
LT_INIT(disable-shared)

AC_CONFIG_SRCDIR([fastboot.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# check for host, only linux and windows support
AC_MSG_CHECKING([host])
AC_CANONICAL_HOST

case $host_os in
	# for windows
	mingw*)
		#FIXME: why AC_CONFIG_LINKS doesn't work?
		#AC_CONFIG_LINKS([usb_os.c:usb_windows.c util_os.c:util_windows.c])
		ln -sf usb_windows.c usb_os.c
		ln -sf util_windows.c util_os.c
		ZIPFILE_INCLUDE="-I\$(top_srcdir)/zlib-1.2.3-lib/include"
		USB_INCLUDE="-I\$(top_srcdir)/usb/api"
		ZLIB_LIBS="\$(top_builddir)/zlib-1.2.3-lib/lib/libz.a"
		USB_LIBS="-L\$(top_srcdir)/prebuilt/usb -lAdbWinApi"
		;;
	# for linux
	linux*)
		#AC_CONFIG_LINKS([usb_os.c:usb_linux.c util_os.c:util_linux.c])
		ln -sf usb_linux.c usb_os.c
		ln -sf util_linux.c util_os.c
		ZIPFILE_INCLUDE=
		FASTBOOT_INCLUDE=
		ZLIB_LIBS="-lz"
		USB_LIBS=
		;;
	*)
	# error
		AC_MSG_ERROR([Unsupport host])
		;;
esac
AC_MSG_RESULT([$host])

AC_SUBST([ZIPFILE_INCLUDE])
AC_SUBST([USB_INCLUDE])
AC_SUBST([ZLIB_LIBS])
AC_SUBST([USB_LIBS])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([z], [inflate])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
# AC_FUNC_MALLOC, this will fail for windows build
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strerror strrchr strtoul])

AC_CONFIG_FILES([Makefile
                 libzipfile/Makefile])
AC_OUTPUT
