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

| Code | Type               | Payload                                       |
| ---- | ------------------ | --------------------------------------------- |
|  00  | Integer value 0    |                                               |
|  01  | Integer value 1    |                                               |
| ...  | ...                |                                               |
|  6d  | Integer value 109  |                                               |
|  6e  | Integer (16 bit)   | [16-bit two's complement signed integer]      |
|  6f  | Integer (32 bit)   | [32-bit two's complement signed integer]      |
|  70  | Integer (64 bit)   | [64-bit two's complement signed integer]      |
|  71  | Integer (128 bit)  | [128-bit two's complement signed integer]     |
|  72  | Float (32 bit)     | [IEEE 754 binary32 floating point]            |
|  73  | Float (64 bit)     | [IEEE 754 binary64 floating point]            |
|  74  | Float (128 bit)    | [IEEE 754 binary128 floating point]           |
|  75  | Decimal (32 bit)   | [IEEE 754 decimal32, Densely Packed Decimal]  |
|  76  | Decimal (64 bit)   | [IEEE 754 decimal64, Densely Packed Decimal]  |
|  77  | Decimal (128 bit)  | [IEEE 754 decimal128, Densely Packed Decimal] |
|  78  | Time               | [64-bit y,d,h,m,s,us]                         |
|  79  | Boolean True       |                                               |
|  7a  | Boolean False      |                                               |
|  7b  | List               |                                               |
|  7c  | Map                |                                               |
|  7d  | End of Container   |                                               |
|  7e  | Empty (no data)    |                                               |
|  7f  | Padding            |                                               |
|  80  | String: 0 bytes    |                                               |
|  81  | String: 1 byte     | [1 octet of data]                             |
|  82  | String: 2 bytes    | [2 octets of data]                            |
|  83  | String: 3 bytes    | [3 octets of data]                            |
|  84  | String: 4 bytes    | [4 octets of data]                            |
|  85  | String: 5 bytes    | [5 octets of data]                            |
|  86  | String: 6 bytes    | [6 octets of data]                            |
|  87  | String: 7 bytes    | [7 octets of data]                            |
|  88  | String: 8 bytes    | [8 octets of data]                            |
|  89  | String: 9 bytes    | [9 octets of data]                            |
|  8a  | String: 10 bytes   | [10 octets of data]                           |
|  8b  | String: 11 bytes   | [11 octets of data]                           |
|  8c  | String: 12 bytes   | [12 octets of data]                           |
|  8d  | String: 13 bytes   | [13 octets of data]                           |
|  8e  | String: 14 bytes   | [14 octets of data]                           |
|  8f  | String: 15 bytes   | [15 octets of data]                           |
|  90  | String             | [byte length] [UTF-8 encoded string]          |
|  91  | Binary Data        | [byte length] [data]                          |
|  92  | Integer value -110 |                                               |
| ...  | ...                |                                               |
|  fe  | Integer value -2   |                                               |
|  ff  | Integer value -1   |                                               |



Scalar Types
------------

### Boolean

True or false.

Examples:

    [78] = false
    [79] = true


### Integer

Integers are always signed, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Values from -110 to 109 are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload.


Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [6e 7f 00] = 127
    [6f 40 42 0f 00] = 1,000,000
    [70 00 f0 5a 2b 17 ff ff ff] = -1000000000000


### Binary Floating Point

IEEE 754 binary floating point types, 32, 64, or 128 bits wide. They can be read directly from the buffer in little endian byte order.

Examples:

    [72 00 00 48 41] = 12.5
    [73 66 66 66 66 66 42 A0 40] = 1281.2


### Decimal Floating Point

IEEE 754 decimal floating point densely packed decimal types, 32, 64, or 128 bits wide. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary. They can be read directly from the buffer in little endian byte order.

Example:

    [76 D0 03 00 00 00 00 30 A2] = -7.50


### Time

Time is represented using the [Smalltime](https://github.com/kstenerud/smalltime) format, stored in little endian byte order.

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

  * Read the lower 2 bits of the first byte to get the width code subfield ( width_code = first_byte & 3 ).
  * If the width code > 0, read from the same location as a little endian unsigned integer of the corresponding width (16, 32, or 64 bits).
  * Shift "right" unsigned by 2 ( length = value >> 2 ) to get the final value.

Examples:

    [00] Length 0.
    [0c] Length 3.
    [a0 0f] Length 1000.


### Binary Data

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian byte order whenever possible.

    [91] [Length] [Octet 0] ... [Octet (Length-1)]


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
| 7c  | 85     | 61 | 6c | 70 | 68 | 61 | 01 | 84     | 62 | 65 | 74 | 61 | 02 | 7d            |
| Map | String | a  | l  | p  | h  | a  | 1  | String | b  | e  | t  | a  | 2  | End container |

First, extract data (values `1` and `2`), including pointers to offset 2 ("alpha") and offset 9 ("beta").

Next, apply null termination by overwriting the type field of the next value following each string:

| 0   | 1      | 2  | 3  | 4  | 5  | 6  | 7       | 8      | 9  | 10 | 11 | 12 | 13      | 14            |
| --- | ------ | -- | -- | -- | -- | -- | ------- | ------ | -- | -- | -- | -- | ------- | ------------- |
| 7c  | 85     | 61 | 6c | 70 | 68 | 61 | **00**  | 84     | 62 | 65 | 74 | 61 | **00**  | 7d            |
| Map | String | a  | l  | p  | h  | a  | **nul** | String | b  | e  | t  | a  | **nul** | End container |



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

List elements are stored using regular object encoding (type field + possible payload), and the list is terminated by an "end of container" marker.

Example:

    [7b 01 68 88 13 7d] = A list containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `empty` type. Values may be any mix of any type, including other containers.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (32-bit float)

Map contents are stored as key-value pair tuples using regular object encoding (type field + possible payload):

    [7c] [key 1] [value 1] [key 2] [value 2] ... [7d]

Example:

    [7c 81 61 01 81 62 02 7d] = A map containg the key-value pairs ("a", 1) ("b", 2)



Other Types
-----------

### Empty

Represents the absence of data. Some languages implement this as the NULL value.

Use this with care, as some languages may have restrictions on how it may be used in data structures.

Example:

    [7e] = No data


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type may occur up to 15 times before any type field (to support aligning anything up to 128 bits wide). A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 6f 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.



Illegal Encodings
-----------------

Illegal encodings must not be used, as they may cause problems or even API violations in certain languages. A decoder may discard illegal encodings, or may even abort processing.

  * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
  * Containers must be properly terminated with `end container` tags. Extra `end container` tags are invalid.
  * All map keys must have corresponding values.
  * Map keys must not be container types or the `empty` type.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000 and 32-bit float 1000.0).
  * An array's length field must match the length of its data.



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
| 7f | 7f | 7f | 6f | 00 | 00 | 00 | 8f |

As an alternative to padding, if you have a known schema for your data, you could structure your read offset in the decoder such that the data just happens to align correctly:

    read_data(buffer+3, buffer_length-3);

| Bytes 0-3      | Bytes 4-7     |
| -------------- | ------------- |
| xx xx xx 6f    | 00 00 00 8f   |
| int32          | 0x8f000000    |



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

    [43 42 45 01 7f 7f 7f 6f 00 00 00 8f] =
    CBE file containing 0x8f000000, padded such that the int32 starts on a 4-byte boundary.



Version History
---------------

June 5, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
