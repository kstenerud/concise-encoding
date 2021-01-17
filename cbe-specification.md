Concise Binary Encoding
=======================

Concise Binary Encoding (CBE) is the binary variant of Concise Encoding: a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

The binary format aims for compactness and machine processing efficiency while maintaining 1:1 compatibility with the [text format](cte-specification.md) (which aims to present data in a human friendly way).

CBE documents must follow the [Concise Encoding structural rules](ce-structure.md). Many terms used in this document are defined there.

**Note**: CBE examples will be represented as series of hex encoded byte values enclosed within square brackets. For example, `[00 01 fe ff]` represents the four byte sequence 0x00, 0x01, 0xfe, 0xff.



Version
-------

Version 1 (prerelease)



Contents
--------

* [Terms](#terms)
* [Version Specifier](#version-specifier)
* [Encoding](#encoding)
  - [Type Field](#type-field)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Decimal Floating Point](#decimal-floating-point)
  - [Binary Floating Point](#binary-floating-point)
  - [UID](#uid)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
* [Array Types](#array-types)
  - [Chunking](#chunking)
    - [Chunk Header](#chunk-header)
    - [Zero Chunk](#zero-chunk)
  - [String](#string)
  - [Resource Identifier](#resource-identifier)
  - [Custom Types](#custom-types)
    - [Binary Encoding](#custom-type-binary-encoding)
    - [Text Encoding](#custom-type-text-encoding)
  - [Typed Array](#typed-array)
  - [Boolean Array](#boolean-array)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
  - [Relationship](#relationship)
* [Peudo-Objects](#peudo-objects)
  - [Marker](#marker)
  - [Reference](#reference)
  - [Metadata Map](#metadata-map)
  - [Comment](#comment)
  - [Padding](#padding)
* [Other Types](#other-types)
  - [NA](#na)
  - [Concatenation](#concatenation)
  - [RESERVED](#reserved)
* [Empty Document](#empty-document)
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



Version Specifier
-----------------

A CBE document begins with a version specifier, which is composed of the octet `0x03`, followed by a version number.

The version number is an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128), representing which version of this specification the document adheres to.

**Example**:

    [03 01] = CBE version 1



Encoding
--------

A CBE document is byte-oriented. All objects are composed of an 8-bit type field and a possible payload that will always end on an 8-bit boundary.


#### Type Field

| Hex | Dec | Type                      | Payload                                         |
| --- | --- | ------------------------- | ----------------------------------------------- |
|  00 |   0 | Integer value 0           |                                                 |
|  01 |   1 | Integer value 1           |                                                 |
| ... | ... | ...                       |                                                 |
|  64 | 100 | Integer value 100         |                                                 |
|  65 | 101 | Decimal Float             | [[Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)] |
|  66 | 102 | Positive Integer          | [byte length] [little endian bytes]             |
|  67 | 103 | Negative Integer          | [byte length] [little endian bytes]             |
|  68 | 104 | Positive Integer (8 bit)  | [8-bit unsigned integer]                        |
|  69 | 105 | Negative Integer (8 bit)  | [8-bit unsigned integer]                        |
|  6a | 106 | Positive Integer (16 bit) | [16-bit unsigned integer, little endian]        |
|  6b | 107 | Negative Integer (16 bit) | [16-bit unsigned integer, little endian]        |
|  6c | 108 | Positive Integer (32 bit) | [32-bit unsigned integer, little endian]        |
|  6d | 109 | Negative Integer (32 bit) | [32-bit unsigned integer, little endian]        |
|  6e | 110 | Positive Integer (64 bit) | [64-bit unsigned integer, little endian]        |
|  6f | 111 | Negative Integer (64 bit) | [64-bit unsigned integer, little endian]        |
|  70 | 112 | Binary Float (16 bit)     | [16-bit [bfloat16](https://software.intel.com/sites/default/files/managed/40/8b/bf16-hardware-numerics-definition-white-paper.pdf), little endian] |
|  71 | 113 | Binary Float (32 bit)     | [32-bit ieee754 binary float, little endian]    |
|  72 | 114 | Binary Float (64 bit)     | [64-bit ieee754 binary float, little endian]    |
|  73 | 115 | UID                       | [128 bits of data, big endian]                  |
|  74 | 116 | RESERVED                  |                                                 |
|  75 | 117 | Relationship              | Subject, Predicate, Object                      |
|  76 | 118 | Comment                   | (String or sub-comment) ... End of Container    |
|  77 | 119 | Metadata Map              | (Key, value) ... End of Container               |
|  78 | 120 | Markup                    | Name, kv-pairs, contents                        |
|  79 | 121 | Map                       | (Key, value) ... End of Container               |
|  7a | 122 | List                      | Object ... End of Container                     |
|  7b | 123 | End of Container          |                                                 |
|  7c | 124 | Boolean False             |                                                 |
|  7d | 125 | Boolean True              |                                                 |
|  7e | 126 | NA (data missing)         | Reason why the data is missing                  |
|  7f | 127 | Padding                   |                                                 |
|  80 | 128 | String: 0 bytes           |                                                 |
|  81 | 129 | String: 1 byte            | [1 octet of UTF-8 data]                         |
|  82 | 130 | String: 2 bytes           | [2 octets of UTF-8 data]                        |
|  83 | 131 | String: 3 bytes           | [3 octets of UTF-8 data]                        |
|  84 | 132 | String: 4 bytes           | [4 octets of UTF-8 data]                        |
|  85 | 133 | String: 5 bytes           | [5 octets of UTF-8 data]                        |
|  86 | 134 | String: 6 bytes           | [6 octets of UTF-8 data]                        |
|  87 | 135 | String: 7 bytes           | [7 octets of UTF-8 data]                        |
|  88 | 136 | String: 8 bytes           | [8 octets of UTF-8 data]                        |
|  89 | 137 | String: 9 bytes           | [9 octets of UTF-8 data]                        |
|  8b | 139 | String: 11 bytes          | [11 octets of UTF-8 data]                       |
|  8a | 138 | String: 10 bytes          | [10 octets of UTF-8 data]                       |
|  8c | 140 | String: 12 bytes          | [12 octets of UTF-8 data]                       |
|  8d | 141 | String: 13 bytes          | [13 octets of UTF-8 data]                       |
|  8e | 142 | String: 14 bytes          | [14 octets of UTF-8 data]                       |
|  8f | 143 | String: 15 bytes          | [15 octets of UTF-8 data]                       |
|  90 | 144 | String                    | [chunk length] [UTF-8 data] ...                 |
|  91 | 145 | Resource Identifier       | [chunk length] [UTF-8 data] ...                 |
|  92 | 146 | Custom (Binary)           | [chunk length] [data] ...                       |
|  93 | 147 | Custom (Text)             | [chunk length] [UTF-8 data] ...                 |
|  94 | 148 | Typed Array               | [type] [chunk length] [elements] ...            |
|  95 | 149 | RESERVED                  |                                                 |
|  96 | 150 | Concatenate               | String                                          |
|  97 | 151 | Marker                    | Positive integer / string                       |
|  98 | 152 | Reference                 | Positive integer / string / resource identifier |
|  99 | 153 | Date                      | [[Compact Date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date)] |
|  9a | 154 | Time                      | [[Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time)] |
|  9b | 155 | Timestamp                 | [[Compact Timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp)] |
|  9c | 156 | Integer value -100        |                                                 |
| ... | ... | ...                       |                                                 |
|  fe | 254 | Integer value -2          |                                                 |
|  ff | 255 | Integer value -1          |                                                 |



Numeric Types
-------------

### Boolean

True or false.

**Examples**:

    [7c] = false
    [7d] = true


### Integer

Integers are encoded in three possible ways:

#### Small Int

Values from -100 to +100 ("small int") are encoded into the type field itself, and can be read directly as 8-bit signed two's complement integers.

#### Fixed Width

Fixed width integers are stored unsigned in widths of 8, 16, 32, and 64 bits (stored in little endian byte order). The sign is encoded in the type field.

    [type] [byte 1 (low)] ... [byte x (high)]

#### Variable width

Variable width integers are encoded as blocks of little endian ordered bytes with a length header. The header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128), denoting how many bytes of integer data follows. The sign is encoded in the type field.

    [type] [length] [byte 1 (low)] ... [byte x (high)]

**Examples**:

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

**Examples**:

    [65 07 4b] = -7.5
    [65 ac 02 d0 9e 38] = 9.21424e80


### Binary Floating Point

Binary floating point values are stored in 32 or 64-bit ieee754 binary floating point format, or in 16-bit [bfloat](https://software.intel.com/sites/default/files/managed/40/8b/bf16-hardware-numerics-definition-white-paper.pdf) format, in little endian byte order.

**Examples**:

    [70 af 44] = 0x1.5e4p+10
    [71 00 e2 af 44] = 0x1.5fc4p+10
    [72 00 10 b4 3a 99 8f 32 46] = 0x1.28f993ab41p+100


### UID

A unique identifier, stored according to [rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2) binary format (i.e. big endian).

**Example**:

    [73 12 3e 45 67 e8 9b 12 d3 a4 56 42 66 55 44 00 00] = UID 123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

### Date

Dates are stored in [compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date) format.

**Example**:

    [99 56 cd 00] = Oct 22, 2051


### Time

Time values are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time) format.

**Example**:

    [9a f7 58 74 fc f6 a7 01 10 45 2f 42 65 72 6c 69 6e] = 13:15:59.529435422/E/Berlin


### Timestamp

Timestamps are stored in [compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp) format.

**Example**:

    [9b 81 ac a0 b5 03 8f 1a ef d1] = Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array Types
-----------

An array is a contiguous sequence of identically sized elements, stored in length delimited chunks. The array type determines how the data is to be interpreted, and the size of each element.


### Chunking

Array data is "chunked", meaning that it is represented as a series of chunks of data, each with its own length field representing the number of elements (not necessarily bytes) in the chunk:

    [chunk-length-a] [chunk-elements-a] [chunk-length-b] [chunk-elements-b] ...

There is no limit to the number of chunks in an array, nor do the chunks have to be the same length. The most common case would be to represent the array as a single chunk, but there might be cases where you need multiple chunks, such as when the array length is not known from the start (for example if it's being built progressively).

#### Chunk Header

All array chunks are preceded by a header containing the chunk length and a continuation bit (the low bit of the fully decoded header). The header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128). Chunk processing continues until the end of a chunk with a continuation bit of 0.

| Field        | Bits | Description                          |
| ------------ | ---- | ------------------------------------ |
| Length       |   *  | Chunk length (in octets)             |
| Continuation |   1  | If 1, another chunk follows this one |

#### Zero Chunk

The chunk header 0x00 indicates a chunk of length 0 with continuation 0, effectively terminating any array. It's no coincidence that 0x00 also acts as the null terminator for C-style strings. An encoder may use this feature to artificially null-terminate strings in order to create immutable-friendly zero-copy documents that support C-style string implementations.

    [90 20 m i s u n d e r s t a n d i n g ...]

vs

    [90 21 m i s u n d e r s t a n d i n g 00 ...]

Note that this technique will only work for the general string type (0x90), not for the short string types 0x80 - 0x8f (which have no chunk headers).

If the source buffer in your decoder is mutable, you could achieve C-style zero-copy without requiring the above technique, using a scheme whereby you pre-cache the type code of the next value, overwrite that type code in the buffer with 0 (effectively "terminating" the string), and then process the next value using the pre-cached type:

    ...                          // buffer = [84 t e s t 6a 10 a0 ...]
    case string (length 4):      // 0x84 = string (length 4)
      cachedType = buffer[5]     // 0x6a (16-bit positive int type)
      buffer[5] = 0              // buffer = [84 t e s t 00 10 a0 ...]
      notifyString(buffer+1)     // [t e s t 00 ...] = null-terminated string "test"
      next(cachedType, buffer+6) // 0x6a, [10 a0 ...] = 16-bit positive int value 40976

**Example**:

    [1d] (14 elements of data) [08] (4 elements of data)

In this case, the first chunk is 14 elements long and has a continuation bit of 1. The second chunk has 4 elements of data and a continuation bit of 0. The total length of the array is 18 elements (element size depends on the array type), split across two chunks.


### String

Strings are encoded in UTF-8 with different type fields depending on the length. The length is in octets, NOT characters.

The general string encoding:

    [90] [Length+continuation] [Octet 0] ... [Octet (Length-1)]

For byte lengths from 0 to 15, there are special fixed-length string types (0x80 - 0x8f) that contain only a single array chunk with an implied length. For longer strings, use the general (0x90) string type.

    [80]
    [81] [octet 0]
    [82] [octet 0] [octet 1]
    ...

**Examples**:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 2a e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺


### Resource Identifier

Resource identifiers are encoded with the type 0x92. The length is in octets, NOT characters.

**Example**:

    [92 aa 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top


### Custom Types

#### Custom Type (Binary Encoding)

Custom binary types are encoded as binary data with the type 0x93. The length is the number of octets used by the data.

**Example**:

    [93 12 04 f6 28 3c 40 00 00 40 40]
    = binary data representing an imaginary custom "cplx" struct
      {
          type:uint8 = 4
          real:float32 = 2.94
          imag:float32 = 3.0
      }


#### Custom Type (Text Encoding)

Custom text types are encoded as UTF-8 strings representing the data, with the type 0x94. The length is in octets, NOT characters.

**Example**:

    [94 1a 63 70 6c 78 28 32 2e 39 34 2b 33 69 29] = custom data encoded as the string "cplx(2.94+3i)"


### Typed Array

A typed array is structured as follows:

| Field        | Description                            |
| ------------ | -------------------------------------- |
| Type         | The type code 0x95 (typed array)       |
| Element Type | The type of the elements in this array |
| Chunk Header | The number of elements following       |
| Elements     | The elements as a sequence of octets   |
| ...          | Possibly more chunks                   |

The length in each array chunk header represents the number of elements in the chunk. The number of octets in an array chunk must be the element count * the element width in octets (e.g. 2 octets for 16-bit elements, 8 octets for 64-bit elements, etc).

The following element types are allowed:

| Type Code                      | Actual Type               | Element Size (bits) |
| ------------------------------ | ------------------------- | ------------------- |
| 0x68 (8-bit positive integer)  | Unsigned int              | 8                   |
| 0x6a (16-bit positive integer) | Unsigned int              | 16                  |
| 0x6c (32-bit positive integer) | Unsigned int              | 32                  |
| 0x6e (64-bit positive integer) | Unsigned int              | 64                  |
| 0x69 (8-bit negative integer)  | 2's complement signed int | 8                   |
| 0x6b (16-bit negative integer) | 2's complement signed int | 16                  |
| 0x6d (32-bit negative integer) | 2's complement signed int | 32                  |
| 0x6f (64-bit negative integer) | 2's complement signed int | 64                  |
| 0x70 (16-bit binary float)     | Bfloat16                  | 16                  |
| 0x71 (32-bit binary float)     | IEEE754 binary float      | 32                  |
| 0x72 (64-bit binary float)     | IEEE754 binary float      | 64                  |
| 0x73 (UID)                     | RFC4122 UUID              | 128                 |
| 0x7d (Boolean true)            | Boolean (0=false, 1=true) | 1                   |

Element byte ordering is according to the element type (big endian for UUID, little endian for everything else).

#### Boolean Array

In boolean arrays, the elements (bits) are encoded 8 per byte, with the first element of the array stored in the least significant bit of the first byte of the encoding. Unused trailing bits in a chunk must be cleared to 0 by an encoder, and must be ignored by a decoder.

For example, the boolean array `{0,0,1,1,1,0,0,0,0,1,0,1,1,1,1}` would encode to `[1c 7a]` with a length of `15`. The encoded value can be directly read on little endian architectures into the multibyte unsigned integer value `0b111101000011100` (`0x7a1c`), such that the least significant bit of the unsigned integer representation is the first element of the array.

**Examples**:

    [95 68 0a 01 02 03 04 05] = byte (unsigned 8-bit) array {0x01, 0x02, 0x03, 0x04, 0x05}
    [95 6c 04 80 84 1e 00 81 84 1e 00] = uint32 array {2000000, 2000001}
    [95 7d 16 e6 06] = bit array {0,1,1,0,1,1,1,0,0,1,1}



Container Types
---------------

### List

A list begins with 0x7a, followed by a series of objects, and is terminated with 0x7b (end of container).

**Example**:

    [7a 01 6a 88 13 7b] = A list containing integers (1, 5000)


### Map

A map begins with 0x79, followed by a series of key-value pairs, and is terminated with 0x7b (end of container).

    [79] [key-1] [value-1] [key-2] [value-2] ... [7b]

**Example**:

    [79 81 61 01 81 62 02 7b] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Markup

Unlike other containers, a markup container requires two end-of-container markers: one to mark the end of the attributes, and another one to mark the end of the contents section:

    [78] [name] [attr-key-1] [attr-value-1] ... [7b] [contents-1] [contents-2] ... [7b]

**Example**:

    [78 84 54 65 78 74 81 61 81 62 7b 89 53 6f 6d 65 20 74 65 78 74 7b] = <Text a=b;Some text>


### Relationship

A relationship always consists of exactly three components, and therefore doesn't use an end-of-container terminator.

    [77] [subject] [predicate] [object]

**Example**:

TODO



Peudo-Objects
-------------

### Marker

A marker begins with the marker type (0x97), followed by a marker ID, and then the marked object.

    [97] [ID] [marked object]

**Example**:

    [97 01 79 8a 73 6f 6d 65 5f 76 61 6c 75 65 90 22 72
     65 70 65 61 74 20 74 68 69 73 20 76 61 6c 75 65 7b]
    = the map {some_value = "repeat this value"}, tagged with integer ID 1


### Reference

A reference begins with the reference type (0x98), followed by either a marker ID or a [resource identifier](#resource-identifier).

**Examples**:

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

Metadata maps are encoded the same as [maps](#map), using the type 0x77.

**Example**:

    [77 82 5f 74 7a 85 61 5f 74 61 67 7b 7b] = metadata map: (_t = ["a_tag"])


### Comment

Comments are encoded the same as [lists](#list), using the type 0x76.

**Example**:

    [76 90 80 01 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20
     66 61 69 6c 73 20 74 6f 20 73 74 61 72 74 20 6f 6e 20 61 72 6d 36 34
     20 75 6e 6c 65 73 73 20 42 20 6c 61 74 63 68 20 69 73 20 73 65 74 7b]
    = Bug #95512: System fails to start on arm64 unless B latch is set


### Padding

Padding is encoded as type 0x7f. Repeat as many times as needed.

**Example**:

    [7f 7f 7f 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.



Other Types
-----------

### NA

    [7e 84 67 6f 6e 65] = Not available for reason "gone"
    [7e 6a 94 01] = Not available for reason 404
    [7e 7e] = Not available with no reason given


### Concatenation

The concatenation operator concatenates the string following onto the previous resource identifier, effectively yielding a new resource identifier.

**Example**:

TODO


### RESERVED

This type is reserved for future expansion of the format, and must not be used.



Empty Document
--------------

An empty document in CBE is signified by using the [NA](#na) type with reason NA as the top-level object:

    [03 01 7e 7e]



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders should use the smallest encoding that stores a value without data loss.

Specialized applications may wish to preserve more numeric type information to distinguish floats from integers, or even to distinguish between data sizes. This is allowed, as it will make no difference to a generic decoder.



Alignment
---------

Applications might require data to be aligned in some cases for optimal decoding performance. For example, some processors might not be able to read unaligned multibyte data types without special (costly) intervention. An encoder could in theory be tuned to insert [padding](#padding) when encoding certain data:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 7f | 7f | 7f | 67 | 00 | 00 | 00 | 8f |

Alignment tuning is usually only useful when the target decoding environment is known prior to encoding. It's mostly an optimization for closed systems.



Version History
---------------

| Date          | Version   |
| ------------- | --------- |
| July 22, 2018 | Draft     |
| TBD           | Version 1 |



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
