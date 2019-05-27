Concise Text Encoding
=====================

A general purpose, human readable representation of semi-structured hierarchical data.

Goals
-----

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Human readable format
  * Minimal complexity
  * Type compatible with [Concise Binary Encoding (CBE)](cbe-specification.md)



Structure
---------

A CTE document is a UTF-8 encoded (with no byte order mark) text document consisting of a single, top-level object. You can store multiple objects in a document by making the top level object a container.

Whitespace is used to separate elements in a container. In maps, the key and value portions of a key-value pair are separated by an equals character `=` and possible whitespace. The key-value pairs themselves are separated by whitespace.

Example:

    {
        # A comment
        "list"        = [1 2 "a string"]
        "sub-map"     = {1="one" 2="two" 3=3000}
        "boolean"     = true
        "binary int"  = -0b10001011
        "octal int"   = 0o644
        "regular int" = -10000000
        "hex int"     = 0xfffe0001
        "float"       = 14.125
        "decimal"     = -d1.02e+40
        "time"        = 2018-07-01T10:53:22.001481Z
        "nil"         = nil
        "bytes"       = h"10 ff 38 9a dd 00 4f 4f 91"
        "url"         = u"https://example.com/"
        1             = "Keys don't have to be strings"
    }

The top-level object can also be a more simple type, such as:

    "A single string object"

or:

    30.09

Objects may be of any type described below.



Scalar Types
------------

### Boolean

Supports the values `true` and `false`.

Example:

    true
    false


### Integer

Represents two's complement signed integers up to a width of 128 bits. Negative values are prefixed with a dash `-` as a sign character.

Integers can be specified in base 2, 8, 10, or 16. For bases other than 10, integers must be prefixed with a special prefix:

| Base | Name        | Digits           | Prefix |
| ---- | ----------- | ---------------- | ------ |
|   2  | Binary      | 01               | 0b     |
|   8  | Octal       | 01234567         | 0o     |
|  10  | Decimal     | 0123456789       | (none) |
|  16  | Hexadecimal | 0123456789abcdef | 0h     |

Examples:

| Notation   | Base | Base-10 Equivalent |
| ---------- | ---- | ------------------ |
| -0b1100    |   2  | -12                |
| 0o755      |   8  | 493                |
| 900000     |  10  | 900000             |
| 0xdeadbeef |  16  | 3735928559         |

Numbers must be written in lower case.


### Floating Point

A floating point number is composed of a whole part and a fractional part, separated by a dot `.`, with an optional exponential portion. Negative values are prefixed with a dash `-`.

    1.0
    -98.413

The exponential portion is denoted by the lowercase character `e`, followed by the signed size of the base-10 exponent. Values with exponential fields must be normalized.

    6.411e+9 = 6411000000
    6.411e-9 = 0.000000006411

The maximum number of significant digits, if not defined a-priori for participating parties, is as many digits as ieee754 allows for the data type.

Numbers must be written in lower case.

There are two types of floating point numbers supported: binary and decimal.


#### Binary Floating Point

Represents ieee754 binary floating point values in widths from 32 to 128 bits.

Examples:

    1.25e+7
    -9.00001

Binary floating point values are more common, and are supported by almost all hardware, but suffer from precision loss in the fractional portion due to the values being represented internally as powers of 2. Converting base-10 fractions to base-2 causes loss of precision because not all base-10 fractions can be accurately represented as sums of base-2 fractions within the constraints of the ieee754 spec. For example, `90.1` in decimal would be somewhere around `90.0999` in binary.

Even though binary floating point values lose accuracy in certain cases, it is important to retain fidelity when transmitting such data.


#### Decimal Floating Point

Represents ieee754 decimal floating point values in widths from 32 to 128 bits. These are primarily used in financial and other applications where binary rounding errors are unacceptable.

Decimal floating point values are differentiated from binary floating point values by prefixing with `d`.

Examples:

    d12.99
    -d100.04
    d2.58411e-50

Decimal floating point values don't lose precision because they are internally represented as powers of 10, but they have slightly less range, and aren't supported in hardware on many platforms (yet).


#### Rules

**There must be at least one digit on each side of the dot character:**

