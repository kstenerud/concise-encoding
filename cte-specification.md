Concise Text Encoding
=====================

A general purpose, human readable representation of semi-structured hierarchical data.

Designed with the following points of focus:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Human readable format
  * Minimal complexity
  * Type compatible with CBE (Concise Binary Encoding)


A CTE document consists of a single, top-level object. You can store multiple objects in a document by making the top level object a container.

For example:

    {
        "list":           [1, 2, "a string"],
        "sub-map":        {1: "one", 2: "two", 3: 3000},
        "boolean":        true,
        "binary int":     b10001011,
        "octal int":      o644,
        "decimal int":    -10000000,
        "hex int":        hfffe0001,
        "float":          14.125,
        "decimal":        d-1.02,
        "time":           2018-07-01T10:53:22.001481,
        "empty":          empty,
        "array of int16": i16(1000, 2000, 3000),
    }



Supported Types
---------------

### Scalar Types

#### Boolean

Supports the values true and false.

Example:

    true
    false


#### Integer

Represents two's complement signed integers up to a width of 128 bits.

Integers can be specified in base 2, 8, 10, or 16. A prefix character determines which base to interpret as:

  * b: base 2 (binary)
  * o: base 8 (octal)
  * h: base 16 (hexadecimal)

With no prefix, base 10 is assumed.

Example:

    900000
    b-1100
    o755
    hdeadbeef


#### Binary Floating Point

Represents ieee754 binary floating point values with widths from 32 to 128. Supports exponential notation using "e" or "E".

Example:

    1.25e+7
    -99.00001


#### Decimal Floating Point

Represents ieee754 decimal floating point values with widths from 64 to 128. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary.

Decimal floating values are differentiated from binary floating values by prefixing a "d".

Example:

    d12.99
    d-100.04


#### Time

Represents a date & time, ISO 8601 extended format, with precision down to the microsecond. Do not surround with quotes. Only dates from 0 CE onwards are supported. To preserve compatibility with CBE, years beyond 564526 are not supported.

Example:

    2018-07-01T10:53:22.001481



### Array Types

#### String

An array of UTF-8 encoded bytes, without a byte order mark (BOM). Strings must be enclosed in double-quotes.

The following escape sequences are allowed:

  * \\      (literal backslash)
  * \"      (double quote)
  * \r      (carriage return)
  * \n      (linefeed)
  * \t      (tab)
  * \000    (byte value in octal)
  * \xff    (byte value in hexadecimal)
  * \uffff  (unicode character)

Example:

    "A string\twith\ttabs\nand\nnewlines"


#### Array

A typed array of scalar objects. Only scalar types are supported, and all members of the array must be of the same type and size.

Arrays must be prefixed by the type:

  * b     (boolean)
  * i8    (8-bit integer)
  * i16   (16-bit integer)
  * i32   (32-bit integer)
  * i64   (64-bit integer)
  * i128  (128-bit integer)
  * f32   (32-bit binary floating point)
  * f64   (64-bit binary floating point)
  * f128  (128-bit binary floating point)
  * d64   (64-bit decimal floating point)
  * d128  (128-bit decimal floating point)
  * t     (time)

Example: An array of three 32-bit integers

    i32(1000000, 2000000, 3000000)



### Container Types

#### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

Commas are optional. A trailing comma on the last entry is allowed.

Example:

    [1, "two", 3.1, {}, empty]


#### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types, and must not be EMPTY. Values may be any mix of any type, including other containers. All keys in a map must be unique.

Commas are optional. A trailing comma on the last entry is allowed.

Example:

    {
        1: "alpha"
        2: "beta"
        "a map": {one: 1}
    }



### Other Types

#### Empty

Denotes the absence of data. Some languages implement this as the NULL value.

Example:

    empty



Illegal Encodings
-----------------

Illegal encodings must not be used, as they will cause problems or even API violations in certain languages. A decoder may discard illegal encodings.

  * Times must be valid. For example: February 31st, while technically encodable, is not allowed.
  * Map keys must not be container types or the EMPTY type.
  * Maps must not contain duplicate keys.
  * An array's element type must be a scalar type. Arrays of arrays, containers, or EMPTY, are not allowed.



File Format
-----------

A CTE file is simply a file containing a single CTE document. CTE files should use the extension "cte".

For example: File "mydata.cte"

    {
        "first": 1,
        "second": 2,
    }



Version History
---------------

July xx, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
