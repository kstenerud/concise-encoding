Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a simple CBE codec.

Everything except decimal types are tested.

No heap allocation is performed by this library.


Assumptions
-----------

 * Assumes densely packed decimal encoding for C decimal types (decimal32, decimal64, decimal128). This is the default for gcc and other compilers using decNumber.
 * Assumes a little endian host.



Requirements
------------

  * CMake 3.5 or higher
  * A C compiler
  * A C++ compiler (for the unit tests)



C Library Dependencies
----------------------

 * stdbool.h: For bool type
 * stdint.h: Fot int types
 * memory.h: For memset()
 * stdarg.h: For variable arguments
 * stdio.h: For vsprintf()



Building
--------

    mkdir build
    cd build
    cmake ..
    make



Running Tests
-------------

    ./test/cbe_test
