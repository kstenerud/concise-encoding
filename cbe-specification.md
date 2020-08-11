Concise Binary Encoding
=======================

Concise Binary Encoding (CBE) is a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data. It aims to support 80% of data use cases in a human friendly way:

 * There are two formats (binary-based CBE and [text-based CTE](cte-specification.md)), which are 1:1 seamlessly compatible. Use the more efficient binary format for data interchange and storage, and transparently convert to/from text only when a human needs to be involved.
 * Supports metadata and comments.
 * Supports cyclic and recursive data.
 * Supports the most commonly used data types:

| Type        | Description                                          |
| ----------- | ---------------------------------------------------- |
| Nil         | No data                                              |
| Boolean     | True or false                                        |
| Integer     | Positive or negative integers                        |
| Float       | Decimal or binary floating point                     |
| UUID        | [RFC-4122 UUID](https://tools.ietf.org/html/rfc4122) |
| Time        | Date, time, or timestamp                             |
| String      | UTF-8 string of arbitrary size                       |
| URI         | [RFC-3986 URI](https://tools.ietf.org/html/rfc3986)  |
| Typed Array | Array of type with fixed width                       |
| List        | List of objects                                      |
| Map         | Maps keyable objects to other objects                |
| Markup      | Data structure similar to XML                        |
| Reference   | References a previously defined object               |
| Metadata    | Data about other data                                |
| Comment     | User definable comment                               |
| Custom      | User-defined data type                               |

CBE is the binary-based counterpart to [Concise Text Encoding](cte-specification.md).



Version
-------

Version 1 (prerelease)



Contents
--------

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
  - [UUID](#uuid)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
* [Array Types](#array-types)
  - [Chunking](#chunking)
    - [Chunk Header](#chunk-header)
    - [Zero Chunk](#zero-chunk)
  - [String](#string)
  - [Verbatim String](#verbatim-string)
  - [URI](#uri)
  - [Custom Types](#custom-types)
    - [Binary Encoding](#custom-type-binary-encoding)
    - [Text Encoding](#custom-type-text-encoding)
  - [Typed Array](#typed-array)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
    - [Tag Name](#markup-tag-name)
    - [Attributes Section](#attributes-section)
    - [Contents Section](#contents-section)
      - [Entity Reference](#entity-reference)
    - [Doctype](#doctype)
    - [Style Sheet](#style-sheet)
    - [Markup Comment](#markup-comment)
* [Peudo-Objects](#peudo-objects)
  - [Marker](#marker)
    - [Marker ID](#marker-id)
  - [Reference](#reference)
  - [Metadata Map](#metadata-map)
  - [Comment](#comment)
    - [Comment Character Restrictions](#comment-string-character-restrictions)
  - [Padding](#padding)
* [Other Types](#other-types)
  - [Nil](#nil)
  - [RESERVED](#reserved)
* [Implied Structure](#implied-structure)
  - [Implied Version](#implied-version)
  - [Inline Containers](#inline-containers)
* [Invalid Encodings](#invalid-encodings)
* [Smallest Possible Size](#smallest-possible-size)
* [Alignment](#alignment)
* [Version History](#version-history)
* [License](#license)



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

A CBE document is a binary encoded document containing data arranged in an ad-hoc hierarchical fashion.

The document begins with a [version specifier](#version-specifier), followed by zero or one objects of any type. To store multiple values in a document, use a container as the top-level object and store other objects within that container.

    [version specifier] [object]


### Version Specifier

The version specifier is an unsigned [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128) with a value greater than 0, representing which version of this specification it adheres to.

The version specifier is mandatory, unless all parties have agreed to omit the specifier and use a specific version.

Note: Because CBE places the version as the first byte in a document, versions from 32 to 126 are disallowed in order to prevent clashes with any ASCII characters that another text encoding format might use.


### Maximum Depth

Since nested objects (in containers such as maps and lists) are possible, it is necessary to impose an arbitrary depth limit to ensure interoperability between implementations. For the purposes of this spec, that limit is 1000 levels of nesting from the top level container to the most nested object (inclusive), unless both sending and receiving parties agree to a different maximum depth.


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
|  66 | 102 | Positive Integer          | [byte length] [little endian bytes]           |
|  67 | 103 | Negative Integer          | [byte length] [little endian bytes]           |
|  68 | 104 | Positive Integer (8 bit)  | [8-bit unsigned integer]                      |
|  69 | 105 | Negative Integer (8 bit)  | [8-bit unsigned integer]                      |
|  6a | 106 | Positive Integer (16 bit) | [16-bit unsigned integer, little endian]      |
|  6b | 107 | Negative Integer (16 bit) | [16-bit unsigned integer, little endian]      |
|  6c | 108 | Positive Integer (32 bit) | [32-bit unsigned integer, little endian]      |
|  6d | 109 | Negative Integer (32 bit) | [32-bit unsigned integer, little endian]      |
|  6e | 110 | Positive Integer (64 bit) | [64-bit unsigned integer, little endian]      |
|  6f | 111 | Negative Integer (64 bit) | [64-bit unsigned integer, little endian]      |
|  70 | 112 | Binary Float (16 bit)     | [16-bit [bfloat16](https://software.intel.com/sites/default/files/managed/40/8b/bf16-hardware-numerics-definition-white-paper.pdf), little endian] |
|  71 | 113 | Binary Float (32 bit)     | [32-bit ieee754 binary float, little endian]  |
|  72 | 114 | Binary Float (64 bit)     | [64-bit ieee754 binary float, little endian]  |
|  73 | 115 | UUID                      | [128 bits of data, big endian]                |
|  74 | 116 | RESERVED                  |                                               |
|  75 | 117 | RESERVED                  |                                               |
|  76 | 118 | Comment                   | (String or sub-comment) ... End of Container  |
|  77 | 119 | Metadata Map              | (Key, value) ... End of Container             |
|  78 | 120 | Markup                    | Name, kv-pairs, contents                      |
|  79 | 121 | Map                       | (Key, value) ... End of Container             |
|  7a | 122 | List                      | Object ... End of Container                   |
|  7b | 123 | End of Container          |                                               |
|  7c | 124 | Boolean False             |                                               |
|  7d | 125 | Boolean True              |                                               |
|  7e | 126 | Nil (no data)             |                                               |
|  7f | 127 | Padding                   |                                               |
|  80 | 128 | String: 0 bytes           |                                               |
|  81 | 129 | String: 1 byte            | [1 octet of UTF-8 data]                       |
|  82 | 130 | String: 2 bytes           | [2 octets of UTF-8 data]                      |
|  83 | 131 | String: 3 bytes           | [3 octets of UTF-8 data]                      |
|  84 | 132 | String: 4 bytes           | [4 octets of UTF-8 data]                      |
|  85 | 133 | String: 5 bytes           | [5 octets of UTF-8 data]                      |
|  86 | 134 | String: 6 bytes           | [6 octets of UTF-8 data]                      |
|  87 | 135 | String: 7 bytes           | [7 octets of UTF-8 data]                      |
|  88 | 136 | String: 8 bytes           | [8 octets of UTF-8 data]                      |
|  89 | 137 | String: 9 bytes           | [9 octets of UTF-8 data]                      |
|  8b | 139 | String: 11 bytes          | [11 octets of UTF-8 data]                     |
|  8a | 138 | String: 10 bytes          | [10 octets of UTF-8 data]                     |
|  8c | 140 | String: 12 bytes          | [12 octets of UTF-8 data]                     |
|  8d | 141 | String: 13 bytes          | [13 octets of UTF-8 data]                     |
|  8e | 142 | String: 14 bytes          | [14 octets of UTF-8 data]                     |
|  8f | 143 | String: 15 bytes          | [15 octets of UTF-8 data]                     |
|  90 | 144 | String                    | [byte length] [UTF-8 data]                    |
|  91 | 145 | Verbatim String           | [byte length] [UTF-8 data]                    |
|  92 | 146 | URI                       | [byte length] [[URI](https://tools.ietf.org/html/rfc3986)] |
|  93 | 147 | Custom (Binary)           | [byte length] [binary data]                   |
|  94 | 148 | Custom (Text)             | [byte length] [UTF-8 data]                    |
|  95 | 149 | Typed Array               | [type] [unit length] [units]                  |
|  96 | 150 | RESERVED                  |                                               |
|  97 | 151 | Marker                    | Positive integer / string                     |
|  98 | 152 | Reference                 | Positive integer / string / URI               |
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

Integers can be positive or negative, and are encoded in three possible ways:

#### Small Int

Values from -100 to +100 ("small int") are encoded into the type field itself, and can be read directly as 8-bit signed two's complement integers.

#### Fixed Width

Fixed width integers are stored unsigned in widths of 8, 16, 32, and 64 bits (stored in little endian byte order). The type field determines the sign.

#### Variable width

Variable width integers are encoded as blocks of little endian ordered bytes with a length header. The header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128), denoting how many bytes of integer data follows. The type field determines the sign.

    [length] [byte 1 (low)] [byte 2] ... [byte x (high)]

#### Examples:

    [60] = 96
    [00] = 0
    [ca] = -54
    [68 7f] = 127
    [68 ff] = 255
    [69 ff] = -255
    [6c 80 96 98 00] = 10000000
    [67 0f ff ee dd cc bb aa 99 88 77 66 55 44 33 22 11] = -0x112233445566778899aabbccddeeff


### Decimal Floating Point

Decimal floating point values are stored in [Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md) format.

Decimal floating point values tend to be smaller, are more precise, and avoid the false precision of binary floating point values. [More info](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md#how-much-precision-do-you-need)

Example:

    [65 07 4b] = -7.5
    [65 ac 02 d0 9e 38] = 9.21424e80


### Binary Floating Point

Binary floating point values are stored in 32 or 64-bit ieee754 binary floating point format, or in 16-bit [bfloat](https://software.intel.com/sites/default/files/managed/40/8b/bf16-hardware-numerics-definition-white-paper.pdf) format, in little endian byte order.

Examples:

    [70 af 44] = 0x1.5e4p+10
    [71 00 e2 af 44] = 0x1.5fc4p+10
    [72 00 10 b4 3a 99 8f 32 46] = 0x1.28f993ab41p+100


### UUID

A [universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier), stored according to [rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2) (i.e. big endian).

Example:

    [73 12 3e 45 67 e8 9b 12 d3 a4 56 42 66 55 44 00 00] = UUID 123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

### Date

Dates are stored in [compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date) format.

Example:

    [99 56 cd 00] = Oct 22, 2051


### Time

Time values are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time) format.

Example:

    [9a f7 58 74 fc f6 a7 01 10 45 2f 42 65 72 6c 69 6e] = 13:15:59.529435422/E/Berlin


### Timestamp

Timestamps are stored in [compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp) format.

Example:

    [9b 81 ac a0 b5 03 8f 1a ef d1] = Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array Types
-----------

An array for the purposes of this spec is a contiguous sequence of identically sized elements, stored in length delimited chunks. The array type determines how the data is to be interpreted.


### Chunking

Array data is "chunked", meaning that it is represented as a series of chunks of data, each with its own length field:

    [length-1] [chunk-1] [length-2] [chunk-2] ...

There is no limit to the number of chunks in an array, nor do the chunks have to be the same size. The most common case would be to represent the array as a single chunk, but there might be cases where you need multiple chunks, such as when the array length is not known from the start (if it's being built progressively).

#### Chunk Header

All array chunks are preceded by a header containing the chunk length and a continuation bit. The header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128). Chunk processing continues until the end of a chunk with a continuation bit of 0.

| Field        | Bits | Description                          |
| ------------ | ---- | ------------------------------------ |
| Length       |   *  | Chunk length (in octets)             |
| Continuation |   1  | If 1, another chunk follows this one |

#### Zero Chunk

The chunk header 0x00 indicates a chunk of length 0 with continuation 0, effectively terminating any array. It's no coincidence that 0x00 also acts as the null terminator for strings in C. An encoder may use this feature to artificially null-terminate strings in order to create immutable-friendly zero-copy documents that support c-style string implementations.

    [90 20 m i s u n d e r s t a n d i n g ...]

vs

    [90 21 m i s u n d e r s t a n d i n g 00 ...]

Note that this technique will only work for the general string type (0x90), not for the short string types 0x80 - 0x8f (which have no chunk headers).

If the source buffer in your decoder is mutable, you could achieve c-style zero-copy without requiring the above technique, using a scheme whereby you pre-cache the type code of the next value, overwrite that type code in the buffer with 0 (effectively "terminating" the string), and then process the next value using the pre-cached type:

    ...                          // buffer = [84 t e s t 6a 10 a0 ...]
    case string (length 4):      // 0x84 = string (length 4)
      cachedType = buffer[5]     // 0x6a (16-bit positive int type)
      buffer[5] = 0              // buffer = [84 t e s t 00 10 a0 ...]
      notifyString(buffer+1)     // [t e s t 00 ...] = null-terminated string "test"
      next(cachedType, buffer+6) // 0x6a, [10 a0 ...] = 16-bit positive int value 40976

#### Chunking Example

    [1d] (14 bytes of data) [08] (4 bytes of data)

In this case, the first chunk is 14 bytes long and has a continuation bit of 1. The second chunk has 4 bytes of data and a continuation bit of 0. The total length of the array is 18 bytes, split across two chunks.


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string. Strings must not contain a byte order mark.

Support for the NUL character (u+0000) is implementation defined due to potential issues with null string delimiters in some languages and platforms. It should be avoided in general for safety and portability. Support for NUL must not be assumed.

The length field holds the length in octets, NOT the character length.

    [90] [Length+continuation] [Octet 0] ... [Octet (Length-1)]

For byte lengths from 0 to 15, there are special top-level inferred-length string types (0x80 - 0x8f) that contain only a single array chunk. For longer strings, use the general (0x90) string type.

    [80]
    [81] [octet 0]
    [82] [octet 0] [octet 1]
    ...

Note: CBE has no concept of escape sequences; all strings must be passed through as-is.

Note: While carriage return (u+000d) is technically allowed in strings, line endings should be converted to linefeed (u+0009) whenever possible to maximize compatibiity between systems and minimize data costs.

Examples:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 2a e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺


### Verbatim String

A verbatim string is a string that must be taken literally (no interpretation, no escape processing, etc) by encoders and decoders, and should be taken literally by all layers of the stack. Decoders must pass along a string's status as "verbatim" or not. How the higher layers handle such information is implementation dependent.

The length field holds the length in octets, NOT the character length.

Example:

    [91 10 5c 6e 5c 6e 5c 6e 5c 6e] = literal "\n\n\n\n" (not to be interpreted as linefeeds)


### URI

Uniform Resource Identifier, which must be valid according to [RFC 3986](https://tools.ietf.org/html/rfc3986).

The length field contains the byte length (length in octets), NOT the character length.

Example:

    [92 aa 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top

    [92 36 6d 61 69 6c 74 6f 3a 4a 6f 68 6e 2e 44 6f 65 40 65 78 61 6d 70
     6c 65 2e 63 6f 6d]
    = mailto:John.Doe@example.com

    [92 66 75 72 6e 3a 6f 61 73 69 73 3a 6e 61 6d 65 73 3a 73 70 65 63 69
     66 69 63 61 74 69 6f 6e 3a 64 6f 63 62 6f 6f 6b 3a 64 74 64 3a 78 6d
     6c 3a 34 2e 31 2e 32]
    = urn:oasis:names:specification:docbook:dtd:xml:4.1.2


### Custom Types

There are many cases where a custom data type is preferable to the standard types. The data might not otherwise be representable, or it might be too bulky using standard types, or you might want the data to be mapped directly to/from memory for performance reasons.

Although not a requirement, it's generally expected that a custom type implementation would provide both a binary and text encoding, with the binary encoding preferred for CBE documents, and the text encoding preferred for CTE documents. Note, however, that both encodings can handle both types. The only difference would be human readability in CTE documents, and codec efficiency in CBE documents.

It's important to avoid parsing ambiguity when designing your custom type encodings. The simplest approach for binary data is to prepend a type field. For text data, "function-style" encoding (`t"mytype(1, 2.0, 'three')"`) is usually sufficient.

#### Custom Type (Binary Encoding)

An array of octets representing a user-defined custom data type. The encoding and interpretation of the octets is implementation defined, and must be understood by both sending and receiving parties. To reduce cross-platform confusion, multibyte data types should be represented in little endian byte order whenever possible.

    [93 12 04 f6 28 3c 40 00 00 40 40]
    = example "cplx" struct{ type uint8(4), real float32(2.94), imag float32(3.0) }


#### Custom Type (Text Encoding)

A string value representing a user-defined custom data type. The encoding and interpretation of the string is implementation defined, and must be understood by both sending and receiving parties.

The custom text data must be a valid [UTF-8 string](#string), and must not contain control characters or non-printable characters.

The length field holds the length in octets, NOT the character length.

    [94 1a 63 70 6c 78 28 32 2e 39 34 2b 33 69 29] = custom data encoded as the string "cplx(2.94+3i)"


### Typed Array

A typed array encodes an array of values of a fixed type and size. The advantage of arrays is that the values are all adjacent to each other in the stream, so that large amounts of data can be easily copied from internal structures in your program, and read from the stream using zero-copy semantics.

Fixed width types boolean, signed/unsigned integer (8-64 bit), binary float (16-64 bit), and UUID can be stored in typed arrays. For other types, use a [list](#list).

A typed array is structured as follows:

| Field        | Description                            |
| ------------ | -------------------------------------- |
| Type         | The type code 0x95 (typed array)       |
| Element Type | The type of the elements in this array |
| Chunk Header | The number of elements following       |
| Elements     | The elements themselves                |
| ...          | Possibly more chunks                   |

The length in each chunk header represents the number of octets in the chunk, not the number of elements. The total length of the array (after all chunks are added) must be a multiple of the array element width.

Element byte ordering is according to the element type (big endian for UUID, little endian for everything else).

For integer types, the "sign" of the type determines whether the elements are signed or unsigned. "Negative" types are interpreted as signed (two's complement), and "positive" types are interpreted as unsigned.

Examples:

    [95 68 0a 01 02 03 04 05] = byte (unsigned 8-bit) array {0x01, 0x02, 0x03, 0x04, 0x05}



Container Types
---------------

### List

A list of objects. Lists can contain any mix of any type, including other containers.

By default, a list is ordered and can contain duplicate values unless otherwise negotiated between all parties (for example via a schema).

List elements are simply objects (type field + possible payload). The list is terminated by an "end of container" marker.

Note: While this spec allows mixed types in lists, not all languages do. Use mixed types with caution. A decoder may abort processing or ignore values of mixed types if the implementation language doesn't support it.

Example:

    [7a 01 6a 88 13 7b] = A list containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Map keys can be any mix of [keyable types](#keyable-types). Values can be any mix of any type, including other containers.

A map is ordered by default unless otherwise negotiated between parties (for example via a schema), and must not contain duplicate keys (including values across numeric types). For example, the following keys would clash:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (decimal float)

Note: The string value "2000" is not numeric, and would not clash.

Map contents are stored as key-value pairs of objects. A key without a paired value is invalid:

    [79] [key 1] [value 1] [key 2] [value 2] ... [7b]

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution. A decoder may abort processing or ignore key-value pairs of mixed types if the implementation language doesn't support it.

#### Keyable types

Only certain types can be used as keys in map-like containers:

* [Numeric types](#numeric-types) except for NaN (not-a-number)
* [Temporal types](#temporal-types)
* [Array types](#array-types)

Nil must not be used as a key, and [references](#reference) are not allowed as keys.

#### Example:

    [79 81 61 01 81 62 02 7b] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Markup

A markup container stores XML-style data, which is essentially a name, a map of attributes, and a list of contents:

    [name] [attributes] [contents]

Markup containers are best suited for presentation data. For regular data, maps and lists are better.

Unlike other containers, a markup container requires two end-of-container markers: one to mark the end of the attributes, and another one to mark the end of the contents section:

    [78] [name] [attr1] [v1] [attr2] [v2] ... [7b] [contents1] [contents2] ... [7b]

#### Markup Tag Name

Although technically any [keyable type](#keyable-types) is allowed in this field, be aware that conversion to XML and HTML might be problematic with some types.

#### Attributes Section

The attributes section behaves like a [map](#map), but be aware that conversion to XML and HTML might be problematic with some types.

#### Contents Section

The contents section behaves similarly to a [list](#list), except that it can only contain:

 * [Content strings](#string)
 * [Verbatim Strings](#verbatim-string)
 * [Comments](#comment)
 * Other markup containers

Whitespace in a markup content string is handled the same as in [XML](https://www.w3.org/TR/REC-xml/#sec-white-space). Any extraneous whitespace should be elided.

Note: Whitespace in [verbatim strings](#verbatim-string) must be delivered as-is (no eliding).

##### Entity Reference

The Concise Encoding formats don't concern themselves with [entity references](https://en.wikipedia.org/wiki/SGML_entity), passing them transparently for higher level layers to use if so desired.

Note: Text sequences that look like entity references (or any other interpretable sequence) in [verbatim strings](#verbatim-string) must NOT be interpreted by any layer in the stack.

#### Doctype

Use a [metadata map](#metadata-map) entry to specify a doctype:

    [77 8c] "xml-doctype" [7a 84] "html" [86] "PUBLIC" [90 b8] "-//W3C//DTD XHTML 1.0 Strict//EN" [93 63] "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" [7b 7b]

#### Style Sheet

Use a [metadata map](#metadata-map) entry to specify an XML style sheet:

    [77 8f] "xml-stylesheet" [79 84] "type" [88] "text/xsl" [84] "href" [8e] "stylesheet.xsl" [7b 7b]



Peudo-Objects
-------------

Pseudo-objects add additional metadata to a real object, or to the document, or affect the interpreted structure of the document in some way. Pseudo-objects can be placed anywhere a real object can be placed, but do not themselves constitute objects (except for [references](#reference)). For example, `(begin-map) ("a key") (pseudo-object) (end-container)` is not valid, because the pseudo-object isn't a real object, and therefore doesn't count as an actual map value for key "a key".

#### Referring Pseudo-objects

_Referring_ pseudo-objects refer to the next object following at the current container level. This will either be a real object, or a visible pseudo-object

#### Invisible Pseudo-objects

_Invsible_ pseudo-objects are effectively invisible to referring pseudo-objects, and are skipped over when searching for the object that is being referred to.


### Marker

A marker is a _referring_, _invisible_ pseudo-object that tags the next object with a [marker ID](#marker-id), such that it can be referenced from another part of the document (or from a different document).

A marker begins with the marker type (0x97), followed by a [marker ID](#marker-id) and then the marked object (except when intervening "invisible" pseudo-objects are present).

    [97] [ID] [marked object]

Rules:

 * A marker cannot mark an object in a different container level. For example: `(begin-list) (marker+ID) (end-list) (string)` is invalid.
 * Marker IDs must be unique within the document; duplicate marker IDs are invalid.

Example:

    [97 01 79 8a 73 6f 6d 65 5f 76 61 6c 75 65 90 22 72
     65 70 65 61 74 20 74 68 69 73 20 76 61 6c 75 65 7b]
    = the map {some_value = "repeat this value"}, tagged with ID integer 1

#### Marker ID

A marker ID is a unique (to the document) identifier for marked objects. A marker ID can either be a positive integer (up to 18446744073709551615, 64 bits), or a string of case-insensitive basic alphanumerics plus underscore (`[0-9A-Za-z_]`) with a minimum length of 1 and a maximum length of 30 (Note: ID strings cannot begin with a numeric digit). Integer marker IDs will generally use less space in the binary format than multibyte strings.

**Note:** Marker ID comparisons are always case-insensitive.

An integer ID:

 * Must be positive
 * Must not be larger than 64 bits
 * Must be represented as an integer type (not as a whole-number float)


### Reference

A reference is a _non-referring_, _visible_ pseudo-object that acts as a stand-in for an object that has been [marked](#marker) elsewhere in this or another document. This could be useful for repeating or cyclic data. Unlike other pseudo-objects, references can be used just like regular objects (for example, `(begin-map) ("a key") (reference) (end-container)` is valid).

A reference begins with the reference type (0x98), followed by either a [marker ID](#marker-id) or a [URI](#uri).

Rules:

 * A reference with a [local marker ID](#marker-id) must refer to another object marked elsewhere in the same document (local reference).
 * A reference must not be used as a map key.
 * Forward references within a document are allowed.
 * Recursive references are allowed.
 * A reference with a URI must point to:
   - Another CBE or CTE document (using no fragment section, thus referring to the entire document)
   - A marker ID inside another CBE or CTE document, using the fragment section of the URI as an ID
 * An implementation may choose to follow or not to follow URI references. Care must be taken when following links, as there will be security implications.
 * An implementation may choose to simply pass along a URI as-is, leaving it up to the user to resolve it or not.
 * References to dead or invalid URI links are not considered invalid per se. How this situation is handled is implementation specific, and must be fully specified in the implementation of your use case.

Examples:

    [98 01] = reference to the object marked with ID 1

    [98 81 61] = reference to the object marked with ID "a"

    [98 93 24 63 6f 6d 6d 6f 6e 2e 63 65 23 6c 65 67 61 6c 65 73 65]
    = reference to relative file "common.ce", ID "legalese"

    [98 93 62 68 74 74 70 73 3a 2f 2f 73 6f 6d 65 77
     68 65 72 65 2e 63 6f 6d 2f 6d 79 5f 64 6f 63 75
     6d 65 6e 74 2e 63 62 65 3f 66 6f 72 6d 61 74 3d
     6c 6f 6e 67]
    = reference to entire document at https://somewhere.com/my_document.cbe?format=long


### Metadata Map

A metadata map is a _referring_, _visible_ pseudo-object containing keyed values which are to be associated with the object following the metadata map.

Metadata is data about the data, which might or might not be of interest to a consumer of the data. An implementation may choose to pass on or ignore metadata maps depending on the use case.

Note: Metadata can refer to other metadata (meta-metadata).

#### Metadata Keys

Keys in metadata maps follow the same rules as for regular maps, except that all string typed keys beginning with the underscore `_` character are reserved for predefined keys, and must only be used in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).

Implementations should make use of the predefined metadata keys where possible to maximize interoperability between systems.

#### Metadata Example

    [77 82 5f 74 7a 85 61 5f 74 61 67 7b 7b] = metadata map: (_t = ["a_tag"])


### Comment

A comment is a _non-referring_, _invisible_, list-style pseudo-object that can only contain strings or other comment containers (to support nested comments).

Although comments are not _referring_ pseudo-objects, they tend to unofficially refer to what follows in the document, similar to how comments are used in source code.

Comments operate similarly to C-like language comments, except that nested comments are allowed.

#### Comment String Character Restrictions

The following characters are explicitly allowed:

 * Horizontal Tab (u+0009)
 * Carriage Return (u+000d)
 * Linefeed (u+000a)

The following characters are disallowed if they aren't in the above allowed section:

 * Control characters (such as u+0000 to u+001f, u+007f to u+009f).
 * Line breaking characters (such as u+2028, u+2029).
 * Byte order mark.

The following characters are allowed if they aren't in the above disallowed section:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

#### Example

    [76 90 80 01 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20
     66 61 69 6c 73 20 74 6f 20 73 74 61 72 74 20 6f 6e 20 61 72 6d 36 34
     20 75 6e 6c 65 73 73 20 42 20 6c 61 74 63 68 20 69 73 20 73 65 74 7b]
    = Bug #95512: System fails to start on arm64 unless B latch is set


### Padding

Padding is _non-referring_ and _invisible_. The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur any number of times before a type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.

Technically, padding could also be used as a sequence point in a CBE-style stream to help synchronize data on a noisy channel (for example, data chunk, 4x padding, data chunk, etc), but such schemes are outside of the scope of this document.

Padding is the only CBE type that has no matching CTE type.


Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Nil is a [contentious value in computer science](https://en.wikipedia.org/wiki/Null_pointer), and so the handling of the nil value is implementation defined. A decoder must consume a nil value, but how (and if) it stores the value is up to the implementation. It's advised to avoid using nil if at all possible.

Example:

    [7e] = No data


### RESERVED

This type is reserved for future expansion of the format, and must not be used.



Implied Structure
-----------------

In some cases it could be desirable to embed a CBE document into another structure, in which case the version and top-level object might always be the same. The implied structure forms allow an implementation to omit certain parts of a document when all parties are made aware of it (such as via a schema or specification).

For general purpose data transmission, it's better to use the full document structure. The implied structure forms are just a way to shave off a few redundant bytes in a tightly defined, specialized system.


### Implied Version

An "implied version" document omits the [version specifier](#version-specifier), which must be specified elsewhere.


### Inline Containers

An "inline container" document is an [implied version](#implied-version) document that also implies an already opened [list](#list) or [map](#map), omitting the begin and possibly also the end container fields. Processing of the document begins inside the implied container. What type of container it is, and and how the container (and thus the document) is terminated, must be specified elsewhere.



Invalid Encodings
-----------------

The structure and format of CBE leaves room for certain encodings that contain problematic data. These are called invalid encodings. A decoder must halt processing when an invalid encoding is encountered.

 * Times must be valid. For example: hour 30, while technically encodable, is invalid.
 * Containers must be properly terminated with `end container` tags. Extra `end container` tags are invalid.
 * All map keys must have corresponding values. A key with a missing value is invalid.
 * Map keys must not be container types, the `nil` type, or values the resolve to NaN (not-a-number).
 * Maps must not contain duplicate keys. This includes numeric keys of different widths or types that resolve to the same value (for example: 16-bit int 1000, 32-bit int 1000, float 1000.0).
 * An array's chunk length field must match the byte-length of its data. An invalid chunk length might not be directly detectable, but in such a case will likely lead to other invalid encodings due to array data being interpreted as other types.
 * All UTF-8 sequences must be complete and valid (no partial characters, unpaired surrogates, etc).
 * RESERVED types are invalid, and must not be used.
 * Metadata map keys beginning with `_` must not be used, except in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders should use the smallest encoding that stores a value without data loss.

Specialized applications (for example marshalers) may wish to preserve more numeric type information to distinguish floats from integers, or even to distinguish between data sizes. This is allowed, as it will make no difference to a generic decoder.



Alignment
---------

Applications might require data to be aligned in some cases for optimal decoding performance. For example, some processors might not be able to read unaligned multibyte data types without special (costly) intervention. An encoder could in theory be tuned to insert `padding` bytes when encoding certain data:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 7f | 7f | 7f | 67 | 00 | 00 | 00 | 8f |

Alignment tuning is usually only useful when the target decoding environment is known prior to encoding. It's mostly an optimization for closed systems.



Version History
---------------

June 5, 2018: First draft



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
