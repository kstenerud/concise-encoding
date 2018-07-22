Concise Binary Encoding
=======================

A general purpose, machine-readable, compact representation of semi-structured hierarchical data.

Designed with the following points of focus:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow native reading directly off the wire
  * Compact data in ways that don't affect reading directly of the wire
  * Balanced space and computation efficiency
  * Minimal complexity



Types Overview
--------------

Many common data types and structures are supported:


### Scalar Types

Binary, stored in little endian byte order.

  * **Boolean**: True or false.
  * **Integer**: Always signed, two's complement, in widths from 8 to 128 bits.
  * **Float**: IEEE 754 floating point, in widths from 32 to 128 bits.
  * **Decimal**: IEEE 754 decimal, in widths from 64 to 128 bits, DPD encoding.
  * **Time**: UTC-based date + time with precision to the second, millisecond or microsecond.


### Array Types

Array types can hold multiple scalar values of the same type and size.

  * **Array**: Array of a scalar type
  * **Bitfield**: Packed "Array" of bits
  * **String**: Array of UTF-8 encoded characters without BOM


### Container Types

Containers can store any type, including other containers. They can also contain mixed types.

  * **List**: A list of any type, including mixed types
  * **Map**: Can use any scalar or array types for keys, and all types for values


### Support Types

Types used in support of other types only. They cannot be used on their own.

  * **Array Length**: Unsigned integer in widths from 8 to 64 bits.


### Other Types

  * **Empty**: Denotes the absence of data.
  * **Padding**: Invisible field used to align data in a CPU friendly manner.



Encoding
--------

All objects are composed of a type field and a possible payload.


### Type Field

