Prekit
======

Prekit is a customized fastboot protocol client implementation, used to
communicate with [tboot][1] (in Pre-OS runtime of Tizen mobile IA) to achieve
some flashing tasks etc.


How to build and install?
-------------------------

This project uses GNU Autotools as build system, so it's quite simple to build
it like below:

    $ autoreconf -i (if you're using git source tree)
    $ ./configure
    $ make
    $ sudo make install

For more advanced usage, please refer to INSTALL.


How to build Windows version?
-----------------------------

First, you need install cross compile toolchain, the below take mingw as
example.

    # apt-get install gcc-mingw32
    $ ./configure --build x86_64-linux-gnu --host i686-w64-mingw32
    $ make

There is a simple way to figure out what "--build" and "--host" should be in
your case.

For "--build", just run
    $ ./config.guess

For "--host", this is the prefix of your cross compile toolchain, for me, it
is i686-w64-mingw32.

Also, you can build with sepcial CFLAGS as necessary, say CFLAGS="-m32" for
building 32bit binary or even add include hearder search directory, that's
the case of i586-mingw32msvc.

After compile finished, you'll find fastboot.exe and usbtest.exe in current
directory.

NOTE: to use them on Windows, you need install correct usb drivers.


NOTICE
------

The source code here is orignal taken from Intel OTC android project.
The mapping between them looks like below.

    OTC android src tree                    source directory
    system/core/fastboot                    .
    system/core/libzipfile                  libzipfile
    system/core/include/zipfile/zipfile.h   libzipfile/zipfile.h

[1]: https://github.com/kangkai/tboot

