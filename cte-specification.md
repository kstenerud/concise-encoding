Concise Text Encoding
=====================

Concise Text Encoding (CTE) is the text variant of Concise Encoding: a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

The text format aims to present data in a human friendly way, encoding data in a manner that can be easily and unambiguously edited in a UTF-8 capable text editor while maintaining 1:1 compatibility with the [binary format](cbe-specification.md) (which aims for compactness and machine processing efficiency).

CTE documents must follow the [Concise Encoding structural rules](ce-structure.md). Many terms used in this document are defined there.



Version
-------

Version 1 (prerelease)



Contents
--------

* [Terms](#terms)
* [Geenral](#general)
  - [Human Editability](#human-editability)
  - [Line Endings](#line-endings)
  - [Escape Sequences](#escape-sequences)
    - [Continuation](#continuation)
    - [Unicode Sequence](#unicode-sequence)
    - [Verbatim Sequence](#verbatim-sequence)
* [Version Specifier](#version-specifier)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Floating Point](#floating-point)
    - [Base-10 Notation](#base-10-notation)
    - [Base-16 Notation](#base-16-notation)
    - [Special Floating Point Values](#special-floating-point-values)
    - [Floating Point Rules](#floating-point-rules)
  - [Numeric Whitespace](#numeric-whitespace)
  - [UID](#uid)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
  - [Time Zones](#time-zones)
    - [Area/Location](#arealocation)
    - [Global Coordinates](#global-coordinates)
    - [Time Offset](#time-offset)
* [Array Types](#array-types)
  - [Element Array Encodings](#element-array-encodings)
  - [Media](#media)
  - [String-Like Array Encodings](#string-like-array-encodings)
  - [String](#string)
  - [Resource Identifier](#resource-identifier)
  - [Custom Binary](#custom-binary)
  - [Custom Text](#custom-text)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
    - [Markup Structure](#markup-structure)
    - [Container End](#container-end)
    - [Content String](#content-string)
  - [Relationship](#relationship)
* [Other Types](#other-types)
  - [Identifier](#identifier)
  - [Nil](#nil)
* [Pseudo-Objects](#pseudo-objects)
  - [Marker](#marker)
  - [Reference](#reference)
  - [Comment](#comment)
    - [Single Line Comment](#single-line-comment)
    - [Multiline Comment](#multiline-comment)
  - [Constant](#constant)
    - [Explicit Constant](#explicit-constant)
  - [NA](#na)
* [Combined Objects](#combined-objects)
* [Empty Document](#empty-document)
* [Letter Case](#letter-case)
* [Whitespace](#whitespace)
  - [Structural Whitespace Characters](#structural-whitespace-characters)
* [Confusable Characters](#confusable-characters)
* [Pretty Printing](#pretty-printing)
* [Version History](#version-history)
* [License](#license)



Terms
-----

The following terms have specific meanings in this specification:

| Term         | Meaning                                                                                                               |
| ------------ | --------------------------------------------------------------------------------------------------------------------- |
| MUST (NOT)   | If this directive is not adhered to, the document or implementation is invalid/non-conformant.                        |
| SHOULD (NOT) | Every effort should be made to follow this directive, but the document/implementation is still valid if not followed. |
| MAY (NOT)    | It is up to the implementation to decide whether to do something or not.                                              |
| CAN          | Refers to a possibility which **MUST** be accommodated by the implementation.                                         |
| CANNOT       | Refers to a situation which **MUST NOT** be allowed by the implementation.                                            |
| OPTIONAL     | The implementation **MUST** support both the existence and the absence of the specified item.                         |
| RECOMMENDED  | Refers to a "best practice", which **SHOULD** be followed if possible.                                                |



General
-------

A CTE document is a UTF-8 encoded text document containing data arranged in an ad-hoc hierarchical fashion.

All characters in a CTE document **MUST** be [text-safe](ce-structure.md#text-safety). Text-unsafe characters **MUST** only be represented using [escape sequences](#escape-sequences) (where allowed). Validation of text-safety **MUST** occur before processing escape-sequences. All other validation of string-like values **MUST** occur **after** decoding any escape sequences contained within.

Whitespace is used to separate elements in a container. In maps, the key and value portions of a key-value pair are separated by an equals character (`=`) and possible whitespace. The key-value pairs themselves are separated by whitespace. Extraneous whitespace is ignored.

**Examples**:

 * CTE v1 empty document: `c1 na`
 * CTE v1 document containing the top-level integer value 1000: `c1 1000`
 * CTE v1 document containing a top-level list: `c1 [a b c]`
 * CTE v1 document containing a top-level map: `c1 {a=1 b=2 c=3}`


### Human Editability

A CTE document **MUST** be editable by a human. This means that it **MUST** contain only valid UTF-8 characters and sequences that can actually be viewed, entered and modified in a UTF-8 capable text editor. Unicode runes that have no width or visibility or direct input method, or are reserved or permanently marked as non-characters, **MUST** not be present in the document.

In the spirit of human editability:

 * Implementations **SHOULD** avoid outputting characters that editors tend to convert automatically.
 * Line lengths **SHOULD** be kept within reasonable amounts in order to avoid excessive horizontal scrolling in an editor.
 * The canonical line ending is linefeed (u+000a)
 * Implementations **SHOULD** convert structural line endings to the operating system's native format when saving a document to disk. See: [line endings](#line-endings)
 * If a certain character is likely to be confusing or problematic to a human reader or editor, it **MUST** be escaped.


### Line Endings

Line endings **CAN** be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for data transmission, the canonical format is LF only. Decoders **MUST** accept all encodings as input, but encoders **SHOULD** output LF when the destination is a foreign or unknown system.


### Escape Sequences

In some contexts, escape sequences **MAY** be used to encode data that would otherwise be cumbersome or impossible to represent. `\` acts as an escape sequence initiator, followed by an escape type character and possible data:

| Sequence (`\` + ...) | Interpretation                          |
| -------------------- | --------------------------------------- |
| `t`                  | horizontal tab (u+0009)                 |
| `n`                  | linefeed (u+000a)                       |
| `r`                  | carriage return (u+000d)                |
| `"`                  | double quote (u+0022)                   |
| `*`                  | asterisk (u+002a)                       |
| `/`                  | slash (u+002f)                          |
| `<`                  | less-than (u+003c)                      |
| `>`                  | greater-than (u+003e)                   |
| `\`                  | backslash (u+005c)                      |
| `\|`                 | pipe (u+007c)                           |
| `_`                  | non-breaking space (u+00a0)             |
| `-`                  | soft-hyphen (u+00ad)                    |
| u+000a               | [continuation](#continuation)           |
| u+000d               | [continuation](#continuation)           |
| `0` - `9`            | [Unicode sequence](#unicode-sequence)   |
| `.`                  | [verbatim sequence](#verbatim-sequence) |

Escape sequences **MUST** be converted before any other processing occurs during the decode process.

#### Continuation

A continuation escape sequence causes the decoder to ignore all whitespace characters until it encounters the next printable character. The escape character (`\`) followed by either LF (u+000a) or CR (u+000d) initiates a continuation.

**Example**:

```
    "The only people for me are the mad ones, the ones who are mad to live, mad to talk, \
     mad to be saved, desirous of everything at the same time, the ones who never yawn or \
     say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles \
     exploding like spiders across the stars."
```

The above string is interpreted as:

```
The only people for me are the mad ones, the ones who are mad to live, mad to talk, mad to be saved, desirous of everything at the same time, the ones who never yawn or say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles exploding like spiders across the stars.
```

#### Unicode Sequence

Unicode escape sequences begin with a backslash (`\`) character, followed by one digit (`0`-`9`) specifying the number of hex digits encoding the codepoint, followed by that number of hex digits (`0`-`f`) representing the hexadecimal value of the codepoint.

**Examples**:

| Sequence  | Digits | Character     |
| --------- | ------ | ------------- |
| `\0`      | 0      | NUL           |
| `\16`     | 1      | ACK           |
| `\27f`    | 2      | DEL           |
| `\3101`   | 3      | ā  (a macron) |
| `\42191`  | 4      | ↑  (up arrow) |
| `\51f415` | 5      | 🐕 (dog)      |

#### Verbatim Sequence

Verbatim escape sequences work similarly to "here" documents in Bash. They're composed as follows:

 * Verbatim sequence escape initiator (`\.`).
 * An end-of-sequence identifier, which is a sequence of [text-safe](ce-structure.md#text-safety), non-whitespace characters (in accordance with [human editability](cte-specification.md#human-editability)).
 * A whitespace terminator to terminate the end-of-sequence identifier (either: SPACE `u+0020`, TAB `u+0009`, LF `u+000a`, or CR+LF `u+000d u+000a`).
 * The string contents.
 * A second instance of the end-of-sequence identifier (without whitespace terminator).

**Example**:

```cte
"Verbatim sequences can occur anywhere escapes are allowed.\n\
\.@@@
In verbatim sequences, everything is interpreted literally until the
end-of-string identifier is encountered (in this case three @ characters).
Characters like ", [, <, \ and such can appear unescaped.

Whitespace (including "leading" whitespace) is also read verbatim.
          For example, this line really is indented 10 spaces.

@@@Normal processing resumes after the terminator, so '\n' and such are interpreted."
```
Which decodes to:
```
Verbatim sequences can occur anywhere escapes are allowed.
In verbatim sequences, everything is interpreted literally until the
end-of-string identifier is encountered (in this case three @ characters).
Characters like ", [, <, \ and such can appear unescaped.

Whitespace (including "leading" whitespace) is also read verbatim.
          For example, this line really is indented 10 spaces.

Normal processing resumes after the terminator, so '
' and such are interpreted.
```



Version Specifier
-----------------

A CTE document begins with a version specifier, which is composed of the character `c` (u+0063), followed immediately by an unsigned integer version number. There **MUST** be no whitespace between the `c` and the version number.

The version specifier and the top-level object **MUST** be separated by whitespace.

**Example**:

* Version specifier (CTE version 1): `c1`
* Complete (and empty) document: `c1 na`



Numeric Types
-------------

### Boolean

Represented by the sequences `true` and `false`.


### Integer

Integer values **CAN** be positive or negative, and **CAN** be represented in various bases. Negative values are prefixed with a dash `-` as a sign character. There is no positive sign character (such as `+`). Encoders **MUST** write values in lower case.

Integers **CAN** be specified in base 2, 8, 10, or 16. Bases other than 10 require a prefix:

| Base | Name        | Digits           | Prefix | Example      | Decimal Equivalent |
| ---- | ----------- | ---------------- | ------ | ------------ | ------------------ |
|   2  | Binary      | 01               | `0b`   | `-0b1100`    | -12                |
|   8  | Octal       | 01234567         | `0o`   | `0o755`      | 493                |
|  10  | Decimal     | 0123456789       |        | `900000`     | 900000             |
|  16  | Hexadecimal | 0123456789abcdef | `0x`   | `0xdeadbeef` | 3735928559         |

### Floating Point

A floating point number is composed of a whole part and a fractional part separated by a dot `.`, with an **OPTIONAL** exponential portion. Negative values are prefixed with a dash `-`.

**Examples**:

    1.0
    -98.413

#### Base-10 Notation

The exponential portion of a base-10 number is denoted by the lowercase character `e`, followed by the signed size of the exponent (using **OPTIONAL** `+` for positive and mandatory `-` for negative). The exponential portion is a signed base-10 number representing the power-of-10 to multiply the significand by. Values **SHOULD** be normalized (only one digit to the left of the decimal point) when using exponential notation.

 * `6.411e+9` = 6411000000
 * `6.411e9` = 6411000000
 * `6.411e-9` = 0.000000006411

Although there is technically no maximum number of significant digits or exponent digits for base-10 floating point notation, care should be taken to ensure that the receiving end will be able to store the value. For example, 64-bit ieee754 floating point values **CAN** represent values with up to 16 significant digits and an exponent range roughly from 10⁻³⁰⁷ to 10³⁰⁷.

#### Base-16 Notation

Base-16 floating point numbers allow 100% accurate representation of ieee754 binary floating point values. They begin with `0x`, and the exponential portion is denoted by the lowercase character `p`. The exponential portion is a signed base-10 number representing the power-of-2 to multiply the significand by. The exponent's sign character **CAN** be omitted if it's positive. Values **SHOULD** be normalized.

 * `0xa.3fb8p+42` = a.3fb8 x 2⁴²
 * `0x1.0p0` = 1

To maintain compatibility with [CBE](cbe-specification.md), base-16 floating point notation **CAN** only be used to represent binary float values that are supported by CBE (16, 32, 64-bit).

#### Special Floating Point Values

 * `inf`: Infinity
 * `-inf`: Negative Infinity
 * `nan`: Not a Number (quiet)
 * `snan`: Not a Number (signaling)

#### Floating Point Rules

**There MUST be one (and only one) dot character:**

| Value          | Notes              |
| -------------- | ------------------ |
| `1`            | Integer, not float |
| `1.0`          | Float              |
| `500000000000` | Integer, not float |
| `5.0e+11`      | Float              |
| `5e+11`        | Invalid            |
| `10.4.5`       | Invalid            |

**There MUST be at least one digit on each side of the dot character:**

| Invalid      | Valid     | Notes                                                    |
| ------------ | --------- | -------------------------------------------------------- |
| `-1.`        | `-1.0`    | Or use integer value `-1`                                |
| `.1`         | `0.1`     |                                                          |
| `.218901e+2` | `21.8901` | Or `2.18901e+1`                                          |
| `-0`         | `-0.0`    | Special case: -0 **CANNOT** be represented as an integer |


### Numeric Whitespace

The `_` character **CAN** be used as "numeric whitespace" when encoding certain numeric values. Other [whitespace](#whitespace) characters are not allowed.

Rules:

 * Only [integer](#integer) and [floating point](#floating-point) types can contain numeric whitespace.
 * [Named values](#named-values) (such as `nan` and `inf`) **MUST NOT** contain numeric whitespace.
 * Numeric whitespace **CAN** only occur between two consecutive numeric digits (`0`-`9`, `a`-`f`, depending on numeric base).
 * Numeric whitespace characters **MUST** be ignored when decoding numeric values.

**Examples**:

Valid:

 * `1_000_000` = 1000000
 * `4_3.5_5_4e9_0` = 43.554e90
 * `-0xa.fee_31p1_00` = -0xa.fee31p100

Invalid:

 * `_1000000`
 * `1000000_`
 * `43_.554e90`
 * `43._554e90`
 * `43.554_e90`
 * `-_43.554e90`
 * `-_0xa.fee31p100`
 * `-0xa.fee31p_100`
 * `-0_xa.fee31p100`


### UID

An [rfc4122 UUID string representation](https://tools.ietf.org/html/rfc4122).

Example:

    123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

Temporal types record time with varying degrees of precision.

Fields other than year **CAN** be pre-padded with zeros (`0`) up to their maximum allowed digits.


### Date

A date is made up of the following fields, separated by a dash character (`-`):

| Field | Mandatory | Min Value | Max Value | Min Digits | Max Digits |
| ----- | --------- | --------- | --------- | ---------- | ---------- |
| Year  |     Y     |         * |         * |          1 |          * |
| Month |     Y     |         1 |        12 |          1 |          2 |
| Day   |     Y     |         1 |        31 |          1 |          2 |

 * BC years are prefixed with a dash (`-`).

**Examples**:

 * `2019-8-5`: August 5, 2019
 * `5081-03-30`: March 30, 5081
 * `-300-12-21`: December 21, 300 BC (proleptic Gregorian)



### Time

A time is made up of the following mandatory and optional fields:

| Field        | Mandatory | Separator | Min Value | Max Value | Min Digits | Max Digits |
| ------------ | --------- | --------- | --------- | --------- | ---------- | ---------- |
| Hour         |     Y     |           |         0 |        23 |          2 |          2 |
| Minute       |     Y     |    `:`    |         0 |        59 |          2 |          2 |
| Second       |     Y     |    `:`    |         0 |        60 |          2 |          2 |
| Subseconds   |     N     |    `.`    |         0 | 999999999 |          0 |          9 |
| Time Zone    |     N     |    `/`    |         - |         - |          - |          - |

**Note**: If the time zone is unspecified, it is assumed to be `Zero` (UTC).

**Examples**:

 * `9:04:21`: 9:04:21 UTC
 * `23:59:59.999999999`: 23:59:59 and 999999999 nanoseconds UTC
 * `12:05:50.102/Z`: 12:05:50 and 102 milliseconds UTC
 * `4:00:00/Asia/Tokyo`: 4:00:00 Tokyo time
 * `17:41:03/-13.54/-172.36`: 17:41:03 Samoa time
 * `9:00:00/L`: 9:00:00 local time


### Timestamp

A timestamp combines a date and a time, separated by a slash character (`/`).

**Examples**:

 * `2019-01-23/14:08:51.941245`: January 23, 2019, at 14:08:51 and 941245 microseconds, UTC
 * `1985-10-26/01:20:01.105/M/Los_Angeles`: October 26, 1985, at 1:20:01 and 105 milliseconds, Los Angeles time
 * `5192-11-01/03:00:00/48.86/2.36`: November 1st, 5192, at 3:00:00, at whatever is in the place of Paris at that time


### Time Zones

The time zone is an **OPTIONAL** field. If omitted, it is assumed to be `Zero` (UTC).

#### Area/Location

An area/location time zone is written in the form `Area/Location`.

**Examples**:

 * `E/Paris`
 * `America/Vancouver`
 * `America/Indiana/Petersburg` (which has area `America` and location `Indiana/Petersburg`)
 * `Etc/UTC` == `Zero` == `Z`
 * `L`

#### Global Coordinates

Global coordinates are written as latitude and longitude to a precision of hundredths of degrees, separated by a slash character (`/`). Negative values are prefixed with a dash character (`-`), and the dot character (`.`) is used as a fractional separator.

**Examples**:

 * `51.60/11.11`
 * `-13.53/-172.37`

#### Time Offset

Time offsets are recorded by using a `+` or `-` character as the time zone separator instead of the `/` character, with the hours and minutes given in the form `hh:mm`.

**Examples**:

 * `1985-10-26/01:20:01.105+07:00`
 * `2000-01-14/10:22:00-02:00`



Array Types
-----------

The standard array encoding format consists of a pipe character (`|`), followed by the array type, mandatory whitespace, the contents, and finally a closing pipe. Depending on the kind of array, the contents are encoded either as whitespace-separated elements, or as a string-like sequence representing the contents:

    |type elem1 elem2 elem3 ...|
    |type contents-represented-as-a-string|

An empty array has a type but no contents:

    |type|

The following array types are available:

| Type   | Description                                 | Encoding Kind |
| ------ | ------------------------------------------- | ------------- |
| `b`    | Boolean                                     | Element       |
| `u8`   | 8-bit unsigned integer                      | Element       |
| `u16`  | 16-bit unsigned integer                     | Element       |
| `u32`  | 32-bit unsigned integer                     | Element       |
| `u64`  | 64-bit unsigned integer                     | Element       |
| `i8`   | 8-bit signed integer                        | Element       |
| `i16`  | 16-bit signed integer                       | Element       |
| `i32`  | 32-bit signed integer                       | Element       |
| `i64`  | 64-bit signed integer                       | Element       |
| `f16`  | 16-bit floating point (bfloat)              | Element       |
| `f32`  | 32-bit floating point (ieee754)             | Element       |
| `f64`  | 64-bit floating point (ieee754)             | Element       |
| `u`    | 128-bit UID                                 | Element       |
| `r`    | [Resource Identifier](#resource-identifier) | String-Like   |
| `ct`   | [Custom Text](#custom-text)                 | String-Like   |
| `cb`   | [Custom Binary](#custom-binary)             | Element       |

**Note**: If an unrecognized array type is encountered, a decoder **MUST** treat it as a [media](#media) array type.

### Element Array Encodings

For element array encodings, any valid representation of the element data type may be used, provided the value fits within the type's width. 

#### Implied Prefix

**OPTIONALLY**, a suffix **CAN** be appended to the type specifier (if the type supports it) to indicate that all values **MUST** be considered to have an implicit prefix.

| Type Suffix | Implied element prefix | Example                         |
| ----------- | ---------------------- | ------------------------------- |
| `b`         | `0b`                   | `\|u8b 10011010 00010101\|`     |
| `o`         | `0o`                   | `\|i16o -7445 644\|`            |
| `x`         | `0x`                   | `\|f32x a.c9fp20 -1.ffe9p-40\|` |

#### Special Array Element Rules

 * Boolean array elements are represented using `0` for false and `1` for true. Whitespace is **OPTIONAL** when encoding a boolean array using `0` and `1` (e.g. `|b 1001|` = `|b 1 0 0 1|`).

**Examples**:

 * `|u8x 9f 47 cb 9a 3c|`
 * `|f32 1.5 0x4.f391p100 30 9.31e-30|`
 * `|i16 0b1001010 0o744 1000 0xffff|`
 * `|u 3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f|`
 * `|b 11010|`


### Media

A media object is a specialization of the typed array. The array type field contains its [media type](http://www.iana.org/assignments/media-types/media-types.xhtml), and the contents are encoded with an implied format of `u8x`.

**Example**:

```cte
|application/x-sh 23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a|
```

Which is the shell script:

```sh
#!/bin/sh

echo hello world
```


### String-Like Array Encodings

String-like array encodings are interpreted as a whole, and **MUST** encode [text-unsafe](ce-specification#text-safety) characters, TAB, CR, LF, pipe (`|`) and backslash (`\`) (as well as their [lookalikes](ce-structure.md#confusable-characters)) as [escape sequences](#escape-sequences).


### String

Strings are enclosed within double-quote (`"`) delimiters, and the elements are string-like. All characters leading up to the closing double-quote (including whitespace) are considered part of the string sequence. A quoted string must encode [text-unsafe](ce-specification#text-safety) characters, TAB, CR, LF, double-quote (`"`) and backslash (`\`) (as well as their [lookalikes](ce-structure.md#confusable-characters)) as [escape sequences](#escape-sequences).

**Example**:

    "Line 1\nLine 2\nLine 3"


### Resource Identifier

A resource identifier is enclosed within the delimiters `@"` and `"`.

A Concise Encoding implementation must interpret only [CTE escape sequences](#escape-sequences) when decoding resource identifiers. Resource-specific escape sequences (such as percent-escapes) must not be interpreted.

 * `@"http://x.y.z?quote=\""` decodes to `http://x.y.z?quote="`,  which the upper layers interpret as `http://x.y.z?quote="`
 * `@"http://x.y.z?quote=%22"` decodes to `http://x.y.z?quote=%22`,  which the upper layers interpret as `http://x.y.z?quote="`

Resource IDs support [concatenation](#concatenation):

    @"http://x.com/":"my-suffix" // = http://x.com/my-suffix

    c1 {
        resources = [
            &query:@"https://some-really-long-url-thats-annoying-to-type.com/query?ref-id="
        ]
        queries = {
            query-1 = $query:1
            query-5000 = $query:5000
        }
    }


### Custom Binary

Custom binary data is encoded using standard typed array syntax with type identifier `cb`. Its elements are encoded like a u8x array (hex encoded byte elements).

**Example**:

    |cb 04 f6 28 3c 40 00 00 40 40|
    = binary data representing an imaginary custom "cplx" struct
      {
          type:uint8 = 4
          real:float32 = 2.94
          imag:float32 = 3.0
      }


### Custom Text

Custom text data is encoded using standard typed array syntax with type identifier `ct`. Its elements are string-like, and it **CAN** contain [escape sequences](#escape-sequences) which must be processed before the converted string is passed to the custom decoder that will interpret it.

**Example**:

    |ct cplx(2.94+3i)|



Container Types
---------------

### List

A list begins with an opening square bracket `[`, contains whitespace separated contents, and finishes with a closing square bracket `]`.

**Example**:

```cte
c1 [
    1
    "two"
    3.1
    {}
    na
]
```


### Map

A map begins with an opening curly brace `{`, contains whitespace separated key-value pairs, and finishes with a closing curly brace `}`.

Map entries are split into key-value pairs using the equals `=` character and **OPTIONAL** whitespace. Key-value pairs must be separated from each other using whitespace. A key without a paired value is invalid.

**Example**:

```cte
c1 {
    1 = "alpha"
    2 = "beta"
    "a map" = {"one"=1 "two"=2}
}
```


### Markup

The CTE encoding of a markup container is similar to XML, except:

 * There are no end tags. All data is contained within the begin `<`, content begin `,`, and end `>` characters.
 * Comments are encoded using `/*` and `*/` instead of `<!--` and `-->`, and **CAN** be nested.

#### Markup Structure

| Section    | Delimiter  | Type                      | Required |
| ---------- | ---------- | ------------------------- | -------- |
| Tag name   | `<`        | [Identifier](#identifier) | Y        |
| Attributes | whitespace | [Map](#map)               |          |
| Contents   | `,`        | [List](#list)             |          |
| End        | `>`        |                           | Y        |

Attributes and contents are **OPTIONAL**. There must be whitespace between the container name and the attributes section (if present), and there **CAN** **OPTIONALLY** be whitespace adjacent to the begin, contents, and end delimiters.

Illustration of markup encodings:

| Attributes | Children | Example                                                        |
| ---------- | -------- | -------------------------------------------------------------- |
|     N      |    N     | `<br>`                                                         |
|     Y      |    N     | `<div "id"="fillme">`                                          |
|     N      |    Y     | `<span,Some text here>`                                        |
|     Y      |    Y     | `<ul "id"="mylist" "style"="boring", <li,first> <li,second> >` |

The contents section is in string processing mode whenever it's not processing a sub-container or comment (initiated by an unescaped `<` or `//` or `/*`).

##### Container End

The markup container ends when an unescaped `>` character is encountered while processing the attributes or contents.

##### Content String

Content strings **CAN** contain [escape sequences](#escape-sequences), which must be processed before applying the structural rules for content strings.

**Example**:

```cte
c1 <View,
    <Image src=@"images/avatar-image.jpg">
    <Text id=HelloText,
        Hello! Please choose a name!
    >
    // <HRule style=thin>
    <TextInput id=NameInput style={height=40 borderColor=gray} OnChange="\.@@
        HelloText.SetText("Hello, " + NameInput.Text + "!")
    @@",
        Name me!
    >
>
```


### Relationship

A relationship container is composed of the delimiters `(` and `)`, containing the subject, predicate, and object.

**Examples**:

 * `(@"https://springfield.gov/people#homer_simpson" @"https://example.org/wife" @"https://springfield.gov/people#marge_simpson")`
 * `(@"https://springfield.gov/people#homer_simpson" @"https://example.org/employer" @"https://springfield.gov/employers/nuclear_power_plant")`



Other Types
-----------

### Identifier

Identifiers are strings, but are written without double-quotes and don't support escape sequences.

**Examples**:

 * `123`
 * `some_id`
 * `25th`
 * `猫`


### Nil

Nil is encoded as `nil`.



Pseudo-Objects
--------------

### Marker

A marker sequence consists of the following, with no whitespace in between:

 * `&` (the marker initiator)
 * A marker ID ([identifier](#identifier))
 * `:` (the marker separator)
 * The marked value

Example:

```cte
c1 [
    &remember_me:"Pretend that this is a huge string"
    &1:{a = 1}
]
```

The string `"Pretend that this is a huge string"` is marked with the string ID `remember_me`, and the map `{a=1}` is marked with the unsigned integer ID `1`.


### Reference

A reference begins with the reference initiator (`$`), followed immediately (with no whitespace) by either a [marker ID](ce-structure.md#marker-id) or a [resource identifier](#resource-identifier).

Example:

```cte
c1 {
    some_object = {
        my_string = &big_string:"Pretend that this is a huge string"
        my_map = &1:{
            a = 1
        }
    }

    reference_to_string = $big_string
    reference_to_map = $1
    reference_to_local_doc = $@"common.cte"
    reference_to_remote_doc = $@"https://somewhere.com/my_document.cbe?format=long"
    reference_to_local_doc_marker = $@"common.cte#legalese"
    reference_to_remote_doc_marker = $@"https://somewhere.com/my_document.cbe?format=long#examples"
}
```


### Comment

Comments **CAN** be written in single-line or multi-line form.

#### Single Line Comment

A single line comment begins at the sequence `//` and continues until the next linefeed (u+000a) is encountered.

#### Multiline Comment

A multiline comment begins at the sequence `/*` and is terminated by the sequence `*/`. Multiline comments support nesting, meaning that further `/*` sequences inside the comment will start subcomments that must also be terminated by their own `*/` sequence. No processing of the comment contents other than detecting comment begin and comment end is peformed.

**Note**: Commenting out strings or markup contents containing the sequences `/*` or `*/` could potentially cause parse errors because the parser won't have any contextual information about the sequences, and will simply treat them as "comment begin" and "comment end". This edge case could be mitigated by pre-emptively escaping all occurrences of `/*` and `*/` that don't represent comment delimiters:

**Example**:

```cte
c1
// Comment before top level object
{
    // Comment before the "name" object.
    // And another comment.
    "name" = "Joe Average" // Comment after the "Joe Average" object.

    "email" = // Comment after the "email" key.
    /* Multiline comment with nested comment inside
      @"mailto:joe@average.org"
      /* Unlike in C, nested multiline
         comments are allowed */
    */
    @"mailto:someone@somewhere.com"

    "data" // Comment after data
    =
    // Comment before some binary data (but not inside it)
    |u8x 01 02 03 04 05 06 07 08 09 0a|
}
```


### Constant

A constant name begins with a hash `#` character, followed by an [identifier](ce-structure.md#identifier).

    #some_const // a const named "some_const", whose type and value are defined in a schema.

#### Explicit Constant

CTE documents containing constants normally cannot be decoded without a matching schema. For cases where this would be problematic, A CTE encoder **MAY** opt to encode explicit constants, which name the constant (to provide meaning for humans) and also provide its value (to support decoding without a schema).

An explicit constant begins with a hash `#` character, followed by an [identifier](ce-structure.md#identifier), then a colon `:`, and finally the object's value.

    #some_const:1 // a const named "some_const", which is the integer 1.

The explicit value must be a real object (not a pseudo-object).

If a decoder has access to the schema that defines a particular constant, the explicit value is ignored (the value from the schema is used).


### NA

NA is encoded as `na`, and supports [concatenation](#concatenation) for the **OPTIONAL** reason field.

**Examples**:

 * `na:"Insufficient privileges"` (not available, with an English language reason)
 * `na:404` (not available for reason code 404)
 * `na` (not available for unknown reason)



Combined Objects
----------------

The combine operator in CTE is `:`. There must be no whitespace between the combine operator and its operands.


**Examples**:

```cte
c1 {
    refs = [
        &ref1:@"https://example.com/"
        &ref2:@"https://example.com/":foo
    ]

    // https://example.com/foo
    example_1 = @"https://example.com/":foo

    // https://example.com/foo
    example_2 = $ref1:foo

    // https://example.com/foo
    example_3 = $ref2

    // https://example.com/200
    example_4 = $ref1:200

    // https://example.com/foo/bar
    example_5 = $ref1:"foo/bar"

    // NA for reasons
    example_6 = na:"Insufficient privileges"
    example_7 = na:{
        code = 409
        en = "database error"
        jp = "データベースエラー"
    }
}
```



Empty Document
--------------

An empty document in CTE is signified by using the [Nil](#nil) type as the top-level object:

```cte
c1 nil
```



Letter Case
-----------

A CTE document must be entirely in lower case, except in the following situations:

 * Strings, string-like arrays, and comments **CAN** contain uppercase characters. Case must be preserved.
 * [Marker IDs](ce-structure.md#marker-id) and [constants](#constant) **CAN** contain uppercase characters. Case must be preserved.
 * [Time zones](#time-zones) are case sensitive, and usually contain both uppercase and lowercase characters.

Everything else, including hexadecimal digits, exponents, and escape sequences, must be lower case.

A decoder must accept data regardless of letter-case because humans will inevitably break these rules, and rejecting a document on such grounds would result in poor UX.

An encoder must output letter case in accordance with this specification.



Whitespace
----------

Whitespace is defined as any character marked as whitespace in the [Unicode](https://unicode.org) database. As of 2021, these characters are:

| Code Point | Name                      |
| ---------- | ------------------------- |
| U+0009     | character tabulation      |
| U+000A     | line feed                 |
| U+000B     | line tabulation           |
| U+000C     | form feed                 |
| U+000D     | carriage return           |
| U+0020     | space                     |
| U+0085     | next line                 |
| U+00A0     | no-break space            |
| U+1680     | ogham space mark          |
| U+2000     | en quad                   |
| U+2001     | em quad                   |
| U+2002     | en space                  |
| U+2003     | em space                  |
| U+2004     | three-per-em space        |
| U+2005     | four-per-em space         |
| U+2006     | six-per-em space          |
| U+2007     | figure space              |
| U+2008     | punctuation space         |
| U+2009     | thin space                |
| U+200A     | hair space                |
| U+2028     | line separator            |
| U+2029     | paragraph separator       |
| U+202F     | narrow no-break space     |
| U+205F     | medium mathematical space |
| U+3000     | ideographic space         |


### Structural Whitespace Characters

Structural whitespace is a sequence of whitespace characters whose purpose is to separate objects in a CTE document (for example, separating objects in a list `[1 2 3 4]`). Such characters are not interpreted literally, are interchangeable, and can be repeated any number of times without altering the meaning or structure of the document. Whitespace characters not intended to be structural will need to be quoted in most contexts to preserve their meaning.

CTE decoders must accept all whitespace characters when decoding structural whitespace. CTE Encoders must produce only the following characters as structural whitespace:

| Code Point | Name                      |
| ---------- | ------------------------- |
| U+0009     | character tabulation      |
| U+000A     | line feed                 |
| U+000D     | carriage return           |
| U+0020     | space                     |


#### Structural Whitespace **can** occur:

 * Around an object.
 * Around array and container delimiters (`|`, `[`, `]`, `{`, `=`, `}`, `<`, `,`, `>`)

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `| u8x 01   02 03   04 |` is equivalent to `|u8x 01 02 03 04|`
 * `{ 1="one" 2 = "two" 3= "three" 4 ="four"}` is equivalent to `{1="one" 2="two" 3="three" 4="four"}`


#### Structural Whitespace **must** occur:

 * Between the [version specifier](#version-specifier) and the first object.
 * Between the end-of-string identifier and the beginning of the data in a [verbatim sequence](#verbatim-sequence).
 * Between a typed array element type specifier and the array contents, and between typed array elements.
 * Between values in a [list](#list) (`["one""two"]` is invalid).
 * Between key-value pairs in a [map](#map) or [markup attributes](#attributes-section) (`{1="one"2="two"}` is invalid).
 * Between a markup's [tag name](#markup-tag-name) and its [attributes](#attributes-section) (if attributes are present).


#### Whitespace **must not** occur:

 * Before the [version specifier](#version-specifier).
 * Between a sentinel character and its associated value (`& 1234`, `$ @"mydoc.cbe"`, `# Planck_Js` are invalid).
 * Between a [marker ID](ce-structure.md#marker-id) and the object it marks (`&123: xyz` is invalid).
 * Between an [explicit constant](#constant) name and its explicit value (`#Planck_Js: 6.62607015e-34` is invalid).
 * Between a [concatenation](#concatenation) operator and its operands (`@"http://x.com/" : 1` is invalid).
 * In time values (`2018.07.01-10 :53:22.001481/Z` is invalid).
 * In numeric values (`0x3 f`, `9. 41`, `3 000`, `9.3 e+3`, `- 1.0` are invalid). Use the [numeric whitespace](#numeric-whitespace) character (`_`) instead where it's valid to do so.



Lookalike Characters
--------------------

Lookalike characters are characters that look confusingly similar to CTE structural symbol characters when viewed by a human. Such characters **MUST** be escaped in CTE documents where a human would likely confuse them for an escape sequence initiator (`\`) or a string object terminator.

| Lookalike      | Escaped             |
| -------------- | ------------------- |
| `"A” string"`  | `"A\4201d string"`  |

The following is a (as of 2021-03-01) complete list of lookalike [Unicode characters](https://unicode.org/charts). This list may change as the Unicode character set evolves over time. Codec developers **MUST** keep their implementation current with the latest lookalike characters.

| Character | Context             | Lookalikes (codepoints)                                                 |
| --------- | ------------------- | ----------------------------------------------------------------------- |
| `"`       | String, Resource ID | 02ba, 02ee, 201c, 201d, 201f, 2033, 2034, 2036, 2037, 2057, 3003, ff02  |
| `*`       | Comment             | 204e, 2055, 2217, 22c6, 2b51, fe61, ff0a                                |
| `/`       | Comment             | 2044, 2215, 27cb, 29f8, 3033, ff0f, 1d10d                               |
| `>`       | Markup Contents     | 00bb, 02c3, 203a, 227b, 232a, 3009, fe65, ff1e                          |
| `\`       | Escapable Content   | 2216, 27cd, 29f5, 29f9, 3035, fe68, ff3c                                |
| `\|`      | Custom Text         | 00a6, 01c0, 2223, 2225, 239c, 239f, 23a2, 23a5, 23aa, 23ae, 23b8, 23b9, 23d0, 2d4f, 3021, fe31, fe33, ff5c, ffdc, ffe4, ffe8, 1028a, 10320, 10926, 10ce5, 10cfa, 1d100, 1d105, 1d1c1, 1d1c2 |



Pretty Printing
---------------

Pretty printing is the act of laying out structural whitespace in a CTE document such that it is easier for humans to parse. CTE documents **SHOULD** always be pretty-printed because their intent is to be read by humans. When this is not the case, use [CBE](cbe-specification.md).

This section specifies how to pretty-print CTE documents.


#### Right Margin

The right margin (maximum column before breaking an object into multiple lines) **SHOULD** be kept "reasonable". "Reasonable" is difficult to define because it depends in part on the kind of data the document contains, and the container depth.

In general, 120 columns **SHOULD** always be considered reasonable, with larger margins depending on the kind and depth of data the document is likely to contain.


#### Indentation

The canonical indentation is 4 spaces (`    `).


#### Lists

Objects in a list **SHOULD** be placed on separate lines.
```cte
[
    @"https://www.imdb.com/title/tt0090605/"
    @"https://www.imdb.com/title/tt1029248/"
]
```

If a list is empty, the closing `]` **SHOULD** be on the same line.
```cte
[]
```

Short lists containing small objects may be placed entirely on one line.
```cte
[a b c d]
```


#### Maps

There **SHOULD** be a space between keys, values and the `=` in key-value pairs, and each key-value pair **SHOULD** be on a separate line.
```cte
{
    aliens = @"https://www.imdb.com/title/tt0090605/"
    moribito = @"https://www.imdb.com/title/tt1029248/"
}
```

If a map is empty, the closing `}` **SHOULD** be on the same line.
```cte
{}
```

Small maps containing small objects may be placed entirely on one line. In such a case, omit the spaces around the `=`.
```cte
{a=b c=d}
```


#### Markup

The closing `>` **SHOULD** only be on a different line if there are contents.
```cte
<a>
```

```cte
<a,
    contents
>
```

The attributes section **SHOULD** be entirely on the same line as the tag name if it's not too long.
```cte
<a x=y>
```

```cte
<a x=y,
    contents
>
```

If the attributes section is too long, the overflow **SHOULD** be broken up into multiple indented lines.
```cte
<img src=@"http://somereallylongdomainname.likereallylong.com/images/2.jpg"
    width=50 height=50 border-left=10 units=px>
```


#### Strings

Strings **SHOULD** use [continuations](#continuation) if the line is getting too long.
```cte
[
    "All that most maddens and torments; all that stirs up the lees of things; \
    all truth with malice in it; all that cracks the sinews and cakes the brain; \
    all the subtle demonisms of life and thought; all evil, to crazy Ahab, were \
    visibly personified, and made practically assailable in Moby Dick. He piled \
    upon the whale's white hump the sum of all the general rage and hate felt by \
    his whole race from Adam down; and then, as if his chest had been a mortar, \
    he burst his hot heart's shell upon it."
]
```


#### Typed Arrays

Typed arrays **SHOULD** be broken up into multiple indented lines if the line is getting too long.
```cte
|u16x aa5c 5e0f e9a7 b65b 3572 96ec da16 6496 6133 5aa1 687f 9ce0 4d10 a39e 3bd3
      bf96 ad12 e64b 298f e137 a99f 5fb8 a8ca e8e7 0595 bc2f 4b64 8b0e 895d ebe7
      fb59 fdb0 1d93 5747 239d b16f 7d9c c48b 5581 13ba 19ca 6f3b 4ba9|
```


#### Comments

Comments **SHOULD** have one space (u+0020) after the comment opening sequence. Multiline-style comments (`/* */`) **SHOULD** also have a space before the closing sequence.
```cte
// abc

/* abc */
```

Long comments **SHOULD** be broken up to fit within the right margin.
```cte
{
    /* When a comment gets too long to fit within the right margin, place
       the overflow on a separate lines, adjusting the indent to keep
       everything aligned. */
}
```

The object following a comment **SHOULD** be on a different line.
```cte
{
    /* See list of request types in request-types.md */
    request-type = ping
}
```



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
