Concise Binary Encoding
=======================


Version
-------

Version 0 (prerelease)



This Document
-------------

This document describes the Concise Binary Encoding (CBE) format, and how codecs for this format must behave.

 * The text variant Concise Text Encoding (CTE) is described in [cte-specification.md](cte-specification.md).
 * The logical structure of Concise Encoding is described in [ce-structure.md](ce-structure.md).



Contents
--------

- [Concise Binary Encoding](#concise-binary-encoding)
  - [Version](#version)
  - [This Document](#this-document)
  - [Contents](#contents)
  - [Terms and Conventions](#terms-and-conventions)
  - [What is Concise Binary Encoding?](#what-is-concise-binary-encoding)
  - [Document Structure](#document-structure)
  - [Version Header](#version-header)
  - [Object Encoding](#object-encoding)
    - [Type Code](#type-code)
    - [Type Code (Plane 7f)](#type-code-plane-7f)
  - [Numeric Types](#numeric-types)
    - [Boolean](#boolean)
    - [Integer](#integer)
      - [Small Integer](#small-integer)
      - [Fixed Width Integer](#fixed-width-integer)
      - [Variable Width Integer](#variable-width-integer)
    - [Decimal Floating Point](#decimal-floating-point)
    - [Binary Floating Point](#binary-floating-point)
    - [UID](#uid)
  - [Temporal Types](#temporal-types)
    - [Date](#date)
    - [Time](#time)
    - [Timestamp](#timestamp)
  - [Array and String Types](#array-and-string-types)
    - [Array Elements](#array-elements)
    - [Array Forms](#array-forms)
      - [Short Form](#short-form)
      - [Chunked Form](#chunked-form)
        - [Chunk Header](#chunk-header)
        - [Bit Array Chunks](#bit-array-chunks)
        - [String Type Array Chunks](#string-type-array-chunks)
        - [Zero Chunk](#zero-chunk)
    - [Array Encoded Types](#array-encoded-types)
      - [String](#string)
      - [Resource Identifier](#resource-identifier)
      - [Bit Array](#bit-array)
      - [Media](#media)
      - [Custom Types](#custom-types)
  - [Container Types](#container-types)
    - [List](#list)
    - [Map](#map)
    - [Record](#record)
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
    - [Record Type](#record-type)
    - [Marker](#marker)
    - [Identifier](#identifier)
  - [Empty Document](#empty-document)
  - [Smallest Possible Size](#smallest-possible-size)
  - [Alignment](#alignment)
  - [Version History](#version-history)
  - [License](#license)



Terms and Conventions
---------------------

**The following bolded, capitalized terms have specific meanings in this document**:

| Term             | Meaning                                                                                                               |
| ---------------- | --------------------------------------------------------------------------------------------------------------------- |
| **MUST (NOT)**   | If this directive is not adhered to, the document or implementation is invalid.                                       |
| **SHOULD (NOT)** | Every effort should be made to follow this directive, but the document/implementation is still valid if not followed. |
| **MAY (NOT)**    | It is up to the implementation to decide whether to do something or not.                                              |
| **CAN**          | Refers to a possibility which **MUST** be accommodated by the implementation.                                         |
| **CANNOT**       | Refers to a situation which **MUST NOT** be allowed by the implementation.                                            |
| **OPTIONAL(LY)** | The implementation **MUST** support both the existence and the absence of the specified item.                         |
| **OPTION(S)**    | Configuration option(s) that implementations **MUST** provide.                                                        |

**Data descriptions and samples will generally be represented as follows**:

 * Character sequences are enclosed within backticks: `this is a character sequence`
 * Byte sequences are represented as a series of two-digit hex values, enclosed within backticks and square brackets: [`f1 33 91`]
 * Data placeholders are put `(between parentheses)`
 * Some explanations will include excerpts from [cbe.dogma](cbe.dogma) (in [Dogma notation](https://dogma-lang.org/)).



What is Concise Binary Encoding?
--------------------------------

Concise Binary Encoding (CBE) is the binary variant of [Concise Encoding](ce-structure.md): a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

The binary format aims for compactness and machine processing efficiency while maintaining 1:1 compatibility with the [text format](cte-specification.md) (which aims to present data in a human friendly way).



Document Structure
------------------

A CBE document is composed of the following parts:

 1. A [version header](#version-header)
 2. A series of optional [intangible objects](ce-structure.md#intangible-objects)
 3. A top-level [data object](ce-structure.md#data-objects)

```dogma
document = version_header
         & (padding | record_type)*
         & (data_object ! local_reference)
         ;
```



Version Header
--------------

The version header is composed of the byte [`81`], followed by an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128) encoded version number representing which version of this specification the document adheres to.

```dogma
version_header         = u8(0x81) & version;
version                = uleb128(uint(~,~));
uleb128(v: bits): bits = """https://en.wikipedia.org/wiki/LEB128#Unsigned_LEB128""";
```

**Example**:

    [81 01] = CBE version 1



Object Encoding
---------------

Object encoding in CBE follows these principles:

 * All objects are composed of a 1 or 2-byte [type code](#type-code) and a possible payload.
 * All objects end on an 8-bit boundary.
 * All variable-length discrete types begin with length fields.
 * All containers and arrays can be built incrementally (you don't need to know their final size before you start encoding their contents).
 * The most commonly used types and values encode into the smallest space while still remaining zero-copy friendly in most places on little endian systems.


### Type Code

All objects encoded in CBE begin with a type code, followed by a possible payload (depending on the type).

**Note**: Integers from -100 to 100 can be encoded directly into the type code with no payload.

| Hex | Type                                              | Payload                                  |
| --- | ------------------------------------------------- | ---------------------------------------- |
|  00 | [Integer value 0](#small-integer)                 |                                          |
|  01 | [Integer value 1](#small-integer)                 |                                          |
| ... | ...                                               |                                          |
|  64 | [Integer value 100](#small-integer)               |                                          |
|  65 | [UID](#uid)                                       | (128 bits of data, big endian)           |
|  66 | [Positive Integer](#variable-width-integer)       | (byte count) (little endian bytes)       |
|  67 | [Negative Integer](#variable-width-integer)       | (byte count) (little endian bytes)       |
|  68 | [Positive Integer (8 bit)](#fixed-width-integer)  | (8-bit unsigned integer)                 |
|  69 | [Negative Integer (8 bit)](#fixed-width-integer)  | (8-bit unsigned integer)                 |
|  6a | [Positive Integer (16 bit)](#fixed-width-integer) | (16-bit unsigned integer, little endian) |
|  6b | [Negative Integer (16 bit)](#fixed-width-integer) | (16-bit unsigned integer, little endian) |
|  6c | [Positive Integer (32 bit)](#fixed-width-integer) | (32-bit unsigned integer, little endian) |
|  6d | [Negative Integer (32 bit)](#fixed-width-integer) | (32-bit unsigned integer, little endian) |
|  6e | [Positive Integer (64 bit)](#fixed-width-integer) | (64-bit unsigned integer, little endian) |
|  6f | [Negative Integer (64 bit)](#fixed-width-integer) | (64-bit unsigned integer, little endian) |
|  70 | [Binary Float (16 bit)](#binary-floating-point)   | (16-bit [bfloat16](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format), little endian) |
|  71 | [Binary Float (32 bit)](#binary-floating-point)   | ([32-bit ieee754 binary float](https://en.wikipedia.org/wiki/Single-precision_floating-point_format), little endian) |
|  72 | [Binary Float (64 bit)](#binary-floating-point)   | ([64-bit ieee754 binary float](https://en.wikipedia.org/wiki/Double-precision_floating-point_format), little endian) |
|  73 | [RESERVED](#reserved)                             |                                          |
|  74 | [RESERVED](#reserved)                             |                                          |
|  75 | [RESERVED](#reserved)                             |                                          |
|  76 | [Decimal Float](#decimal-floating-point)          | ([compact float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)) |
|  77 | [Local Reference](#local-reference)               | (identifier)                             |
|  78 | [Boolean False](#boolean)                         |                                          |
|  79 | [Boolean True](#boolean)                          |                                          |
|  7a | [Date](#date)                                     | ([compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date))           |
|  7b | [Time](#time)                                     | ([compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time))           |
|  7c | [Timestamp](#timestamp)                           | ([compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp)) |
|  7d | [Null](#null)                                     |                                          |
|  7e | [RESERVED](#reserved)                             |                                          |
|  7f | [Plane 7f](#type-code-plane-7f)                   | (See [Plane 7f](#type-code-plane-7f))    |
|  80 | [String: 0 bytes](#string)                        |                                          |
|  81 | [String: 1 byte](#string)                         | (1 byte of UTF-8 data)                   |
|  82 | [String: 2 bytes](#string)                        | (2 bytes of UTF-8 data)                  |
|  83 | [String: 3 bytes](#string)                        | (3 bytes of UTF-8 data)                  |
|  84 | [String: 4 bytes](#string)                        | (4 bytes of UTF-8 data)                  |
|  85 | [String: 5 bytes](#string)                        | (5 bytes of UTF-8 data)                  |
|  86 | [String: 6 bytes](#string)                        | (6 bytes of UTF-8 data)                  |
|  87 | [String: 7 bytes](#string)                        | (7 bytes of UTF-8 data)                  |
|  88 | [String: 8 bytes](#string)                        | (8 bytes of UTF-8 data)                  |
|  89 | [String: 9 bytes](#string)                        | (9 bytes of UTF-8 data)                  |
|  8b | [String: 11 bytes](#string)                       | (11 bytes of UTF-8 data)                 |
|  8a | [String: 10 bytes](#string)                       | (10 bytes of UTF-8 data)                 |
|  8c | [String: 12 bytes](#string)                       | (12 bytes of UTF-8 data)                 |
|  8d | [String: 13 bytes](#string)                       | (13 bytes of UTF-8 data)                 |
|  8e | [String: 14 bytes](#string)                       | (14 bytes of UTF-8 data)                 |
|  8f | [String: 15 bytes](#string)                       | (15 bytes of UTF-8 data)                 |
|  90 | [String](#string)                                 | (chunk header) (UTF-8 data) ...          |
|  91 | [Resource Identifier](#resource-identifier)       | (chunk header) (UTF-8 data) ...          |
|  92 | [Custom Type](#custom-types)                      | (type code) (chunk header) (data) ...    |
|  93 | [Array: Unsigned Int8](#array-encoded-types)      | (chunk header) (8-bit elements) ...      |
|  94 | [Array: Bit](#bit-array)                          | (chunk header) (1-bit elements) ...      |
|  95 | [Padding](#padding)                               |                                          |
|  96 | [Record](#record)                                 | (identifier) (value) ... (end container) |
|  97 | [Edge](#edge)                                     | (source) (description) (destination) (end container) |
|  98 | [Node](#node)                                     | (value) (child node) ... (end container) |
|  99 | [Map](#map)                                       | (key, value) ... (end container)         |
|  9a | [List](#list)                                     | (value) ... (end container)              |
|  9b | [End of Container](#container-types)              |                                          |
|  9c | [Integer value -100](#small-integer)              |                                          |
| ... | ...                                               |                                          |
|  fe | [Integer value -2](#small-integer)                |                                          |
|  ff | [Integer value -1](#small-integer)                |                                          |


### Type Code (Plane 7f)

Bulkier or less common types are encoded into a secondary type plane, which adds a second byte to the type code. Most of the [array types](#array-encoded-types) encoded in this plane also include a [short form encoding](#short-form), where the length is encoded into the type field.

Types from plane 7f begin with the type code prefix [`7f`], followed by their type code in that plane. For example, the type for signed 16-bit array with 8 elements is [`7f 28`], and the type for [media](#media) is [`7f f3`].

| Hex | Type                                          | Elems | Payload                                   |
| --- | --------------------------------------------- | ----- | ----------------------------------------- |
|  00 | [Array: UID](#array-encoded-types)            |    0  | (128-bit big endian element) x0           |
| ... | ...                                           |  ...  | ...                                       |
|  0f | [Array: UID](#array-encoded-types)            |   15  | (128-bit big endian element) x15          |
|  10 | [Array: Signed Int8](#array-encoded-types)    |    0  | (8-bit element) x0                        |
| ... | ...                                           |  ...  | ...                                       |
|  1f | [Array: Signed Int8](#array-encoded-types)    |   15  | (8-bit element) x15                       |
|  20 | [Array: Unsigned Int16](#array-encoded-types) |    0  | (16-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  2f | [Array: Unsigned Int16](#array-encoded-types) |   15  | (16-bit little endian element) x15        |
|  30 | [Array: Signed Int16](#array-encoded-types)   |    0  | (16-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  3f | [Array: Signed Int16](#array-encoded-types)   |   15  | (16-bit little endian element) x15        |
|  40 | [Array: Unsigned Int32](#array-encoded-types) |    0  | (32-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  4f | [Array: Unsigned Int32](#array-encoded-types) |   15  | (32-bit little endian element) x15        |
|  50 | [Array: Signed Int32](#array-encoded-types)   |    0  | (32-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  5f | [Array: Signed Int32](#array-encoded-types)   |   15  | (32-bit little endian element) x15        |
|  60 | [Array: Unsigned Int64](#array-encoded-types) |    0  | (64-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  6f | [Array: Unsigned Int64](#array-encoded-types) |   15  | (64-bit little endian element) x15        |
|  70 | [Array: Signed Int64](#array-encoded-types)   |    0  | (64-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  7f | [Array: Signed Int64](#array-encoded-types)   |   15  | (64-bit little endian element) x15        |
|  80 | [Array: BFloat16](#array-encoded-types)       |    0  | (16-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  8f | [Array: BFloat16](#array-encoded-types)       |   15  | (16-bit little endian element) x15        |
|  90 | [Array: Binary Float32](#array-encoded-types) |    0  | (32-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  9f | [Array: Binary Float32](#array-encoded-types) |   15  | (32-bit little endian element) x15        |
|  a0 | [Array: Binary Float64](#array-encoded-types) |    0  | (64-bit little endian element) x0         |
| ... | ...                                           |  ...  | ...                                       |
|  af | [Array: Bin Float64](#array-encoded-types)    |   15  | (64-bit little endian element) x15        |
| ... | [RESERVED](#reserved)                         |       |                                           |
|  e0 | [Array: UID](#array-encoded-types)            |    ∞  | (chunk header) (128-bit B-E elements) ... |
|  e1 | [Array: Signed Int8](#array-encoded-types)    |    ∞  | (chunk header) (8-bit elements) ...       |
|  e2 | [Array: Unsigned Int16](#array-encoded-types) |    ∞  | (chunk header) (16-bit L-E elements) ...  |
|  e3 | [Array: Signed Int16](#array-encoded-types)   |    ∞  | (chunk header) (16-bit L-E elements) ...  |
|  e4 | [Array: Unsigned Int32](#array-encoded-types) |    ∞  | (chunk header) (32-bit L-E elements) ...  |
|  e5 | [Array: Signed Int32](#array-encoded-types)   |    ∞  | (chunk header) (32-bit L-E elements) ...  |
|  e6 | [Array: Unsigned Int64](#array-encoded-types) |    ∞  | (chunk header) (64-bit L-E elements) ...  |
|  e7 | [Array: Signed Int64](#array-encoded-types)   |    ∞  | (chunk header) (64-bit L-E elements) ...  |
|  e8 | [Array: BFloat16](#array-encoded-types)       |    ∞  | (chunk header) (16-bit L-E elements) ...  |
|  e9 | [Array: Binary Float32](#array-encoded-types) |    ∞  | (chunk header) (32-bit L-E elements) ...  |
|  ea | [Array: Binary Float64](#array-encoded-types) |    ∞  | (chunk header) (64-bit L-E elements) ...  |
| ... | [RESERVED](#reserved)                         |       |                                           |
|  f0 | [Marker](#marker)                             |    1  | (byte length) (UTF-8 data)                |
|  f1 | [Record Type](#record-type)                   |    ∞  | (ID) (key) ... (end container)            |
|  f2 | [Remote Reference](#remote-reference)         |    1  | (chunk header) (UTF-8 data) ...           |
|  f3 | [Media](#media)                               |    ∞  | (byte length) (UTF-8 data) (chunk header) (bytes) ... |
| ... | [RESERVED](#reserved)                         |       |                                           |



Numeric Types
-------------

### Boolean

Represents true or false.

```dogma
false = u8(0x78);
true  = u8(0x79);
```

**Examples**:

    [78] = false
    [79] = true


### Integer

CBE encoders **MUST** by default output integer values in the smallest type they'll fit into:

| Values                                     | Best Fit Type                                     |
| ------------------------------------------ | ------------------------------------------------- |
| ± `0` - `100`                              | [small integer](#small-integer)                   |
| ± `0x65` - `0xff`                          | [8-bit integer](#fixed-width-integer)             |
| ± `0x100` - `0xffff`                       | [16-bit integer](#fixed-width-integer)            |
| ± `0x10000` - `0xffffffff`                 | [32-bit integer](#fixed-width-integer)            |
| ± `0x100000000` - `0xffffffffffff`         | [variable width integer](#variable-width-integer) |
| ± `0x1000000000000` - `0xffffffffffffffff` | [64-bit integer](#fixed-width-integer)            |
| ± `0x10000000000000000` - ∞                | [variable width integer](#variable-width-integer) |

Integers are encoded in three possible ways:

#### Small Integer

Values from -100 to +100 ("small int") are encoded into the type code itself, and can be read directly as 8-bit signed two's complement integers.

```dogma
int_small = s8(-100~100);
```

#### Fixed Width Integer

Fixed width integers are stored as their absolute values in widths of 8, 16, 32, and 64 bits (in little endian byte order). The type code holds the sign of the integer.

```dogma
int_8_positive  = u8(0x68) & u8(~);
int_8_negative  = u8(0x69) & u8(~);
int_16_positive = u8(0x6a) & u16(~);
int_16_negative = u8(0x6b) & u16(~);
int_32_positive = u8(0x6c) & u32(~);
int_32_negative = u8(0x6d) & u32(~);
int_64_positive = u8(0x6e) & u64(~);
int_64_negative = u8(0x6f) & u64(~);
```

**Note**: Because the sign is encoded into the type code, it's possible to encode the value 0 with a negative sign. `-0` is not representable as an integer, and **MUST** be passed on to the application as a floating point type.

#### Variable Width Integer

Variable width integers are encoded as a block of little endian ordered bytes, prefixed with a length header. The length header is encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128), denoting how many bytes of integer data follows. The sign is encoded in the type code.

```dogma
int_vlength_positive   = u8(0x66) & var(pref,length_prefix) & ordered(uint(pref.length*8, ~));
int_vlength_negative   = u8(0x67) & var(pref,length_prefix) & ordered(uint(pref.length*8, ~));
length_prefix          = uleb(var(length, 1~));
uleb(v)                = uleb128(uint(0,v));
uleb128(v: bits): bits = """https://en.wikipedia.org/wiki/LEB128#Unsigned_LEB128""";
```

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

```dogma
decimal_float                  = u8(0x76) & compact_float(~);
compact_float(v: number): bits = """https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md""";
```

**Examples**:

    [76 07 4b] = -7.5
    [76 ac 02 d0 9e 38] = 9.21424e+80


### Binary Floating Point

Binary floating point values are stored in 32 or 64-bit [ieee754 binary floating point](https://en.wikipedia.org/wiki/IEEE_754) format, or in 16-bit [bfloat](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format) format, in little endian byte order.

```dogma
binary_float_16         = u8(0x70) & f16(~);
binary_float_32         = u8(0x71) & f32(~);
binary_float_64         = u8(0x72) & f64(~);
bfloat(v: number): bits = """https://en.wikipedia.org/wiki/Bfloat16_floating-point_format""";
```

**Examples**:

    [70 af 44] = 0x1.5ep+10
    [71 00 e2 af 44] = 0x1.5fc4p+10
    [72 00 10 b4 3a 99 8f 32 46] = 0x1.28f993ab41p+100


### UID

A unique identifier, stored according to [rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2) binary format.

**Note**: This is the only data type in CBE that is stored in **big endian** byte order ([as required by rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2)).

```dogma
uid = u8(0x65) & uint(128, ~};
```

**Example**:

    [65 12 3e 45 67 e8 9b 12 d3 a4 56 42 66 55 44 00 00] = UID 123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

Temporal types are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md) format.

**Note**: [compact time zero values](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#zero-values) are not allowed in CBE.


### Date

Dates are stored in [compact date](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date) format.

```dogma
date               = u8(0x7a) & compact_date;
compact_date: bits = """https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time""";
```

**Example**:

    [7a 56 cd 00] = Oct 22, 2051


### Time

Time values are stored in [compact time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-time) format.

```dogma
time               = u8(0x7b) & compact_time;
compact_time: bits = """https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-date""";
```

**Example**:

    [7b f7 58 74 fc f6 a7 fd 10 45 2f 42 65 72 6c 69 6e] = 13:15:59.529435422/E/Berlin


### Timestamp

Timestamps are stored in [compact timestamp](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp) format.

```dogma
timestamp               = u8(0x7c) & compact_timestamp;
compact_timestamp: bits = """https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md#compact-timestamp""";
```

**Example**:

    [7c 81 ac a0 b5 03 8f 1a ef d1] = Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array and String Types
----------------------

An array is a contiguous sequence of identically sized elements, stored in length delimited chunks. The [array type](#array-encoded-types) determines the size of each element and how the data is to be interpreted. [String types](ce-structure.md#string-types) are implemented as arrays with an element size of 1 byte.


### Array Elements

Array elements have a fixed type and size, determined by the [array type](#array-encoded-types). Length fields in array chunks represent the number of *elements*, so for example a uint32 array chunk of length 3 contains 12 bytes of array data (3 elements x 4 bytes per element), and a bit array chunk of length 10 would contain 2 bytes of array data (10 elements, 8 elements per byte, zero-padded to an 8-bit boundary).


### Array Forms

All array types have a [chunked form](#chunked-form), and many also have a [short form](#short-form).

**Examples**:

 * [`82 61 62`] = string (short form, length 2) with elements 'a', 'b'
 * [`93 04 01 02`] = unsigned 8-bit array (chunked form, length 2 - see [chunk header](#chunk-header)) with elements 1, 2.
 * [`7f 22 01 00 02 00`] = unsigned 16-bit array (plane 7f, short form, length 2) with elements 1, 2

#### Short Form

Short form arrays have their length encoded in the lower 4 bits of the type code itself in order to save space when encoding arrays with lengths from 0 to 15 elements.

**Note**: Not all array types have a short form.

```dogma
array_short_uid   = u8(0x7f) & u4(0) & array_short(uid(~));
array_short_s8    = u8(0x7f) & u4(1) & array_short(s8(~));
array_short_u16   = u8(0x7f) & u4(2) & array_short(u16(~));
array_short_s16   = u8(0x7f) & u4(3) & array_short(s16(~));
array_short_u32   = u8(0x7f) & u4(4) & array_short(u32(~));
...
array_short(type) = u4(var(count, ~)) & type{count};
```

CBE encoders **MUST** use the short form whenever it is possible to do so, unless explicitly configured to do otherwise.

**Examples**:

 * [`83 61 62 63`] = the string "abc" (short form - length is part of the type code)
 * [`90 06 61 62 63`] = the string "abc" (chunked form - length is a separate field)


#### Chunked Form

In chunked form, array data is represented as a series of chunks of data, each with its own [header](#chunk-header) containing the number of elements in the chunk and a continuation bit that tells if more chunks follow the current one.

```dogma
array_chunked_uid     = plane7f(0xe0) & array_chunk(uid(~));
array_chunked_s8      = plane7f(0xe1) & array_chunk(s8(~));
array_chunked_u16     = plane7f(0xe2) & array_chunk(u16(~));
array_chunked_s16     = plane7f(0xe3) & array_chunk(s16(~));
array_chunked_u32     = plane7f(0xe4) & array_chunk(u32(~));
...
array_chunk(type)     = var(header, array_chunk_header)
                      & type{header.count}
                      & [header.continuation = 1: array_chunk(type);]
                      ;
array_chunk_header    = uleb128(uany(var(count, ~)) & u1(var(continuation, ~)));
```

An array **CAN** contain any number of chunks, and the chunks don't have to be the same length. The most common use case would be to represent the entire array as a single chunk, but there might be cases where you need multiple chunks, such as when the array length is not known at the time when encoding has started (for example if it's being built progressively).

**Example**:

        chunk header                                 chunk header
        |                                            |
    [93 1d 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 08 01 02 03 04]
     |     |                                            |
     type  data                                         data

In this example (unsigned 8-bit int array), the first chunk of the array has 14 elements and has a continuation bit of 1 (chunk header [`1d`]). The second chunk has 4 elements and a continuation bit of 0 (chunk header [`08`]). The total length of the array is thus 18 elements, split across two chunks.

##### Chunk Header

All array chunks are preceded by an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128) encoded header containing the chunk length and a continuation bit (in the low bit of the fully decoded header). Chunk processing continues until the end of a chunk with a continuation bit of 0.

```dogma
array_chunk_header    = uleb128(uany(var(count, ~)) & u1(var(continuation, ~)));
```

**Examples**:

 * [`03`] = Chunk length 1 with the continuation bit set
 * [`80 02`] = Chunk length 256 with the continuation bit cleared
 * [`00`] = Chunk length 0 with the continuation bit cleared (terminates any array)

##### Bit Array Chunks

[Bit array](#bit-array) chunks with continuation=1 **MUST** have a length that is a multiple of 8 so that subsequent chunk data will begin on an 8-bit boundary. Only the final chunk (continuation=0) of a bit array **CAN** be of arbitrary size (the last chunk of array data will be [zero-padded to an 8-bit boundary](#bit-array)).

##### String Type Array Chunks

To ensure compatibility across all platforms, array chunks for string data (UTF-8) **MUST** always end on a character boundary (do not split multibyte characters between chunks).

##### Zero Chunk

The chunk header [`00`] indicates a chunk of length 0 with continuation 0, effectively terminating any array. It's no coincidence that 0x00 also acts as the NUL terminator for C-style strings. An encoder could use this feature to artificially NUL-terminate strings in order to create immutable-friendly zero-copy documents that support C-style string implementations.

    [90 20 m i s u n d e r s t a n d i n g ...]

vs

    [90 21 m i s u n d e r s t a n d i n g 00 ...]

This technique will only work for the general string type (0x90), not for the short string types 0x80 - 0x8f (which have no chunk headers).

**Note**: If the source buffer in your decoder is mutable, you could achieve C-style zero-copy without requiring the above technique, using a scheme whereby you pre-cache the type code of the next value, overwrite that type code's memory location in the buffer with 0 (effectively "terminating" the string), and then process the next value using the pre-cached type code:

    ...                          // buffer = [84 t e s t 6a 10 a0 ...]
    case string (length 4):      // 0x84 = string (length 4)
      cachedType = buffer[5]     // 0x6a (16-bit positive int type)
      buffer[5] = 0              // buffer = [84 t e s t 00 10 a0 ...]
      notifyString(buffer+1)     // [t e s t 00 ...] = NUL-terminated string "test"
      next(cachedType, buffer+6) // 0x6a, [10 a0 ...] = 16-bit positive int value 40976


### Array Encoded Types

The following types are encoded using [array encoding](#array-forms):

| Array Type                                                                                   | Element Size (bits) | Byte Order    | Type Codes      |
| -------------------------------------------------------------------------------------------- | ------------------- | ------------- | --------------- |
| Unsigned int                                                                                 | 8                   | -             | 93              |
| Unsigned int                                                                                 | 16                  | Little Endian | 7f:20-2f, 7f:e2 |
| Unsigned int                                                                                 | 32                  | Little Endian | 7f:40-4f, 7f:e4 |
| Unsigned int                                                                                 | 64                  | Little Endian | 7f:60-6f, 7f:e6 |
| 2's complement signed int                                                                    | 8                   | -             | 7f:10-1f, 7f:e1 |
| 2's complement signed int                                                                    | 16                  | Little Endian | 7f:30-3f, 7f:e3 |
| 2's complement signed int                                                                    | 32                  | Little Endian | 7f:50-5f, 7f:e5 |
| 2's complement signed int                                                                    | 64                  | Little Endian | 7f:70-7f, 7f:e6 |
| [Bfloat16](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)                     | 16                  | Little Endian | 7f:80-8f, 7f:e8 |
| [IEEE754 binary float](https://en.wikipedia.org/wiki/Single-precision_floating-point_format) | 32                  | Little Endian | 7f:90-9f, 7f:e9 |
| [IEEE754 binary float](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) | 64                  | Little Endian | 7f:a0-af, 7f:ea |
| [RFC4122 UUID](https://tools.ietf.org/html/rfc4122#section-4.1.2)                            | 128                 | Big Endian    | 7f:00-0f, 7f:e0 |
| [String](#string)                                                                            | 8                   | -             | 80-8f, 90       |
| [Resource ID](#resource-identifier)                                                          | 8                   | -             | 91              |
| [Bit](#bit-array)                                                                            | 1                   | Little Endian | 94              |
| [Media](#media)                                                                              | 8                   | -             | 7f:f3           |
| [Custom Type](#custom-types)                                                                 | 8                   | -             | 92              |

See [cbe.dogma](cbe.dogma) for complete array encoding descriptions.

#### String

Strings are encoded as UTF-8.

The chunked string encoding form is:

```dogma
string_chunked     = u8(0x90) & string_chunk;
string_chunk       = var(header, array_chunk_header)
                   & sized(header.count*8, char_string*)
                   & [header.continuation = 1: string_chunk;]
                   ;
array_chunk_header = uleb128(unsigned(0,var(count, ~)) & u1(var(continuation, ~));
char_string        = unicode(C,L,M,N,P,S,Z);
```

Strings also have a [short form](#short-form) length encoding using types 0x80-0x8f:

```dogma
string_short = u4(8) & u4(var(count, ~)) & sized(count*8, char_string*);
```

**Examples**:

    [8b 4d 61 69 6e 20 53 74 72 65 65 74] = Main Street
    [8d 52 c3 b6 64 65 6c 73 74 72 61 c3 9f 65] = Rödelstraße
    [90 2a e8 a6 9a e7 8e 8b e5 b1 b1 e3 80 80 e6 97 a5 e6 b3 b0 e5 af ba] = 覚王山　日泰寺

#### Resource Identifier

Resource identifiers are encoded similarly to a long-form [string](#string), but with type [`91`].

```dogma
resource_id = u8(0x91) & string_chunk;
```

**Example**:

    [91 aa 01 68 74 74 70 73 3a 2f 2f 6a 6f 68 6e 2e 64 6f 65 40 77 77 77
     2e 65 78 61 6d 70 6c 65 2e 63 6f 6d 3a 31 32 33 2f 66 6f 72 75 6d 2f
     71 75 65 73 74 69 6f 6e 73 2f 3f 74 61 67 3d 6e 65 74 77 6f 72 6b 69
     6e 67 26 6f 72 64 65 72 3d 6e 65 77 65 73 74 23 74 6f 70]
    = https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top

#### Bit Array

Bit array elements are stored in little endian bit order (the first element is stored in the least significant bit of the first byte of the encoded array). Array chunks **MUST** have a length such that `length % 8 == 0` (failure to do so is an [error condition](ce-structure.md#error-processing)), except for the last [chunk](#bit-array-chunks) which can have any length. Unused trailing (upper) bits in the last [chunk](#bit-array-chunks) **MUST** be cleared to 0 by an encoder, and **MUST** be discarded by a decoder.

```dogma
array_bit            = u8(0x94) & array_bit_chunk* array_bit_chunk_last;
array_bit_chunk      = uleb128(uany(var(count, mult_8(~))) & u1(1))
                     & reversed(1, u1(~){count})
                     ;
array_bit_chunk_last = uleb128(uany(var(count, ~)) & u1(0))
                     & aligned(8, reversed(1, u1(~){count}), u1(0)*)
                     ;
mult_8(v)            = [v%8 = 0: v;];
```

For example, the bit array `{0,0,1,1,1,0,0,0,0,1,0,1,1,1,1}` would encode to [`1c 7a`] with a length of `15`. The encoded value can be directly read on little endian architectures into the multi-byte unsigned integer value `0b111101000011100` (`0x7a1c`), such that the least significant bit of the unsigned integer representation is the first element of the array.

**Example**:

    [94 16 76 06] = bit array {0,1,1,0,1,1,1,0,0,1,1}

#### Media

A media object has type [`7f f3`] and is composed of a length-prefixed [media type](http://www.iana.org/assignments/media-types/media-types.xhtml), followed by a byte array containing the media data.

```dogma
media            = plane7f(0xf3)
                 & var(pref,length_prefix)
                 & sized(pref.length*8, media_type)
                 & array_chunk(u8(~))
                 ;
media_type       = media_type_word & '/' & media_type_word;
media_type_word  = char_media_first & char_media*;
char_media_first = 'a'~'z' | 'A'~'Z';
char_media       = ('!' ~ '~')! ( '(' | ')' | '<' | '>'
                                | '@' | ',' | ';' | ':'
                                | '\\' | '"' | '/' | '['
                                | ']' | '?' | '='
                                )
                 ;
```

**Example**:

     *1 *2 *3 *4                                              *5
    [7f f3 10 61 70 70 6c 69 63 61 74 69 6f 6e 2f 78 2d 73 68 38 

     *6
     23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a]

Points of interest:

| Point | Description                                     |
| ----- | ----------------------------------------------- |
|  *1   | Primary type: 0x7f = Plane 7f                   |
|  *2   | Plane 7f subtype: 0xf3 = Media                  |
|  *3   | Media Type length: 0x10 = 16 bytes              |
|  *4   | String Data: `application/x-sh`                 |
|  *5   | Chunk Header: 0x38 = length 28, no continuation |
|  *6   | Media bytes                                     |

The media in this example is the shell script (media type "application/x-sh"):

```sh
#!/bin/sh

echo hello world
```

#### Custom Types

Custom type values have type code [`92`], followed by a custom type code (encoded as an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128)), followed by a byte array containing the custom data.

```dogma
custom_type      = u8(0x92) & custom_type_code & array_chunk(u8(~));
custom_type_code = uleb(~);
```

**Note**: Custom data in [text form](ce-structure.md#custom-type-forms) **MUST** be converted to binary form before being encoded into CBE, as CBE does **not** support the text form.

**Example**: a fictional "complex number" custom type with real and imaginary components represented using float32, assigned to custom type code 1

      {
          real:      float32 = 2.94 (40 3c 28 f6)
          imaginary: float32 = 3.0  (40 40 00 00)
      }

Encoded as a custom type (note: the multi-byte values are encoded in little endian byte order):

     *1 *2 *3 *4          *5
    [92 01 10 f6 28 3c 40 00 00 40 40]

Points of interest:

| Point | Description                                                  |
| ----- | ------------------------------------------------------------ |
|  *1   | Primary type: 0x92 = Custom                                  |
|  *2   | Custom type: 0x01                                            |
|  *3   | Chunk Header: 0x10 = length 8, no continuation               |
|  *4   | Custom data (the first 4 bytes contain the "real" portion)   |
|  *5   | The "imaginary" portion of our fictional complex number type |



Container Types
---------------

### List

A list has type code [`9a`], followed by a series of zero or more objects, and is terminated with [`9b`] (end of container).

```dogma
list          = u8(0x9a) & data_object* & end_container;
end_container = u8(0x9b);
```

**Example**:

    [9a 01 6a 88 13 9b] = A list containing integers (1, 5000)


### Map

A map has type code [`99`], followed by a series of zero or more key-value pairs, and is terminated with [`9b`] (end of container).

```dogma
map           = u8(0x99) & key_value* & end_container;
key_value     = keyable_object & data_object;
end_container = u8(0x9b);
```

**Example**:

    [99 81 61 01 81 62 02 9b] = A map containing the key-value pairs ("a" = 1) ("b" = 2)


### Record

A record has type code [`96`], followed by an [identifier](#identifier), followed by a series of values matching the order that the keys are defined in the associated [record type](#record-type), and is terminated with [`9b`] (end of container).

```dogma
record        = u8(0x96) & identifier & data_object* & end_container;
end_container = u8(0x9b);
```

**Example**:

A record built from the record type identified by "a" (which must be defined at the top of the document), with the first key's associated value set to 5:

    [96 01 61 05 9b]


### Edge

An edge has type code [`97`], followed by a source object, then a description object, then a destination object, and is terminated with [`9b`] (end of container).

```dogma
edge          = u8(0x97) & non_null_object & data_object & non_null_object & end_container;
end_container = u8(0x9b);
```

**Example**:

    [97
     91 24 68 74 74 70 3a 2f 2f 73 2e 67 6f 76 2f 68 6f 6d 65 72
     91 22 68 74 74 70 3a 2f 2f 65 2e 6f 72 67 2f 77 69 66 65
     91 24 68 74 74 70 3a 2f 2f 73 2e 67 6f 76 2f 6d 61 72 67 65
     9b]
    = the relationship graph: @(@"http://s.gov/homer" @"http://e.org/wife" @"http://s.gov/marge")


### Node

A node has type code [`98`], followed by a value object and zero or more child nodes, and is terminated with [`9b`] (end of container).

```dogma
node          = u8(0x98) & data_object & (node | data_object)* & end_container;
end_container = u8(0x9b);
```

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

```dogma
null = u8(0x7d);
```


### RESERVED

This type is reserved for future expansion of the format, and **MUST NOT** be used. If a decoder encounters a reserved type code, it is an [error condition](ce-structure.md#error-processing).



Peudo-Objects
-------------

### Local Reference

A local reference has type code [`0x77`], followed by a marker [identifier](#identifier).

```dogma
local_reference = u8(0x77) & identifier;
```

**Examples**:

    [77 01 61] = reference to the object marked with ID "a"

### Remote Reference

A remote reference is encoded in the same manner as a [resource identifier](#resource-identifier), except with a different type code ([`7f f2`]).

```dogma
remote_reference = plane7f(0xf2) & string_chunk;
```

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

Padding is encoded as type [`95`]. Repeat as many times as needed.

```dogma
padding = u8(0x95);
```

**Example**:

    [95 95 95 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.



Structural Objects
------------------

### Record Type

A record type has type code [`7f f1`], followed by an [identifier](#identifier), followed by a series of keys, and is terminated with [`9b`] (end of container).

```dogma
record_type   = plane7f(0xf1) & identifier & keyable_object* & end_container;
end_container = u8(0x9b);
```

**Example**:

A record type named "a", containing the key "b":

    [7f f1 01 61 81 62 9b]


### Marker

A marker has type code [`7f f0`], followed by a marker [identifier](#identifier), and then the marked object.

```dogma
marked_object(type) = plane7f(0xf0) & identifier & type;
```

**Example**:

    [7f f0 01 61
     99
     8a 73 6f 6d 65 5f 76 61 6c 75 65
     90 22 72 65 70 65 61 74 20 74 68 69 73 20 76 61 6c 75 65
     9b]
    = the map {"some_value" = "repeat this value"}, tagged with the ID "a".


### Identifier

Identifiers begin with an [unsigned LEB128](https://en.wikipedia.org/wiki/LEB128) length field (min length 1 byte), followed by that many **bytes** of UTF-8 data.

The length field **CANNOT** be 0.

```dogma
identifier      = var(pref,length_prefix) & sized(pref.length*8, char_identifier*);
length_prefix   = uleb(var(length, 1~));
char_identifier = unicode(Cf,L,M,N) | '_' | '.' | '-';
```

**Note**: Identifiers are **not** standalone objects; they are always part of another object.

**Examples**:

    [07 73 6f 6d 65 5f 69 64] = some_id
    [0f e7 99 bb e9 8c b2 e6 b8 88 e3 81 bf ef bc 95] = 登録済み５



Empty Document
--------------

An empty document in CBE is signified by using [null](#null) as the top-level object:

    [81 01 7d]



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders **SHOULD** use the smallest encoding that stores a value without data loss.

Specialized applications **MAY** wish to preserve more numeric type information to distinguish floats from integers, or even to distinguish between data sizes. This is allowed, as it will make no difference to a generic decoder (although it will bloat the document).



Alignment
---------

Applications might require data to be aligned in some cases for optimal decoding performance. For example, some processors might not be able to read unaligned multi-byte data types without special (costly) intervention. An encoder could in theory be tuned to insert [padding](#padding) when encoding certain data, trading document size for encoding/decoding efficiency:

|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
| -- | -- | -- | -- | -- | -- | -- | -- |
| 95 | 95 | 95 | 67 | 00 | 00 | 00 | 8f |

Alignment tuning is usually only useful when the target decoding environment is known prior to encoding (It's mostly an optimization for closed systems).



Version History
---------------

| Date          | Version   |
| ------------- | --------- |
| July 22, 2018 | Draft     |
| TBD           | Version 1 |



License
-------

Copyright (c) 2018-2023 Karl Stenerud. All rights reserved.

Distributed under the [Creative Commons Attribution License](https://creativecommons.org/licenses/by/4.0/legalcode) ([license deed](https://creativecommons.org/licenses/by/4.0).
