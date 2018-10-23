Concise Binary Encoding
=======================

A general purpose, machine-readable, compact representation of semi-structured hierarchical data.

Designed with the following goals:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow native reading directly off the wire
  * Balanced space and computation efficiency
  * Minimal complexity



Types Overview
--------------

Many common data types and structures are supported:


### Scalar Types

Binary, stored in little endian byte order.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Boolean  | True or false                                        |
| Integer  | Signed two's complement, from 8 to 128 bits          |
| Float    | IEEE 754 floating point, from 32 to 128 bits         |
| Decimal  | IEEE 754 densely packed decimal, from 64 to 128 bits |
| Time     | UTC-based date & time, to the microsecond            |


### Array Types

Array types can hold multiple scalar values of the same type and size.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Array    | Array of a scalar type                               |
| Bitfield | Packed "array" of bits                               |
| String   | Array of UTF-8 encoded bytes (no BOM)                |


### Container Types

Containers can store any type, including other containers and mixed types.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| List     | A list containing any types                          |
| Map      | Scalar or array types for keys, any types for values |


### Other Types

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Empty    | Denotes the absence of data                          |
| Padding  | Used to align data in a CPU friendly manner.         |



Encoding
--------

All objects are composed of a type field and possibly a payload.


### Type Field

| Code | Type               | Payload                                        |
| ---- | ------------------ | ---------------------------------------------- |
|  00  | Integer value 0    |                                                |
|  01  | Integer value 1    |                                                |
| ...  | ...                | ...                                            |
|  68  | Integer value 104  |                                                |
|  69  | Boolean True       |                                                |
|  6a  | Boolean False      |                                                |
|  6b  | List               | [object] ... [end container]                   |
|  6c  | Map                | [key object, value object] ... [end container] |
|  6d  | End of Container   |                                                |
|  6e  | Empty (no data)    |                                                |
|  6f  | Padding            |                                                |
|  70  | String: 0 bytes    |                                                |
|  71  | String: 1 byte     | [1 octet of data]                              |
|  72  | String: 2 bytes    | [2 octets of data]                             |
|  73  | String: 3 bytes    | [3 octets of data]                             |
|  74  | String: 4 bytes    | [4 octets of data]                             |
|  75  | String: 5 bytes    | [5 octets of data]                             |
|  76  | String: 6 bytes    | [6 octets of data]                             |
|  77  | String: 7 bytes    | [7 octets of data]                             |
|  78  | String: 8 bytes    | [8 octets of data]                             |
|  79  | String: 9 bytes    | [9 octets of data]                             |
|  7a  | String: 10 bytes   | [10 octets of data]                            |
|  7b  | String: 11 bytes   | [11 octets of data]                            |
|  7c  | String: 12 bytes   | [12 octets of data]                            |
|  7d  | String: 13 bytes   | [13 octets of data]                            |
|  7e  | String: 14 bytes   | [14 octets of data]                            |
|  7f  | String: 15 bytes   | [15 octets of data]                            |
|  80  | String             | [length] [UTF-8 encoded string]                |
|  81  | Array: Boolean     | [length] [bitfield]                            |
|  82  | Array: Int 8       | [length] [8-bit signed integers]               |
|  83  | Array: Int 16      | [length] [16-bit signed integers]              |
|  84  | Array: Int 32      | [length] [32-bit signed integers]              |
|  85  | Array: Int 64      | [length] [64-bit signed integers]              |
|  86  | Array: Int 128     | [length] [128-bit signed integers]             |
|  87  | Array: Float 32    | [length] [32-bit floats]                       |
|  88  | Array: Float 64    | [length] [64-bit floats]                       |
|  89  | Array: Float 128   | [length] [128-bit floats]                      |
|  8a  | Array: Decimal 64  | [length] [64-bit decimals]                     |
|  8b  | Array: Decimal 128 | [length] [128-bit decimals]                    |
|  8c  | Array: Time        | [length] [64-bit times]                        |
|  8d  | Integer            | [16-bit two's complement signed integer]       |
|  8e  | Integer            | [32-bit two's complement signed integer]       |
|  8f  | Integer            | [64-bit two's complement signed integer]       |
|  90  | Integer            | [128-bit two's complement signed integer]      |
|  91  | Float              | [IEEE 754 binary32 floating point]             |
|  92  | Float              | [IEEE 754 binary64 floating point]             |
|  93  | Float              | [IEEE 754 binary128 floating point]            |
|  94  | Decimal            | [IEEE 754 decimal64, Densely Packed Decimal]   |
|  95  | Decimal            | [IEEE 754 decimal128, Densely Packed Decimal]  |
|  96  | Time               | [64-bit y,d,h,m,s,us]                          |
|  97  | Integer value -105 |                                                |
| ...  | ...                | ...                                            |
|  fe  | Integer value -2   |                                                |
|  ff  | Integer value -1   |                                                |



Types
-----

### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur up to 15 times before any type field (to support aligning anything up to 128 bits wide). A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types (such as arrays) fall on an aligned address for faster direct reads off the buffer.

Example:

    [6f 6f 6f 84 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    Array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded such that the integers begin on a 4-byte boundary.


### Empty Type

Represents the absence of data. Some languages implement this as the NULL value.

Use this with care, as some languages may have restrictions on how it may be used in data structures.

Example:

    [6e] = No data


### Boolean Type

True or false.

Examples:

    [69] = true
    [6a] = false


### Integer Type

Integers are always signed, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Values from -105 to 104 are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload.


Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [8d 7f 00] = 127
    [8e 40 42 0f 00] = 1,000,000
    [8f 00 f0 5a 2b 17 ff ff ff] = -1000000000000


### Floating Point Type

IEEE 754 binary floating point types, 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Examples:

    [91 00 00 48 41] = 12.5
    [92 66 66 66 66 66 42 A0 40] = 1281.2


### Decimal Type

IEEE 754 decimal64 and decimal128 densely packed decimal types, used in financial applications where emulation of decimal rounding is necessary. They can be 64 or 128 bits wide, and can be read directly off the buffer in little endian byte order.

Example:

    [94 D0 03 00 00 00 00 30 A2] = -7.50


### Time Type

Date/time values use an ordinal format (day-of-year rather than day-of-month), are UTC based, and are binary packed into a signed 64-bit integer. The packed representation can be read directly off the buffer in little endian byte order. They are comparable, but cannot be used arithmetically.

#### Encoding

| Field       | Bits | Min     | Max    | Extraction Algorithm |
| ----------- | ---- | ------- | ------ | -------------------- |
| Year        |   18 | -131072 | 131071 | (value >> 46)        |
| Day         |    9 |       1 |    366 | (value >> 37) & 511  |
| Hour        |    5 |       0 |     23 | (value >> 32) & 31   |
| Minute      |    6 |       0 |     59 | (value >> 26) & 63   |
| Second      |    6 |       0 |     60 | (value >> 20) & 63   |
| Microsecond |   20 |       0 | 999999 | value & 1048575      |

Note: Day goes to 366 to support leap years, and second goes to 60 to support leap seconds.

#### The Year Field

The year field is interpreted as a signed two's complement integer. Values <= 0 represent dates in the BC era. The Anno Domini system has no zero year (there is no 0 BC or 0 AD), so BC era dates are offset by 1 (0 = 1 BC, -1 = 2 BC, and so on). This allows for dates from 131073 BC to 131071 AD.

Example:

    [96 2e bc 0d 59 48 66 f0 01] = 1985-10-26T08:22:16.900142Z = Oct 26th, 1985 8:22:16.900142 GMT


### Array Type

An array of scalar values. All elements are of the same type and width.

    [Type] [Length] [Element 0] ... [Element (Length-1)]

The elements themselves do *NOT* contain a type field; only a payload. The content type is inferred from the array type.

The string and bitfield arrays are handled a little differently, and will be discussed separately.

#### Array Length Field

The array length field is an unsigned integer that represents the number of elements in the array, not the byte count (the only exception to this is the string type - see below).

The two lowest bits are the width code, and determine the full field width:

| Code |  Width  | Bit Layout        |
| ---- | ------- | ----------------- |
|   0  |  6 bits | 00LLLLLL          |
|   1  | 14 bits | 10LLLLLL +  8 x L |
|   2  | 30 bits | 01LLLLLL + 24 x L |
|   3  | 62 bits | 11LLLLLL + 56 x L |

Legend: LSB to the left, MSB to the right, L = length bit

To read the length:

  * Read the first byte and mask the lower 2 bits to get the width subfield ( width = byte & 3 ).
  * Read from the same location as an unsigned integer of the correct width and shift "right" by 2 ( length = value >> 2 ).

Examples:

    [87 00] Array of 32-bit floats, length 0.
    [83 0c 18 fc 00 00 e8 03] Array of 16-bit integers, length 3, contents (-1000, 0, 1000).
    [88 a0 0f ...] = Array of 64-bit floats, length 1000 (contents omitted for brevity).


#### Special Case: Boolean Array

Boolean arrays are encoded as little endian bitfields (least significant bit first). The length of the array is in bits.

Bitfields start at the low bit (0) of the first byte, and end on one of the bits in byte (n/8)-1 (remember: array unit is bits, not bytes). Bits in the higher positions above the last bit of the last byte are cleared.

|     |  Byte 0  |  Byte 1  | ... | Byte (n/8)-1 |
| --- | -------- | -------- | --- | ------------ |
| Bit | 01234567 | 01234567 | ... | 01234xxx     |
|     | <- LSB   |          |     | MSB ->       |

For example:

    [81 a4 B1 F9 4C C3 D9 01] = 10001101100111110011001011000011100110111


### String Type

Strings are specialized byte arrays, containing the UTF-8 representation of a string WITHOUT a byte order mark (BOM). The length field contains the byte length (length in octets), NOT the character length.

For byte lengths from 0 to 15, there are special top-level inferred-length string types. For larger strings, use the general (0x80) string type.

Examples:

    [7b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [7d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [80 54 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺

Implementations requiring null-terminated strings can modify the underlying buffer after extracting values and offsets.

Example: Map containing {"alpha": 1, "beta": 2}

Original data:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7  | 8      | 9  | 10 | 11 | 12 | 13 | 14            |
| --- | ------ | -- | -- | -- | -- | -- | -- | ------ | -- | -- | -- | -- | -- | ------------- |
| 6c  | 75     | 61 | 6c | 70 | 68 | 61 | 01 | 74     | 62 | 65 | 74 | 61 | 02 | 6d            |
| Map | String | a  | l  | p  | h  | a  | 1  | String | b  | e  | t  | a  | 2  | End container |

First, extract data, including pointers to offset 2 ("alpha") and offset 9 ("beta").

Next, apply null termination by overwriting the type field of the next value following the string:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7   | 8      | 9  | 10 | 11 | 12 | 13  | 14            |
| --- | ------ | -- | -- | -- | -- | -- | --- | ------ | -- | -- | -- | -- | --- | ------------- |
| 6c  | 75     | 61 | 6c | 70 | 68 | 61 | 00  | 74     | 62 | 65 | 74 | 61 | 00  | 6d            |
| Map | String | a  | l  | p  | h  | a  | nul | String | b  | e  | t  | a  | nul | End container |


### List Type

Lists are generalized object containers that can contain any type, including other containers. Elements of a list don't have to be all of the same type.

List elements are stored using regular object encoding (type field + possible payload), and the list is terminated by an "end of container" marker.

Example:

    [6b 01 8d 88 13 6d] = List containing integers (1, 5000)


### Map Type

A map associates objects (keys) with other objects (values). Keys may be scalar or array types, and must not be `empty`. Values may be of any type, including other containers.

Map contents are stored as key-value pair tuples using regular object encoding (type field + possible payload):

    [6c] [key 1] [value 1] [key 2] [value 2] ... [6d]

All keys in a map must be unique, even mathematically and across type widths. For example, you cannot store both 0x1000 (16-bit) and 0x00001000 (32-bit) as keys in the same map.

Example:

    [6c 71 61 01 71 62 02 6d] = Map containg the key-value pairs ("a", 1) ("b", 2)



Illegal Encodings
-----------------

Illegal encodings must not be used, as they will cause problems or even API violations in certain languages & platforms. A decoder may discard illegal encodings.

  * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
  * Map keys must not be container types or the `empty` type.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000).
  * An array's length field must match the length of its data.



Smallest Possible Size
----------------------

Preservation of the original data width information is not considered important by default (with the exception of arrays). Encoders are encouraged to find the smallest width of the same data type that stores a value without loss.

For specialized applications, an encoder implementation may choose to preserve larger type widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications may require data to be aligned in some cases. For example, some processors cannot read unaligned multibyte data types without compiler intervention. Others can read the data unaligned, but may take more processing time to do so. An encoder could be tuned to insert `padding` bytes when encoding certain types.

This can be especially beneficial for arrays, where the cost of a little upfront padding overhead makes the entire array more efficient to read on certain architectures.

    read_data(buffer, buffer_length);

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | ... |
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --- |
| 6f | 6f | 6f | 84 | 80 | 38 | 01 | 00 | ff | ff | ff | 7f | fe | ff | ff | 7f | ... |

As an alternative to padding, if you have a known schema for your data, you could structure your read offset in the decoder such that the data just happens to align correctly:

    read_data(buffer+3, buffer_length-3);

| Bytes 0-3      | Bytes 4-7     | Bytes 8-11  | Bytes 12-15 | ... |
| -------------- | ------------- | ----------- | ----------- | --- |
| xx xx xx 84    | 80 38 01 00   | ff ff ff 7f | fe ff ff 7f | ... |
| Array of int32 | Length 20,000 | 0x7fffffff  | 0x7ffffffe  | ... |



File Format
-----------

The CBE file format allows storing of CBE encoded data in a standard way.


### Naming

CBE files should use the extension `cbe`. For example: `mydata.cbe`


### Contents

A CBE file is composed of a CBE header, followed by a CBE encoded object.

    [CBE header] [CBE encoded object]


### Header

The 4-byte header is composed of the characters `C`, `B`, `E`, and then a version number.

For example, a file encoded in CBE format version 1 would begin with the header `[43 42 45 01]`


### Encoded Object

The encoded data following the header contains optional padding bytes, followed by a single top-level object. You can store multiple objects by using a collection or array as the "single" object.

Example:

    [43 42 45 01 6f 6f 6f 84 80 38 01 00 ff ff ff 7f fe ff ff 7f ...] =
    CBE file containing an array of 20,000 int32s (0x7fffffff, 0x7ffffffe, ...), padded such that the integers start on a 4-byte boundary.



Version History
---------------

June 5, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
