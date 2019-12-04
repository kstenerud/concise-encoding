Concise Binary Encoding
=======================

Concise Binary Encoding (CBE) is a general purpose, machine-readable, compact binary representation of semi-structured hierarchical data. CBE is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a power and bandwidth friendly way:

 * There are two formats: [binary-based CBE](cbe-specification.md) and [text-based CTE](cte-specification.md).
 * 1:1 type compatibility between formats. Use the more efficient binary format for data interchange and storage, and transparently convert to/from text only when a human needs to be involved.
 * Documents and specifications are versioned to support future expansion.
 * Supports metadata and comments.
 * Supports references to other parts of the document or to other documents.
 * Supports the most commonly used data types:
   - Nil
   - Boolean
   - Integer
   - Float
   - Time
   - String
   - URI
   - Bytes
   - List
   - Map
   - Reference
   - Metadata
   - Comment



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
  - [Markup](#markup)
    - [Container Name](#markup-container-name)
    - [Attributes Section](#attributes-section)
    - [Contents Section](#contents-section)
    - [Content String](#content-string)
    - [Verbatim Sequence](#verbatim-sequence)
    - [Escape Sequence](#escape-sequence)
    - [Entity Reference](#entity-reference)
    - [Doctype](#doctype)
    - [Style Sheet](#style-sheet)
    - [Markup Comment](#markup-comment)
  - [Inline Containers](#inline-containers)
* [Metadata](#metadata)
  - [Metadata Association](#metadata-association)
  - [Metadata Map](#metadata-map)
  - [Comment](#comment)
    - [Comment Character Restrictions](#comment-character-restrictions)
* [Other Types](#other-types)
  - [Nil](#nil)
  - [Marker](#marker)
    - [Tag Value](#tag-value)
  - [Reference](#reference)
  - [Padding](#padding)
  - [RESERVED](#reserved)
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

A CBE document is a binary encoded document consisting of a version specifier, followed by a single, top-level object of any type. To store multiple values in a CBE document, use a container as the top-level object and store other objects within that container.

    [version specifier] [object]


### Version Specifier

The version specifier is an unsigned [RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md) with a value greater than 0, representing which version of this specification it adheres to.

The version specifier is mandatory.

Note: Because CBE places the version as the first byte in a document, the version value 0x76 is invalid. If 0x76 were allowed, it would clash with CTE when differentiating the file type by its contents because CTE uses `v` (0x76) as its first byte.


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
|  76 | 118 | Comment                   | (String or sub-comment) ... End of Container  |
|  77 | 119 | Metadata Map              | Key Value ... End of Container                |
|  78 | 120 | Markup                    | Name, kv-pairs, contents                      |
|  79 | 121 | Map                       | Key Value ... End of Container                |
|  7a | 122 | List                      | Object ... End of Container                   |
|  7b | 123 | End of Container          |                                               |
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
|  92 | 146 | URI                       | [byte length] [[URI](https://tools.ietf.org/html/rfc3986)] |
|  93 | 147 | RESERVED                  |                                               |
|  94 | 148 | RESERVED                  |                                               |
|  95 | 149 | RESERVED                  |                                               |
|  96 | 150 | RESERVED                  |                                               |
|  97 | 151 | Marker                    | Positive Integer / dequotable string          |
|  98 | 152 | Reference                 | Positive Integer / dequotable string / URI    |
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

    [9b 40 56 d0 0a 3a 8f 1a ef d1] = Oct 26, 1985 1:22:16 at location 33.99, -117.93



Array Types
-----------

An "array" for the purposes of this spec is a contiguous sequence of octets, preceded by a length field. The array type determines how those octets are to be interpreted.


### Array Length Field

All arrays are preceded by an array length field, representing the number of octets in the array.

The array length field is encoded as an [RVLQ](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md).


### String

Strings are specialized byte arrays, containing the UTF-8 representation of a string. Strings must not contain a byte order mark (u+feff) or the NUL (u+0000) character.

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

    [7a 01 6a 88 13 7b] = A list containing integers (1, 5000)


### Map

A map associates objects (keys) with other objects (values). Keys can be any mix of scalar or array types. A key must not be a container type, the `nil` type, or a NaN (not-a-number) value. Values can be any mix of any type, including other containers.

A map is ordered by default unless otherwise negotiated between parties (for example via a schema), or the user has specified that order doesn't matter.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash, and are therefore invalid:

 * 2000 (16-bit integer)
 * 2000 (32-bit integer)
 * 2000.0 (decimal float)

Map contents are stored as key-value pairs of objects:

    [79] [key 1] [value 1] [key 2] [value 2] ... [7b]

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution. A decoder may abort processing or ignore key-value pairs of mixed key types if the implementation language doesn't support it.

Example:

    [79 81 61 01 81 62 02 7b] = A map containg the key-value pairs ("a", 1) ("b", 2)


### Markup

A markup container stores XML-style data, which is essentially a map of attributes, followed by a list of contents.

Markup containers are best suited to presentation. For regular data, maps and lists are better.

Unlike other containers, a markup container requires two end-of-container markers: one to mark the end of the attributes, and another one to mark the end of the contents section:

    [78] [name] [attr1] [v1] [attr2] [v2] ... [7b] [contents] [contents] ... [7b]

#### Markup Container Name

Although technically any type is allowed in this field, be aware that XML and HTML have restrictions on what they allow.

#### Attributes Section

The attributes section behaves like a [map](#map). Be aware that XML and HTML have restrictions on what they allow in these fields.

#### Contents Section

The contents section behaves similarly to a [list](#list), except that it can only contain:

 * [Content strings](#content-string)
 * [Comments](#comment)
 * Other markup containers

#### Content String

A content string is encoded as a [string](#string), with additional processing requirements and restrictions in order to maintain compatibility with [CTE](cte-specification.md#markup):

 * An unescaped backtick (`` ` ``) character initiates a [verbatim sequence](#verbatim-sequence).
 * An unescaped backslash (`\`) character initiates an [escape sequence](#escape-sequence).
 * The sequences `<*` and `*>` must not be present unescaped (they must be escaped to `\<*` and `*\>`).

##### Verbatim Sequence

A verbatim sequences is a section of a string that must not be interpreted in any way (no special interpretation of whitespace, character sequences, escape sequences, backticks etc) until the specified end sequence is encountered. The contents must be a valid [string](#string).

A verbatim sequence is composed of the following:

 * Backtick (`` ` ``).
 * An end-of-string identifier, which is a sequence of printable, non-whitespace characters (in accordance with [human editability](cte-specification.md#human-editability)).
 * A single whitespace sequence to terminate the end-of-string identifier (either: SPACE `u+0020`, TAB `u+0009`, LF `u+000a`, or CR+LF `u+000d u+000a`).
 * The string contents.
 * A second instance of the end-of-string identifier (without whitespace termination).

Example:

```
discussion = `:::
A verbatim string is not constrained like normal strings are.
It can contain problematic characters like ", `, \ and such without problems.

The `\` at the end of this line is not a continuation: \

Three colons (`:`) are being used to mark the end-of-string in this case,
so we can't use that exact character sequence in the string contents.

Whitespace, including newlines and "leading" whitespace, is also read verbatim.
        For example, this line really is indented 8 spaces.:::
```

##### Escape Sequence

An escape sequence initiates special processing to allow specifying characters or sequences that would otherwise not be possible.

The following escape sequences are valid within a content string:

| Sequence                | Interpretation              |
| ----------------------- | --------------------------- |
| `` \` ``                | backtick (u+0060)           |
| `\<`                    | less-than (u+003c)          |
| `\>`                    | greater-than (u+003e)       |
| `\\`                    | backslash (u+005c)          |
| `\_`                    | non-breaking space (u+00a0) |
| `\u0001` - `\uffff`     | unicode character           |
| `\` + entity name + `;` | entity reference            |

A decoder must interpret escape sequences and pass the translated value to the application.

For entity references, a decoder must only validate the format (starts with a backslash, ends with a semicolon, name is valid). The entire entity reference sequence (including `\` and `;`) must be passed unchanged to the application.

##### Entity Reference

Entity references are the same as in XML and HTML, except that the backslash (`\`) is used instead of the ampersand (`&`) to initiate a reference (e.g. `\gt;` instead of `&gt;`).

Because the list of allowable entity references in XML and HTML can change independently of this specification, a codec must not interpret entities. Rather, it must pass them unchanged so that the application can deal with them according to whichever spec it adheres to.

#### Doctype

Use a [metadata map](#metadata-map) entry to specify a doctype:

    [77 8c] "xml-doctype" [7a 84] "html" [86] "PUBLIC" [90 20] "-//W3C//DTD XHTML 1.0 Strict//EN" [92 31] "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd" [7b 7b]

#### Style Sheet

Use a [metadata map](#metadata-map) entry to specify an XML style sheet:

    [77 8f] "xml-stylesheet" [7a 84] "type" [88] "text/xsl" [84] "href" [90 10] "stylesheet.xsl" [7b 7b]

#### Markup Comment

Strings within a comment in a markup contents section have the requirements and restrictions of both [markup content strings](#content-string) and [comment strings](#comment-string-character-restrictions).


### Inline Containers

CBE documents in data communication messages are often implemented as lists or maps at the top level. To help save bytes, CBE allows "inline" top-level list and map containers as a special case.

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


### Metadata Map

A metadata map contains keyed values which are associated with the object that follows the metadata map.

Keys in metadata maps follow the same rules as for regular maps, except that all string typed keys beginning with the underscore `_` character are reserved for predefined keys, and must only be used in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).

Implementations should make use of the predefined metadata keys whenever possible to maximize interoperability between systems.

Example:

    [77 82 5f 74 85 61 5f 74 61 67 7b] = metadata map: (_t = ["a_tag"])


### Comment

A comment is a specialized list container that can only contain strings or other comment containers (to support nested comments). Comments are user-defined string metadata equivalent to comments in a source code document. Comments do not officially refer to other objects, although conventionally they tend to refer to what follows in the document, be it a single object, a series of objects, a distant object, or they might even be entirely standalone. This is similar to how source code comments are used.

Strings inside comment containers have additional restrictions:

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

    [76 93 40 42 75 67 20 23 39 35 35 31 32 3a 20 53 79 73 74 65 6d 20 66
     61 69 6c 73 20 74 6f 20 73 74 61 72 74 20 6f 6e 20 61 72 6d 36 34 20
     75 6e 6c 65 73 73 20 42 20 6c 61 74 63 68 20 69 73 20 73 65 74 7b]
    = Bug #95512: System fails to start on arm64 unless B latch is set



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages might have restrictions on how and if it can be used.

Example:

    [7e] = No data


### Marker

A marker tags an object in the document with a [tag value](#tag-value) such that it can be referenced in another part of the document. The next object following the marker is associated with the marker's tag value. Markers are not objects themselves, and are for all intents and purposes invisible to all other objects (they don't count as values in collections, for example).

A marker begins with the marker type (0x79), followed by a [tag value](#tag-value).

Rules:

 * A marker cannot mark a comment; instead it would mark the next non-comment object.
 * A marker must not be followed by another marker (even with comments in between); markers must reference objects, not other markers.
 * Marker tags are globally unique to the document; duplicate tags are invalid.

Example:

    [97 01 79 8a 73 6f 6d 65 5f 76 61 6c 75 65 90 11 72
     65 70 65 61 74 20 74 68 69 73 20 76 61 6c 75 65 7b]
    = the map {some_value = "repeat this value"}, tagged with integer 1

#### Tag Value

A tag value is a globally unique (to the document) identifier for marked objects. A tag value can be either a positive integer or a dequotable string. A dequotable string is a string that can be used in a Concise Text Encoding document with double-quotes omitted (see: [Unquoted String](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md#unquoted-string)):

 * The string does not begin with a character from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, and underscore (`_`).
 * The string does not contain characters from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, numerals 0-9, underscore (`_`), and dash (`-`).
 * The string does not contain unicode characters or sequences that would be mistaken by a human reader for symbol characters in the u+0000 to u+007f range (``!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~``).
 * The string does not contain escape sequences or whitespace or line breaks or unprintable characters.
 * The string is not empty (`""`).


### Reference

A reference is a shorthand used in place of an actual object to indicate that it is the same object as the one marked with the given tag value (it's much like a pointer, with the tag value acting as a labeled address). References can be useful for keeping the size down when there is repeating information in your document, or for following DRY principles in a configuration document. One could also use URI references as an include mechanism, whereby parts of a document are stored in separate locations.

A reference begins with the reference type (0x7a), followed by either a [tag value](#tag-value) or a [URI](#uri).

Rules:

 * A reference with a [tag value](#tag-value) must reference another object in the same document (local reference).
 * Forward references within a document are allowed. An implementation must keep track of unresolved local references, and resolve them as the markers are decoded.
 * A fully decoded document with unresolved local references is invalid.
 * Recursive references are allowed.
 * A reference with a URI must point to:
   - Another CBE or CTE document (using no fragment section, thus referring to the entire document)
   - A tag value inside another CBE or CTE document, using the fragment section of the URI as a tag identifier
 * An implementation may choose to follow URI references, but care must be taken when doing this, as there are security implications when following unknown links.
 * An implementation may also choose to simply pass along a URI as-is, leaving it up to the user to resolve it or not.
 * References to dead or invalid URI links are not considered invalid per se. How this situation is handled is implementation specific, and should be fully specified in the implementation of your use case.

Examples:

    [98 01] = reference to the object tagged with the integer value 1

    [98 81 61] = reference to the object tagged with the string value "a"

    [98 92 12 63 6f 6d 6d 6f 6e 2e 63 65 23 6c 65 67 61 6c 65 73 65]
    = reference to relative file "common.ce", tag "legalese"

    [98 92 31 68 74 74 70 73 3a 2f 2f 73 6f 6d 65 77
     68 65 72 65 2e 63 6f 6d 2f 6d 79 5f 64 6f 63 75
     6d 65 6e 74 2e 63 62 65 3f 66 6f 72 6d 61 74 3d
     6c 6f 6e 67]
    = reference to entire document at https://some-external-place/my_document.cbe?format=long


### Padding

The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur any number of times before a type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Example:

    [7f 7f 7f 6c 00 00 00 8f] = 0x8f000000, padded such that the 32-bit integer begins on a 4-byte boundary.

Technically, padding could also be used as a sequence point in a CBE-style stream to help synchronize data on a noisy channel (for example, data chunk, 4x padding, data chunk, etc), but streaming is outside of the scope of this document.


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
 * Metadata map keys beginning with `_` must not be used, except in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).



Smallest Possible Size
----------------------

Preservation of the original numeric data type information is not considered important by default. Encoders should use the smallest type that stores a value without data loss.

For specialized applications, an encoder implementation may choose to preserve larger types and widths as a tradeoff in processing cost vs data size, or to preserve type information.



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
