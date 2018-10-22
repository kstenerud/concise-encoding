Concise Text Encoding
=====================

A general purpose, human readable representation of semi-structured hierarchical data.

Designed with the following points of focus:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Human readable format
  * Minimal complexity
  * Type compatible with CBE (Concise Binary Encoding)


A CTE document consists of a single, top-level object. You can store multiple objects in a document by making the top level object a container.

For example:

    {
        "list":           [1, 2, "a string"],
        "sub-map":        {1: "one", 2: "two", 3: 3000},
        "boolean":        true,
        "binary int":     10001011b,
        "octal int":      644o,
        "decimal int":    -10000000,
        "hex int":        fffe0001h,
        "float":          14.125,
        "decimal":        -1.02d,
        "time":           2018-07-01T10:53:22.001481,
        "empty":          empty,
        "array of int16": i16(1000, 2000, 3000),
    }

Whitespace is generally ignored when parsing a CTE document, as are commas when parsing containers or arrays (commas only serve an aesthetic purpose).



Supported Types
---------------

### Scalar Types

#### Boolean

Supports the values true and false.

Example:

    true
    false


#### Integer

Represents two's complement signed integers up to a width of 128 bits.

Integers can be specified in base 2, 8, 10, or 16. A prefix character determines which base to interpret as:

  * b: base 2 (binary)
  * o: base 8 (octal)
  * h: base 16 (hexadecimal)

With no prefix, base 10 is assumed.

Example:

    900000
    -1100b
    755o
    deadbeefh


#### Binary Floating Point

Represents ieee754 binary floating point values with widths from 32 to 128. Supports exponential notation using "e" or "E".

Example:

    1.25e+7
    -99.00001


#### Decimal Floating Point

Represents ieee754 decimal floating point values with widths from 64 to 128. Decimal floating point values are typically used in financial applications where emulation of decimal rounding is necessary.

Decimal floating values are differentiated from binary floating values by prefixing a "d".

Example:

    12.99d
    -100.04d


#### Time

Represents a date & time.

The date-time format is based on the ISO 8601 extended format, but is more restricted:

 * Time zones are mandatory.
 * Time intervals are not allowed.
 * All times must contain both a date and time component.
 * Only the fractional seconds field is optional. All other fields are mandatory.
 * The allowed formats are more restricted.

Like ISO 8601, the date and time components are separted by a capital T, and have optional fractional seconds.

General format:

    yyyy-mm-ddThh:mm:ss.ssssssZ
    yyyy-dddThh:mm:ss.ssssssZ

This allows date/time values to be parsed by a conforming iso8601 parser.

##### Time Zone Designators

The time zone designator may be a timezone offset in the format +HH:MM or -HH:MM, or the "Zulu time" designator "Z" to refer to UTC.

| Field | Name      | Meaning          | Minimum | Maximum |
| ----- | --------- | ---------------- | ------- | ------- |
| +-    | Direction | Offset direction | -       | +       |
| HH    | Hour      | Hour offset      | 00      | 23      |
| MM    | Minute    | Minute offset    | 00      | 59      |

Examples:

    Z (offset 0)
    +00:00 (offset 0)
    -00:00 (offset 0)
    -01:30 (1 hour and 30 minutes behind UTC)
    +01:00 (1 hour ahead of UTC, such as Berlin in winter)

##### Date Component

Date fields are separated by dashes. Dates may be represented either in year-month-day format (YYYY-MM-DD) or in day-of-year format (YYYY-DDD).

###### Eras and Zero Year

Years may refer to the AD or BC era. AD years have no prefix, and BC years have a dash "-" prefix.

The Anno Domini system has no zero year (there is no 0 BC or 0 AD). To keep mathematical continuity in the date format, all BC dates have an absolute year value that is one less than the BC year (the year 0000 refers to 1 BC, -0001 to 2 BC, and so on). The year 0000 (1 BC) may be represented with or without a dash prefix (both 0000 and -0000 are valid). All other BC years must have a dash prefix.

###### Year-Month-Day Format

    YYYY-MM-DD

| Field | Name  | Meaning       | Minimum | Maximum |
| ----- | ----- | ------------- | ------- | ------- |
| YYYY  | Year  | Year          | -inf    | +inf    |
| MM    | Month | Month of year | 01      | 12      |
| DD    | Day   | Day of month  | 01      | 01      |

The year field must be 4 or more digits long (not including the optional dash for era), and the month and day fields must be exactly 2 digits long.

###### Day-of-Year Format

    YYYY-DDD

