Concise Binary Encoding
=======================

A general purpose, machine-readable, compact representation of semi-structured hierarchical data.

Goals
-----

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow the most common systems to read directly off the wire
  * Balanced space and computation efficiency
  * Minimal complexity
  * Type compatible with [Concise Text Encoding (CTE)](cte-specification.md)



Structure
---------

A CBE document is a binary encoded document consisting of a single, top-level object. You can store multiple objects in a document by making the top level object a container.



Encoding
--------

All objects are composed of an 8-bit type field and possibly a payload.


#### Type Field

| Hex | Dec | Type                       | Payload                                       |
| --- | --- | -------------------------- | --------------------------------------------- |
|  00 |   0 | Integer value 0            |                                               |
|  01 |   1 | Integer value 1            |                                               |
| ... | ... | ...                        |                                               |
|  69 | 105 | Integer value 105          |                                               |
|  6a | 106 | Positive Integer (8 bit)   | [8-bit positive integer]                      |
|  6b | 107 | Positive Integer (16 bit)  | [16-bit positive integer]                     |
|  6c | 108 | Positive Integer (32 bit)  | [32-bit positive integer]                     |
|  6d | 109 | Positive Integer (64 bit)  | [64-bit positive integer]                     |
|  6e | 110 | Positive Integer (128 bit) | [128-bit positive integer]                    |
|  6f | 111 | Nil (no data)              |                                               |
|  70 | 112 | Boolean False              |                                               |
|  71 | 113 | Boolean True               |                                               |
|  72 | 114 | Binary Float (32 bit)      | [IEEE 754 binary32 floating point]            |
|  73 | 115 | Binary Float (64 bit)      | [IEEE 754 binary64 floating point]            |
|  74 | 116 | Binary Float (128 bit)     | [IEEE 754 binary128 floating point]           |
|  75 | 117 | Decimal Float (32 bit)     | [IEEE 754 decimal32, Densely Packed Decimal]  |
|  76 | 118 | Decimal Float (64 bit)     | [IEEE 754 decimal64, Densely Packed Decimal]  |
|  77 | 119 | Decimal Float (128 bit)    | [IEEE 754 decimal128, Densely Packed Decimal] |
|  78 | 120 | Time                       | 64-bit [smalltime](https://github.com/kstenerud/smalltime/blob/master/smalltime-specification.md) |
|  79 | 121 | Time                       | 64-bit [nanotime](https://github.com/kstenerud/smalltime/blob/master/nanotime-specification.md) |
|  7a | 122 | Negative Integer (8 bit)   | [8-bit negative integer]                      |
|  7b | 123 | Negative Integer (16 bit)  | [16-bit negative integer]                     |
|  7c | 124 | Negative Integer (32 bit)  | [32-bit negative integer]                     |
|  7d | 125 | Negative Integer (64 bit)  | [64-bit negative integer]                     |
|  7e | 126 | Negative Integer (128 bit) | [128-bit negative integer]                    |
|  7f | 127 | Padding                    |                                               |
|  80 | 128 | String: 0 bytes            |                                               |
|  81 | 129 | String: 1 byte             | [1 octet of data]                             |
|  82 | 130 | String: 2 bytes            | [2 octets of data]                            |
|  83 | 131 | String: 3 bytes            | [3 octets of data]                            |
|  84 | 132 | String: 4 bytes            | [4 octets of data]                            |
|  85 | 133 | String: 5 bytes            | [5 octets of data]                            |
|  86 | 134 | String: 6 bytes            | [6 octets of data]                            |
|  87 | 135 | String: 7 bytes            | [7 octets of data]                            |
|  88 | 136 | String: 8 bytes            | [8 octets of data]                            |
|  89 | 137 | String: 9 bytes            | [9 octets of data]                            |
|  8a | 138 | String: 10 bytes           | [10 octets of data]                           |
|  8b | 139 | String: 11 bytes           | [11 octets of data]                           |
|  8c | 140 | String: 12 bytes           | [12 octets of data]                           |
|  8d | 141 | String: 13 bytes           | [13 octets of data]                           |
|  8e | 142 | String: 14 bytes           | [14 octets of data]                           |
|  8f | 143 | String: 15 bytes           | [15 octets of data]                           |
|  90 | 144 | String                     | [byte length] [UTF-8 encoded string]          |
|  91 | 145 | Binary Data                | [byte length] [data]                          |
|  92 | 146 | Comment                    | [byte length] [UTF-8 encoded string]          |
|  93 | 147 | List                       |                                               |
|  94 | 148 | Map                        |                                               |
|  95 | 149 | End of Container           |                                               |
|  96 | 150 | Integer value -106         |                                               |
| ... | ... | ...                        |                                               |
|  fe | 254 | Integer value -2           |                                               |
|  ff | 255 | Integer value -1           |                                               |



Scalar Types
------------

### Boolean

True or false.

Examples:

    [70] = false
    [71] = true


### Integer

Integers are encoded as positive or negative, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Values from -106 to 105 ("small int") are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload as UNSIGNED integers (not two's complement), and the field type determines the sign to be applied.

Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [6a 7f] = 127
    [6a ff] = 255
    [7a ff] = -255
    [6c 40 42 0f 00] = 1,000,000
    [7d 00 10 a5 d4 e8 00 00 00] = -1000000000000

#### Negative Integers

Negative integers are encoded with the sign implicit in the type field. This means that negative fields can store smaller values than any language implementation can for the same width integer data type (we can store 1 extra bit). Because of this, negative 128 bit values that don't fit into a signed 128 bit integer are not allowed (the high bit of a negative 128 bit integer must always be 0).


### Binary Floating Point

IEEE 754 binary floating point types, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Examples:

    [72 00 00 48 41] = 12.5
    [73 cd cc cc cc cc 04 94 40] = 1281.2


### Decimal Floating Point

IEEE 754 decimal floating point densely packed decimal types, 32, 64, or 128 bits wide. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary. They can be read directly from the buffer in little endian byte order.

Example:

    [75 4b 00 00 b2] = -7.5
    [75 0c 32 00 32] = 1281.2


### Time

Time is represented using the [Smalltime](https://github.com/kstenerud/smalltime/blob/master/smalltime-specification.md) or [Nanotime](https://github.com/kstenerud/smalltime/blob/master/nanotime-specification.md)  formats, stored in little endian byte order.

Example:

    [78 2e bc 0d 59 48 6b f0 01] = Oct 26, 1985, 8:22:16.900142 +00:00
    [79 ff c9 9a fb be df cf 1d] = Dec 31, 1999, 23:59:59.999999999 +00:00



Array Types
-----------

An "array" for the purposes of this spec is a contiguous series of octets, preceded by a length field.


### Array Length Field

The array length field is a little endian encoded unsigned integer that represents the number of octets in the array.

The two lowest bits (in the first byte as it is stored little endian) form the width code, and determine the full field width:

| Code |  Width  | Bit Layout                                                              |
| ---- | ------- | ----------------------------------------------------------------------- |
|   0  |  6 bits | LLLLLL00                                                                |
|   1  | 14 bits | LLLLLL01 LLLLLLLL                                                       |
|   2  | 30 bits | LLLLLL10 LLLLLLLL LLLLLLLL LLLLLLLL                                     |
|   3  | 62 bits | LLLLLL11 LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL |

To read the length:

  * Read the lower 2 bits of the first byte to get the width code subfield (`width_code = first_byte & 3`).
  * If the width code > 0, read from the same location as a little endian unsigned integer of the corresponding width (16, 32, or 64 bits).
  * Shift "right" unsigned by 2 (`length = value >> 2`) to get the final value.

Examples:

    [00] Length 0.
    [0c] Length 3.
    [a0 0f] Length 1000.


### Binary Data

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian byte order whenever possible.

    [91] [Length] [Octet 0] ... [Octet (Length-1)]

Examples:

    [91 14 01 02 03 04 05] = binary array {0x01, 0x02, 0x03, 0x04, 0x05}


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string WITHOUT a byte order mark (BOM). The length field contains the byte length (length in octets), NOT the character length.

    [90] [Length] [Octet 0] ... [Octet (Length-1)]

For byte lengths from 0 to 15, there are special top-level inferred-length string types (0x80 - 0x8f). For longer strings, use the general (0x90) string type.

    [80]
    [81] [octet 0]
    [82] [octet 0] [octet 1]
    ...

Examples:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 54 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺


### Comment

Comments are string metadata that may be placed inside a document. While they may be placed anywhere an object may be placed, they are semantically ignored by the parser (they do not constitute values). For example, the sequence [(list) (int8 value 1) (comment "this is a comment") (int8 value 2) (end)] semantically resolves to a list containing the values 1 and 2, with the user possibly being informed of the comment's occurrence (at the decoder's discretion).

A decoder is free to discard or preserve comments. 

The length field contains the byte length (length in octets), NOT the character length.

    [92] [Length] [Octet 0] ... [Octet (Length-1)]

#### Character Restrictions

The following characters are disallowed in comments:

 * Control characters (Unicode 0000-001f, 007f-009f) EXCEPT for TAB (u+0009), which is allowed
 * Line breaking characters (such as u+2028, u+2029)
 * Byte order mark

The following characters are allowed in comments only if they don't also appear in the above disallowed list:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

Example:

    [92 01 01 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20 66
     61 69 6c 73 20 74 6f 20 73 74 61 72 74 20 6f 6e 20 61 72 6d 36 34 20
     75 6e 6c 65 73 73 20 42 20 6c 61 74 63 68 20 69 73 20 73 65 74]
    = Bug #95512: System fails to start on arm64 unless B latch is set



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

List elements are simply objects (type field + possible payload). The list is terminated by an "end of container" marker.

Note: While this spec allows mixed types in lists, not all languages do. Use mixed types with caution.

Example:

    [93 01 6b 88 13 95] = A list containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `nil` type. Values may be any mix of any type, including other containers.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (32-bit float)

Map contents are stored as key-value pairs of objects:

    [94] [key 1] [value 1] [key 2] [value 2] ... [95]

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution.

Example:

    [94 81 61 01 81 62 02 95] = A map containg the key-value pairs ("a", 1) ("b", 2)



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages may have restrictions on how and if it may be used.

Example:

    [6f] = No data


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type may occur any number of times before a type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.



Illegal Encodings
-----------------

Illegal encodings must not be used, as they may cause problems or even API violations in certain languages. A decoder may discard illegal encodings, or may even abort processing.

  * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
  * Containers must be properly terminated with `end container` tags. Extra `end container` tags are invalid.
  * All map keys must have corresponding values.
  * Map keys must not be container types or the `nil` type.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000 and 32-bit float 1000.0).
  * An array's length field must match the byte-length of its data.



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders are encouraged to find the smallest type and width that stores a numeric value without data loss.

For specialized applications, an encoder implementation may choose to preserve larger types and widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications may require data to be aligned in some cases. For example, some processors cannot read unaligned multibyte data types without compiler intervention. Others can read the data unaligned, but may take more processing time to do so. An encoder could be tuned to insert `padding` bytes when encoding certain types.

    read_data(buffer, buffer_length);

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 7f | 7f | 7f | 6c | 00 | 00 | 00 | 8f |

As an alternative to padding, if you have a known schema for your data, you could structure your read offset in the decoder such that the data just happens to align correctly:

    read_data(buffer+3, buffer_length-3);

| Bytes 0-3      | Bytes 4-7     |
| -------------- | ------------- |
| xx xx xx 6c    | 00 00 00 8f   |
| positive int32 | 0x8f000000    |



File Format
-----------

The CBE file format allows storing of CBE encoded data in a standard way.


### Naming

CBE files should use the extension `cbe`. For example: `mydata.cbe`


### Contents

A CBE file is composed of a CBE header, followed by a CBE encoded object.

    [CBE header] [CBE encoded object]


### CBE Header

The 4-byte header is composed of the characters `C`, `B`, `E`, and then a version number.

For example, a file encoded in CBE format version 1 would begin with the header `[43 42 45 01]`


### Encoded Object

The encoded data following the header contains optional padding bytes, followed by a single top-level object. You can store multiple objects by using a collection as the "single" object.

Example:

    [43 42 45 01 7f 7f 7f 6c 00 00 00 8f] =
    CBE file containing 0x8f000000, padded such that the 32-bit integer starts on a 4-byte boundary.



Version History
---------------

June 5, 2018: Preview Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
