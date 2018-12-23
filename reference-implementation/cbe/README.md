Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a simple CBE codec.


Assumptions
-----------

 * Assumes densely packed decimal encoding for C decimal types (_Decimal32, _Decimal64, _Decimal128). This is the default for gcc and other compilers using decNumber.
 * Assumes a little endian host.



Requirements
------------

  * CMake 3.5 or higher
  * A C compiler
  * A C++ compiler (for the unit tests)



Dependencies
------------

 * decimal/decimal (if using C++): For C++ decimal float types
 * stdbool.h: For bool type
 * stdint.h: Fot int types



Building
--------

    mkdir build
    cd build
    cmake ..
    make



Running Tests
-------------

    ./test/cbe_test
