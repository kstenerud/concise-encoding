Concise Binary Encoding
=======================

Concise Binary Encoding (CBE) is a general purpose, machine-readable, compact binary representation of semi-structured hierarchical data.

CBE is non-cyclic and hierarchical like XML, JSON, BSON, etc, and supports the most common data types natively. The more common types and values tend to use less space. Its encoding is byte oriented to simplify codec implementation and off-the-wire inspection.



Version
-------

Version 1 (prerelease)



Features
--------

 * 1:1 type compatiblility between the binary and text formats. Converting between CBE and [CTE](https://github.com/kstenerud/concise-text-encoding) is transparent, allowing you to use the much smaller and energy efficient binary format for data interchange and storage, converting to/from text only when and where a human needs to be involved.
 * Native support for the most commonly used data types. Concise Encoding aims to support 80% of data use cases natively.
 * Support for metadata and comments.
 * Completely redesigned from the ground up to balance user readability, encoded size, and codec complexity.
 * The formats are fully specified, eliminating ambiguities and covering edge cases, thus facilitating compatibility between implementations and reducing complexity.
 * Documents and specifications are versioned to support future expansion.
 * Easy to parse (very few sub-byte fields).
 * Binary format to minimize transmission costs.
 * Little endian byte ordering where possible to allow the most common systems to read directly off the wire.



Contents
--------

* [Introduction](#introduction)
  - [Supported Types](#supported-types)
* [Terms](#terms)
* [Structure](#structure)
  - [Version Specifier](#version-specifier)
  - [Maximum Depth](#maximum-depth)
  - [Maximum Length](#maximum-length)
* [Encoding](#encoding)
  - [Type Field](#type-field)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Decimal Floating Point](#decimal-floating-point)
  - [Binary Floating Point](#binary-floating-point)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
* [Array Types](#array-types)
  - [Array Length Field](#array-length-field)
  - [Bytes](#bytes)
  - [String](#string)
  - [URI](#uri)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Inline Containers](#inline-containers)
* [Metadata](#metadata)
  - [Metadata Association](#metadata-association)
  - [Metadata Types](#metadata-types)
    - [Name Clashes](#name-clashes)
    - [Predefined Keys](#predefined-keys)
  - [Comment](#comment)
    - [Comment Character Restrictions](#comment-character-restrictions)
* [Other Types](#other-types)
  - [Nil](#nil)
  - [Padding](#padding)
  - [RESERVED](#reserved)
* [Invalid Encodings](#invalid-encodings)
* [Smallest Possible Size](#smallest-possible-size)
* [Alignment](#alignment)
* [Version History](#version-history)
* [License](#license)



Introduction
------------

Many ad-hoc hierarchical data encoding schemes exist today, but the genre has yet to reach its full potential.

JSON was a major improvement over XML, reducing bloat and boilerplate, and more closely modeling the actual data types and data structures used in real-world programs. Many JSON-inspired binary formats later emerged, with varying levels of compatibility.

Unfortunately, since JSON was originally designed to be transparently interpreted by a Javascript engine (now considered a security risk), it lacked many fundamental data types & value ranges and was poorly defined, leading to incompatibility, ambiguity, and tricky edge cases with no clear solution. The binary formats suffered similar problems, and also tended to add many uncommon types that bloated them unnecessarily.

Concise Encoding is the next step in the evolution of ad-hoc hierarchical data formats, aiming to address the shortfalls of the current generation.


### Supported Types

The following types are natively supported, and have full type compatibility with [CTE](https://github.com/kstenerud/concise-text-encoding):

 * Nil
 * Boolean
 * Integer
 * Float
 * Time
 * String
 * URI
 * Bytes
 * List
 * Map
 * Metadata
 * Comment



Terms
-----

The following terms have specific meanings in this specification:

| Term         | Meaning                                                                                                               |
| ------------ | --------------------------------------------------------------------------------------------------------------------- |
| Must (not)   | If this directive is not adhered to, the document or implementation is invalid/non-conformant.                        |
| Should (not) | Every effort should be made to follow this directive, but the document/implementation is still valid if not followed. |
| May (not)    | It is up to the implementation to decide whether to do something or not.                                              |
| Can (not)    | Refers to a possibility or constraint which must be accommodated by the implementation.                               |
| Optional     | The implementation must support both the existence and the absence of the specified item.                             |
| Recommended  | Refers to a "best practice", which should be followed if possible.                                                    |



Structure
---------

A CBE document is a binary encoded document consisting of a version specifier, followed by a single, top-level object of any type. To store multiple values in a CBE document, use a container as the top-level object and store other objects within that container.

    [version specifier] [object]


### Version Specifier

The version specifier is an unsigned [RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md) with a value greater than 0, representing which version of this specification it adheres to.

The version specifier is mandatory.


### Maximum Depth

Since nested objects (in containers such as maps and lists) are possible, it is necessary to impose an arbitrary depth limit to insure interoperability between implementations. For the purposes of this spec, that limit is 1000 levels of nesting from the top level container to the most nested object (inclusive), unless both sending and receiving parties agree to a different maximum depth.


### Maximum Length

Maximum lengths (max list length, max map length, max array length, max total objects in document, max byte length, etc) are implementation defined, and should be negotiated beforehand. A decoder is free to discard documents that threaten to exceed its resources.



Encoding
--------

A CBE document is byte-oriented. All objects are composed of an 8-bit type field and a possible payload that will always end on an 8-bit boundary.


#### Type Field

| Hex | Dec | Type                      | Payload                                       |
| --- | --- | ------------------------- | --------------------------------------------- |
|  00 |   0 | Integer value 0           |                                               |
|  01 |   1 | Integer value 1           |                                               |
| ... | ... | ...                       |                                               |
|  64 | 100 | Integer value 100         |                                               |
|  65 | 101 | Decimal Float             | [[Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)] |
|  66 | 102 | Positive Integer          | [[RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)] |
|  67 | 103 | Negative Integer          | [[RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)] |
|  68 | 104 | Positive Integer (8 bit)  | [8-bit unsigned integer]                      |
|  69 | 105 | Negative Integer (8 bit)  | [8-bit unsigned integer]                      |
|  6a | 106 | Positive Integer (16 bit) | [16-bit unsigned integer, little endian]      |
|  6b | 107 | Negative Integer (16 bit) | [16-bit unsigned integer, little endian]      |
|  6c | 108 | Positive Integer (32 bit) | [32-bit unsigned integer, little endian]      |
|  6d | 109 | Negative Integer (32 bit) | [32-bit unsigned integer, little endian]      |
|  6e | 110 | Positive Integer (64 bit) | [64-bit unsigned integer, little endian]      |
|  6f | 111 | Negative Integer (64 bit) | [64-bit unsigned integer, little endian]      |
|  70 | 112 | Binary Float (32 bit)     | [32-bit ieee754 binary float, little endian]  |
|  71 | 113 | Binary Float (64 bit)     | [64-bit ieee754 binary float, little endian]  |
|  72 | 114 | RESERVED                  |                                               |
|  73 | 115 | RESERVED                  |                                               |
|  74 | 116 | RESERVED                  |                                               |
|  75 | 117 | RESERVED                  |                                               |
|  76 | 118 | RESERVED                  |                                               |
|  77 | 119 | RESERVED                  |                                               |
|  78 | 120 | List                      |                                               |
|  79 | 121 | Map                       |                                               |
|  7a | 122 | End of Container          |                                               |
|  7b | 123 | Metadata                  | Any object except padding, metadata, comment  |
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
|  97 | 151 | RESERVED                  |                                               |
|  98 | 152 | RESERVED                  |                                               |
|  99 | 153 | Date                      | [[Compact Date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date)] |
|  9a | 154 | Time                      | [[Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time)] |
|  9b | 155 | Timestamp                 | [[Compact Timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp)] |
|  9c | 156 | Integer value -100        |                                               |
| ... | ... | ...                       |                                               |
|  fe | 254 | Integer value -2          |                                               |
|  ff | 255 | Integer value -1          |                                               |



Numeric Types
-------------

### Boolean

True or false.

Examples:

    [7c] = false
    [7d] = true


### Integer

Integers are encoded as positive or negative values, and can be of any width. The fixed width types (16 to 64 bit) are stored in little endian byte order.

Values from -100 to +100 ("small int") are encoded into the type field itself, and can be read directly as 8-bit signed two's complement integers. Values outside of this range are stored as separate types, with the payload containing the absolute value and the field type determining the sign to be applied.

Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [68 7f] = 127
    [68 ff] = 255
    [69 ff] = -255
    [66 bd 84 40] = 1000000
    [6c 80 96 98 00] = 10000000
    [67 9d 8d a5 94 a0 00] = -1000000000000


### Decimal Floating Point

Decimal floating point values are represented using the [Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md) format.

Example:

    [65 07 4b] = -7.5
    [65 82 2c b8 9e 50] = 9.21424e80


### Binary Floating Point

Binary floating point values are stored using 32 or 64-bit ieee754 binary floating point format in little endian byte order. Binary types should only be used to support legacy systems that are unable to handle decimal rounded values, or that rely on specific binary payload contents. Decimal floating point values tend to be smaller, and also avoid the false precision of binary floating point values. [More info](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md#how-much-precision-do-you-need)

Examples:

    [70 00 e2 af 44] = 0x1.5fc4p10
    [71 00 10 b4 3a 99 8f 32 46] = 0x1.28f993ab41p100



Temporal Types
--------------

### Date

Dates are represented using the [compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date) format.

Example:

    [99 56 01 66] = Oct 22, 2051


### Time

Time is represented using the [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time) format.

Example:

    [9a 6e cf ee b1 e8 f8 01 10 45 2f 42 65 72 6c 69 6e] = 13:15:59.529435422/E/Berlin


### Timestamp

Timestamps are represented using the [compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp) format.

Example:

    [9b 40 56 d0 0a 3a 8f 9a f7 28] Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array Types
-----------

An "array" for the purposes of this spec is a contiguous sequence of octets, preceded by a length field. The array type determines how those octets are to be interpreted.


### Array Length Field

All arrays are preceded by an array length field, representing the number of octets in the array.

The array length field is encoded as an [RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md).


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string. Strings must not contain a byte order mark (u+feff) or the NUL (u+0000) character, but can contain any other valid character.

The length field holds the byte length (length in octets), NOT the character length.

    [90] [Length] [Octet 0] ... [Octet (Length-1)]

For byte lengths from 0 to 15, there are special top-level inferred-length string types (0x80 - 0x8f). For longer strings, use the general (0x90) string type.

    [80]
    [81] [octet 0]
    [82] [octet 0] [octet 1]
    ...

Note: Escape sequences within strings are NOT interpteted; they are passed through as-is.

Note: While carriage return (u+000d) is technically allowed in strings, line endings should be converted to linefeed (u+0009) whenever possible to maximize compatibiity between systems.

Examples:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 15 e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺


### URI

Uniform Resource Identifier, which must be valid according to [RFC 3986](https://tools.ietf.org/html/rfc3986).

The length field contains the byte length (length in octets), NOT the character length.

Example:

    [92 55 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top

    [92 1b 6d 61 69 6c 74 6f 3a 4a 6f 68 6e 2e 44 6f 65 40 65 78 61 6d 70
     6c 65 2e 63 6f 6d]
    = mailto:John.Doe@example.com

    [92 33 75 72 6e 3a 6f 61 73 69 73 3a 6e 61 6d 65 73 3a 73 70 65 63 69
     66 69 63 61 74 69 6f 6e 3a 64 6f 63 62 6f 6f 6b 3a 64 74 64 3a 78 6d
     6c 3a 34 2e 31 2e 32]
    = urn:oasis:names:specification:docbook:dtd:xml:4.1.2


### Bytes

An array of octets. This data type should only be used as a last resort if the other data types are unable to represent the data you need. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian byte order whenever possible.

    [91] [Length] [Octet 0] ... [Octet (Length-1)]

Examples:

    [91 05 01 02 03 04 05] = byte array {0x01, 0x02, 0x03, 0x04, 0x05}



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

A list is ordered by default unless otherwise understood between parties (for example via a schema), or the user has specified that order doesn't matter. Other characteristics (such as non-duplicate data in the case of sets) must be a-priori understood between parties.

List elements are simply objects (type field + possible payload). The list is terminated by an "end of container" marker.

Note: While this spec allows mixed types in lists, not all languages do. Use mixed types with caution.

Example:

    [78 01 6a 88 13 7a] = A list containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Keys can be any mix of scalar or array types. A key must not be a container type, the `nil` type, or a NaN (not-a-number) value. Values can be any mix of any type, including other containers.

A map is ordered by default unless otherwise understood between parties (for example via a schema), or the user has specified that order doesn't matter.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash, and are therefore invalid:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (decimal float)

Map contents are stored as key-value pairs of objects:

    [79] [key 1] [value 1] [key 2] [value 2] ... [7a]

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution. A decoder may abort processing or ignore key-value pairs of mixed key types if the implementation language doesn't support it.

Example:

    [79 81 61 01 81 62 02 7a] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Inline Containers

CBE documents in data communication messages are often implemented as lists or maps at the top level. To help save bytes, CBE allows "inline" top-level containers as a special case.

An "inline container" document contains no version specifier, no container initiator, and no container end. It is up to the implementation to specify the CBE version in use, and how the container is delimited.



Metadata
--------

Metadata is data about the data. It describes whatever data follows it in a document, which might or might not necessarily be of interest to a consumer of the data. For this reason, decoders are free to ignore and discard metadata if they so choose. Senders and receivers should negotiate beforehand how to react to metadata.

Metadata must only be placed in front of another object. It cannot be placed at the end of a document (because there would be no object to refer to). A CBE document containing only metadata and no real objects is invalid.


### Metadata Association

Metadata objects are pseudo-objects that can be placed anywhere a real object can be placed, but do not count as objects themselves. Instead, metadata is associated with the object that follows it. For example:

    (map) ("a key") (metadata) ("a value") (end)

In this case, the metadata refers to the value `"a value"`, but the actual data for purposes of decoding the map is `(map) ("a key") ("a value") (end)`.

    (map) ("a key") (metadata) (end)

This map is invalid, because it resolves to `(map) ("a key") (end)`, with no value associated with the key (the metadata doesn't count).

Metadata can also refer to other metadata, for example:

    (map) (metadata-1) (metadata) ("a key") ("a value") (end)

In this case, `(metadata)` refers to the string `"a key"`, and `(metadata-1)` refers to `(metadata)`. The actual map is `(map) ("a key") ("a value") (end)`.

#### Exception: Comments

The metadata association rules do not apply to [comments](#comment). Comments stand entirely on their own, and do not officially refer to anything, nor can any other metadata refer to a comment (i.e. comments are invisible to other metadata).


### Metadata Types

Metadata begins with the metadata type (0x7b), followed by the object (type + possible payload) that will represent the metadata. The type immediately following a metadata type can be any type except for the padding (0x7f), metadata (0x7b), and comment (0x93) types.

Generally, the most useful metadata type will be a map.

Example:

    [7b 79 81 61 01 7a] = metadata map: {a = 1}

#### Name Clashes

There are various metadata standards in use today (https://en.wikipedia.org/wiki/Metadata_standard). Care should be taken to ensure that your chosen metadata system doesn't clash with other established naming schemes.

#### Predefined Keys

The [Concise Encoding Metadata specification](https://github.com/kstenerud/concise-encoding-metadata/blob/master/concise-encoding-metadata.md) contains a list of prefedined metadata keys for use in CTE and CBE. All metadata map keys beginning with `_` are reserved, and must not be used except according to the prefedined metadata keys specification.

Implementations should make use of the predefined keys whenever possible to maximize interoperability between systems.

#### Example

    [7b 79 82 5f 74 85 61 5f 74 61 67 7a] = metadata map: {_t = ["a_tag"]}


### Comment

Comments are user-defined string metadata equivalent to comments in a source code document. Comments do not officially refer to other objects, although conventionally they tend to refer to what follows in the document, be it a single object, a series of objects, a distant object, or they might even be entirely standalone. This is similar to how source code comments are used.

Comments have additional restricions on their content beyond those of normal strings:

#### Comment Character Restrictions

The following characters are explicitly allowed:

 * Horizontal Tab (u+0009)
 * Linefeed (u+000a)

The following characters are disallowed if they aren't in the above allowed section:

 * Control characters (such as u+0000 to u+001f, u+007f to u+009f).
 * Line breaking characters (such as u+2028, u+2029).
 * Byte order mark.

The following characters are allowed if they aren't in the above disallowed section:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

#### Comment Line Endings

As only the linefeed character is allowed for line endings, an encoder must convert native line endings (for example CR+LF) to linefeeds for encoding. A decoder may convert linefeeds to another line ending format if desired.

#### Example

    [93 40 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20 66 61
     69 6c 73 20 74 6f 20 73 74 61 72 74 20 6f 6e 20 61 72 6d 36 34 20 75
     6e 6c 65 73 73 20 42 20 6c 61 74 63 68 20 69 73 20 73 65 74]
    = Bug #95512: System fails to start on arm64 unless B latch is set



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages might have restrictions on how and if it can be used.

Example:

    [7e] = No data


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur any number of times before a type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.


### RESERVED

This type is reserved for future expansion of the format, and must not be used.



Invalid Encodings
-----------------

The structure and format of CBE leaves room for certain encodings that contain problematic data. These are called invalid encodings. A decoder must halt processing when an invalid encoding is encountered.

 * Times must be valid. For example: hour 30, while technically encodable, is invalid.
 * Containers must be properly terminated with `end container` tags. Extra `end container` tags are invalid.
 * All map keys must have corresponding values. A key with a missing value is invalid.
 * Map keys must not be container types, the `nil` type, or values the resolve to NaN (not-a-number).
 * Maps must not contain duplicate keys. This includes numeric keys of different widths or types that resolve to the same value (for example: 16-bit 0x1000 and 32-bit 0x00001000 and 32-bit float 1000.0).
 * An array's length field must match the byte-length of its data. An invalid array length might not be directly detectable, but in such a case will likely lead to other invalid encodings due to array data being interpreted as other types.
 * All UTF-8 sequences must be complete and valid (no partial characters, unpaired surrogates, etc).
 * RESERVED types are invalid, and must not be used.
 * Metadata map keys beginning with `_` must not be used, except for those listed in this specifiction.



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders are encouraged to find the smallest type and width that stores a numeric value without data loss.

For specialized applications, an encoder implementation may choose to preserve larger types and widths as a tradeoff in processing cost vs data size.



Alignment
---------

Applications might require data to be aligned in some cases for optimal decoding performance. For example, some processors might not be able to read unaligned multibyte data types without special (costly) intervention. An encoder could in theory be tuned to insert `padding` bytes when encoding certain data:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 7f | 7f | 7f | 67 | 00 | 00 | 00 | 8f |

Alignment is usually only useful when the target decoder is known prior to encoding. It is mostly an optimization for closed systems.



Version History
---------------

June 5, 2018: First draft



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