| Code | Type               | Payload                                           |
| ---- | ------------------ | ------------------------------------------------- |
|  00  | Integer value 0    |                                                   |
|  01  | Integer value 1    |                                                   |
| ...  | ...                | ...                                               |
|  67  | Integer value 103  |                                                   |
|  68  | Empty (no data)    |                                                   |
|  69  | Time (second)      | [40-bit data]                                     |
|  6a  | Time (millisecond) | [48-bit data]                                     |
|  6b  | Time (microsecond) | [64-bit data]                                     |
|  6c  | List               | [object] ... [end of container]                   |
|  6d  | Map                | [key object, value object] ... [end of container] |
|  6e  | End of Container   |                                                   |
|  6f  | Padding            |                                                   |
|  70  | String (empty)     |                                                   |
|  71  | String (1 byte)    | [1 octet of data]                                 |
|  72  | String (2 bytes)   | [2 octets of data]                                |
|  73  | String (3 bytes)   | [3 octets of data]                                |
|  74  | String (4 bytes)   | [4 octets of data]                                |
|  75  | String (5 bytes)   | [5 octets of data]                                |
|  76  | String (6 bytes)   | [6 octets of data]                                |
|  77  | String (7 bytes)   | [7 octets of data]                                |
|  78  | String (8 bytes)   | [8 octets of data]                                |
|  79  | String (9 bytes)   | [9 octets of data]                                |
|  7a  | String (10 bytes)  | [10 octets of data]                               |
|  7b  | String (11 bytes)  | [11 octets of data]                               |
|  7c  | String (12 bytes)  | [12 octets of data]                               |
|  7d  | String (13 bytes)  | [13 octets of data]                               |
|  7e  | String (14 bytes)  | [14 octets of data]                               |
|  7f  | String (15 bytes)  | [15 octets of data]                               |
|  80  | String             | [length] [UTF-8 encoded string]                   |
|  81  | Array Boolean      | [length] [bitfield]                               |
|  82  | Array Int 8        | [length] [8-bit signed integers]                  |
|  83  | Array Int 16       | [length] [16-bit signed integers]                 |
|  84  | Array Int 32       | [length] [32-bit signed integers]                 |
|  85  | Array Int 64       | [length] [64-bit signed integers]                 |
|  86  | Array Int 128      | [length] [128-bit signed integers]                |
|  87  | Array Float 32     | [length] [32-bit floats]                          |
|  88  | Array Float 64     | [length] [64-bit floats]                          |
|  89  | Array Float 128    | [length] [128-bit floats]                         |
|  8a  | Array Decimal 64   | [length] [64-bit decimals]                        |
|  8b  | Array Decimal 128  | [length] [128-bit decimals]                       |
|  8c  | Array Time         | [length] [64-bit times]                           |
|  8d  | Integer            | [16-bit two's complement signed integer]          |
|  8e  | Integer            | [32-bit two's complement signed integer]          |
|  8f  | Integer            | [64-bit two's complement signed integer]          |
|  90  | Integer            | [128-bit two's complement signed integer]         |
|  91  | Float              | [IEEE 754 binary32 floating point]                |
|  92  | Float              | [IEEE 754 binary64 floating point]                |
|  93  | Float              | [IEEE 754 binary128 floating point]               |
|  94  | Decimal            | [IEEE 754 decimal64, Densely Packed Decimal]      |
|  95  | Decimal            | [IEEE 754 decimal128, Densely Packed Decimal]     |
|  96  | False              |                                                   |
|  97  | True               |                                                   |
|  98  | Integer value -104 |                                                   |
| ...  | ...                | ...                                               |
|  fe  | Integer value -2   |                                                   |
|  ff  | Integer value -1   |                                                   |



Types
-----

### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. A decoder must read it and discard it. An encoder may add padding between objects to help larger data types (such as arrays) fall on an aligned address for faster direct reads off the buffer.

Example:

    [6f 6f 6f 84 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    Array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded to begin on a 4-byte boundary.


### Empty Type

Represents the absence of data. Some languages implement this as the NULL value.

Example:

    [98] = No data


### Boolean Type

True or false. As the data itself is stored in the type field, there is no payload.

Examples:

    [96] = false
    [97] = true


### Integer Type

Integers are always signed, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Values from -104 to 103 are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload.


Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [8d 00 7f] = 127
    [8e 40 42 0f 00] = 1,000,000
    [8f 00 f0 5a 2b 17 ff ff ff] = -1000000000000


### Floating Point Type

IEEE 754 binary floating point types, at 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Examples:

    [91 00 00 48 41] = 12.5
    [92 66 66 66 66 66 42 A0 40] = 1281.2


### Decimal Type

IEEE 754 decimal64 and decimal128 types, used in financial applications where emulation of decimal rounding is necessary. They can be 64 or 128 bits wide, and can be read directly off the buffer in little endian byte order.

Example:

    [94 D0 03 00 00 00 00 30 A2] = -7.50


### Time Type

Date/time values follow the Gregorian calendar, are UTC based, and are mathematically packed into unsigned integers using base-10 multiplication. Their integer representation can be read directly off the buffer in little endian byte order.

Encoded times are comparable, but cannot be used arithmetically.


#### 40-bit Time

| Field  | Min | Max   | Multiplier | Notes                              |
| ------ | --- | ----- | ---------- | ---------------------------------- |
| Year   | 0   | 33648 |  32676480  |                                    |
| Month  | 0   |    11 |   2723040  | Encoded as 0-11, representing 1-12 |
| Day    | 0   |    30 |     87840  | Encoded as 0-30, representing 1-31 |
| Hour   | 0   |    23 |      3660  |                                    |
| Minute | 0   |    59 |        61  |                                    |
| Second | 0   |    60 |         1  | 0-60 to allow for leap seconds     |

Time fields can be extracted as follows:

* second field = value % 61
* minute field = (value / 61) % 60
* hour field = (value / 3660) % 24
* day field = ((value / 87840) % 31) + 1
* month field = ((value / 2723040) % 12) + 1
* year field = value / 32676480

Example:

    [69 21 34 57 e1 0e] = 1955-11-05T08:21:00Z = Nov 5th, 1955 08:21:00 GMT


#### 48-bit Microsecond Timestamp

| Field       | Min | Max   | Multiplier  | Notes                              |
| ----------- | --- | ----- | ----------- | ---------------------------------- |
| Year        | 0   |  8613 | 32676480000 |                                    |
| Month       | 0   |    11 |  2723040000 | Encoded as 0-11, representing 1-12 |
| Day         | 0   |    30 |    87840000 | Encoded as 0-30, representing 1-31 |
| Hour        | 0   |    23 |     3660000 |                                    |
| Minute      | 0   |    59 |       61000 |                                    |
| Second      | 0   |    60 |        1000 | 0-60 to allow for leap seconds     |
| Microsecond | 0   |   999 |           1 |                                    |

Example:

    [6a 12 55 a3 6b e8 3b] = 2015-10-21T14:28:09.714Z = Oct 21st, 2015 14:28:09.714 GMT


#### 64-bit Nanosecond Timestamp

| Field       | Min | Max    | Multiplier     | Notes                              |
| ----------- | --- | ------ | -------------- | ---------------------------------- |
| Year        | 0   | 564526 | 32676480000000 |                                    |
| Month       | 0   |     11 |  2723040000000 | Encoded as 0-11, representing 1-12 |
| Day         | 0   |     30 |    87840000000 | Encoded as 0-30, representing 1-31 |
| Hour        | 0   |     23 |     3660000000 |                                    |
| Minute      | 0   |     59 |       61000000 |                                    |
| Second      | 0   |     60 |        1000000 | 0-60 to allow for leap seconds     |
| Microsecond | 0   |    999 |           1000 |                                    |
| Nanosecond  | 0   |    999 |              1 |                                    |

Example:

    [6b ae a3 95 f2 b2 88 e6 00] = 1985-10-26T08:22:16.900142Z = Oct 26th, 1985 8:22:16.900142 GMT


### Array Type

An array of scalar values. All elements are of the same type and width.

    [Type] [Length] [Element 0] ... [Element (Length-1)]

The elements themselves do *NOT* contain a type field; only a payload. The content type is inferred from the array type.

The string and bitfield arrays are handled a little differently, and will be discussed separately.

#### Length Field

The array length field is an unsigned integer that represents the number of elements in the array, not the byte count (the only exception to this is the string type).

The two lowest bits are the width code, and determine the full field width:

| Code |  Width  | Bit Layout in Memory (little endian) LSB to the left, L = length bit    |
| ---- | ------- | ----------------------------------------------------------------------- |
|   0  |  6 bits | 00LLLLLL                                                                |
|   1  | 14 bits | 10LLLLLL LLLLLLLL                                                       |
|   2  | 30 bits | 01LLLLLL LLLLLLLL LLLLLLLL LLLLLLLL                                     |
|   3  | 62 bits | 11LLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL |

To read the length:

  * Read the first byte, lower 2 bits to get the width subfield ( width = byte & 3 ).
  * Read from the same location as an unsigned integer of the correct width and shift "right" by 2 ( length = value >> 2 ).

Examples:

    [87 00] Array of 32-bit floats, length 0.
    [83 0c 18 fc 00 00 e8 03] Array of 16-bit integers, length 3, contents (-1000, 0, 1000).
    [88 a0 0f ...] = Array of 64-bit floats, length 1000 (contents omitted for brevity).


#### Special Case: Boolean Array

Boolean arrays are encoded as little endian bitfields (least significant bit first). The length of the array is in bits.

Bitfields start at the low bit (0) of the first byte, and end on one of the bits in byte (n/8)-1 (remember: array unit is bits, not bytes). Bits in the higher positions of the last byte are cleared.

|     |  Byte 0  |  Byte 1  | ... | Byte (n/8)-1 |
| --- | -------- | -------- | --- | ------------ |
| Bit | 01234567 | 01234567 | ... | 01234xxx     |
|     | <- LSB   |          |     | MSB ->       |

For example:

    [81 a4 B1 F9 4C C3 D9 01] = 10001101100111110011001011000011100110111


### String Type

Strings are specialized byte arrays, containing the UTF-8 representation of a string WITHOUT a byte order mark (BOM). The length field contains the byte length (length in octets), not the character length.

For byte lengths from 0 to 15, there are special top-level inferred-length string types. For larger strings, use the array type.

Examples:

    [7b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [7d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [80 54 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺

Implementations requiring null-terminated strings can modify the underlying buffer after extracting values and offsets.

Example: Map containing {"alpha": 1, "beta": 2}

Original data:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7  | 8      | 9  | 10 | 11 | 12 | 13 | 14            |
| --- | ------ | -- | -- | -- | -- | -- | -- | ------ | -- | -- | -- | -- | -- | ------------- |
| 6d  | 75     | 61 | 6c | 70 | 68 | 61 | 01 | 74     | 62 | 65 | 74 | 61 | 02 | 6e            |
| Map | String | a  | l  | p  | h  | a  | 1  | String | b  | e  | t  | a  | 2  | End container |

First, extract data, including pointers to offset 2 ("alpha") and offset 9 ("beta").

Next, apply null termination:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7   | 8      | 9  | 10 | 11 | 12 | 13  | 14            |
| --- | ------ | -- | -- | -- | -- | -- | --- | ------ | -- | -- | -- | -- | --- | ------------- |
| 6d  | 75     | 61 | 6c | 70 | 68 | 61 | 00  | 74     | 62 | 65 | 74 | 61 | 00  | 6e            |
| Map | String | a  | l  | p  | h  | a  | nul | String | b  | e  | t  | a  | nul | End container |


### List Type

Lists are generalized object containers that can contain any type, including other containers. Elements of a list don't have to be all of the same type.

List elements are stored using regular object encoding (type field + possible payload), and the list is terminated by an "end of container" marker.

Example:

    [6c 01 8d 88 13 6e] = List containing integers (1, 5000)


### Map Type

A map associates objects (keys) with other objects (values). Keys may be scalar or array types, and must not be EMPTY. Values may be of any type, including other containers.

Map contents are stored as key-value pair tuples using regular object encoding (type field + possible payload):

    [6d] [key 1] [value 1] [key 2] [value 2] ... [6e]

All keys in a map must be unique, even across type widths. For example, you cannot store both 1000 (16-bit) and 1000 (32-bit) as keys in the same map.

Example:

    [6d 71 61 01 71 62 02 6e] = Map containg the key-value pairs ("a", 1) ("b", 2)



Illegal Encodings
-----------------

Illegal encodings must not be used, as they will cause problems or even API violations in certain languages. A decoder may discard illegal encodings.

  * Times must be valid. For example: February 31st, while technically encodable, is not allowed.
  * Map keys must not be container types or the EMPTY type.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths that resolve to the exact same value.
  * An array's element type must be a scalar type. Arrays of arrays, containers, or EMPTY, are not allowed.
  * An array's length field must match the length of its data.



Smallest Possible Size
----------------------

Preservation of the original data width information is not considered important by default. Encoders are encouraged to find the smallest width of the same data type that stores a value without loss.

For specialized applications, an encoder implementation may choose to preserve larger type widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications may require data to be aligned in some cases. For example, some processors cannot read unaligned multibyte data types without compiler intervention. Others can read the data unaligned, but may take more processing time to do so. An encoder could be tuned to insert PADDING bytes when encoding certain types.

This can be especially beneficial for arrays, where the cost of a little upfront padding overhead makes the entire array more efficient to read.

    read_data(buffer, buffer_length);

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | ... |
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --- |
| 6f | 6f | 6f | 84 | 80 | 38 | 01 | 00 | ff | ff | ff | 7f | fe | ff | ff | 7f | ... |

Alternatively, if you have a known schema for your data, you could structure your read offset on the decoder end such that the data just happens to align correctly:

    read_data(buffer+3, buffer_length-3);

| Bytes 0-3      | Bytes 4-7     | Bytes 8-11  | Bytes 12-15 | ... |
| -------------- | ------------- | ----------- | ----------- | --- |
| xx xx xx 84    | 80 38 01 00   | ff ff ff 7f | fe ff ff 7f | ... |
| Array of int32 | Length 20,000 | 0x7fffffff  | 0x7ffffffe  | ... |



File Format
-----------

The CBE file format allows storing of CBE encoded data in a standard way.


### Naming

CBE files should use the extension "cbe". For example: "mydata.cbe"


### Contents

A CBE file is composed of a CBE header, followed by a CBE encoded object.

    [CBE header] [CBE encoded object]


### Header

The 4-byte header is composed of the characters 'C', 'B', 'E', and then a version number.

For example, a file encoded in CBE format version 1 would begin with the header [43 42 45 01]


### Encoded Object

The encoded data following the header must contain zero or more padding bytes followed by a single top-level object. You can store multiple objects by using a collection or array as the "single" object.

Example:

    [43 42 45 01 6f 6f 6f 84 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    CBE file containing an array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded to begin on a 4-byte boundary.



Version History
---------------

June 5, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
