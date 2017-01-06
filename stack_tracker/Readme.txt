This directory contains the test suite for the ARM EBC Stack Tracker.
See https://github.com/pbatard/edk2/commits/ebc-arm5

* head.inc/tail.inc contain common code for test preamble and end.

* cloaked.asm tests stack manipulation using a different register than R0

* matrix.asm tests the full matrix of natural/64-bit 4 parameters native calls

* max.asm tests the maximum number of parameters

* realloc.asm forces a stack tracker reallocation, and tests the stack tracker
  performance when large blocks of stack data are set aside and released

* switch.asm (not run as part of the test suite) is meant to confirm that the
  stack tracker can work when the whole stack buffer is switched. This is not
  included when running the test suite, as the common code for EBC VM does not
  seem to allow such an operation in the first place.

* shutdown.asm simply shuts down the (virtual) machine after all tests have run.
