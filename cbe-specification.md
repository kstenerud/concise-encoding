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

All objects are composed of a type field and possibly a payload.


#### Type Field

| Code | Type               | Payload                                        |
| ---- | ------------------ | ---------------------------------------------- |
|  00  | Integer value 0    |                                                |
|  01  | Integer value 1    |                                                |
| ...  | ...                | ...                                            |
|  67  | Integer value 103  |                                                |
|  68  | Integer            | [16-bit two's complement signed integer]       |
|  69  | Integer            | [32-bit two's complement signed integer]       |
|  6a  | Integer            | [64-bit two's complement signed integer]       |
|  6b  | Integer            | [128-bit two's complement signed integer]      |
|  6c  | Float              | [IEEE 754 binary32 floating point]             |
|  6d  | Float              | [IEEE 754 binary64 floating point]             |
|  6e  | Float              | [IEEE 754 binary128 floating point]            |
|  6f  | Decimal            | [IEEE 754 decimal32, Densely Packed Decimal]   |
|  70  | Decimal            | [IEEE 754 decimal64, Densely Packed Decimal]   |
|  71  | Decimal            | [IEEE 754 decimal128, Densely Packed Decimal]  |
|  72  | Time               | [64-bit y,d,h,m,s,us]                          |
|  73  | Array: Boolean     | [length] [bitfield]                            |
|  74  | Array: Int 8       | [length] [8-bit signed integers]               |
|  75  | Array: Int 16      | [length] [16-bit signed integers]              |
|  76  | Array: Int 32      | [length] [32-bit signed integers]              |
|  77  | Array: Int 64      | [length] [64-bit signed integers]              |
|  78  | Array: Int 128     | [length] [128-bit signed integers]             |
|  79  | Array: Float 32    | [length] [32-bit floats]                       |
|  7a  | Array: Float 64    | [length] [64-bit floats]                       |
|  7b  | Array: Float 128   | [length] [128-bit floats]                      |
|  7c  | Array: Decimal 32  | [length] [32-bit decimals]                     |
|  7d  | Array: Decimal 64  | [length] [64-bit decimals]                     |
|  7e  | Array: Decimal 128 | [length] [128-bit decimals]                    |
|  7f  | Array: Time        | [length] [64-bit times]                        |
|  80  | String: 0 bytes    |                                                |
|  81  | String: 1 byte     | [1 octet of data]                              |
|  82  | String: 2 bytes    | [2 octets of data]                             |
|  83  | String: 3 bytes    | [3 octets of data]                             |
|  84  | String: 4 bytes    | [4 octets of data]                             |
|  85  | String: 5 bytes    | [5 octets of data]                             |
|  86  | String: 6 bytes    | [6 octets of data]                             |
|  87  | String: 7 bytes    | [7 octets of data]                             |
|  88  | String: 8 bytes    | [8 octets of data]                             |
|  89  | String: 9 bytes    | [9 octets of data]                             |
|  8a  | String: 10 bytes   | [10 octets of data]                            |
|  8b  | String: 11 bytes   | [11 octets of data]                            |
|  8c  | String: 12 bytes   | [12 octets of data]                            |
|  8d  | String: 13 bytes   | [13 octets of data]                            |
|  8e  | String: 14 bytes   | [14 octets of data]                            |
|  8f  | String: 15 bytes   | [15 octets of data]                            |
|  90  | String             | [byte length] [UTF-8 encoded string]           |
|  91  | List               | [object] ... [end container]                   |
|  92  | Map                | [key object, value object] ... [end container] |
|  93  | End of Container   |                                                |
|  94  | Empty (no data)    |                                                |
|  95  | Padding            |                                                |
|  96  | Boolean False      |                                                |
|  97  | Boolean True       |                                                |
|  98  | Integer value -104 |                                                |
| ...  | ...                | ...                                            |
|  fe  | Integer value -2   |                                                |
|  ff  | Integer value -1   |                                                |



Scalar Types
------------

### Boolean

True or false.

Examples:

    [97] = true
    [96] = false


### Integer

Integers are always signed, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Values from -104 to 103 are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload.


Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [68 7f 00] = 127
    [69 40 42 0f 00] = 1,000,000
    [6a 00 f0 5a 2b 17 ff ff ff] = -1000000000000


### Binary Floating Point

IEEE 754 binary floating point types, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Examples:

    [6c 00 00 48 41] = 12.5
    [6d 66 66 66 66 66 42 A0 40] = 1281.2


### Decimal Floating Point

IEEE 754 decimal floating point densely packed decimal types, 32, 64, or 128 bits wide. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary. They can be read directly from the buffer in little endian byte order.

Example:

    [70 D0 03 00 00 00 00 30 A2] = -7.50


### Time

Time is represented using the [Smalltime](https://github.com/kstenerud/smalltime) format.

#### Encoding

Time fields are bit-encoded according to the following table, which lists their bit positions, bit width, and minimum and maximum allowed values.

| Field       | Position | Width | Min     | Max    | Extraction Algorithm |
| ----------- | -------- | ----- | ------- | ------ | -------------------- |
| Year        |       46 |    18 | -131072 | 131071 | (value >> 46)        |
| Day         |       37 |     9 |       1 |    366 | (value >> 37) & 511  |
| Hour        |       32 |     5 |       0 |     23 | (value >> 32) & 31   |
| Minute      |       26 |     6 |       0 |     59 | (value >> 26) & 63   |
| Second      |       20 |     6 |       0 |     60 | (value >> 20) & 63   |
| Microsecond |        0 |    20 |       0 | 999999 | value & 1048575      |


#### The Year Field

The year field is interpreted as a signed two's complement integer. Values <= 0 represent dates in the BC era.

As the Anno Domini system has no zero year (there is no 0 BC or 0 AD), BC era dates are interpreted offset by 1 in order to preserve numeric continuity (0 = 1 BC, -1 = 2 BC, and so on). Thus, years from 131073 BC to 131071 AD are supported.


#### The Day Field

Day goes to 366 to support leap years.


#### The Second Field

Second goes to 60 to support leap seconds.

Example:

| Encoded Value     | ISO8601 Equivalent        | Meaning                           |
| ----------------- | ------------------------- | --------------------------------- |
| 0x1f06568590dbc2e | 1985-299T08:22:16.900142Z | Oct 26th, 1985 8:22:16.900142 UTC |



Array Types
-----------

### Standard Array

An array of scalar values. All elements are of the same type and width.

    [Type] [Length] [Element 0] ... [Element (Length-1)]

The elements themselves do *NOT* contain a type field; only a payload. The content type is inferred from the array type.

The string and bitfield arrays are handled a little differently, and are discussed separately.

#### Array Length Field

The array length field is a little endian encoded unsigned integer that represents the number of *elements* in the array, not the byte count (the only exception to this is the string type, which is discussed separately).

The two lowest bits are the width code, and determine the full field width:

| Code |  Width  | Bit Layout        |
| ---- | ------- | ----------------- |
|   0  |  6 bits | LLLLLL00          |
|   1  | 14 bits | LLLLLL01 +  8 x L |
|   2  | 30 bits | LLLLLL10 + 24 x L |
|   3  | 62 bits | LLLLLL11 + 56 x L |

To read the length:

  * Read the first byte and mask the lower 2 bits to get the width code subfield ( width_code = first_byte & 3 ).
  * If the width code > 0, read from the same location as an unsigned integer of the corresponding width (16, 32, 64 bits).
  * Shift "right" unsigned by 2 ( length = value >> 2 ) to get the final value.

Examples:

    [79 00] Array of 32-bit floats, length 0.
    [75 0c 18 fc 00 00 e8 03] Array of 16-bit integers, length 3, contents (-1000, 0, 1000).
    [7a a0 0f ...] = Array of 64-bit floats, length 1000 (contents omitted for brevity).


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string WITHOUT a byte order mark (BOM). The length field contains the byte length (length in octets), NOT the character length.

For byte lengths from 0 to 15, there are special top-level inferred-length string types (0x80 - 0x8f). For longer strings, use the general (0x90) string type.

Examples:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 54 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺

For implementations requiring null-terminated strings, one possible option would be to modify the underlying buffer after extracting values and offsets.

Example: Map containing {"alpha": 1, "beta": 2}

Original data:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7  | 8      | 9  | 10 | 11 | 12 | 13 | 14            |
| --- | ------ | -- | -- | -- | -- | -- | -- | ------ | -- | -- | -- | -- | -- | ------------- |
| 92  | 85     | 61 | 6c | 70 | 68 | 61 | 01 | 84     | 62 | 65 | 74 | 61 | 02 | 93            |
| Map | String | a  | l  | p  | h  | a  | 1  | String | b  | e  | t  | a  | 2  | End container |

First, extract data (values `1` and `2`), including pointers to offset 2 ("alpha") and offset 9 ("beta").

Next, apply null termination by overwriting the type field of the next value following each string:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7   | 8      | 9  | 10 | 11 | 12 | 13  | 14            |
| --- | ------ | -- | -- | -- | -- | -- | --- | ------ | -- | -- | -- | -- | --- | ------------- |
| 92  | 85     | 61 | 6c | 70 | 68 | 61 | 00  | 84     | 62 | 65 | 74 | 61 | 00  | 93            |
| Map | String | a  | l  | p  | h  | a  | nul | String | b  | e  | t  | a  | nul | End container |


### Boolean Array

Boolean arrays are encoded as little endian bitfields (least significant bit is in the first byte). The length field represents the number of bits (not bytes) in the array.

Bitfields start at the low bit (0) of the first byte, and end on one of the bits in byte (n/8)-1. Bits in the higher positions above the last bit of the last byte are cleared.

The common convention is to represent byte values with the MSB to the "left". However, bitfields are "logically" understood to be LSB-first bit arrays (whereby the lowest bit of the first byte is the first entry in the array).


| Bit Ordering  |  Byte 0  |  Byte 1  | ... | Byte (n/8)-1 |
| ------------- | -------- | -------- | --- | ------------ |
| Common        | 76543210 | 76543210 | ... | ___43210     |
| Logical       | 01234567 | 01234567 | ... | 01234___     |

For example, an encoded bit array and its logical representation:

    [73 a4 B1 F9 4C C3 D9 01] = 10001101100111110011001011000011100110111



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

List elements are stored using regular object encoding (type field + possible payload), and the list is terminated by an "end of container" marker.

Example:

    [91 01 68 88 13 93] = List containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `empty` type. Values may be any mix of any type, including other containers.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (32-bit float)

Map contents are stored as key-value pair tuples using regular object encoding (type field + possible payload):

    [91] [key 1] [value 1] [key 2] [value 2] ... [93]

Example:

    [91 81 61 01 81 62 02 93] = Map containg the key-value pairs ("a", 1) ("b", 2)



Other Types
-----------

### Empty

Represents the absence of data. Some languages implement this as the NULL value.

Use this with care, as some languages may have restrictions on how it may be used in data structures.

Example:

    [94] = No data


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur up to 15 times before any type field (to support aligning anything up to 128 bits wide). A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types (such as arrays) fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [95 95 95 76 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    Array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded such that the integers begin on a 4-byte boundary.



Illegal Encodings
-----------------

Illegal encodings must not be used, as they may cause problems or even API violations in certain languages. A decoder may discard illegal encodings, or may even abort processing.

  * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
  * Containers must be properly terminated with `end container` tags. Extra `end container` tags are also invalid.
  * All map keys must have corresponding values.
  * Map keys must not be container types or the `empty` type.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000 and 32-bit float 1000.0).
  * An array's length field must match the length of its data.



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default (with the exception of arrays). Encoders are encouraged to find the smallest type and width that stores a numeric value without data loss.

For specialized applications, an encoder implementation may choose to preserve larger types and widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications may require data to be aligned in some cases. For example, some processors cannot read unaligned multibyte data types without compiler intervention. Others can read the data unaligned, but may take more processing time to do so. An encoder could be tuned to insert `padding` bytes when encoding certain types.

This can be especially beneficial for arrays, where the cost of a little upfront padding overhead makes the entire array more efficient to read on certain architectures.

    read_data(buffer, buffer_length);

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | ... |
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --- |
| 95 | 95 | 95 | 76 | 80 | 38 | 01 | 00 | ff | ff | ff | 7f | fe | ff | ff | 7f | ... |

As an alternative to padding, if you have a known schema for your data, you could structure your read offset in the decoder such that the data just happens to align correctly:

    read_data(buffer+3, buffer_length-3);

| Bytes 0-3      | Bytes 4-7     | Bytes 8-11  | Bytes 12-15 | ... |
| -------------- | ------------- | ----------- | ----------- | --- |
| xx xx xx 76    | 80 38 01 00   | ff ff ff 7f | fe ff ff 7f | ... |
| Array of int32 | Length 20,000 | 0x7fffffff  | 0x7ffffffe  | ... |



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

The encoded data following the header contains optional padding bytes, followed by a single top-level object. You can store multiple objects by using a collection or array as the "single" object.

Example:

    [43 42 45 01 95 95 95 76 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    CBE file containing an array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded such that the integers start on a 4-byte boundary.



Version History
---------------

June 5, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
