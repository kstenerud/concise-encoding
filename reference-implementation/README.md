Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a simple CBE codec.

Everything except decimal types are tested.


Notes
-----

Assumes densely packed decimal encoding for C decimal types (decimal32, decimal64, decimal128). This is the default for gcc and other compilers using decNumber.


Requirements
------------

  * CMake 3.5 or higher
  * A C and C++ compiler



Building
--------

    mkdir build
    cd build
    cmake ..
    make



Running Tests
-------------

    ./test/cbe_test
