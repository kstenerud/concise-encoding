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



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/



Version History
---------------

May 27, 2018: Version 1



Types Overview
--------------

Many common data types and structures are supported:


### Scalar Types

Binary, stored in little endian byte order.

  * **Boolean**: Supports the values true and false.
  * **Integer**: Always signed, two's complement, in widths from 8 to 128 bits.
  * **Float**: IEEE 754 floating point, in widths from 32 to 128 bits.
  * **Decimal**: IEEE 754 decimal, in widths from 64 to 128 bits. Both BID and DPD are supported.
  * **Date**: UTC-based date with precision to the second.
  * **Timestamp**: UTC-based date with precision to the millisecond or nanosecond.


### Array Types

Array types can hold multiple scalar values of the same type and size.

  * **Array**: Array of any scalar type
  * **Bytes**: Array of octets
  * **String**: Array of octets in UTF-8 encoding without BOM


### Container Types

Containers can store any type, including other containers. They can also contain mixed types.

  * **List**: A list of any type, including mixed types
  * **Map**: Can use any scalar or array types for keys, and all types for values


### Support Types

Types used in support of other types only. They cannot be used on their own.

  * **Length**: Unsigned integer in widths from 8 to 64 bits.


### Other Types

  * **Empty**: Denotes the absence of data.



Encoding
--------

All objects are composed of a type field and a possible payload.


### Type Field

