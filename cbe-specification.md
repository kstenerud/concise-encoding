Concise Binary Encoding
=======================


Version
-------

Version 0 (prerelease)



This Document
-------------

This document describes the encoding format of Concise Binary Encoding (CBE), and how codecs of this format must behave.

The logical structure of Concise Encoding is described [in its own document](ce-structure.md).



Contents
--------

- [Concise Binary Encoding](#concise-binary-encoding)
  - [Version](#version)
  - [This Document](#this-document)
  - [Contents](#contents)
  - [Terms and Conventions](#terms-and-conventions)
  - [What is Concise Binary Encoding?](#what-is-concise-binary-encoding)
  - [Document Structure](#document-structure)
  - [Document Version Specifier](#document-version-specifier)
  - [Object Encoding](#object-encoding)
    - [Type Field (Plane 7f)](#type-field-plane-7f)
  - [Numeric Types](#numeric-types)
    - [Boolean](#boolean)
    - [Integer](#integer)
      - [Small Int](#small-int)
      - [Fixed Width Int](#fixed-width-int)
      - [Variable width Int](#variable-width-int)
    - [Decimal Floating Point](#decimal-floating-point)
    - [Binary Floating Point](#binary-floating-point)
    - [UID](#uid)
  - [Temporal Types](#temporal-types)
    - [Date](#date)
    - [Time](#time)
    - [Timestamp](#timestamp)
  - [Array Types](#array-types)
    - [Array Elements](#array-elements)
    - [Array Forms](#array-forms)
      - [Short Form](#short-form)
      - [Chunked Form](#chunked-form)
        - [Chunk Header](#chunk-header)
        - [Bit Array Chunks](#bit-array-chunks)
        - [String-like Array Chunks](#string-like-array-chunks)
        - [Zero Chunk](#zero-chunk)
    - [Supported Array Types](#supported-array-types)
      - [String](#string)
      - [Resource Identifier](#resource-identifier)
      - [Bit Array](#bit-array)
      - [Media](#media)
      - [Custom Types](#custom-types)
  - [Container Types](#container-types)
    - [List](#list)
    - [Map](#map)
    - [Struct Instance](#struct-instance)
    - [Edge](#edge)
    - [Node](#node)
  - [Other Types](#other-types)
    - [Null](#null)
    - [RESERVED](#reserved)
  - [Peudo-Objects](#peudo-objects)
    - [Local Reference](#local-reference)
    - [Remote Reference](#remote-reference)
  - [Invisible Objects](#invisible-objects)
    - [Comment](#comment)
    - [Padding](#padding)
  - [Structural Objects](#structural-objects)
    - [Struct Template](#struct-template)
    - [Marker](#marker)
    - [Identifier](#identifier)
  - [Empty Document](#empty-document)
  - [Smallest Possible Size](#smallest-possible-size)
  - [Alignment](#alignment)
  - [Version History](#version-history)
  - [License](#license)



Terms and Conventions
---------------------

**The following bolded, capitalized terms have specific meanings in this specification**:

| Term             | Meaning                                                                                                               |
| ---------------- | --------------------------------------------------------------------------------------------------------------------- |
| **MUST (NOT)**   | If this directive is not adhered to, the document or implementation is invalid/non-conformant.                        |
| **SHOULD (NOT)** | Every effort should be made to follow this directive, but the document/implementation is still valid if not followed. |
| **MAY (NOT)**    | It is up to the implementation to decide whether to do something or not.                                              |
| **CAN**          | Refers to a possibility which **MUST** be accommodated by the implementation.                                         |
| **CANNOT**       | Refers to a situation which **MUST NOT** be allowed by the implementation.                                            |
| **OPTIONAL(LY)** | The implementation **MUST** support both the existence and the absence of the specified item.                         |
| **OPTION(S)**    | Configuration option(s) that implementations **MUST** provide.                                                        |

**Sample data will generally be represented as follows**:

 * Character sequences are enclosed within backticks: `this is a character sequence`
 * Byte sequences are represented as a series of two-digit hex values, enclosed within backticks and square brackets: [`f1 33 91`]



What is Concise Binary Encoding?
--------------------------------

Concise Binary Encoding (CBE) is the binary variant of [Concise Encoding](ce-structure.md): a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

The binary format aims for compactness and machine processing efficiency while maintaining 1:1 compatibility with the [text format](cte-specification.md) (which aims to present data in a human friendly way).



Document Structure
------------------

Documents begin with a [version specifier](#document-version-specifier), possibly followed by [invisible](ce-structure.md#invisible-objects) and [structural](ce-structure.md#structural-objects) objects, and then ultimately followed by the top-level [data object](ce-structure.md#data-objects).

    [version specifier] [optional invisible and structural objects] [top-level data object]



Document Version Specifier
--------------------------

The version specifier is composed of the octet `0x81`, followed by a version number (an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128) representing which version of this specification the document adheres to).

**Example**:

    [81 01] = CBE version 1



Object Encoding
---------------

A CBE document is byte-oriented. All objects are composed of a type field (1 or 2 bytes long) and a possible payload that will always end on an 8-bit boundary. Variable length types always begin with length fields, and all types always end deteriministically at an 8-bit boundary with no lookahead required. This ensures that the end of a CBE document can always be deterministically found with no scan-ahead or backtracking.

Containers and arrays can always be built incrementally (you don't need to know their final size before you start encoding their contents).

The types are structured such that the most commonly used types and values encode into the smallest space while still remaining zero-copy capable in most places on little endian systems.


| Hex | Type                                            | Payload                                  |
| --- | ----------------------------------------------- | ---------------------------------------- |
|  00 | [Integer value 0](#small-int)                   |                                          |
|  01 | [Integer value 1](#small-int)                   |                                          |
| ... | ...                                             |                                          |
|  64 | [Integer value 100](#small-int)                 |                                          |
|  65 | [UID](#uid)                                     | [128 bits of data, big endian]           |
|  66 | [Positive Integer](#variable-width-int)         | [byte length] [little endian bytes]      |
|  67 | [Negative Integer](#variable-width-int)         | [byte length] [little endian bytes]      |
|  68 | [Positive Integer (8 bit)](#fixed-width-int)    | [8-bit unsigned integer]                 |
|  69 | [Negative Integer (8 bit)](#fixed-width-int)    | [8-bit unsigned integer]                 |
|  6a | [Positive Integer (16 bit)](#fixed-width-int)   | [16-bit unsigned integer, little endian] |
|  6b | [Negative Integer (16 bit)](#fixed-width-int)   | [16-bit unsigned integer, little endian] |
|  6c | [Positive Integer (32 bit)](#fixed-width-int)   | [32-bit unsigned integer, little endian] |
|  6d | [Negative Integer (32 bit)](#fixed-width-int)   | [32-bit unsigned integer, little endian] |
|  6e | [Positive Integer (64 bit)](#fixed-width-int)   | [64-bit unsigned integer, little endian] |
|  6f | [Negative Integer (64 bit)](#fixed-width-int)   | [64-bit unsigned integer, little endian] |
|  70 | [Binary Float (16 bit)](#binary-floating-point) | [16-bit [bfloat16](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format), little endian] |
|  71 | [Binary Float (32 bit)](#binary-floating-point) | [[32-bit ieee754 binary float](https://en.wikipedia.org/wiki/Single-precision_floating-point_format), little endian] |
|  72 | [Binary Float (64 bit)](#binary-floating-point) | [[64-bit ieee754 binary float](https://en.wikipedia.org/wiki/Double-precision_floating-point_format), little endian] |
|  73 | [RESERVED](#reserved)                           |                                          |
|  74 | [RESERVED](#reserved)                           |                                          |
|  75 | [RESERVED](#reserved)                           |                                          |
|  76 | [Decimal Float](#decimal-floating-point)        | [[Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)] |
|  77 | [Local Reference](#local-reference)             | [length] [UTF-8 data]                    |
|  78 | [Boolean False](#boolean)                       |                                          |
|  79 | [Boolean True](#boolean)                        |                                          |
|  7a | [Date](#date)                                   | [[Compact Date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date)]           |
|  7b | [Time](#time)                                   | [[Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time)]           |
|  7c | [Timestamp](#timestamp)                         | [[Compact Timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp)] |
|  7d | [Null](#null)                                   |                                          |
|  7e | [RESERVED](#reserved)                           |                                          |
|  7f | [Plane 7f](#type-field-plane-7f)                | (See [Plane 7f](#type-field-plane-7f))   |
|  80 | [String: 0 bytes](#string)                      |                                          |
|  81 | [String: 1 byte](#string)                       | [1 octet of UTF-8 data]                  |
|  82 | [String: 2 bytes](#string)                      | [2 octets of UTF-8 data]                 |
|  83 | [String: 3 bytes](#string)                      | [3 octets of UTF-8 data]                 |
|  84 | [String: 4 bytes](#string)                      | [4 octets of UTF-8 data]                 |
|  85 | [String: 5 bytes](#string)                      | [5 octets of UTF-8 data]                 |
|  86 | [String: 6 bytes](#string)                      | [6 octets of UTF-8 data]                 |
|  87 | [String: 7 bytes](#string)                      | [7 octets of UTF-8 data]                 |
|  88 | [String: 8 bytes](#string)                      | [8 octets of UTF-8 data]                 |
|  89 | [String: 9 bytes](#string)                      | [9 octets of UTF-8 data]                 |
|  8b | [String: 11 bytes](#string)                     | [11 octets of UTF-8 data]                |
|  8a | [String: 10 bytes](#string)                     | [10 octets of UTF-8 data]                |
|  8c | [String: 12 bytes](#string)                     | [12 octets of UTF-8 data]                |
|  8d | [String: 13 bytes](#string)                     | [13 octets of UTF-8 data]                |
|  8e | [String: 14 bytes](#string)                     | [14 octets of UTF-8 data]                |
|  8f | [String: 15 bytes](#string)                     | [15 octets of UTF-8 data]                |
|  90 | [String](#string)                               | [chunk length] [UTF-8 data] ...          |
|  91 | [Resource Identifier](#resource-identifier)     | [chunk length] [UTF-8 data] ...          |
|  92 | [Custom Type](#custom-types)                    | [chunk length] [data] ...                |
|  93 | [Array: Unsigned Int8](#supported-array-types)  | [chunk length] [8-bit elements] ...      |
|  94 | [Array: Bit](#bit-array)                        | [chunk length] [1-bit elements] ...      |
|  95 | [Padding](#padding)                             |                                          |
|  96 | [Struct Instance](#struct-instance)             | ID, Value ... End of Container           |
|  97 | [Edge](#edge)                                   | Source, Description, Destination, End of Container |
|  98 | [Node](#node)                                   | Value, Child Node ... End of Container   |
|  99 | [Map](#map)                                     | (Key, Value) ... End of Container        |
|  9a | [List](#list)                                   | Object ... End of Container              |
|  9b | [End of Container](#container-types)            |                                          |
|  9c | [Integer value -100](#small-int)                |                                          |
| ... | ...                                             |                                          |
|  fe | [Integer value -2](#small-int)                  |                                          |
|  ff | [Integer value -1](#small-int)                  |                                          |


### Type Field (Plane 7f)

Types from plane 7f are represented using two bytes instead of one, with the prefix `[7f]`. For example, the type for signed 16-bit array with 8 elements is `[7f 28]`, and the type for media is `[7f e1]`.

| Hex | Type                                            | Elems | Payload                                   |
| --- | ----------------------------------------------- | ----- | ----------------------------------------- |
|  00 | [Array: UID](#supported-array-types)            |    0  | [128-bit big endian element] x0           |
| ... | ...                                             |  ...  | ...                                       |
|  0f | [Array: UID](#supported-array-types)            |   15  | [128-bit big endian element] x15          |
|  10 | [Array: Signed Int8](#supported-array-types)    |    0  | [8-bit element] x0                        |
| ... | ...                                             |  ...  | ...                                       |
|  1f | [Array: Signed Int8](#supported-array-types)    |   15  | [8-bit element] x15                       |
|  20 | [Array: Unsigned Int16](#supported-array-types) |    0  | [16-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  2f | [Array: Unsigned Int16](#supported-array-types) |   15  | [16-bit little endian element] x15        |
|  30 | [Array: Signed Int16](#supported-array-types)   |    0  | [16-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  3f | [Array: Signed Int16](#supported-array-types)   |   15  | [16-bit little endian element] x15        |
|  40 | [Array: Unsigned Int32](#supported-array-types) |    0  | [32-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  4f | [Array: Unsigned Int32](#supported-array-types) |   15  | [32-bit little endian element] x15        |
|  50 | [Array: Signed Int32](#supported-array-types)   |    0  | [32-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  5f | [Array: Signed Int32](#supported-array-types)   |   15  | [32-bit little endian element] x15        |
|  60 | [Array: Unsigned Int64](#supported-array-types) |    0  | [64-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  6f | [Array: Unsigned Int64](#supported-array-types) |   15  | [64-bit little endian element] x15        |
|  70 | [Array: Signed Int64](#supported-array-types)   |    0  | [64-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  7f | [Array: Signed Int64](#supported-array-types)   |   15  | [64-bit little endian element] x15        |
|  80 | [Array: BFloat16](#supported-array-types)       |    0  | [16-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  8f | [Array: BFloat16](#supported-array-types)       |   15  | [16-bit little endian element] x15        |
|  90 | [Array: Binary Float32](#supported-array-types) |    0  | [32-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  9f | [Array: Binary Float32](#supported-array-types) |   15  | [32-bit little endian element] x15        |
|  a0 | [Array: Binary Float64](#supported-array-types) |    0  | [64-bit little endian element] x0         |
| ... | ...                                             |  ...  | ...                                       |
|  af | [Array: Bin Float64](#supported-array-types)    |   15  | [64-bit little endian element] x15        |
| ... | [RESERVED](#reserved)                           |       |                                           |
|  e0 | [Array: UID](#supported-array-types)            |    ∞  | [chunk length] [128-bit B-E elements] ... |
|  e1 | [Array: Signed Int8](#supported-array-types)    |    ∞  | [chunk length] [8-bit elements] ...       |
|  e2 | [Array: Unsigned Int16](#supported-array-types) |    ∞  | [chunk length] [16-bit L-E elements] ...  |
|  e3 | [Array: Signed Int16](#supported-array-types)   |    ∞  | [chunk length] [16-bit L-E elements] ...  |
|  e4 | [Array: Unsigned Int32](#supported-array-types) |    ∞  | [chunk length] [32-bit L-E elements] ...  |
|  e5 | [Array: Signed Int32](#supported-array-types)   |    ∞  | [chunk length] [32-bit L-E elements] ...  |
|  e6 | [Array: Unsigned Int64](#supported-array-types) |    ∞  | [chunk length] [64-bit L-E elements] ...  |
|  e7 | [Array: Signed Int64](#supported-array-types)   |    ∞  | [chunk length] [64-bit L-E elements] ...  |
|  e8 | [Array: BFloat16](#supported-array-types)       |    ∞  | [chunk length] [16-bit L-E elements] ...  |
|  e9 | [Array: Binary Float32](#supported-array-types) |    ∞  | [chunk length] [32-bit L-E elements] ...  |
|  ea | [Array: Binary Float64](#supported-array-types) |    ∞  | [chunk length] [64-bit L-E elements] ...  |
| ... | [RESERVED](#reserved)                           |       |                                           |
|  f0 | [Marker](#marker)                               |    1  | [length] [UTF-8 data]                     |
|  f1 | [Struct Template](#struct-template)             |    ∞  | ID, Key ... End of Container              |
|  f2 | [Remote Reference](#remote-reference)           |    1  | [resource id]                             |
|  f3 | [Media](#media)                                 |    ∞  | [media type] [chunk length] [data] ...    |
| ... | [RESERVED](#reserved)                           |       |                                           |



Numeric Types
-------------

### Boolean

True or false.

**Examples**:

    [78] = false
    [79] = true


### Integer

CBE encoders **MUST** output integer values in the smallest form possible, unless explicitly configured to do otherwise:

| Values                                 | Form                                          |
| -------------------------------------- | --------------------------------------------- |
| ± 0 - 100                              | [small integer](#small-int)                   |
| ± 0x65 - 0xff                          | [8-bit integer](#fixed-width-int)             |
| ± 0x100 - 0xffff                       | [16-bit integer](#fixed-width-int)            |
| ± 0x10000 - 0xffffffff                 | [32-bit integer](#fixed-width-int)            |
| ± 0x100000000 - 0xffffffffffff         | [variable width integer](#variable-width-int) |
| ± 0x1000000000000 - 0xffffffffffffffff | [64-bit integer](#fixed-width-int)            |
| ± 0x10000000000000000 and up           | [variable width integer](#variable-width-int) |

Integers are encoded in three possible ways:

#### Small Int

Values from -100 to +100 ("small int") are encoded into the type field itself, and can be read directly as 8-bit signed two's complement integers.

#### Fixed Width Int

Fixed width integers are stored as their absolute values in widths of 8, 16, 32, and 64 bits (in little endian byte order). The type field implies the sign of the integer.

    [type] [byte 1 (low)] ... [byte x (high)]

**Note**: Because the sign is encoded into the type field, it's possible to encode the value 0 with a negative sign. `-0` is not representable as an integer in most environments, so care must be taken after decoding to ensure that the sign is not lost (the most common approach is to convert it to a floating point type).

#### Variable width Int

Variable width integers are encoded as a block of little endian ordered bytes, prefixed with a length header. The length header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128), denoting how many bytes of integer data follows. The sign is encoded in the type field.

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

    [76 07 4b] = -7.5
    [76 ac 02 d0 9e 38] = 9.21424e+80


### Binary Floating Point

Binary floating point values are stored in 32 or 64-bit [ieee754 binary floating point](https://en.wikipedia.org/wiki/IEEE_754) format, or in 16-bit [bfloat](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format) format, in little endian byte order.

**Examples**:

    [70 af 44] = 0x1.5ep+10
    [71 00 e2 af 44] = 0x1.5fc4p+10
    [72 00 10 b4 3a 99 8f 32 46] = 0x1.28f993ab41p+100


### UID

A unique identifier, stored according to [rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2) binary format.

**Note**: This is the only data type that is stored in **big endian** byte order ([as required by rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2)).

**Example**:

    [65 12 3e 45 67 e8 9b 12 d3 a4 56 42 66 55 44 00 00] = UID 123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

Temporal types are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md) format.

**Note**: [zero values](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#zero-values) are not allowed in CBE.


### Date

Dates are stored in [compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date) format.

**Example**:

    [7a 56 cd 00] = Oct 22, 2051


### Time

Time values are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time) format.

**Example**:

    [7b f7 58 74 fc f6 a7 fd 10 45 2f 42 65 72 6c 69 6e] = 13:15:59.529435422/E/Berlin


### Timestamp

Timestamps are stored in [compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp) format.

**Example**:

    [7c 81 ac a0 b5 03 8f 1a ef d1] = Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array Types
-----------

An array is a contiguous sequence of identically sized elements, stored in length delimited chunks. The [array type](#supported-array-types) determines how the data is to be interpreted, and the size of each element.


### Array Elements

Array elements have a fixed size determined by the [array type](#supported-array-types). Length fields in array chunks represent the number of *elements*, so for example a uin32 array chunk of length 3 contains 12 bytes of array data (3 elements x 4 bytes per element).


### Array Forms

All arrays have a [chunked form](#chunked-form), and many also have a [short form](#short-form).

**Primary plane, short form**:

| Field        | Bits | Description                                      |
| ------------ | ---- | ------------------------------------------------ |
| Type         |    4 | Upper 4 bits in [primary plane](#type-field)     |
| Length       |    4 | Number of elements (0-15)                        |
| Elements     |    ∞ | The elements as a sequence of octets             |

**Primary plane, chunked form**:

| Field        | Bits | Description                                      |
| ------------ | ---- | ------------------------------------------------ |
| Type         |   8  | Type in [primary plane](#type-field)             |
| Chunk Header |   ∞  | The number of elements in this chunk             |
| Elements     |   ∞  | The elements as a sequence of octets             |
| ...          |   ∞  | Possibly more chunks                             |

**Plane 7f, short form**:

| Field        | Bits | Description                                      |
| ------------ | ---- | ------------------------------------------------ |
| Type (plane) |    8 | 0x7f (plane 7f)                                  |
| Type         |    4 | Upper 4 bits in [plane 7f](#type-field-plane-7f) |
| Length       |    4 | Number of elements (0-15)                        |
| Elements     |    ∞ | The elements as a sequence of octets             |

**Plane 7f, chunked form**:

| Field        | Bits | Description                                      |
| ------------ | ---- | ------------------------------------------------ |
| Type (plane) |   8  | 0x7f (plane 7f)                                  |
| Type         |   8  | Type in [plane 7f](#type-field-plane-7f)         |
| Chunk Header |   ∞  | The number of elements in this chunk             |
| Elements     |   ∞  | The elements as a sequence of octets             |
| ...          |   ∞  | Possibly more chunks                             |

The length represents the number of **elements** (not bytes) in the array/chunk.

**Examples**:

 * `[93 04 01 02]` = unsigned 8-bit array with elements 1, 2
 * `[7f 22 01 00 02 00]` = unsigned 16-bit array (note: plane 7f) with elements 1, 2

#### Short Form

Short form arrays have their length encoded in the lower 4 bits of the type field itself in order to save space when encoding arrays with lengths from 0 to 15 elements. Not all array types have a short form.

CBE encoders **MUST** use the short form whenever it is possible to do so, unless explicitly configured to do otherwise.

**Examples**:

 * `[83 61 62 63]` = the string "abc" (short form - length is part of the type field)
 * `[90 06 61 62 63]` = the string "abc" (chunked form - length is a separate field)


#### Chunked Form

In chunked form, array data is "chunked", meaning that it is represented as a series of chunks of data, each with its own [header](#chunk-header) containing the number of elements in the chunk and a continuation bit that tells if more chunks follow the current one:

    [chunk-header-a] [chunk-elements-a] [chunk-header-b] [chunk-elements-b] ...

An array **CAN** contain any number of chunks, and the chunks don't have to be the same length. The most common use case would be to represent the entire array as a single chunk, but there might be cases where you need multiple chunks, such as when the array length is not known at the time when encoding has started (for example if it's being built progressively).

**Example**:

    [1d] (14 elements of data) [08] (4 elements of data)

In this case, the first chunk of the array has 14 elements and has a continuation bit of 1. The second chunk has 4 elements and a continuation bit of 0. The total length of the array is 18 elements (element size depends on the array type), split across two chunks.

##### Chunk Header

All array chunks are preceded by a header containing the chunk length and a continuation bit (the low bit of the fully decoded header). The header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128). Chunk processing continues until the end of a chunk with a continuation bit of 0.

| Field        | Bits | Description                          |
| ------------ | ---- | ------------------------------------ |
| Length       |   ∞  | Chunk length (element count)         |
| Continuation |   1  | If 1, another chunk follows this one |

**Examples**:

 * `[03]` = Chunk length 1 with the continuation bit set
 * `[80 02]` = Chunk length 256 with the continuation bit cleared

##### Bit Array Chunks

Bit array chunks with continuation=1 **MUST** have a length that is a multiple of 8 so that the chunk data begins and ends on an 8-bit boundary. Only the final chunk (continuation=0) of a bit array **CAN** be of arbitrary size.

##### String-like Array Chunks

Array chunks for string-like data (UTF-8) **MUST** always end on a character boundary (do not split multibyte characters between chunks). Decoders **MAY** validate string data on every chunk received, which will fail if a chunk ends on an incomplete character.

##### Zero Chunk

The chunk header 0x00 indicates a chunk of length 0 with continuation 0, effectively terminating any array. It's no coincidence that 0x00 also acts as the null terminator for C-style strings. An encoder may use this feature to artificially null-terminate strings in order to create immutable-friendly zero-copy documents that support C-style string implementations.

    [90 20 m i s u n d e r s t a n d i n g ...]

vs

    [90 21 m i s u n d e r s t a n d i n g 00 ...]

Note that this technique will only work for the general string type (0x90), not for the short string types 0x80 - 0x8f (which have no chunk headers).

If the source buffer in your decoder is mutable, you could achieve C-style zero-copy without requiring the above technique, using a scheme whereby you pre-cache the type code of the next value, overwrite that type code's memory location in the buffer with 0 (effectively "terminating" the string), and then process the next value using the pre-cached type:

    ...                          // buffer = [84 t e s t 6a 10 a0 ...]
    case string (length 4):      // 0x84 = string (length 4)
      cachedType = buffer[5]     // 0x6a (16-bit positive int type)
      buffer[5] = 0              // buffer = [84 t e s t 00 10 a0 ...]
      notifyString(buffer+1)     // [t e s t 00 ...] = null-terminated string "test"
      next(cachedType, buffer+6) // 0x6a, [10 a0 ...] = 16-bit positive int value 40976


### Supported Array Types

| Array Type                                                                                   | Size (bits) | Byte Order    |
| -------------------------------------------------------------------------------------------- | ----------- | ------------- |
| Unsigned int                                                                                 | 8           | -             |
| Unsigned int                                                                                 | 16          | Little Endian |
| Unsigned int                                                                                 | 32          | Little Endian |
| Unsigned int                                                                                 | 64          | Little Endian |
| 2's complement signed int                                                                    | 8           | -             |
| 2's complement signed int                                                                    | 16          | Little Endian |
| 2's complement signed int                                                                    | 32          | Little Endian |
| 2's complement signed int                                                                    | 64          | Little Endian |
| [Bfloat16](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)                     | 16          | Little Endian |
| [IEEE754 binary float](https://en.wikipedia.org/wiki/Single-precision_floating-point_format) | 32          | Little Endian |
| [IEEE754 binary float](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) | 64          | Little Endian |
| [RFC4122 UUID](https://tools.ietf.org/html/rfc4122#section-4.1.2)                            | 128         | Big Endian    |
| [String](#string)                                                                            | 8           | -             |
| [Resource ID](#resource-identifier)                                                          | 8           | -             |
| [Bit](#bit-array)                                                                            | 1           | Little Endian |
| [Media](#media)                                                                              | 8           | -             |
| [Custom Type](#custom-types)                                                                 | 8           | -             |

#### String

Strings are encoded as UTF-8.

The general string encoding form is:

    [90] [chunk header] [Octet 0] ... [Octet (Length-1)]

Strings also have a [short form](#short-form) length encoding using types 0x80-0x8f:

    [80]
    [81] [octet 0]
    [82] [octet 0] [octet 1]
    ...

**Examples**:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 2a e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺

#### Resource Identifier

Resource identifiers are encoded like a long-form [string](#string), but with type `[91]`.

**Example**:

    [91 aa 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top

#### Bit Array

In bit arrays, the elements (bits) are encoded 8 per byte, (such that a 0 bit represents false and a 1 bit represents true), with the first element of the array stored in the least significant bit of the first byte of the encoded elements. Unused trailing (upper) bits in the [last chunk](#bit-array-chunks) **MUST** be cleared to 0 by an encoder, and **MUST** be discarded by a decoder.

For example, the bit array `{0,0,1,1,1,0,0,0,0,1,0,1,1,1,1}` would encode to `[1c 7a]` with a length of `15`. The encoded value can be directly read on little endian architectures into the multibyte unsigned integer value `0b111101000011100` (`0x7a1c`), such that the least significant bit of the unsigned integer representation is the first element of the array.

**Example**:

    [94 16 76 06] = bit array {0,1,1,0,1,1,1,0,0,1,1}

#### Media

The media object is composed of two sub-arrays: an implied string containing the [media type](http://www.iana.org/assignments/media-types/media-types.xhtml), and an implied uint8 array containing the media object's contents. Since the sub-array's types are already known, they do not themselves contain array type fields (the types are implied).

| Field        | Description                                  |
| ------------ | -------------------------------------------- |
| Plane 7f     | The type code 0x7f                           |
| Type         | The type code 0xe1 (media)                   |
| Chunk Header | The number of media type bytes in this chunk |
| Elements     | The characters as a sequence of octets       |
| ...          | Possibly more chunks until continuation = 0  |
| Chunk Header | The number of media bytes in this chunk      |
| Elements     | The bytes as a sequence of octets            |
| ...          | Possibly more chunks until continuation = 0  |

**Example**:

     *1 *2 *3 *4                                              *5
    [7f f3 20 61 70 70 6c 69 63 61 74 69 6f 6e 2f 78 2d 73 68 38 

     *6
     23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a]

Points of interest:

| Point | Description                                      |
| ----- | ------------------------------------------------ |
|  *1   | Type (0x7f = plane 7f)                           |
|  *2   | Plane 7f subtype (0xf3 = media)                  |
|  *3   | Chunk Header (0x20 = length 16, no continuation) |
|  *4   | String Data `application/x-sh`                   |
|  *5   | Chunk Header (0x38 = length 28, no continuation) |
|  *6   | Media bytes                                      |

The media in this example is the shell script (media type "application/x-sh"):

```sh
#!/bin/sh

echo hello world
```

#### Custom Types

Custom types are encoded as binary data with the array type 0x92.

**Example**:

    [92 12 01 f6 28 3c 40 00 00 40 40]
    = binary data representing a fictional custom "cplx" struct
      {
          type:uint8 = 1
          real:float32 = 2.94 (40 3c 28 f6)
          imag:float32 = 3.0  (40 40 00 00)
      }



Container Types
---------------

### List

A list begins with 0x7a, followed by a series of zero or more objects, and is terminated with 0x7b (end of container).

**Example**:

    [9a 01 6a 88 13 9b] = A list containing integers (1, 5000)


### Map

A map begins with 0x79, followed by a series of zero or more key-value pairs, and is terminated with 0x7b (end of container).

    [99] [key-1] [value-1] [key-2] [value-2] ... [9b]

**Example**:

    [99 81 61 01 81 62 02 9b] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Struct Instance

A struct instance begins with 0x75, followed by a template [identifier](#identifier), followed by a series of values to match the order that their keys are defined in the associated [template](#struct-template), and is terminated with 0x7b (end of container).

    [96] [val1] [val2] [val3] ... [9b]

**Example**:

A struct instance built from template "a", with the first key's associated value set to 5:

    [96 01 61 05 9b]


### Edge

An edge consists of a source, then a description, then a destination, and is terminated with 0x7b (end of container).

    [97] [source] [description] [destination] [9b]

**Example**:

    [97
     91 24 68 74 74 70 3a 2f 2f 73 2e 67 6f 76 2f 68 6f 6d 65 72
     91 22 68 74 74 70 3a 2f 2f 65 2e 6f 72 67 2f 77 69 66 65
     91 24 68 74 74 70 3a 2f 2f 73 2e 67 6f 76 2f 6d 61 72 67 65
     9b]
    = the relationship graph: @(@"http://s.gov/homer" @"http://e.org/wife" @"http://s.gov/marge")


### Node

A node begins with 0x78, followed by a value object and zero or more child nodes, and is terminated with 0x7b (end of container).

    [98] [value] [node] ... [9b]

**Example**:

    [98 01 98 03 98 05 9b 98 04 9b 9b 98 02 9b 9b]
    = the binary tree:
      1
     / \
    2   3
       / \
      4   5



Other Types
-----------

### Null

Null is encoded as `[7d]`.


### RESERVED

This type is reserved for future expansion of the format, and **MUST NOT** be used.



Peudo-Objects
-------------

### Local Reference

A local reference begins with the reference type (0x77), followed by a marker [identifier]](#identifier).

    [77 (length) (ID string data)]

**Examples**:

    [77 01 61] = reference to the object marked with ID "a"

### Remote Reference

A remote reference is encoded in the same manner as a [resource identifier](#resource-identifier), except with a different type code (`[7f f2]`).

**Examples**:

    [7f f2 24 63 6f 6d 6d 6f 6e 2e 63 65 23 6c 65 67 61 6c 65 73 65]
    = reference to relative file "common.ce", ID "legalese" (common.ce#legalese)

    [7f f2 4e 68 74 74 70 73 3a 2f 2f 65 78 61 6d 70 6c 65 2e 6f 72 67 2f 63 69 74 69 65 73 2f 66 72 61 6e 63 65 23 70 61 72 69 73]
    = remote reference to https://example.org/cities/france#paris, where "paris" is the local marker ID in that document



Invisible Objects
-----------------

### Comment

Comments are not supported in CBE. An encoder **MUST** skip all comments when converting CTE to CBE.


### Padding

Padding is encoded as type 0x95. Repeat as many times as needed.

**Example**:

    [95 95 95 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.



Structural Objects
------------------

### Struct Template

The struct template type is in plane 7f, subtype f1, followed by a template [identifier](#identifier), followed by keys of the template, and is terminated with 0x9b (end of container).

    [7f f1] [key1] [key2] [key3] ... [9b]

**Example**:

A struct template named "a", containing the key "b":

    [7f f1 01 61 81 62 9b]


### Marker

The marker is in plane 7f, subtype 0xf0, followed by a marker [identifier](#identifier), and then the marked object.

    [7f f0 (length) (ID string data) (marked object)]

**Example**:

    [7f f0 01 61
     99
     8a 73 6f 6d 65 5f 76 61 6c 75 65
     90 22 72 65 70 65 61 74 20 74 68 69 73 20 76 61 6c 75 65
     9b]
    = the map {"some_value" = "repeat this value"}, tagged with the ID "a".


### Identifier

Identifiers begin with an 8-bit header containing a 7-bit length (min length 1 byte, max length 127 bytes). The high bit of the header field **MUST** be cleared to 0. The length header is followed by that many **bytes** of UTF-8 data.

The length field **CANNOT** be 0.

| Field        | Bits | Value             |
| ------------ | ---- | ----------------- |
| RESERVED     |   1  | 0                 |
| Length       |   7  | 1-127             |
| UTF-8 Data   |   ∞  | UTF-8 string data |

**Note**: Identifiers are **not** standalone objects. They are always part of another object.

**Examples**:

    [07 73 6f 6d 65 5f 69 64] = some_id
    [0f e7 99 bb e9 8c b2 e6 b8 88 e3 81 bf ef bc 95] = 登録済み５



Empty Document
--------------

An empty document in CBE is signified by using [null](#null) type the top-level object:

    [81 01 7d]



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders **SHOULD** use the smallest encoding that stores a value without data loss.

Specialized applications **MAY** wish to preserve more numeric type information to distinguish floats from integers, or even to distinguish between data sizes. This is allowed, as it will make no difference to a generic decoder.



Alignment
---------

Applications might require data to be aligned in some cases for optimal decoding performance. For example, some processors might not be able to read unaligned multibyte data types without special (costly) intervention. An encoder could in theory be tuned to insert [padding](#padding) when encoding certain data:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 95 | 95 | 95 | 67 | 00 | 00 | 00 | 8f |

Alignment tuning is usually only useful when the target decoding environment is known prior to encoding. It's mostly an optimization for closed systems.



Version History
---------------

| Date          | Version   |
| ------------- | --------- |
| July 22, 2018 | Draft     |
| TBD           | Version 1 |



License
-------

Copyright (c) 2018-2022 Karl Stenerud. All rights reserved.

Distributed under the [Creative Commons Attribution License](https://creativecommons.org/licenses/by/4.0/legalcode) ([license deed](https://creativecommons.org/licenses/by/4.0).