| Field | Name  | Meaning       | Minimum | Maximum |
| ----- | ----- | ------------- | ------- | ------- |
| YYYY  | Year  | Year          | -inf    | +inf    |
| DDD   | Day   | Day of year   | 001     | 366     |

The year field must be 4 or more digits long (not including the optional dash for era), and the day field must be exactly 3 digits long.

Use this format if it is undesirable calculate the Gregorian date representation.

###### The Gregorian Calendar

All dates in YYYY-MM-DD format are assumed to use the Gregorian calendar. For dates prior to October 15th, 1582, the proplectic Gregorian calendar is used, whereby the Gregorian calendar system is applied backwards preceeding its introduction. Care must be taken due to the 10 days stricken from the calendar in the Gregorian reform (October 4th - October 14th, 1582).


##### Time Component

The time component has mandatory hour, minute, and second fields, exactly two digits long, separated by colons.

    HH:MM:SS

A time component may also contain an optional field for fractional seconds, separated by a period. This field may be from 1 to 6 digits, supporting time to the microsecond.

    HH:MM:SS.ssssss

| Field  | Name     | Meaning           | Minimum | Maximum | Notes              |
| ------ | -------- | ----------------- | ------- | ------- | ------------------ |
| HH     | Hour     | Hour of day       | 00      | 23      |                    |
| MM     | Minute   | Minute of hour    | 00      | 59      |                    |
| SS     | Second   | Second of minute  | 00      | 60      | 60 for leap second |
| ssssss | Fraction | Fractional second | (empty) | 999999  | optional field     |


##### Examples

    2018-07-01T10:53:22.001481

which is equivalent to:

    2018-184T10:53:22.001481



### Array Types

#### String

An array of UTF-8 encoded bytes, without a byte order mark (BOM). Strings must be enclosed in double-quotes.

The following escape sequences are allowed:

  * `\\`      (literal backslash)
  * `\"`      (double quote)
  * `\r`      (carriage return)
  * `\n`      (linefeed)
  * `\t`      (tab)
  * `\000`    (byte value in octal)
  * `\xff`    (byte value in hexadecimal)
  * `\uffff`  (unicode character)

Example:

    "A string\twith\ttabs\nand\nnewlines"


#### Array

A typed array of scalar objects. Only scalar types are supported, and all members of the array must be of the same type and size.

An array begins with an array type prefix, an opening parenthesis `(`, whitespace and/or comma separated contents, and finally a closing parenthesis `)`. A trailing comma on the last entry is allowed.

#### Array Type Prefixes:

  * `b`     (boolean)
  * `i8`    (8-bit integer)
  * `i16`   (16-bit integer)
  * `i32`   (32-bit integer)
  * `i64`   (64-bit integer)
  * `i128`  (128-bit integer)
  * `f32`   (32-bit binary floating point)
  * `f64`   (64-bit binary floating point)
  * `f128`  (128-bit binary floating point)
  * `d64`   (64-bit decimal floating point)
  * `d128`  (128-bit decimal floating point)
  * `t`     (time)

Example: An array of three 32-bit integers

    i32(1000000, 2000000, 3000000)



### Container Types

#### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

A list begins with an opening square bracket `[`, whitespace  and/or comma separated contents, and finally a closing bracket `]`. A trailing comma on the last entry is allowed.

Example:

    [1, "two", 3.1, {}, empty]


#### Map

A map associates objects (keys) with other objects (values). Keys may be any mix of scalar or array types, and must not be EMPTY. Values may be any mix of any type, including other containers. All keys in a map must be unique.

Map entries are split into key-value pairs using the colon `:` character and optional whitespace. Key-value pairs are separated from each other using whitespace and/or commas (either whitespace, or commas, or both). A comma is also allowed on the last entry.

A map begins with an opening curly brace `{`, whitespace and/or comma separated key-value pairs, and finally a closing brace `}`.

Example:

    {
        1: "alpha"
        2: "beta"
        "a map": {one: 1}
    }



### Other Types

#### Empty

Denotes the absence of data. Some languages implement this as the NULL value.

Example:

    empty



Illegal Encodings
-----------------

Illegal encodings must not be used, as they will cause problems or even API violations in certain languages. A decoder may discard illegal encodings.

  * Times must be valid. For example: February 31st, while technically encodable, is not allowed.
  * Map keys must not be container types or the EMPTY type.
  * Maps must not contain duplicate keys.
  * An array's element type must be a scalar type. Arrays of arrays, containers, or EMPTY, are not allowed.



File Format
-----------

A CTE file is simply a file containing a single CTE document. CTE files should use the extension "cte".

For example: File `mydata.cte`

    {
        "first": 1,
        "second": 2,
    }



Version History
---------------

July 22, 2018: Version 1



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