| Invalid    | Valid   | Notes                                                |
| ---------- | ------- | ---------------------------------------------------- |
| -1.        | -1.0    | Or use integer value -1                              |
| .1         | 0.1     |                                                      |
| .218901e+2 | 21.8901 | Or 2.18901e+1                                        |
| -0         | -0.0    | Special case: -0 cannot be represented as an integer |

**If there is an exponential portion, there must also be a dot:**

| Invalid    | Valid    |
| ---------- | -------- |
| 1e+100     | 1.0e+100 |

**Exponential notation must be normalized (one and only one digit to the left of the dot, non-zero):**

| Invalid    | Valid      |
| ---------- | ---------- |
| 22e+50     | 2.2e+51    |
| 508.44e+10 | 5.0844e+12 |
| -1000e+5   | -1.0e+8    |
| 65.0e-20   | 6.5e-21    |
| 0.5e+10    | 5.0e+11    |


#### Infinity and Not a Number

The following are special floating point values:

 * `inf`: Infinity
 * `-inf`: Negative Infinity
 * `nan`: Not a Number (non-signaling)

Although ieee754 allows many different NaN values (signaling or non-signaling + payload), all NaN values for the purpose of this spec are considered equal and non-signaling, and must be treated as equivalent when used as map keys (meaning you cannot have multiple NAN keys in a map).

A decoder is free to decode infinity and NaN as binary or decimal floating point values.


### Numbers With Underscores

All numeric values of any type (with the exception of +- "nan" and "inf") may contain any number of underscores at any point after the first character and before the last character. Underscores are treated as a kind of "numeric whitespace", and are ignored during parsing.

Examples:

    1_000_000 = 1000000
    -_7_._4_e_+_100 = -7.4e+100
    -d_4_10.___2_2 = -d410.22

Invalid:

    _100
    _-d6.33
    50_
    n_an
    -_inf


### Time

Represents a date & time.

