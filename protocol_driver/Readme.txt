This directory contains the drivers used by the protocol.asm sample.

When loaded, these drivers install a custom protocol, that can be used to
validate EBC to native access, and especially the marshalling of CALLEX
parameters.

These were compiled from the driver.c source on Debian 8.0 (Sid),
using the included Makefile. Note that you should have initialized the
gnu-efi submodule in the top directory to be able to compile the driver.