| Code | Type               | Payload                                             |
| ---- | ------------------ | --------------------------------------------------- |
|  00  | Integer value 0    |                                                     |
|  01  | Integer value 1    |                                                     |
| ...  | ...                | ...                                                 |
|  64  | Integer value 100  |                                                     |
|  65  | Empty (no data)    |                                                     |
|  66  | False              |                                                     |
|  67  | True               |                                                     |
|  68  | Date               | [40-bit data]                                       |
|  69  | Timestamp (ms)     | [48-bit data]                                       |
|  6a  | Timestamp (ns)     | [64-bit data]                                       |
|  6b  | End of Container   |                                                     |
|  6c  | List               | [object] ... [end of container]                     |
|  6d  | Map                | [key object, value object] ... [end of container]   |
|  6e  | Array              | [element type] [length in elements] [element] ...   |
|  6f  | String             | [length in octets] [octets]                         |
|  70  | String (empty)     |                                                     |
|  71  | String (1 byte)    | [1 octet of data]                                   |
|  72  | String (2 bytes)   | [2 octets of data]                                  |
|  73  | String (3 bytes)   | [3 octets of data]                                  |
|  74  | String (4 bytes)   | [4 octets of data]                                  |
|  75  | String (5 bytes)   | [5 octets of data]                                  |
|  76  | String (6 bytes)   | [6 octets of data]                                  |
|  77  | String (7 bytes)   | [7 octets of data]                                  |
|  78  | String (8 bytes)   | [8 octets of data]                                  |
|  79  | String (9 bytes)   | [9 octets of data]                                  |
|  7a  | String (10 bytes)  | [10 octets of data]                                 |
|  7b  | String (11 bytes)  | [11 octets of data]                                 |
|  7c  | String (12 bytes)  | [12 octets of data]                                 |
|  7d  | String (13 bytes)  | [13 octets of data]                                 |
|  7e  | String (14 bytes)  | [14 octets of data]                                 |
|  7f  | String (15 bytes)  | [15 octets of data]                                 |
|  80  | Bytes  (empty)     |                                                     |
|  81  | Bytes  (1 byte)    | [1 octet of data]                                   |
|  82  | Bytes  (2 bytes)   | [2 octets of data]                                  |
|  83  | Bytes  (3 bytes)   | [3 octets of data]                                  |
|  84  | Bytes  (4 bytes)   | [4 octets of data]                                  |
|  85  | Bytes  (5 bytes)   | [5 octets of data]                                  |
|  86  | Bytes  (6 bytes)   | [6 octets of data]                                  |
|  87  | Bytes  (7 bytes)   | [7 octets of data]                                  |
|  88  | Bytes  (8 bytes)   | [8 octets of data]                                  |
|  89  | Bytes  (9 bytes)   | [9 octets of data]                                  |
|  8a  | Bytes  (10 bytes)  | [10 octets of data]                                 |
|  8b  | Bytes  (11 bytes)  | [11 octets of data]                                 |
|  8c  | Bytes  (12 bytes)  | [12 octets of data]                                 |
|  8d  | Bytes  (13 bytes)  | [13 octets of data]                                 |
|  8e  | Bytes  (14 bytes)  | [14 octets of data]                                 |
|  8f  | Bytes  (15 bytes)  | [15 octets of data]                                 |
|  90  | Bytes              | [length in octets] [octets]                         |
|  91  | Integer            | [16-bit two's complement signed integer]            |
|  92  | Integer            | [32-bit two's complement signed integer]            |
|  93  | Integer            | [64-bit two's complement signed integer]            |
|  94  | Integer            | [128-bit two's complement signed integer]           |
|  95  | Float              | [IEEE 754 binary32 floating point]                  |
|  96  | Float              | [IEEE 754 binary64 floating point]                  |
|  97  | Float              | [IEEE 754 binary128 floating point]                 |
|  98  | Decimal            | [IEEE 754 decimal64, Binary Integer Decimal]        |
|  99  | Decimal            | [IEEE 754 decimal128, Binary Integer Decimal]       |
|  9a  | Decimal            | [IEEE 754 decimal64, Densely Packed Decimal]        |
|  9b  | Decimal            | [IEEE 754 decimal128, Densely Packed Decimal]       |
|  9c  | Integer value -100 |                                                     |
| ...  | ...                | ...                                                 |
|  fe  | Integer value -2   |                                                     |
|  ff  | Integer value -1   |                                                     |



Types
-----


### Length Type

A length is unsigned, and can only be used in array types to denote their length. It is encoded as follows:

| Code | Type              | Payload                                             |
| ---- | ----------------- | --------------------------------------------------- |
|  00  | Integer value 0   |                                                     |
|  01  | Integer value 1   |                                                     |
| ...  | ...               | ...                                                 |
|  fc  | Integer value 252 |                                                     |
|  fd  | Integer           | (64-bit unsigned integer)                           |
|  fe  | Integer           | (32-bit unsigned integer)                           |
|  ff  | Integer           | (16-bit unsigned integer)                           |

Examples:

    [f0] = Length 240
    [ff ff 00] = length 255
    [fe a0 86 01 00] = Length 100000


### Empty Type

Represents the absence of data. Some languages implement this as the NULL value.

Example:

    [95] = No data


### Boolean Type

True or false. As the data itself is stored in the type field, there is no payload.

Examples:

    [66] = false
    [67] = true


### Integer Type

Integers are always signed, and can be 8, 16, 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Values from -100 to 100 are encoded in the type field, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored in the payload.


Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [91 00 7f] = 127
    [92 40 42 0f 00] = 1,000,000
    [93 00 f0 5a 2b 17 ff ff ff] = -1000000000000


### Floating Point Type

IEEE 754 binary floating point types, at 32, 64, or 128 bits wide. They can be read directly off the buffer in little endian byte order.

Examples:

    [95 00 00 48 41] = 12.5
    [96 66 66 66 66 66 42 A0 40] = 1281.2


### Decimal Type

IEEE 754 decimal64 and decimal128 types, used in financial applications where emulation of decimal rounding is necessary. They can be 64 or 128 bits wide, and can be read directly off the buffer in little endian byte order.

Both binary integer decimal (BID) and densely packed decimal (DPD) are supported, as neither has yet displaced the other.

Example:

    [9a D0 03 00 00 00 00 30 A2] = -7.50 as a 64-bit densely packed decimal


### Date Type

Dates follow the Gregorian calendar, are UTC based, and are mathematically packed into unsigned integers using base-10 multiplication. Their integer representation can be read directly off the buffer in little endian byte order.

Encoded dates are comparable, but cannot be used arithmetically.


#### 40-bit Date

| Field  | Min | Max   | Multiplier | Notes                              |
| ------ | --- | ----- | ---------- | ---------------------------------- |
| Year   | 0   | 33648 |  32676480  |                                    |
| Month  | 0   |    11 |   2723040  | Encoded as 0-11, representing 1-12 |
| Day    | 0   |    30 |     87840  | Encoded as 0-30, representing 1-31 |
| Hour   | 0   |    23 |      3660  |                                    |
| Minute | 0   |    59 |        61  |                                    |
| Second | 0   |    60 |         1  | 0-60 to allow for leap seconds     |

Date fields can be extracted as follows:

* second field = value % 61
* minute field = (value / 61) % 60
* hour field = (value / 3660) % 24
* day field = ((value / 87840) % 31) + 1
* month field = ((value / 2723040) % 12) + 1
* year field = value / 32676480

Example:

    [68 21 34 57 e1 0e] = 1955-11-05T08:21:00Z = Nov 5th, 1955 08:21:00 GMT


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

    [69 12 55 a3 6b e8 3b] = 2015-10-21T14:28:09.714Z = Oct 21st, 2015 14:28:09.714 GMT


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

    [6a ae a3 95 f2 b2 88 e6 00] = 1985-10-26T08:22:16.900142Z = Oct 26th, 1985 8:22:16.900142 GMT


### Array Type

An array of scalar values. All elements MUST be of the same type AND width.

    [6e] [Element Type] [Length] [Element 0] ... [Element (Length-1)]

The elements themselves do *NOT* contain a type field; only a payload. The type is specified in the `Element Type` field of the array. In this way, the elements of an array can be read directly off the buffer, accessible by type punning a portion of the buffer as an array of the native type.

Note: Arrays cannot contain other arrays or containers or the EMPTY type.

Example:

    [6e 91 03 18 fc 00 00 e8 03] = array of 16-bit integers, containing -1000, 0, 1000.


### Bytes Type

The bytes type is an array that is implicitly of type 8-bit signed integer. It has no element type field.

The generalized form is as follows:

    [90] [Length] [Byte 0] ...  [Byte (Length-1)]

There are also specialized forms for lengths 15 and under, where the length is encoded into the type field.

Examples:

    [82 00 00] = 2 byte array containing 0x00, 0x00.
    [90 12 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12] = 18 byte array containing values from 1 to 18.


### String Type

Strings are specialized byte arrays, containing the UTF-8 representation of a string WITHOUT a byte order mark (BOM). The length field contains the byte length (length in octets), not the character length.

The generalized form is as follows:

    [6f] [Length] [Byte 0] ...  [Byte (Length-1)]

There are also specialized forms for byte lengths 15 and under, where the length is encoded into the type field.

Examples:

    [7b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [7c 52 6f 64 65 6c 73 74 72 61 c3 9f 65] = Rodelstraße
    [6f 15 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺


### List Type

Lists are generalized object containers that can contain any type, including other containers. Elements of a list don't have to be all of the same type.

List elements are stored using regular object encoding (type field + possible payload), and the list is terminated by an "end of container" marker.

Example:

    [6c 01 91 88 13 6b] = List containing integers (1, 5000)


### Map Type

A map associates objects (keys) with other objects (values). Keys may be scalar or array types, and must not be EMPTY. Values may be of any type, including other containers.

Map contents are stored as key-value pair tuples using regular object encoding (type field + possible payload):

    [6c] [key 1] [value 1] [key 2] [value 2] ... [6b]

All keys in a map must be unique, even across type widths. For example, you cannot store both 1000 (16-bit) and 1000 (32-bit) as keys in the same map.

Example:

    [6d 71 61 01 71 62 02 6b] = Map containg the key-value pairs ("a", 1) ("b", 2)



Illegal Encodings
-----------------

Illegal encodings must not be used, as they will cause problems or even API violations in certain languages. A decoder may discard illegal encodings.

  * Dates must be valid. For example: February 31st, while technically encodable, is not allowed.
  * Map keys must not be container types or the EMPTY type.
  * Maps must not contain duplicate keys. This includes keys that resolve to the same value.
  * An array's element type must be a scalar type. Arrays of arrays, containers, or EMPTY are not allowed.



Smallest Possible Size
----------------------

Preservation of the original data width information is not considered important by default. Encoders are encouraged to find the smallest width of the same data type that stores a value without loss.

For specialized applications, an encoder implementation may choose to preserve larger type widths as a tradeoff in processing cost vs data size. This must not be considered illegal by a decoder.



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

The encoded data following the header must be a single top-level object. You can store multiple objects by using a collection or array as the "single" object.
