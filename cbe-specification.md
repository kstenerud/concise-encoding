Concise Binary Encoding
=======================

Concise Binary Encoding (CBE) is a general purpose, machine-readable, compact representation of semi-structured hierarchical data.

CBE is non-cycic and hierarchical like XML and JSON, and supports the most common data types natively. CBE is type compatible with [Concise Text Encoding (CTE)](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md), but is a binary format for space efficiency. The more common types and values tend to use less space. Its encoding is primarily byte oriented to simplify codec implementation and off-the-wire inspection.



Features
--------

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Easy to parse (very few sub-byte fields)
  * Binary format to minimize transmission costs
  * Little endian byte ordering to allow the most common systems to read directly off the wire
  * Balanced space and computation efficiency
  * Minimal complexity
  * Type compatible with [Concise Text Encoding (CTE)](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md)



Contents
--------

* [Structure](#structure)
  - [Maximum Depth](#maximum-depth)
* [Encoding](#encoding)
  - [Type Field](#type-field)
* [Scalar Types](#scalar-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Binary Floating Point](#binary-floating-point)
  - [Decimal Floating Point](#decimal-floating-point)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
* [Array Types](#array-types)
  - [Array Length Field](#array-length-field)
  - [Bytes](#bytes)
  - [String](#string)
  - [URI](#uri)
  - [Comment](#comment)
    - [Comment Character Restrictions](#comment-character-restrictions)
* [Container Types](#container-types)
  - [List](#list)
  - [Unordered Map](#unordered-map)
  - [Ordered Map](#ordered-map)
  - [Inline Containers](#inline-containers)
* [Other Types](#other-types)
  - [Nil](#nil)
  - [Padding](#padding)
  - [RESERVED](#reserved)
* [Invalid Encodings](#invalid-encodings)
* [Smallest Possible Size](#smallest-possible-size)
* [Alignment](#alignment)
* [File Format](#file-format)
* [Version History](#version-history)
* [License](#license)



Structure
---------

A CBE document is a binary encoded document consisting of a single, top-level object of any type. To store multiple values in a CBE document, use a container as the top-level object and store other objects within that container.

### Maximum Depth

Since nested objects (in containers such as maps and lists) are possible, it is necessary to impose an arbitrary depth limit to insure interoperability between implementations. For the purposes of this spec, that limit is 1000 levels of nesting from the top level container to the most nested object (inclusive), unless both sending and receiving parties agree to a different max depth.



Encoding
--------

All objects are composed of an 8-bit type field and possibly a payload.


#### Type Field

| Hex | Dec | Type                      | Payload                                       |
| --- | --- | ------------------------- | --------------------------------------------- |
|  00 |   0 | Integer value 0           |                                               |
|  01 |   1 | Integer value 1           |                                               |
| ... | ... | ...                       |                                               |
|  64 | 100 | Integer value 100         |                                               |
|  65 | 101 | Positive Integer (8 bit)  | [8-bit unsigned integer]                      |
|  66 | 102 | Positive Integer (16 bit) | [16-bit positive integer]                     |
|  67 | 103 | Positive Integer (32 bit) | [32-bit positive integer]                     |
|  68 | 104 | Positive Integer (64 bit) | [64-bit positive integer]                     |
|  69 | 105 | Positive Integer          | [[RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)] |
|  6a | 106 | Date                      | [[Compact Date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date)] |
|  6b | 107 | Time                      | [[Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time)] |
|  6c | 108 | Timestamp                 | [[Compact Timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp)] |
|  6d | 109 | RESERVED                  |                                               |
|  6e | 110 | RESERVED                  |                                               |
|  6f | 111 | RESERVED                  |                                               |
|  70 | 112 | RESERVED                  |                                               |
|  71 | 113 | RESERVED                  |                                               |
|  72 | 114 | List                      |                                               |
|  73 | 115 | Unordered Map             |                                               |
|  74 | 116 | Ordered Map               |                                               |
|  75 | 117 | End of Container          |                                               |
|  76 | 118 | Binary Float (32 bit)     | [IEEE 754 binary32 floating point]            |
|  77 | 119 | Binary Float (64 bit)     | [IEEE 754 binary64 floating point]            |
|  78 | 120 | Binary Float (128 bit)    | [IEEE 754 binary128 floating point]           |
|  79 | 121 | Decimal Float (32 bit)    | [IEEE 754 decimal32, Binary Integer Decimal]  |
|  7a | 122 | Decimal Float (64 bit)    | [IEEE 754 decimal64, Binary Integer Decimal]  |
|  7b | 123 | Decimal Float (128 bit)   | [IEEE 754 decimal128, Binary Integer Decimal] |
|  7c | 124 | Boolean False             |                                               |
|  7d | 125 | Boolean True              |                                               |
|  7e | 126 | Nil (no data)             |                                               |
|  7f | 127 | Padding                   |                                               |
|  80 | 128 | String: 0 bytes           |                                               |
|  81 | 129 | String: 1 byte            | [1 octet of data]                             |
|  82 | 130 | String: 2 bytes           | [2 octets of data]                            |
|  83 | 131 | String: 3 bytes           | [3 octets of data]                            |
|  84 | 132 | String: 4 bytes           | [4 octets of data]                            |
|  85 | 133 | String: 5 bytes           | [5 octets of data]                            |
|  86 | 134 | String: 6 bytes           | [6 octets of data]                            |
|  87 | 135 | String: 7 bytes           | [7 octets of data]                            |
|  88 | 136 | String: 8 bytes           | [8 octets of data]                            |
|  89 | 137 | String: 9 bytes           | [9 octets of data]                            |
|  8a | 138 | String: 10 bytes          | [10 octets of data]                           |
|  8b | 139 | String: 11 bytes          | [11 octets of data]                           |
|  8c | 140 | String: 12 bytes          | [12 octets of data]                           |
|  8d | 141 | String: 13 bytes          | [13 octets of data]                           |
|  8e | 142 | String: 14 bytes          | [14 octets of data]                           |
|  8f | 143 | String: 15 bytes          | [15 octets of data]                           |
|  90 | 144 | String                    | [byte length] [UTF-8 encoded string]          |
|  91 | 145 | Bytes                     | [byte length] [data]                          |
|  92 | 146 | URI                       | [byte length] [[URI](https://tools.ietf.org/html/rfc3986)]                           |
|  93 | 147 | Comment                   | [byte length] [UTF-8 encoded string]          |
|  94 | 148 | RESERVED                  |                                               |
|  95 | 149 | RESERVED                  |                                               |
|  96 | 150 | RESERVED                  |                                               |
|  9a | 151 | Negative Integer          | [[RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)] |
|  98 | 152 | Negative Integer (64 bit) | [64-bit negative integer]                     |
|  99 | 153 | Negative Integer (32 bit) | [32-bit negative integer]                     |
|  9a | 154 | Negative Integer (16 bit) | [16-bit negative integer]                     |
|  9b | 155 | Negative Integer (8 bit)  | [8-bit unsigned integer]                      |
|  9c | 156 | Integer value -100        |                                               |
| ... | ... | ...                       |                                               |
|  fe | 254 | Integer value -2          |                                               |
|  ff | 255 | Integer value -1          |                                               |



Scalar Types
------------

### Boolean

True or false.

Examples:

    [7c] = false
    [7d] = true


### Integer

Integers are encoded as positive or negative values, and can be of any width. The fixed width types (16 to 64 bit) are stored in little endian byte order.

Values from -100 to +100 ("small int") are encoded into the type field itself, and may be read directly as 8-bit signed two's complement integers. Values outside of this range are stored as separate types, with the payload containing the absolute value and the field type determining the sign to be applied.

Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [65 7f] = 127
    [65 ff] = 255
    [9b ff] = -255
    [67 40 42 0f 00] = 1,000,000
    [98 00 10 a5 d4 e8 00 00 00] = -1000000000000


### Binary Floating Point

IEEE 754 binary floating point types, 32, 64, or 128 bits wide. They are stored in little endian byte order.

Examples:

    [76 00 00 48 41] = 12.5
    [77 cd cc cc cc cc 04 94 40] = 1281.2


### Decimal Floating Point

IEEE 754 decimal floating point binary integer decimal types, 32, 64, or 128 bits wide. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary. They are stored in little endian byte order.

Example:

    [79 4b 00 00 b2] = -7.5
    [79 0c 32 00 32] = 1281.2


Temporal Types
--------------

### Date

Dates are represented using the [compact date](https://github.com/kstenerud/compact-date/blob/master/compact-date-specification.md#compact-date) format.

Example:

    [78 2e bc 0d 59 48 6b f0 01] = Oct 26, 1985, 8:22:16.900142 +00:00
    [79 ff c9 9a fb be df cf 1d] = Dec 31, 1999, 23:59:59.999999999 +00:00


### Time

Time is represented using the [compact time](https://github.com/kstenerud/compact-date/blob/master/compact-date-specification.md#compact-time) format.

Example:

    [78 2e bc 0d 59 48 6b f0 01] = Oct 26, 1985, 8:22:16.900142 +00:00
    [79 ff c9 9a fb be df cf 1d] = Dec 31, 1999, 23:59:59.999999999 +00:00


### Timestamp

Timestamps are represented using the [compact timestamp](https://github.com/kstenerud/compact-date/blob/master/compact-date-specification.md#compact-timestamp) format.

Example:

    [78 2e bc 0d 59 48 6b f0 01] = Oct 26, 1985, 8:22:16.900142 +00:00
    [79 ff c9 9a fb be df cf 1d] = Dec 31, 1999, 23:59:59.999999999 +00:00



Array Types
-----------

An "array" for the purposes of this spec is a contiguous sequence of octets, preceded by a length field.


### Array Length Field

All array types are preceded by an array length field, representing the number of octets in the array.

The array length field is encoded as an [RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md).


### Bytes

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian byte order whenever possible.

    [91] [Length] [Octet 0] ... [Octet (Length-1)]

Examples:

    [91 14 01 02 03 04 05] = byte array {0x01, 0x02, 0x03, 0x04, 0x05}


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string. Strings must not contain a byte order mark (BOM) or the NUL (u+0000) character, but may contain any other valid character.

The length field holds the byte length (length in octets), NOT the character length.

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


### URI

Uniform Resource Identifier, which must be valid according to [RFC 3986](https://tools.ietf.org/html/rfc3986).

The length field contains the byte length (length in octets), NOT the character length.

Example:

    [92 55 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top

    [92 6c 6d 61 69 6c 74 6f 3a 4a 6f 68 6e 2e 44 6f 65 40 65 78 61 6d 70
     6c 65 2e 63 6f 6d]
    = mailto:John.Doe@example.com

    [92 cc 75 72 6e 3a 6f 61 73 69 73 3a 6e 61 6d 65 73 3a 73 70 65 63 69
     66 69 63 61 74 69 6f 6e 3a 64 6f 63 62 6f 6f 6b 3a 64 74 64 3a 78 6d
     6c 3a 34 2e 31 2e 32]
    = urn:oasis:names:specification:docbook:dtd:xml:4.1.2


### Comment

Comments are string metadata that may be placed inside a document. They may be placed anywhere an object may be placed, and are semantically ignored by the parser (they do not constitute values). For example, the sequence [(list) (int8 value 1) (comment "this is a comment") (int8 value 2) (end)] semantically resolves to a list containing the values 1 and 2, with the user possibly being informed of the comment's occurrence (at the decoder's discretion).

A decoder is free to discard or preserve comments. 

The length field contains the byte length (length in octets), NOT the character length.

    [93] [Length] [Octet 0] ... [Octet (Length-1)]

#### Comment Character Restrictions

Comment contents must contain only complete and valid UTF-8 sequences. Escape sequences are not interpreted (they are passed on verbatim).

The following characters are disallowed:

 * Control characters (Unicode u+0000 to u+001f, u+007f to u+009f) with the exception of:
   - Horizontal Tab (u+0009)
   - Linefeed (u+000a)
 * Line breaking characters (such as u+2028, u+2029).
 * Byte order mark.

The following characters are allowed in comments if they aren't in the above disallowed list:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

As only the linefeed character is allowed for line endings, an encoder must convert native line endings (for example CR+LF) to linefeeds for encoding. A decoder may convert linefeeds to another line ending format if desired.

Example:

    [93 01 01 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20 66
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

    [72 01 6b 88 13 75] = A list containing integers (1, 5000)


### Unordered Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `nil` type. Values may be any mix of any type, including other containers.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash, and are therefore invalid:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (32-bit float)

Map contents are stored as key-value pairs of objects:

    [73] [key 1] [value 1] [key 2] [value 2] ... [75]

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution. A decoder may abort processing or ignore key-value pairs of mixed key types if the implementation language doesn't support it.

An implementation must not rely upon any particular ordering of key-value pairs when reading an unordered map.

Example:

    [73 81 61 01 81 62 02 75] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Ordered Map

An ordered map works the same as an unordered map, except that the order of the key-value pairs in the map is significant, and must be preserved.

Example:

    [74 81 61 01 81 62 02 75] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Inline Containers

CBE documents in data communication messages are often implemented as lists or maps at the top level. To help save bytes, CBE allows "inline" top-level containers as a special case.

The use of and type of inline container as the top level object must be agreed to by all parties involved (usually via a protocol specification), and an alternate method to delimit the beginning and end of the container (such as a length field) must be provided. In such a case, the "container begin" (0x94 for list, 0x95 for map) and "container end" (0x96) markers may be omitted in the top level container.

Inline containers are not supported in the [CBE file format](#file-format).



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages may have restrictions on how and if it may be used.

Example:

    [7e] = No data


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type may occur any number of times before a type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 67 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.


### RESERVED

This type is reserved for future expansion of the format, and must not be used.



Invalid Encodings
-----------------

The structure and format of CBE leaves room for certain encodings that contain problematic data. These are called invalid encodings. A decoder must halt processing when an invalid encoding is encountered.

  * Times must be valid. For example: hour 30, while technically encodable, is invalid.
  * Containers must be properly terminated with `end container` tags. Extra `end container` tags are invalid.
  * All map keys must have corresponding values. A key with a missing value is invalid.
  * Map keys must not be container types or the `nil` type. Keys of these types are invalid.
  * Maps must not contain duplicate keys. This includes numeric keys of different widths or types that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000 and 32-bit float 1000.0). Duplicate map keys are invalid.
  * An array's length field must match the byte-length of its data. An invalid array length may not be directly detectable, but in such a case will likely lead to other invalid encodings due to array data being interpreted as other types.
  * UTF-8 strings must contain only valid UTF-8 sequences that evaluate to complete, valid characters.
  * *RESERVED* types are invalid.



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders are encouraged to find the smallest type and width that stores a numeric value without data loss.

For specialized applications, an encoder implementation may choose to preserve larger types and widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications may require data to be aligned in some cases for optimal decoding performance. For example, some processors may not be able to read unaligned multibyte data types without special (costly) intervention. An encoder could in theory be tuned to insert `padding` bytes when encoding certain data:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 7f | 7f | 7f | 67 | 00 | 00 | 00 | 8f |

Alignment is usually only useful when the target decoder is known prior to encoding. It is mostly an optimization for closed systems.



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

    [43 42 45 01 7f 7f 7f 67 00 00 00 8f] =
    CBE file containing 0x8f000000, padded such that the 32-bit integer starts on a 4-byte boundary.



Version History
---------------

June 5, 2018: Preview Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
