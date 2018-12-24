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

Whitespace is used to separate elements in a container. Maps separate keys and values using a colon `:`, and key-value pairs using whitespace.

Example:

    {
        # A comment
        "list":        [1 2 "a string"]
        "sub-map":     {1: "one" 2: "two" 3: 3000}
        "boolean":     true
        "binary int":  -10001011b
        "octal int":   644o
        "regular int": -10000000
        "hex int":     fffe0001h
        "float":       14.125
        "decimal":     -1.02d
        "time":        2018-07-01T10:53:22.001481Z
        "empty":       empty
        "bytes":       b(10ff389add004f4f91)
    }

The top-level object can also be a more simple type, such as:

    "A single string object"

or:

    30.09

Objects may be of any type described below.



Scalar Types
------------

### Boolean

Supports the values `true` and `false`, and their aliases `t` and `f`.

Example:

    true
    false
    t
    f


### Integer

Represents two's complement signed integers up to a width of 128 bits. Negative values are prefixed with a dash `-`.

Integers can be specified in base 2, 8, 10, or 16. A suffix character determines which base to interpret as:

| Base | Name        | Suffix |
| ---- | ----------- | ------ |
|   2  | Binary      | b      |
|   8  | Octal       | o      |
|  10  | Decimal     | (none) |
|  16  | Hexadecimal | h      |

Examples:

| Notation  | Base | Decimal Integer Value |
| --------- | ---- | --------------------- |
| 900000    |  10  | 900000                |
| -1100b    |   2  | -12                   |
| 755o      |   8  | 493                   |
| deadbeefh |  16  | 3735928559            |

Numbers must be written in lower case.


### Binary Floating Point

Represents ieee754 binary floating point values in widths from 32 to 128 bits. Supports exponential notation using `e`. Negative values are prefixed with a dash `-`, and the radix character is `.`. Other allowable values are `nan` (not a number), and positive & negative infinity (`inf`, `-inf`).

Examples:

    1.25e+7
    -9.00001
    nan
    inf
    -inf

Numbers must be written in lower case.


### Decimal Floating Point

Represents ieee754 decimal floating point values in widths from 32 to 128 bits. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary. Supports exponential notation using `e`. Negative values are prefixed with a dash `-`, and the radix character is `.`. Other allowable values are `nand` (not a number), and positive & negative infinity (`infd`, `-infd`).

Decimal floating point values are differentiated from binary floating point values by adding a `d` suffix.

Examples:

    12.99d
    -100.04d
    nand
    infd
    -infd

Numbers must be written in lower case.


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

### Binary Data

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. Contents may be represented as hexadeximal digits or using base64 encoding. To reduce cross-platform confusion, multibyte data types stored within the binary blob should be represented in little endian order whenever possible.

Binary data begins with an encoding prefix, an opening parenthesis `(`, contents, and finally a closing parenthesis `)`. The contents may be broken up anywhere with whitespace.

The encoding prefix determines the data encoding:

| Type | Encoding                              |
| ---- | ------------------------------------- |
|   h  | hexadecimal (1 byte per 2 characters) |
|   b  | base64 (3 bytes per 4 characters)     |

For this specification, base64 is as described in [RFC 4648](https://tools.ietf.org/html/rfc4648#section-4) (i.e. using `+` and `/`, for characters 62 and 63, and `=` as padding).

For encoding `h`, hexadecimal characters `a`-`f` must be in lowercase.


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



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

A list begins with an opening square bracket `[`, whitespace separated contents, and finally a closing bracket `]`.

Example:

    [1 "two" 3.1 {} empty]


### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types. A key must not be a container type or the `empty` type. Values may be any mix of any type, including other containers. All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * 2000
 * 2000.0
 * 2000.0d

Map entries are split into key-value pairs using the colon `:` character and optional whitespace. Key-value pairs are separated from each other using whitespace. A key without a paired value is not allowed.

A map begins with an opening curly brace `{`, whitespace separated key-value pairs, and finally a closing brace `}`.

Example:

    {
        1: "alpha"
        2: "beta"
        "a map": {one: 1}
    }



Other Types
-----------

### Empty

Denotes the absence of data. Some languages implement this as the "null" value. you may also use its alias `e`.

Use `empty` with care, as some languages have restrictions on how it may be used in data structures.

Example:

    empty
    e



Comments
--------

Comments may be placed before or after any object. Any number of comments may occur in a row. A parser is free to discard comments.

A comment begins with a `#` character and terminates on the next newline character (U+000A).

Example:

    # Comment before top level object
    {
        # Comment before the "name" object.
        # And another comment.
        "name": "Joe Average" # Comment before the "email" object.
        "email": # Comment before the "joe@average.org" object.
        "joe@average.org"
        "numbers" # after numbers
        :
        # Before binary data (but not inside it)
        h(01 02 03 04 05 06 07 08 09 0a)
    }
    # Comments at the
    # end of the document.



Letter Case
-----------

A CTE document must be entirely in lower case, except for the following:

 * String contents: `"A string may contain UPPER CASE. Escape sequences must be lower case: \x3d"`
 * Time values must be in upper case to conform with ISO 8601: `2018-07-01T10:53:22.001481Z`
 * base64 encoding makes use of uppercase characters in its code table.

Everything else, including hexadecimal digits and escape sequences, must be lower case.



Whitespace
----------

Whitespace characters are ignored by a CTE parser. Any number of whitespace characters may occur in a sequence.


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
 * Between array/container openings & closings: `[`, `]`, `{`, `}`, `(`, `)`
 * Between encoding characters in a binary data array (both base64 and hex).

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `h( 01 02 03   0 4 )` is equivalent to `h(01020304)`
 * `{ 1:"one" 2 : "two" 3: "three" 4 :"four"}` is equivalent to `{1:"one" 2:"two" 3:"three" 4:"four"}`


### Whitespace **must** occur:

 * Between key-value pairs in a map: `{1:"one"2:"two"3:"three"4:"four"}` is invalid.


### Whitespace **must not** occur:

 * Between a binary data encoding specifier and the opening parenthesis: `h (` is invalid.
 * Splitting a time value: `2018-07-01 T 10:53:22.001481 Z` is invalid.
 * Splitting a numeric value: `3f h`, `9.41 d`, `3 000`, `9.3 e+3` are invalid.
 * Splitting `true`, `false`, or `empty`: `t rue` is invalid.


### Whitespace is interpreted literally (not ignored) within a string:

    "This string has spaces
    and a newline, which are all preserved."



Illegal Encodings
-----------------

Illegal encodings must not be used, as they may cause problems or even API violations in certain languages. A parser may discard illegal encodings, or may even abort processing.

 * Numeric values must be representable in their respective binary formats (integer, binary float, decimal float).
 * Times must be valid. For example: hour 30, while technically encodable, is not allowed.
 * Map keys must not be container types or the `empty` type.
 * Maps must not contain duplicate keys (this includes mathematically equivalent numeric keys).
 * Upper case text is not allowed, except as described in section `Letter Case`.
 * Whitespace must only occur as described in section `Whitespace`.



File Format
-----------

A CTE file is simply a file containing a single CTE document. Recall that a CTE document consists of a single top-level object, and that you can store multiple objects by making the top level object a container. CTE files should be named using the extension `cte`.

For example: File `mydata.cte`

    # This is an example CTE document!
    # Remember: Any number of comments can appear in a row.
    {
        # Here are some mapped values...
        "first": 1
        "second": 2
    }



Version History
---------------

July 22, 2018: Preview Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
