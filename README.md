# raspbootin64 Pi4

Simple boot-over-serial bootloader for the Raspberry Pi4 for aarch64 kernels.

This repository contains 2 components: Raspbootin64 and Raspbootcom.

## Raspbootin64

Based on: [raspbootin64](https://github.com/bztsrc/raspi3-tutorial/tree/master/14_raspbootin64)

Raspbootin is the actual bootloader that you install on the SD Card for your
Raspberry Pi. Copy the `sdcard/kernel8.img` in place of the `kernel8.img` on
the SD Card and you are ready for use.

**Compiling**

- Get the right cross compiler toolchain (e.g. building on x86_64) from [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads)
- Take a look into the `raspbootin64/Makefile` and modify `TOOLCHAIN` !

Switch to `raspbootin64/` and type `make`.

## Raspbootcom

Based on: [raspbootin](https://github.com/mrvn/raspbootin)

Raspbootcom is a simple boot server and terminal program for the other
side of the serial connection. You need to run this on another
computer, the one the serial port of your Raspberry Pi4 is conneted to.
On start Raspbootcom is in terminal mode. Any input on stdin is passed
to the Raspberry Pi4 and any reply from the Raspberry Pi4 is printed to
stdout. The Raspbootin bootloader will send 3 breaks (0x03) over the
serial connection when it wants to boot a kernel and Raspbootcom then
switches into kernel sending mode, reads the kernel from disk and
sends it to the Raspberry Pi4. After that it goes back into terminal
mode so you can see the output from the Raspberry Pi4 and interact with
it.

The kernel is read fresh every time it is send so you do not need to
restart Raspbootcom every time the kernel image changes. Raspbootcom also
survives unplugging and replugging of an USB serial converter and will
automatically reopen the device when you replug it. 

**Compiling**

Switch to `raspbootcom/` and type `make`.

## Usage

- Copy the `sdcard/kernel8.img` to the SD Card for the Raspberry Pi4.
- Run `raspbootcom/raspbootcom /dev/ttyUSB0 /where/you/have/your/own/kernel8.img`
- Turn on the Raspberry Pi4.