The date-time format is a restricted form of the [ISO 8601](https://www.iso.org/iso-8601-date-and-time-format.html) extended date format:

 * Time zone is always zero (Z).
 * Times must contain both a date and time component.
 * Only the fractional seconds field is optional. All other fields are mandatory.
 * The allowed formats are more restricted.

Like ISO 8601, the date and time components are separted by a capital `T`.

General format:

    yyyy-mm-ddThh:mm:ss.ssssssZ
    yyyy-dddThh:mm:ss.ssssssZ

This allows date/time values to be parsed by a conforming iso8601 parser.

#### Time Zone Designator

The time zone designator is always zero `Z`.

#### Date Component

Date fields are separated by dashes. Dates may be represented either in Gregorian date format `YYYY-MM-DD` or in ordinal date format `YYYY-DDD`.

To maintain compatibility with Concise Binary Encoding, only year values from -131072 (which equals -131073 after zero year adjustment) to 131071 are supported.

##### Eras and Zero Year

Years may refer to the AD or BC era. AD years have no prefix, and BC years have a dash `-` prefix.

The Anno Domini system has no zero year (there is no 0 BC or 0 AD). To maintain compatibility with the ISO 8601 format, all BC dates have an absolute year value that is one less than the BC year (thus, the year 0000 refers to 1 BC, -0001 to 2 BC, and so on). The year 0000 (1 BC) may be represented with or without a dash prefix (both 0000 and -0000 are valid). All other BC years must have a dash prefix.

##### Gregorian Date Format

    YYYY-MM-DD

| Field | Meaning       | Minimum | Maximum |
| ----- | ------------- | ------- | ------- |
| YYYY  | Year          | -131072 | 131071  |
| MM    | Month of year | 01      | 12      |
| DD    | Day of month  | 01      | 31      |

The year field must be 4 or more digits long (not including the optional dash for era), and the month and day fields must be exactly 2 digits long.

All dates in Gregorian format must use the Gregorian calendar. When outputting dates prior to October 15th, 1582, the proplectic Gregorian calendar must be used (whereby the Gregorian calendar system is applied backwards preceding its introduction). Beware: Here be dragons!

##### Ordinal Date Format

    YYYY-DDD

| Field | Meaning       | Minimum | Maximum | Notes              |
| ----- | ------------- | ------- | ------- | ------------------ |
| YYYY  | Year          | -131072 | 131071  |                    |
| DDD   | Day of year   | 001     | 366     | 366 for leap years |

The year field must be 4 or more digits long (not including the optional dash for era), and the day field must be exactly 3 digits long.

Use this format when it is undesirable to use the Gregorian date representation.


#### Time Component

The time component has mandatory hour, minute, and second fields, exactly two digits long, separated by colons.

    HH:MM:SS

A time component may also contain an optional field for fractional seconds, separated by a period `.`. The fractional second field may be from 1 to 6 digits, supporting time down to the microsecond.

    HH:MM:SS.ssssss

| Field  | Meaning           | Minimum | Maximum | Notes              |
| ------ | ----------------- | ------- | ------- | ------------------ |
| HH     | Hour of day       | 00      | 23      |                    |
| MM     | Minute of hour    | 00      | 59      |                    |
| SS     | Second of minute  | 00      | 60      | 60 for leap second |
| ssssss | Fractional second | 0       | 999999  | optional field     |


#### Examples

    2018-07-01T10:53:22.001481Z

which is equivalent to:

    2018-184T10:53:22.001481Z



Array Types
-----------

### Bytes

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian order whenever possible.

Byte array data is enclosed in double-quotes `"`, and is prefixed by the encoding type. The encoded contents may contain whitespace at any point.

Encoding Types:

| Type   | Prefix | Notes                                                     |
| ------ | ------ | --------------------------------------------------------- |
| Hex    |    h   | 1 byte represented by 2 lowercase hexadecimal characters  |
| Safe85 |    8   | [Safe85 encoding](https://github.com/kstenerud/safe-encoding/blob/master/safe85-specification.md) |

Examples:

    h"39 12 82 e1 81 39 d9 8b 39 4c 63 9d 04 8c"

    h"1 f 4 8 ae 4 56 3" (looks terrible, but is valid)

    8"8F2{*RVCLI8LDzZ!3e"

    8"CmsAT9+UpvN!1v=H_SgpMm@mDHDFy(I[~!{I@2
    yx1MU*1I[u!)NL20.1LOvFN-+cu1M_VMH_)d)HD=
    T)I6F~3Ml=.;JP_@>Ln!H$N-xV.1MUpTNKoD71L(
    nBIZop{LR-.0Nh}Y.1ML**I>@ziISc.1OfbXN"


### String

An array of UTF-8 encoded bytes, without a byte order mark (BOM). Strings must be enclosed in double-quotes `"`.

The following escape sequences are allowed inside a string's contents, and must be in lower case:

| Sequence            | Interpretation                  |
| ------------------- | ------------------------------- |
| `\\`                | literal backslash               |
| `\"`                | double quote                    |
| `\r`                | carriage return                 |
| `\n`                | linefeed                        |
| `\t`                | horizontal tab                  |
| `\x00` - `\xff`     | one octet, hexadecimal notation |
| `\u0000` - `\uffff` | unicode character               |

Example:

    "A string\twith\ttabs\nand\nnewlines"


### URI

Uniform Resource Identifier, structured in accordance with [RFC 3986](https://tools.ietf.org/html/rfc3986). URIs are enclosed in double-quotes, prefixed with a lowercase `u`.

Examples:

    u"https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top"

    u"mailto:John.Doe@example.com"

    u"urn:oasis:names:specification:docbook:dtd:xml:4.1.2"

    u"https://example.com/percent-encoding/?double-quote=%22"



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

A list begins with an opening square bracket `[`, whitespace separated contents, and finally a closing bracket `]`.

Note: While this spec allows mixed types in lists, not all languages do. Use mixed types with caution.

Example:

    [1 "two" 3.1 {} nil]


### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `nil` type. Values may be any mix of any type, including other containers. All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * 2000
 * 2000.0
 * 2000.0d

Map entries are split into key-value pairs using the equals `=` character and optional whitespace. Key-value pairs are separated from each other using whitespace. A key without a paired value is not allowed in a map.

A map begins with an opening curly brace `{`, whitespace separated key-value pairs, and finally a closing brace `}`.

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution.

Example:

    {
        1 = "alpha"
        2 = "beta"
        "a map" = {one = 1}
    }



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages may have restrictions on how and if it may be used.

Example:

    nil



Comments
--------

Comments may be placed before or after any object. Any number of comments may occur in a row. A parser is free to preserve or discard comments.

Comments must only contain printable characters, and valid UTF-8 whitespace characters that do not induce a line change (u+000a, u+000b, u+000c, u+000d, etc are not allowed).

A comment begins with a `#` character, followed by an optional space (U+0020) which is discarded if present. If multiple spaces follow the `#`, only the first is discarded. Everything else up to (but not including) the next carriage return (U+000D) or newline (U+000A), including whitespace, is preserved as-is.

(using underscore `_` to represent space U+0020)

| CTE Document    | Comment Value |
| --------------- | ------------- |
| `#`             | (empty)       |
| `#_`            | (empty)       |
| `#__`           | `_`           |
| `#A_comment`    | `A_comment`   |
| `#_A_comment`   | `A_comment`   |
| `#__A_comment_` | `_A_comment_` |

Example:

    # Comment before top level object
    {
        # Comment before the "name" object.
        # And another comment.
        "name" = "Joe Average" # Comment before the "email" object.
        "email" = # Comment before the "joe@average.org" object.
        "joe@average.org"
        "numbers" # Comment after numbers
        =
        #
        # Comment before some binary data (but not inside it)
        h"01 02 03 04 05 06 07 08 09 0a"
    }
    # Comments at the
    # end of the document.



Letter Case
-----------

A CTE document must be entirely in lower case, with the following exceptions:

 * String contents: `"A string may contain UPPER CASE. Escape sequences must be lower case: \x3d"`
 * Time values must be in upper case to conform with ISO 8601: `2018-07-01T10:53:22.001481Z`
 * Safe85 encoding, because it makes use of uppercase characters in its code table.

Everything else, including hexadecimal digits, exponents and escape sequences, must be lower case.



Whitespace
----------

Whitespace characters outside of strings and comments are ignored by a CTE parser. Any number of whitespace characters may occur in a sequence.


### Valid Whitespace Characters

While there are many characters classified as "whitespace" within the Unicode set, only the following are valid whitespace characters in a CTE document:

| Code Point | Name            |
| ---------- | --------------- |
| U+0009     | horizontal tab  |
| U+000A     | line feed       |
| U+000D     | carriage return |
| U+0020     | space           |


### Whitespace **may** occur:

 * Before an object (including at the beginning of a document)
 * After an object (including at the end of a document)
 * Between array/container openings & closings: `[`, `]`, `{`, `}`, `/`
 * Between encoding characters in a byte array.

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `h" 01 02 03   0 4 "` is equivalent to `h"01020304"`
 * `{ 1="one" 2 = "two" 3= "three" 4 ="four"}` is equivalent to `{1="one" 2="two" 3="three" 4="four"}`


### Whitespace **must** occur:

 * Between values in a list: `[12"one-two or twelve?"]` is invalid
 * Between key-value pairs in a map: `{1="one"2="two"3="three"4="four"}` is invalid.


### Whitespace **must not** occur:

 * Between a byte array encoding type and the opening double-quote: `h "` is invalid.
 * Splitting a time value: `2018-07-01 T 10:53:22.001481 Z` is invalid.
 * Splitting a numeric value: `3f h`, `9.41 d`, `3 000`, `9.3 e+3`, `- 1.0` are invalid.
 * Splitting special values: `t rue`, `ni l`, `i nf`, `n a n` are invalid.


### Whitespace is interpreted literally (not ignored) within a string or comment:

    "This string has spaces
    and a newline, which are all preserved."

    # Comment whitepsace      is preserved.


Illegal Encodings
-----------------

Illegal encodings must not be used, as they may cause problems or even API violations in certain languages. A parser may discard illegal encodings, or may even abort processing.

 * Numeric values must be representable in their respective binary formats (integer, binary float, decimal float).
 * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
 * Map keys must not be container types or the `nil` type.
 * Maps must not contain duplicate keys (this includes mathematically equivalent numeric keys).
 * Upper case text is not allowed, except as described in section [Letter Case](#letter-case).
 * Whitespace must only occur as described in section [Whitespace](#whitespace).



File Format
-----------

A CTE file is simply a file containing a single CTE document. Recall that a CTE document consists of a single top-level object, and that you can store multiple objects by making the top level object a container. CTE files should be named using the extension `cte`.

For example: File `mydata.cte`

    # This is an example CTE document.
    # Remember: Any number of comments can appear in a row.
    {
        # Here are some mapped values...
        "first" = 1
        "second" = 2
    }



Version History
---------------

July 22, 2018: Preview Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
