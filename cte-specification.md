Concise Text Encoding
=====================


Version
-------

Version 0 (prerelease)



This Document
-------------

This document describes the Concise Text Encoding (CTE) format, and how codecs for this format must behave.

 * The Concise Binary Encoding (CBE) format is described in [cbe-specification.md](cbe-specification.md).
 * The logical structure of Concise Encoding is described in [ce-structure.md](ce-structure.md).



Contents
--------

- [Concise Text Encoding](#concise-text-encoding)
  - [Version](#version)
  - [This Document](#this-document)
  - [Contents](#contents)
  - [Terms and Conventions](#terms-and-conventions)
  - [What is Concise Text Encoding?](#what-is-concise-text-encoding)
  - [Text Format](#text-format)
    - [Lookalike Characters](#lookalike-characters)
    - [Line Endings](#line-endings)
    - [Human Editability](#human-editability)
  - [Document Structure](#document-structure)
  - [Document Version Specifier](#document-version-specifier)
  - [Numeric Types](#numeric-types)
    - [Boolean](#boolean)
    - [Integer](#integer)
    - [Floating Point](#floating-point)
      - [Base-10 Notation](#base-10-notation)
      - [Base-16 Notation](#base-16-notation)
      - [Special Floating Point Values](#special-floating-point-values)
      - [Floating Point Rules](#floating-point-rules)
    - [Numeric Whitespace](#numeric-whitespace)
    - [UID](#uid)
  - [Temporal Types](#temporal-types)
    - [Date](#date)
    - [Time](#time)
    - [Timestamp](#timestamp)
    - [Time Zones](#time-zones)
      - [Area/Location](#arealocation)
      - [Global Coordinates](#global-coordinates)
      - [UTC](#utc)
      - [UTC Offset](#utc-offset)
    - [Why not ISO 8601 or RFC 3339?](#why-not-iso-8601-or-rfc-3339)
  - [Array Types](#array-types)
    - [String-Like Arrays](#string-like-arrays)
      - [String-Like Array Validation](#string-like-array-validation)
      - [Escape Sequences](#escape-sequences)
      - [Continuation](#continuation)
      - [Unicode Codepoint](#unicode-codepoint)
      - [Verbatim Sequence](#verbatim-sequence)
      - [String](#string)
      - [Resource Identifier](#resource-identifier)
    - [General Array Encoding](#general-array-encoding)
      - [General Array Forms](#general-array-forms)
      - [General Array Types](#general-array-types)
      - [Implied Prefix](#implied-prefix)
      - [Bit Array Elements](#bit-array-elements)
      - [Float Array Elements](#float-array-elements)
      - [Media](#media)
        - [Media Contents](#media-contents)
      - [Custom Types](#custom-types)
  - [Container Types](#container-types)
    - [List](#list)
    - [Map](#map)
    - [Struct Instance](#struct-instance)
    - [Edge](#edge)
    - [Node](#node)
  - [Other Types](#other-types)
    - [Null](#null)
  - [Pseudo-Objects](#pseudo-objects)
    - [Local Reference](#local-reference)
    - [Remote Reference](#remote-reference)
  - [Invisible Objects](#invisible-objects)
    - [Comment](#comment)
      - [Single Line Comment](#single-line-comment)
      - [Multiline Comment](#multiline-comment)
    - [Padding](#padding)
  - [Structural Objects](#structural-objects)
    - [Struct Template](#struct-template)
    - [Marker](#marker)
  - [Empty Document](#empty-document)
  - [Letter Case](#letter-case)
    - [Overriding Rule for Decoders](#overriding-rule-for-decoders)
  - [Structural Whitespace](#structural-whitespace)
  - [Pretty Printing](#pretty-printing)
    - [Right Margin](#right-margin)
    - [Indentation](#indentation)
    - [Pretty Printing Lists](#pretty-printing-lists)
    - [Pretty Printing Maps](#pretty-printing-maps)
    - [Pretty Printing Struct Templates](#pretty-printing-struct-templates)
    - [Pretty Printing Struct Instances](#pretty-printing-struct-instances)
    - [Pretty Printing Nodes](#pretty-printing-nodes)
    - [Pretty Printing Edges](#pretty-printing-edges)
    - [Pretty Printing Strings](#pretty-printing-strings)
    - [Pretty Printing Primitive Type Arrays](#pretty-printing-primitive-type-arrays)
    - [Pretty Printing Comments](#pretty-printing-comments)
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



What is Concise Text Encoding?
------------------------------

Concise Text Encoding (CTE) is the text variant of [Concise Encoding](ce-structure.md): a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

The text format aims to present data in a human friendly way, encoding data in a manner that can be easily and unambiguously edited in a UTF-8 capable text editor while maintaining 1:1 compatibility with the [binary format](cbe-specification.md) (which aims for compactness and machine processing efficiency).



Text Format
-----------

A CTE document is a UTF-8 encoded text document containing data arranged in an ad-hoc hierarchical fashion.

All text in a CTE document **MUST** be [CTE safe](ce-structure.md#character-safety). Validation of CTE safety **MUST** occur _before_ all other processing (such as [escape-sequence](#escape-sequences) decoding).

[Structural whitespace](#structural-whitespace) is used to separate structural elements in a document (such as container contents). In [maps](#map), the key and value portions of a key-value pair are separated by an equals character (`=`) and possible [structural whitespace](#structural-whitespace). The key-value pairs themselves are separated by [structural whitespace](#structural-whitespace). Extraneous [structural whitespace](#structural-whitespace) is ignored.

**Examples**:

 * CTE v1 empty document: `c1 null`
 * CTE v1 document containing the top-level integer value 1000: `c1 1000`
 * CTE v1 document containing a top-level list: `c1 ["a" "b" "c"]`
 * CTE v1 document containing a top-level map: `c1 {"a"=1 "b"=2 "c"=3}`


### Lookalike Characters

Lookalike characters are characters that look confusingly similar to [string-like array](#string-like-arrays) delimiters when viewed by a human. Such characters **MUST** be escaped in [string-like arrays](#string-like-arrays).

| Lookalike      | Escaped             |
| -------------- | ------------------- |
| `"A” string"`  | `"A\{201d} string"` |

The unicode.org site provides an [online utility](https://util.unicode.org/UnicodeJsps/confusables.jsp?a=%22&r=None) to find confusable characters.


The following is (as of 2022-06-09) a complete list of [lookalike Unicode characters](https://util.unicode.org/UnicodeJsps/confusables.jsp). This list may change as the Unicode character set evolves over time. Codec developers **MUST** keep their implementation up to date with the latest lookalike characters.

| Character | Lookalikes (codepoints)                                                |
| --------- | ---------------------------------------------------------------------- |
| `"` | [02ba, 02dd, 02ee, 02f6, 05f2, 05f4, 1cd3, 201c, 201d, 201f, 2033, 2034, 2036, 2037, 2057, 3003, ff02](https://util.unicode.org/UnicodeJsps/confusables.jsp?a=%22&r=None) |
| `\` | [2216, 27cd, 29f5, 29f9, 20f2, 3035, 31d4, 4e36, fe68, ff3c, 1d20f, 1d23b](https://util.unicode.org/UnicodeJsps/confusables.jsp?a=%5C&r=None) |


### Line Endings

Line endings **CAN** be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for document sharing purposes the canonical format is LF only.

 * Decoders **MUST** accept both line ending types as input.
 * Encoders **MUST** output LF only by default. Encoders **MAY** offer an option to output CR+LF line endings.


### Human Editability

In the spirit of human editability:

 * Implementations **SHOULD** avoid outputting characters that different editors tend to display inconsistently (for example, TAB).
 * Line lengths **SHOULD** be kept within reasonable amounts in order to avoid excessive horizontal scrolling in an editor.



Document Structure
------------------

Documents begin with a [version specifier](#document-version-specifier), possibly followed by [invisible](ce-structure.md#invisible-objects) and [structural](ce-structure.md#structural-objects) objects, and then ultimately followed by the top-level [data object](ce-structure.md#data-objects).

    (version specifier) (optional invisible and structural objects) (top-level data object)



Document Version Specifier
--------------------------

The version specifier is composed of the character `c` (u+0063), followed immediately by an unsigned integer version number. The document **MUST NOT** begin with a byte order mark (BOM), and there **MUST NOT** be anything (whitespace or otherwise) between the `c` and the version number.

**Note**: Due to the [overriding letter case rule for decoders](#overriding-rule-for-decoders), a decoder **MUST** also accept uppercase `C` (u+0043).

The version specifier and the top-level object **MUST** be separated by [structural whitespace](#structural-whitespace).

**Example**:

* Version specifier (CTE version 1): `c1`
* Complete (and empty) document: `c1 null`



Numeric Types
-------------

### Boolean

Represented by the text sequences `true` and `false`.


### Integer

Integer values **CAN** be positive or negative, and **CAN** be represented in various bases. Negative values **MUST** be prefixed with a dash `-` as a sign character. Encoders **MUST** write values in lower case.

Integers **CAN** be specified in base 2, 8, 10, or 16. Bases other than 10 require a prefix:

| Base | Name        | Digits           | Prefix | Example      | Decimal Equivalent |
| ---- | ----------- | ---------------- | ------ | ------------ | ------------------ |
|   2  | Binary      | 01               | `0b`   | `-0b1100`    | -12                |
|   8  | Octal       | 01234567         | `0o`   | `0o755`      | 493                |
|  10  | Decimal     | 0123456789       |        | `900000`     | 900000             |
|  16  | Hexadecimal | 0123456789abcdef | `0x`   | `0xdeadbeef` | 3735928559         |

Encoders **MUST** output integers in base 10 by default.


### Floating Point

A floating point number is composed of an implied base (signified by an **OPTIONAL** prefix), a significand portion (composed of a whole part and an **OPTIONAL** fractional part), and an **OPTIONAL** exponential portion, such that the value is calculated as:

    value = significand × baseᵉˣᵖᵒⁿᵉⁿᵗ

 * The significand and exponential portions are separated by an exponent marker (either `e` or `p`, depending on the base).
 * The whole and fractional parts of the significand are separated by a radix point (`.`).
 * A dash `-` **MUST** be prepended to the front of negative floating point value as the sign indicator (before any other prefix).

**Note**: A float value that contains only a whole significand portion (fractional part = 0 and exponent = 1) will be interpreted/printed as an integer.

Encoders **MUST** output decimal float values in [base-10 notation](#base-10-notation) and binary float values in [base-16 notation](#base-16-notation) by default.

**Examples**:

```cte
c1
[
    1.0
    5e-5
    -98.413e50
    3.14
]
```

#### Base-10 Notation

Base-10 notation is used to represent [decimal floating point numbers](ce-structure.md#decimal-floating-point).

The exponential portion of a base-10 number is denoted by the character `e`, followed by the signed size of the exponent (using **OPTIONAL** `+` for positive, and mandatory `-` for negative). The exponential portion is a signed base-10 number representing the power-of-10 to multiply the significand by. Values **SHOULD** be normalized (only one digit to the left of the decimal point) when using exponential notation.

    value = significand × 10ᵉˣᵖᵒⁿᵉⁿᵗ

Although there is technically no maximum number of significant digits or exponent digits for base-10 floating point notation, care should be taken to ensure that the receiving end will be able to store the value. For example, the 64-bit ieee754 floating point type can represent values with up to 16 significant digits and an exponent range roughly from 10⁻³⁰⁷ to 10³⁰⁷.

**Examples**:

```cte
c1
[
    6.411e+9 // 6411000000
    6.411e9  // 6411000000
    6411e6   // 6411000000
    6.411e-9 // 0.000000006411
]
```

#### Base-16 Notation

Base-16 notation is used to represent [binary floating point numbers](ce-structure.md#binary-floating-point) because it allows 100% accurate representation of the actual value.

Base-16 notation **MUST** have a prefix of `0x`, and the exponential portion is denoted by the character `p`. The exponential portion is a signed base-10 number representing the power-of-2 to multiply the significand by. The exponent's sign character **CAN** be omitted if it's positive. Values **SHOULD** be normalized.

    value = significand × 2ᵉˣᵖᵒⁿᵉⁿᵗ

To maintain compatibility with [CBE](cbe-specification.md), values in base-16 notation **MUST NOT** exceed the range of ieee754 64-bit binary float. A value outside of this range is a [data error](ce-structure.md#data-errors).

**Examples**:

```cte
c1
[
    0xa.3fb8p+42 // a.3fb8 x 2⁴²
    -0x1p0       // -1
]
```

#### Special Floating Point Values

```cte
c1
[
    inf  // Infinity
    -inf // Negative Infinity
    nan  // Not a Number (quiet)
    snan // Not a Number (signaling)
]
```

#### Floating Point Rules

 * Encoders **MUST** output exponent portion markers in lowercase only (`e` or `p`).
 * Decoders **MUST** accept uppercase (`E` or `P`) and lowercase (`e` or `p`) exponent portion markers.
 * There **MUST** be at least one digit on each side of a radix point (`.`) when present.

| Invalid      | Valid     | Notes                                                    |
| ------------ | --------- | -------------------------------------------------------- |
| `-1.`        | `-1.0`    | Or just use the integer value `-1`                       |
| `.1`         | `0.1`     |                                                          |
| `.218901e+2` | `21.8901` | Or `2.18901e+1`, or `0.218901e+2`                        |


### Numeric Whitespace

The `_` character **CAN** be used as "numeric whitespace" when encoding integers and floating point numbers. Other [whitespace](#structural-whitespace) characters are not allowed.

CTE encoders **MAY** offer configuration options to output numeric whitespace, but **MUST** by default output no numeric whitespace.

Rules:

 * Only [integer](#integer) and [floating point](#floating-point) types **CAN** contain numeric whitespace.
 * [Special floating point values](#special-floating-point-values) **MUST NOT** contain numeric whitespace.
 * Numeric whitespace **CAN** only occur between two adjacent numeric digits (`0`-`9`, `a`-`f`, depending on numeric base).
 * Numeric whitespace characters **MUST** be ignored when decoding numeric values.

**Examples**:

Valid:

```cte
c1
[
    1_000_000        // 1000000
    4_3.5_5_4e9_0    // 43.554e90
    -0xa.fee_31p1_00 // -0xa.fee31p100
]
```

Invalid:

 * `_1000000`
 * `1000000_`
 * `43_.554e90`
 * `43,_554e90`
 * `43.554_e90`
 * `-_43.554e90`
 * `-_0xa.fee31p100`
 * `-0xa.fee31p_100`
 * `-0_xa.fee31p100`


### UID

An [rfc4122 UUID string representation](https://tools.ietf.org/html/rfc4122).

**Example**:

```cte
c1
123e4567-e89b-12d3-a456-426655440000
```


Temporal Types
--------------

Temporal types record time with varying degrees of precision.

Fields other than year **CAN** be pre-padded with zeros (`0`) up to their maximum allowed digits.


### Date

A date is made up of the following fields, separated by a dash character (`-`):

| Field | Mandatory | Min Value | Max Value | Min Digits | Max Digits |
| ----- | --------- | --------- | --------- | ---------- | ---------- |
| Year  |     Y     |        -∞ |         ∞ |          1 |          ∞ |
| Month |     Y     |         1 |        12 |          1 |          2 |
| Day   |     Y     |         1 |        31 |          1 |          2 |

 * BC years are prefixed with a dash (`-`).

**Examples**:

```cte
c1
[
    2019-8-5   // August 5, 2019
    5081-03-30 // March 30, 5081
    -300-12-21 // December 21, 300 BC (proleptic Gregorian)
]
```


### Time

A time is made up of the following mandatory and **OPTIONAL** fields:

| Field        | Mandatory | Separator  | Min Value | Max Value | Min Digits | Max Digits |
| ------------ | --------- | ---------- | --------- | --------- | ---------- | ---------- |
| Hour         |     Y     |            |         0 |        23 |          1 |          2 |
| Minute       |     Y     |    `:`     |         0 |        59 |          2 |          2 |
| Second       |     Y     |    `:`     |         0 |        60 |          2 |          2 |
| Subseconds   |     N     |    `.`     |         0 | 999999999 |          0 |          9 |
| Time Zone    |     N     |    `/`     |         - |         - |          - |          - |

**Examples**:

```cte
c1
[
    09:04:21                // 9:04:21 UTC
    23:59:59.999999999      // 23:59:59 and 999999999 nanoseconds UTC
    12:05:50.102/Z          // 12:05:50 and 102 milliseconds UTC
    4:00:00/Asia/Tokyo      // 4:00:00 Tokyo time
    17:41:03/-13.54/-172.36 // 17:41:03 Samoa time
    9:00:00/L               // 9:00:00 local time
]
```


### Timestamp

A timestamp combines a date and a time, separated by a slash character (`/`).

**Examples**:

```cte
c1
[
    2019-01-23/14:08:51.941245            // January 23, 2019, at 14:08:51 and 941245 microseconds, UTC
    1985-10-26/01:20:01.105/M/Los_Angeles // October 26, 1985, at 1:20:01 and 105 milliseconds, Los Angeles time
    5192-11-01/03:00:00/48.86/2.36        // November 1st, 5192, at 3:00:00, at whatever is in the place of Paris at that time
]
```

### Time Zones

The time zone is an **OPTIONAL** field. If omitted, it is assumed to be `Zero` (UTC).

#### Area/Location

An area/location time zone is written in the form `Area/Location`.

**Examples**:

 * `E/Paris`
 * `Asia/Tokyo`
 * `America/Indiana/Petersburg` (which has area `America` and location `Indiana/Petersburg`)
 * `Etc/UTC` == `Zero` == `Z`
 * `L`

#### Global Coordinates

Global coordinates are written as latitude and longitude in degrees to a precision of hundredths, separated by a slash character (`/`).

 * Negative values **MUST** be prefixed with a dash character (`-`)
 * A period (`.`) is used as a fractional separator.

**Examples**:

 * `50.45/30.52` (Kyiv, near Independence Square)
 * `-13.53/-172.37` (Samoa)

#### UTC

Simply omit the time zone entirely, which causes the time zone to default to UTC.

#### UTC Offset

UTC offsets are recorded by using a `+` (for positive offsets) or `-` (for negative offsets) character as the time zone separator instead of the `/` character, with the hours and minutes given in the form `hhmm`.

**Examples (using timestamps)**:

```cte
c1
[
    1985-10-26/01:20:01.105+0700
    2000-01-14/10:22:00-0200
]
```


### Why not ISO 8601 or RFC 3339?

[RFC 3339](https://datatracker.ietf.org/doc/html/rfc3339) was developed as a greatly simplified profile of [ISO 8601](https://en.wikipedia.org/wiki/ISO_8601) to be used in internet protocols. RFC 3339's criticisms of ISO 8601 are valid:

- It has bad defaults.
- It tries to do too many things.
- It has too much optional functionality (most of which is unused in the real world).
- It's non-free, making it even harder to write compliant, bug-free implementations (or trust any that claim to be).

This renders ISO 8601 overcomplicated and prone to misinterpretation, incompatibilities, and bugs.

RFC 3339 is a much simpler design for timestamped internet events, and is well suited to that purpose. However, it lacks functionality that a general purpose date format would require:

- It only supports time offsets (+01:00, -13:00, etc), not real time zones.
- It doesn't support BCE dates.
- It allows multiple interpretations of year values less than 4 digits long, which is a security risk and a source of bugs.



Array Types
-----------

Array types are encoded in two primary forms: [general form](#general-array-encoding) and [string-like form](#string-like-arrays).


### String-Like Arrays

String-like arrays are a special convenience encoding for commonly used string types.

A string-like array **MUST** contain only valid UTF-8 characters. The contents are enclosed within double-quote (`"`) delimiters. All characters leading up to the closing double-quote (including whitespace) are considered part of the string sequence. String-like arrays **CAN** contain [escape sequences](#escape-sequences).

    "contents"

[CTE unsafe](ce-structure.md#character-safety) characters **MUST** always be escaped.

Double-quotes (`"`) and backslash (`\`) (as well as their [lookalikes](#lookalike-characters)) **MUST** be encoded as [escape sequences](#escape-sequences) (except when inside of a [verbatim sequence](#verbatim-sequence)). TAB, CR and LF **SHOULD** be escaped as well.

#### String-Like Array Validation

Validating a string-like array in CTE is a multi-step process:

1. Validate the raw string contents for [CTE safety](ce-structure.md#character-safety).
2. Decode all [escape sequences](#escape-sequences) to produce a final string.
3. [Validate the final string](ce-structure.md#string-like-arrays).

#### Escape Sequences

Within [string-like arrays](#string-like-arrays), escape sequences **CAN** be used to encode data that would otherwise be cumbersome or impossible to represent. Backslash (`\`) acts as an escape sequence initiator, followed by an escape type character and possible payload data depending on the type:

| Escape Type Character    | Interpretation                          |
| ------------------------ | --------------------------------------- |
| u+0074 (`t`)             | horizontal tab (u+0009)                 |
| u+006e (`n`)             | linefeed (u+000a)                       |
| u+0072 (`r`)             | carriage return (u+000d)                |
| u+0022 (`"`)             | double quote (u+0022)                   |
| u+002a (`*`)             | asterisk (u+002a)                       |
| u+002f (`/`)             | slash (u+002f)                          |
| u+005c (`\`)             | backslash (u+005c)                      |
| u+005f (`_`)             | [non-breaking space](https://en.wikipedia.org/wiki/Non-breaking_space) (u+00a0) |
| u+002d (`-`)             | [soft hyphen](https://en.wikipedia.org/wiki/Soft_hyphen) (u+00ad) |
| u+000a (linefeed)        | [continuation](#continuation)           |
| u+000d (carriage return) | [continuation](#continuation)           |
| u+002b (`{`)             | [Unicode codepoint](#unicode-codepoint) |
| u+002e (`.`)             | [verbatim sequence](#verbatim-sequence) |

#### Continuation

A continuation escape sequence causes the decoder to ignore all [structural whitespace](#structural-whitespace) characters until it encounters a character that is not [structural whitespace](#structural-whitespace). The escape character (`\`) followed by either LF (u+000a) or CR (u+000d) initiates a continuation.

**Example**:

```cte
c1 "The only people for me are the mad ones, the ones who are mad to live, mad to talk, \
     mad to be saved, desirous of everything at the same time, the ones who never yawn or \
     say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles \
     exploding like spiders across the stars."
```

The above string is interpreted as:

    The only people for me are the mad ones, the ones who are mad to live, mad to talk, mad to be saved, desirous of everything at the same time, the ones who never yawn or say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles exploding like spiders across the stars.

#### Unicode Codepoint

A Unicode codepoint escape sequence represents a single Unicode character as a hexadecimal codepoint.

The escape sequence begins with a backslash (`\`) character, followed by an opening curly brace (`{`), followed by any number of hexadecimal digits representing the codepoint, and is finally terminated by a closing curly brace (`}`). Leading zeroes are allowed for stylistic purposes (e.g. `\{0020}`).

**Warning**: Decoders **MUST NOT** allow codepoints to overflow (e.g. `\{10000000000000020}` overflowing a uint32 or uint64 accumulator to produce codepoint 0x20). An out-of-range codepoint is a [data error](ce-structure#data-errors).

**Examples**:

| Sequence   | Digits | Codepoint | Character     |
| ---------- | ------ | --------- | ------------- |
| `\{c}`     | 1      | u+000c    | Form Feed     |
| `\{df}`    | 2      | u+00df    | ß  (Eszett)   |
| `\{101}`   | 3      | u+0101    | ā  (a macron) |
| `\{2191}`  | 4      | u+2191    | ↑  (up arrow) |
| `\{1f415}` | 5      | u+1f415   | 🐕 (dog)      |

`"gro\{df}e"` = `"große"`

#### Verbatim Sequence

A verbatim escape sequence works like a ["here document"](https://en.wikipedia.org/wiki/Here_document). It's composed as follows:

 * Verbatim sequence escape initiator (`\.`).
 * An end-of-sequence sentinel, which is a sequence of [CTE safe](ce-structure.md#character-safety), non-whitespace characters.
 * An unescaped whitespace terminator to terminate the end-of-sequence sentinel (either: SPACE `u+0020`, LF `u+000a`, or CR+LF `u+000d u+000a`).
 * The string contents.
 * A second instance of the end-of-sequence sentinel (without whitespace terminator). Note: Unlike in many languages, this sequence does _not_ have to occur alone on its own line.

**Notes**:

 * Verbatim sequence sentinels are **case sensitive**.
 * TAB (`u+0009`) **MUST NOT** be used as an end-of-sequence sentinel terminator because any editor that converts tabs to spaces would effectively alter the verbatim contents (only the first space would terminate the sentinel; the other spaces would become part of the verbatim data).
 * CR alone (without a following LF) **MUST NOT** be used as an end-of-sequence sentinel terminator. Decoders **MUST NOT** stop processing a sentinel after only reading a CR character; they **MUST** verify that a LF follows and then discard the whole CR+LF sequence before stopping. Failure to do so would cause an LF to be included as part of the verbatim data.
 * A malformed sentinel terminator is a [structural error](ce-structure.md#structural-errors).

**Example**:

```cte
c1
"Verbatim sequences can occur anywhere escapes are allowed.\n\.@@@
In verbatim sequences, everything is interpreted literally until the
end-of-string sentinel is encountered (in this case three @ characters).

Characters like " and \ are no longer special: \n and \t appear as-is.

Continuations are also not processed in a verbatim sequence. \
          For example, this line really is indented 10 spaces.

Normal processing resumes after the terminator@@@, so escape sequences\nare once again \
          interpreted."
```

Which decodes to:

```text
Verbatim sequences can occur anywhere escapes are allowed.
In verbatim sequences, everything is interpreted literally until the
end-of-string sentinel is encountered (in this case three @ characters).

Characters like " and \ are no longer special: \n and \t appear as-is.

Continuations are also not processed in a verbatim sequence. \
          For example, this line really is indented 10 spaces.

Normal processing resumes after the terminator, so escape sequences
are once again interpreted.
```

#### String

A string is encoded as a [string-like array](#string-like-arrays), enclosed within double-quote delimiters (`"`).

**Example**:

```cte
c1
"Line 1\nLine 2\nLine 3"
```

Which is a document containing the string:

```text
Line 1
Line 2
Line 3
```

#### Resource Identifier

A resource identifier is encoded as a [string-like array](#string-like-arrays), enclosed within double-quote delimiters (`"`) and prefixed with an at symbol (`@`).

    @"contents"

**Note**: A decoder **MUST** interpret only [CTE escape sequences](#escape-sequences). Resource-specific escape sequences (such as [percent encoding](https://en.wikipedia.org/wiki/Percent-encoding)) **MUST NOT** be interpreted.

**Example**:

```cte
c1
[
    // CTE decodes this as `http://x.y.z?quote="`
    // The application layer interprets it as `http://x.y.z?quote="`
    @"http://x.y.z?quote=\""

    // CTE decodes this as `http://x.y.z?quote=%22`
    // The application layer interprets it as `http://x.y.z?quote="`
    @"http://x.y.z?quote=%22"
]
```


### General Array Encoding

The general form of array encoding consists of an array type and array contents separated by [structural whitespace](#structural-whitespace), all enclosed between pipe (`|`) characters:

    |array-type array-contents|

The array type field **MUST** only contain characters that are allowed in [media types](https://www.rfc-editor.org/rfc/rfc6838#section-4.2).

#### General Array Forms

Depending on the kind of array, the contents are encoded in elemental form as [whitespace](#structural-whitespace)-separated elements, or in string form as quoted string contents following the same rules as [string-like arrays](#string-like-arrays):

    |type elem1 elem2 elem3 ...|  // elemental form
    |type "string-like contents"| // string form

An empty array has a type but no contents:

    |type|

In elemental form, array elements **CAN** be written using any valid representation of the array's element type and size:

```cte
c1
[
    |i16 -1000 1000 15000|
    |f32 1.5 3.914e+20 nan|
]
```

#### General Array Types

The following array types are available:

| Denoted Type | Prefix? | Description                   | Encoding Kind          |
| ------------ | ------- | ----------------------------- | ---------------------- |
| `b`          |         | Bit                           | Element                |
| `u8`         | b, o, x | 8-bit unsigned integer        | Element                |
| `u16`        | b, o, x | 16-bit unsigned integer       | Element                |
| `u32`        | b, o, x | 32-bit unsigned integer       | Element                |
| `u64`        | b, o, x | 64-bit unsigned integer       | Element                |
| `i8`         | b, o, x | 8-bit signed integer          | Element                |
| `i16`        | b, o, x | 16-bit signed integer         | Element                |
| `i32`        | b, o, x | 32-bit signed integer         | Element                |
| `i64`        | b, o, x | 64-bit signed integer         | Element                |
| `f16`        | x       | 16-bit binary float (bfloat)  | Element                |
| `f32`        | x       | 32-bit binary float (ieee754) | Element                |
| `f64`        | x       | 64-bit binary float (ieee754) | Element                |
| `u`          |         | 128-bit UID                   | Element                |
| `c<id>`      |         | [Custom Types](#custom-types) | Element or string-Like |
| `<type/sub>` |         | [Media](#media)               | Element or string-Like |

Array types are [case-insensitive](#letter-case).

If an array type field contains a slash (`/`), it **MUST** be interpreted as a media object (e.g. `|a/b "stuff"|` is a media object, regardless of whether media type "a/b" is known or [registered](http://www.iana.org/assignments/media-types/media-types.xhtml)). If the array type field does not contain a slash, it **MUST NOT** be interpreted as a media object.

An invalid array type field is a [data error](ce-structure#data-errors).

For some types, array elements **MAY** be represented in different bases by applying a prefix (`0b`, `0o`, `0x`) to the element.

#### Implied Prefix

**OPTIONALLY**, a suffix **CAN** be appended to the type specifier itself (if the type supports it) to indicate that _all_ element values **MUST** be considered to have an implicit prefix (except for [special float values](#special-floating-point-values)).

| Type Suffix | Implied element prefix | Example                         |
| ----------- | ---------------------- | ------------------------------- |
| `b`         | `0b`                   | `\|u8b 10011010 00010101\|`     |
| `o`         | `0o`                   | `\|i16o -7445 644\|`            |
| `x`         | `0x`                   | `\|f32x a.c9fp20 -1.ffe9p-40\|` |

CTE encoders **MUST NOT** use implied prefix arrays by default.

CTE encoders **MUST** output integer array elements of non-implied arrays in [base-10 notation](#base-10-notation) by default.

**Examples**:

```cte
c1
[
    |u8x 9f 47 cb 9a 3c|
    |f32 1.5 0x4.f391p100 30 9.31e-30|
    |i16 0b1001010 0o744 1000 0x7fff|
    |u 3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f|
    |b 11010|
]
```

#### Bit Array Elements

Bit array elements are represented using `0` for false and `1` for true. [Whitespace](#structural-whitespace) is **OPTIONAL** when encoding a bit array:

```cte
c1
[
    |b 1 0 0 1| // bits 1, 0, 0, 1
    |b 1001|    // bits 1, 0, 0, 1
    |b 10 01|   // bits 1, 0, 0, 1
]
```

#### Float Array Elements

Like their standalone counterparts, [special float values](#special-floating-point-values) are allowed in floating point arrays:

    |f32 0x1.5da nan -inf 0xc.1f3p38|

Float array element values written in decimal form will be **silently rounded** as they're converted to binary floats. This is unavoidable due to differences in float parsers on different platforms, and is another reason why you should always use [CBE](cbe-specification.md) instead of CTE when ingesting data from an untrusted source (see [security and limits](ce-structure.md#security-and-limits)).

Float arrays **MAY** be written using the [implied prefix](#implied-prefix) `x`.

CTE encoders **MUST NOT** use implied prefix arrays by default.

CTE encoders **MUST** output float array elements in [base-16 notation](#base-16-notation) by default.


#### Media

In media objects, the [array type field](#general-array-types) is the [media type](http://www.iana.org/assignments/media-types/media-types.xhtml), and the [contents](#media-contents) field contains the media data:

```cte
c1
[
    |text/plain "stuff"|        // text encoded media
    |text/plain 73 74 75 66 66| // byte encoded media
    |text/plain|                // empty media
]
```

##### Media Contents

If the actual media contents consists only of valid UTF-8 text, it **CAN** be represented in [string form](#general-array-forms). Otherwise, it **MUST** be represented in binary form using hex byte values as if it were a `u8x` array:

* As text: `|type/subtype "contents"|`
* As binary: `|type/subtype 63 6f 6e 74 65 6e 74 73|`

**Example**:

```cte
c1
|application/x-sh 23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a|
```

Which is equivalent to:

```cte
c1
|application/x-sh "\.@@
#!/bin/sh

echo hello world
@@"|
```

Both of the above examples represent a document containing the shell script:

```sh
#!/bin/sh

echo hello world
```


#### Custom Types

In custom objects, the type field is a concatenation of the letter `c` and the unsigned integer custom type, and can have a [binary or textual form](#general-array-forms).

The binary form is encoded like a u8x array (hex encoded byte elements).

**Example: Binary encoded custom type value with type code 99**:

```cte
c1
|c99 01 f6 28 3c 40 00 00 40 40|
```

The textual form is encoded using a quoted string.

**Example: Text encoded custom type value with type code 99**:

```cte
c1
|c99 "2.94+3i"|
```



Container Types
---------------

### List

A list begins with an opening square bracket `[`, contains [structural whitespace](#structural-whitespace) separated contents, and finishes with a closing square bracket `]`.

**Example**:

```cte
c1
[
    1
    "two"
    3.1
    {}
]
```


### Map

A map begins with an opening curly brace `{`, contains [structural whitespace](#structural-whitespace) separated key-value pairs, and finishes with a closing curly brace `}`.

Map entries are split into key-value pairs using the equals `=` character and **OPTIONAL** [structural whitespace](#structural-whitespace). Key-value pairs **MUST** be separated from each other using [structural whitespace](#structural-whitespace). A key without a paired value is invalid.

**Example**:

```cte
c1
{
    1 = "alpha"
    2 = "beta"
    "a map" = {"one"=1 "two"=2}
}
```


### Struct Instance

A struct instance begins with `@`, followed by a template [identifier](ce-structure.md#identifier), followed by `(`, followed by a series of [whitespace](#structural-whitespace) separated values in the same order that their keys are defined in the associated template, and is terminated with `)`.

There **MUST NOT** be whitespace at any point between the `@` and the opening `(` (e.g. `@my_struct(...)`, **not** `@ my_struct (...)`).

**Example**:

```cte
c1
[
    @vehicle<"make"       "model"      "drive" "sunroof">
    @vehicle("Ford"       "Explorer"   "4wd"   true     )
    @vehicle("Toyota"     "Corolla"    "fwd"   false    )
    @vehicle("Honda"      "Civic"      "fwd"   false    )
    @vehicle("Alfa Romeo" "Giulia 952" "awd"   true     )
]
```


### Edge

An edge container is composed of the delimiters `@(` and `)`, containing the whitespace separated source, description, and destination.

Edges are most commonly used in conjunction with [references](#local-reference) or [resource identifiers](#resource-identifier) to produce arbitrary graphs.

**Examples**:

Weighted graph edge:

```cte
c1
{
    "vertices" = [
      &a:{}
      &b:{}
    ]
    "edges" = [
      @($a 200 $b)
    ]
}
```

Relationship graph edge:

```cte
c1
@(
    @"https://springfield.gov/people#homer_simpson"
    @"https://example.org/wife"
    @"https://springfield.gov/people#marge_simpson"
)
```


### Node

A node begins with an opening parenthesis `(`, contains a value (object) followed by zero or more whitespace separated child nodes, and is closed with a closing parenthesis `)`.

Nodes are recorded in **depth-first**, **node-right-left** order, which ensures that the textual representation looks like the actual tree structure it describes when rotated 90 degrees clockwise.

**Example**:

```cte
c1
// The tree structure:
//        2
//       / \
//      5   7
//     /   /|\
//    9   6 1 2
//   /   / \
//  4   8   5
//
(2
    (7
        2
        1
        (6
            5
            8
        )
    )
    (5
        (9
            4
        )
    )
)
```

Viewed rotated, it resembles the tree it describes:

![tree rotated](img/tree-rotated.svg)



Other Types
-----------

### Null

Null is encoded as `null`.



Pseudo-Objects
--------------

### Local Reference

A local reference begins with a reference initiator (`$`), followed immediately (with no whitespace) by a marker [identifier](ce-structure.md#identifier) that has been defined elsewhere in the current document.

**Example**:

```cte
c1
{
    "some_object" = {
        "my_string" = &remember_me:"Remember this string"
        "my_map" = &1:{
            "a" = 1
        }
    }

    "reference_to_string" = $remember_me
    "reference_to_map" = $1
}
```


### Remote Reference

A remote reference is encoded as a [string-like array](#string-like-arrays), enclosed within double-quote delimiters (`"`) and prefixed with a reference initiator (`$`).

    $"https://www.ietf.org/"

**Example**:

```cte
c1
{
    "reference_to_local_doc" = $"common.cte"
    "reference_to_remote_doc" = $"https://somewhere.com/my_document.cbe?format=long"
    "reference_to_local_doc_marker" = $"common.cte#legalese"
    "reference_to_remote_doc_marker" = $"https://somewhere.com/my_document.cbe?format=long#examples"
}
```



Invisible Objects
-----------------

### Comment

Comments **CAN** be written in single-line or multi-line form, and do not process escape sequences.

Comments **MUST ONLY** contain [CTE safe](ce-structure.md#character-safety) characters.

#### Single Line Comment

A single line comment begins at the sequence `//` and continues until the next LF (u+000a) or CR (u+000d) is encountered. No checks for nested comments are performed.

#### Multiline Comment

A multiline comment (aka block comment) begins at the sequence `/*` and is terminated by the sequence `*/`. Multiline comments support nesting, meaning that further `/*` sequences inside the comment will start subcomments that **MUST** also be terminated by their own `*/` sequence. No processing of the comment contents other than detecting comment begin and comment end is peformed.

**Note**: Commenting out strings containing `/*` or `*/` could potentially cause parse errors because the parser won't have any contextual information about the sequences, and will simply treat them as "comment begin" and "comment end". This edge case could be mitigated by pre-emptively escaping all occurrences of `/*` and `*/` in string-like objects:

```cte
c1
{
    // Pre-emptively escape the "/" to avoid a false nested comment end
    "comment end" = "*\/"

    // Pre-emptively escape the "*" to avoid a false nested comment begin
    "comment begin" = "/\*"

/*
    // When block-commented out, it still parses properly
    "comment end" = "*\/"
    "comment begin" = "/\*"
*/
}
```

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
      /* Nested multiline
         comments are allowed */
    */
    @"mailto:someone@somewhere.com"

    "a" = "We're inside a string, so /* this is not a comment; it's part of the string! */"

    "data" =
    // A comment before some binary data
    |u8x 01 02 03 // A comment inside the binary array
         04 05 06 07 /* Another comment inside */ 08 09 0a|

    // Comment before the end of the top-level object (the map), but not after!
}
```


### Padding

Padding is not supported in CTE. An encoder **MUST** skip all padding when converting CBE to CTE.



Structural Objects
------------------

### Struct Template

A struct template begins with `@`, followed by a template [identifier](ce-structure.md#identifier), followed by `<`, followed by a series of [whitespace](#structural-whitespace) separated keys, and is terminated with `>`.

There **MUST NOT** be whitespace at any point between the `@` and the opening `<` (e.g. `@my_struct<...>`, **not** `@ my_struct <...>`).

**Example**:

```cte
c1
// Define a struct template:
@dog<"name" "gender">

// Now we can create instances of this struct:
[
    @dog("Fido" "m")
    @dog("Fifi" "f")
]
```


### Marker

A marker sequence consists of the following, with no whitespace in between:

 * `&` (the marker initiator)
 * A marker [identifier](ce-structure.md#identifier)
 * `:` (the marker separator)
 * The marked data object

Example:

```cte
c1
[
    &remember_me:"Remember this string"
    &1:{"a" = 1}
]
```

The string `"Remember this string"` is marked with the ID `remember_me`, and the map `{"a"=1}` is marked with the ID `1`.



Empty Document
--------------

An empty document in CTE is signified by using [null](#null) type as the top-level object:

```cte
c1
null
```



Letter Case
-----------

A CTE document **MUST** be entirely in lower case, except in the following situations:

 * [String-like arrays](#string-like-arrays) **CAN** contain uppercase and lowercase characters.
 * [Comments](#comment) **CAN** contain uppercase and lowercase characters.
 * [Identifiers](ce-structure.md#identifier) **CAN** contain uppercase and lowercase characters.
 * [Time zones](#time-zones) are case sensitive, and usually contain uppercase and lowercase characters.

For the above situations, a CTE encoder **MUST** preserve letter case. In all other situations, a CTE encoder **MUST** convert to lower case.

### Overriding Rule for Decoders

Humans will inevitably get letter case wrong when writing into a CTE document (because they copy-pasted it from somewhere, because they have caps-lock on, because it's just muscle memory to do it that way, etc). Rejecting a document on letter case grounds would be poor U/X, so some decoder lenience is necessary:

A CTE decoder **MUST** accept documents that break lowercase requirements (including version specifier, hexadecimal digits, array types, escape sequences, etc).

**Example**:

A CTE decoder **MUST** accept documents such as:

```cte
C1
[
    |U8 0XF1 0X5A|
    "Some text\Nwith a newline and a \{1F415}"
    0XFFFF
    0B10010101
    INF
    NAN
    1.8E+22
]
```

However, the following would be invalid:

 * `4:00:00/ASIA/TOKYO` (time zones are case sensitive)
 * `[ &a:"marked text" $A ]` (identifiers are case sensitive)
 * `"\.ZZZ terminated by zzz"` (verbatim sentinels are case sensitive)

And the following would likely fail at the application layer (but _not_ in the CTE decoder):

 * `|TEXT/XML "<xml/>"|` ([IANA registered](http://www.iana.org/assignments/media-types/media-types.xhtml) as `text/xml`, not `TEXT/XML`)


Structural Whitespace
---------------------

Structural whitespace is a sequence of whitespace characters whose purpose is to separate objects in a CTE document (for example, separating objects in a list `[1 2 3 4]`). Such characters are not interpreted literally, are interchangeable, and can be repeated any number of times without altering the meaning or structure of the document.

**Allowed structural whitespace characters:**

| Code Point | Name                       |
| ---------- | -------------------------- |
| U+0009     | TAB (character tabulation) |
| U+000A     | LF (line feed)             |
| U+000D     | CR (carriage return)       |
| U+0020     | SP (space)                 |


**Structural Whitespace CAN occur**:

 * Around an object.
 * Around array and container delimiters (`|`, `[`, `]`, `{`, `=`, `}`, `(`, `)`, `<`, `>`)

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `| u8x 01   02 03   04 |` is equivalent to `|u8x 01 02 03 04|`
 * `{ 1="one" 2 = "two" 3= "three" 4 ="four"}` is equivalent to `{1="one" 2="two" 3="three" 4="four"}`


**Structural Whitespace MUST occur**:

 * Between the [version specifier](#document-version-specifier) and the first object.
 * Between the end-of-string sentinel and the beginning of the data in a [verbatim sequence](#verbatim-sequence).
 * Between a primitive type array element type specifier and the array contents, and between array elements.
 * Between values in a [list](#list) (`["one""two"]` is invalid).
 * Between key-value pairs in a [map](#map) (`{1="one"2="two"}` is invalid).


**Whitespace MUST NOT occur**:

 * Before the [version specifier](#document-version-specifier).
 * Between a prefix character and its payload (`& 1234`, `$ abc`, `@ "mydoc.cbe"` are invalid).
 * Between a [marker](#marker) identifier and the object it marks (`&123: xyz` and `&123 :xyz` are invalid).
 * In time values (`2018-07-01-10 :53:22.001481/Z` is invalid).
 * In numeric values (`0x3 f`, `9. 41`, `3 000`, `9.3 e+3`, `- 1.0` are invalid). Use the [numeric whitespace](#numeric-whitespace) character (`_`) instead (where it's valid to do so).
 * Anywhere between a [struct template](#struct-template) or [struct instance](#struct-instance) initiator and its opening character (`@ my_struct<>`, `@my_struct <>`, `@ my_struct()`,  and `@my_struct ()` are invalid).



Pretty Printing
---------------

Pretty printing is the act of laying out [structural whitespace](#structural-whitespace) in a CTE document such that it is easier for humans to parse. CTE documents **SHOULD** always be pretty-printed (except when intended for single-line log entries) because the purpose of CTE is to be read by humans.

Use [CBE](cbe-specification.md) wherever possible instead of minified CTE, and convert to (pretty-printed) CTE only in places where a human will be reading the data.

The following sections describe how to pretty-print CTE documents.


### Right Margin

The right margin (maximum column before breaking an object into multiple lines) **SHOULD** be kept "reasonable". "Reasonable" is difficult to define because it depends in part on the kind of data the document contains, and the container depth.

In general, 120 columns **SHOULD** always be considered reasonable, with larger margins depending on the kind and depth of data the document is likely to contain.


### Indentation

The canonical indentation is 4 spaces (`    `). CTE encoders **SHOULD** always emit indentation unless the destination is a single-line log entry.


### Pretty Printing Lists

Objects in a list **SHOULD** be placed on separate lines.

```cte
c1
[
    @"https://www.imdb.com/title/tt0090605/"
    @"https://www.imdb.com/title/tt1029248/"
]
```

If a list is empty, the closing `]` **SHOULD** be on the same line.

```cte
c1
[]
```

Short lists containing small objects **MAY** be placed entirely on one line.

```cte
c1
["a" "b" "c" "d"]
```


### Pretty Printing Maps

There **SHOULD** be a space between keys, values and the `=` in key-value pairs, and each key-value pair **SHOULD** be on a separate line.

```cte
c1
{
    "aliens" = @"https://www.imdb.com/title/tt0090605/"
    "moribito" = @"https://www.imdb.com/title/tt1029248/"
}
```

If a map is empty, the closing `}` **SHOULD** be on the same line.

```cte
c1
{}
```

Small maps containing small objects **MAY** be placed entirely on one line. In such a case, omit the spaces around the `=`.

```cte
c1
{"a"="b" "c"="d"}
```


### Pretty Printing Struct Templates

Struct templates **SHOULD** follow a similar pretty printing strategy to [lists](#pretty-printing-lists).


### Pretty Printing Struct Instances

Struct instances **SHOULD** follow a similar pretty printing strategy to [lists](#pretty-printing-lists).


### Pretty Printing Nodes

In order to keep the tree as readable as possible to a human:

* There **SHOULD NOT** be whitespace between the left `(` and the node value.
* All child nodes **SHOULD** be on separate lines at the next indentation depth.
* If the node has child nodes, the closing `)` **SHOULD** be on a separate line, at the same indentation depth as the `(`.
* If the node has no children, the closing `)` **SHOULD** be on the same line.

See the example in [node](#node).


### Pretty Printing Edges

Edge components **SHOULD** be broken up into multiple lines if they're too long.

```cte
c1
@(
    @"https://springfield.gov/people#homer_simpson"
    @"https://example.org/wife"
    @"https://springfield.gov/people#marge_simpson"
)
```


### Pretty Printing Strings

Strings **SHOULD** use [continuations](#continuation) if the line is getting too long.

```cte
c1
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


### Pretty Printing Primitive Type Arrays

Primitive type arrays **SHOULD** be broken up into multiple indented lines if the line is getting too long.

```cte
c1
|u16x aa5c 5e0f e9a7 b65b 3572 96ec da16 6496 6133 5aa1 687f 9ce0 4d10 a39e 3bd3
      bf96 ad12 e64b 298f e137 a99f 5fb8 a8ca e8e7 0595 bc2f 4b64 8b0e 895d ebe7
      fb59 fdb0 1d93 5747 239d b16f 7d9c c48b 5581 13ba 19ca 6f3b 4ba9|
```


### Pretty Printing Comments

In the event that a machine generating CTE documents wants to also output comments, the following rules apply:

Comments **SHOULD** have one space (u+0020) after the comment opening sequence. Multiline-style comments (`/* */`) **SHOULD** also have a space before the closing sequence.

```cte
c1
{
    // abc

    /* abc */
}
```

Long comments **SHOULD** be broken up to fit within the right margin.

```cte
c1
{
    /* When a comment gets too long to fit within the right margin, place
       the overflow on a separate lines, adjusting the indent to keep
       everything aligned. */
}
```

The object following a comment **SHOULD** be on a different line.

```cte
c1
{
    /* See list of request types in request-types.md */
    "request-type" = "ping"
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

Copyright (c) 2018-2022 Karl Stenerud. All rights reserved.

Distributed under the [Creative Commons Attribution License](https://creativecommons.org/licenses/by/4.0/legalcode) ([license deed](https://creativecommons.org/licenses/by/4.0).
