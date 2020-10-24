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
  - [UUID](#uuid)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
  - [Time Zones](#time-zones)
    - [Area/Location](#arealocation)
    - [Global Coordinates](#global-coordinates)
* [Array Types](#array-types)
  - [Element Array Encodings](#element-array-encodings)
  - [String-Like Array Encodings](#string-like-array-encodings)
  - [URI](#uri)
  - [Custom Binary](#custom-binary)
  - [Custom Text](#custom-text)
  - [String](#string)
    - [Quoted String](#quoted-string)
    - [Unquoted String](#unquoted-string)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
    - [Markup Structure](#markup-structure)
    - [Container End](#container-end)
    - [Content String](#content-string)
* [Pseudo-Objects](#pseudo-objects)
  - [Marker](#marker)
  - [Reference](#reference)
  - [Metadata Map](#metadata-map)
  - [Comment](#comment)
    - [Single Line Comment](#single-line-comment)
    - [Multiline Comment](#multiline-comment)
* [Other Types](#other-types)
  - [Null](#null)
* [Letter Case](#letter-case)
* [Whitespace](#whitespace)
* [Pretty Printing](#pretty-printing)
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



General
-------

A CTE document is a UTF-8 encoded text document containing data arranged in an ad-hoc hierarchical fashion.

Whitespace is used to separate elements in a container. In maps, the key and value portions of a key-value pair are separated by an equals character (`=`) and possible whitespace. The key-value pairs themselves are separated by whitespace. Extraneous whitespace is ignored.

**Examples**:

 * CTE v1 empty document: `c1 @null`
 * CTE v1 document containing the top-level integer value 1000: `c1 1000`
 * CTE v1 document containing a top-level list: `c1 [a b c]`
 * CTE v1 document containing a top-level map: `c1 {a=1 b=2 c=3}`


### Human Editability

A CTE document must be editable by a human. This means that it must contain only valid UTF-8 characters and sequences that can actually be viewed, entered and modified in a UTF-8 capable text editor. Unicode runes that have no width or visibility or direct input method, or are reserved or permanently marked as non-characters, must not be present in the document.

In the spirit of human editability:

 * Implementations should avoid outputting characters that editors tend to convert automatically.
 * Line lengths should be kept within reasonable amounts in order to avoid excessive horizontal scrolling in an editor.
 * The canonical line ending is linefeed (u+000a)
 * Implementations should convert structural line endings to the operating system's native format when saving a document to disk. See: [line endings](#line-endings)
 * If a certain character is likely to be confusing or problematic to a human reader or editor, it should be escaped.


### Line Endings

Line endings can be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for data transmission, the canonical format is LF only. Decoders must accept all encodings as input, but encoders should output LF when the destination is a foreign or unknown system.


### Escape Sequences

In some contexts, escape sequences may be used to encode data that would otherwise be cumbersome or impossible to represent. `\` acts as an escape sequence initiator, followed by an escape type character and possible data:

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

**Note**: The `*` and `/` escape sequences can help to avoid edge cases when [commenting out](#comment) big chunks of a document.

Escape sequences must be converted before any other processing occurs during the decode process.

#### Continuation

A continuation escape sequence causes the decoder to ignore all whitespace characters until it encounters the next printable character. An escape character (`\`) followed by either LF (u+000a) or CR (u+000d) initiates a continuation.

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
 * An end-of-sequence identifier, which is a sequence of printable, non-control, non-whitespace characters (in accordance with [human editability](cte-specification.md#human-editability)).
 * A whitespace terminator to terminate the end-of-sequence identifier (either: SPACE `u+0020`, TAB `u+0009`, LF `u+000a`, or CR+LF `u+000d u+000a`).
 * The string contents.
 * A second instance of the end-of-sequence identifier (without whitespace terminator).

**Example**:

```
"Verbatim sequences can occur anywhere escapes are allowed.
\.@@@
In verbatim sequences, everything is interpreted literally until the
end-of-string identifier is encountered (in this case three @ characters).
Characters like ", [, <, \ and such can appear unescaped.

Whitespace (including "leading" whitespace) is also read verbatim.
          For example, this line really is indented 10 spaces.

@@@After a verbatim sequence, normal processing resumes, so '\t' and such are interpreted."
```
Which is decoded to:
```
Verbatim sequences can occur anywhere escapes are allowed.
In verbatim sequences, everything is interpreted literally until the
end-of-string identifier is encountered (in this case three @ characters).
Characters like ", [, <, \ and such can appear unescaped.

Whitespace (including "leading" whitespace) is also read verbatim.
          For example, this line really is indented 10 spaces.

After a verbatim sequence, normal processing resumes, so '  ' and such are interpreted.
```



Version Specifier
-----------------

A CTE document begins with a version specifier, which is composed of the character `c` (u+0063), followed immediately by an unsigned integer version number. There must be no whitespace between the `c` and the version number.

The version specifier and the top-level object must be separated by whitespace.

**Example**:

* Version specifier (CTE version 1): `c1`
* Complete (and empty) document: `c1 @null`



Numeric Types
-------------

### Boolean

Represented by the values `@true` and `@false`.


### Integer

Integer values can be positive or negative, and can be represented in various bases. Negative values are prefixed with a dash `-` as a sign character. Values must be written in lower case.

Integers can be specified in base 2, 8, 10, or 16. Bases other than 10 require a prefix:

| Base | Name        | Digits           | Prefix | Example      | Decimal Equivalent |
| ---- | ----------- | ---------------- | ------ | ------------ | ------------------ |
|   2  | Binary      | 01               | `0b`   | `-0b1100`    | -12                |
|   8  | Octal       | 01234567         | `0o`   | `0o755`      | 493                |
|  10  | Decimal     | 0123456789       |        | `900000`     | 900000             |
|  16  | Hexadecimal | 0123456789abcdef | `0x`   | `0xdeadbeef` | 3735928559         |


### Floating Point

A floating point number is composed of a whole part and a fractional part, separated by a dot `.`, with an optional exponential portion. Negative values are prefixed with a dash `-`.

**Examples**:

    1.0
    -98.413

#### Base-10 Notation

The exponential portion of a base-10 number is denoted by the lowercase character `e`, followed by the signed size of the exponent (using optional `+` for positive and mandatory `-` for negative). The exponent portion is a signed base-10 number representing the power-of-10 to multiply the significand by. Values should be normalized (only one digit to the left of the decimal point) when using exponential notation.

 * `6.411e+9` = 6411000000
 * `6.411e9` = 6411000000
 * `6.411e-9` = 0.000000006411

There is no technical maximum number of significant digits or exponent digits, but care should be taken to ensure that the receiving end will be able to store the value. 64-bit ieee754 floating point values, for example, can represent values with up to 16 significant digits.

#### Base-16 Notation

Base-16 floating point numbers allow 100% accurate representation of ieee754 binary floating point values. They begin with `0x`, and the exponential portion is denoted by the lowercase character `p`. The exponential portion is a signed base-10 number representing the power-of-2 to multiply the significand by. The exponent's sign character can be omitted if it's positive. Values should be normalized.

 * `0xa.3fb8p+42` = a.3fb8 x 2 ^ 42
 * `0x1.0p0` = 1

To maintain compatibility with [CBE](cbe-specification.md), base-16 floating point notation can only be used represent values up to 64-bit ieee754 binary floats.

#### Special Floating Point Values

 * `@inf`: Infinity
 * `-@inf`: Negative Infinity
 * `@nan`: Not a Number (quiet)
 * `@snan`: Not a Number (signaling)

#### Floating Point Rules

**There must be one (and only one) dot character:**

| Value          | Notes              |
| -------------- | ------------------ |
| `1`            | Integer, not float |
| `1.0`          | Float              |
| `500000000000` | Integer, not float |
| `5.0e+11`      | Float              |
| `5e+11`        | Invalid            |
| `10.4.5`       | Invalid            |

**There must be at least one digit on each side of the dot character:**

| Invalid      | Valid     | Notes                                                |
| ------------ | --------- | ---------------------------------------------------- |
| `-1.`        | `-1.0`    | Or use integer value `-1`                            |
| `.1`         | `0.1`     |                                                      |
| `.218901e+2` | `21.8901` | Or `2.18901e+1`                                      |
| `-0`         | `-0.0`    | Special case: -0 cannot be represented as an integer |


### Numeric Whitespace

The `_` character can be used as "numeric whitespace" when encoding certain numeric values. Other [whitespace](#whitespace) characters are not allowed.

Rules:

 * Only [integer](#integer) and [floating point](#floating-point) types can contain numeric whitespace.
 * [Named values](#named-values) (such as `@nan` and `@inf`) must not contain numeric whitespace.
 * Numeric whitespace can only occur between two consecutive numeric digits (`0`-`9`, `a`-`f`, depending on numeric base).
 * Numeric whitespace characters must be ignored when decoding numeric values.

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


### UUID

The 128-bit universally unique identifier begins with an at (`@`) character, and must follow the UUID string representation defined in [rfc4122](https://tools.ietf.org/html/rfc4122).

Example:

    @123e4567-e89b-12d3-a456-426655440000



Temporal Types
--------------

Temporal types record time with varying degrees of precision.

Fields other than year can be pre-padded with zeros (`0`) up to their maximum allowed digits for aesthetic reasons if desired.


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

The time zone is an optional field. If omitted, it is assumed to be `Zero` (UTC).

#### Area/Location

An area/location time zone is written in the form `Area/Location`.

**Examples**:

 * `E/Paris`
 * `America/Vancouver`
 * `Etc/UTC` == `Zero` == `Z`
 * `L`


#### Global Coordinates

Global coordinates are written as latitude and longitude to a precision of hundredths of degrees, separated by a slash character (`/`). Negative values are prefixed with a dash character (`-`), and the dot character (`.`) is used as a fractional separator.

**Examples**:

 * `51.60/11.11`
 * `-13.53/-172.37`



Array Types
-----------

The standard array encoding format consists of a pipe character (`|`), followed by optional whitespace, the array type, mandatory whitespace, the contents, and finally a closing pipe. Depending on the kind of array, the contents are encoded either as whitespace-separated elements, or as a string-like sequence representing the contents:

    |type elem1 elem2 elem3 ...|
    |type contents-represented-as-a-string|

An empty array has a type but no contents:

    |type|

The following array types are available:

| Type   | Description                     | Encoding Kind |
| ------ | ------------------------------- | ------------- |
| `b`    | Boolean                         | Element       |
| `u8`   | 8-bit unsigned integer          | Element       |
| `u16`  | 16-bit unsigned integer         | Element       |
| `u32`  | 32-bit unsigned integer         | Element       |
| `u64`  | 64-bit unsigned integer         | Element       |
| `i8`   | 8-bit signed integer            | Element       |
| `i16`  | 16-bit signed integer           | Element       |
| `i32`  | 32-bit signed integer           | Element       |
| `i64`  | 64-bit signed integer           | Element       |
| `f16`  | 16-bit floating point (bfloat)  | Element       |
| `f32`  | 32-bit floating point (ieee754) | Element       |
| `f64`  | 64-bit floating point (ieee754) | Element       |
| `uu`   | 128-bit UUID                    | Element       |
| `u`    | URI                             | String-Like   |
| `ct`   | Custom Text                     | String-Like   |
| `cb`   | Custom Binary                   | Element       |


### Element Array Encodings

For element array encodings, any valid representation of the element data type may be used, provided the value fits within the type's width. 

#### Implied Prefix

Optionally, a suffix can be appended to the type specifier (if the type supports it) to indicate that all values must be considered to have an implicit prefix.

| Suffix | Implied element prefix | Example                         |
| ------ | ---------------------- | ------------------------------- |
| `b`    | `0b`                   | `\|u8b 10011010 00010101\|`     |
| `o`    | `0o`                   | `\|i16o -7445 644\|`            |
| `x`    | `0x`                   | `\|f32x a.c9fp20 -1.ffe9p-40\|` |

#### Special Rules

 * UUID array elements may optionally be represented without the initial `@` sentinel.
 * Boolean array elements may optionally be represented using `0` for false and `1` for true. Whitespace is optional when encoding a boolean array using `0` and `1` (e.g. `|b 1001|` = `|b 1 0 0 1|` = `|b true false false true|`)

**Examples**:

    |u8x 9f 47 cb 9a 3c|
    |f32 1.5 0x4.f391p100 30 9.31e-30|
    |i16 0b1001010 0o744 1000 0xffff|
    |uu 3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f|
    |b 11010|


### String-Like Array Encodings

String-like array encodings are interpreted as a whole, and must encode control characters, non-printable characters, pipe (`|`) and backslash (`\`) as [escape sequences](#escape-sequences).


### URI

A Concise Encoding implementation must not interpret or generate percent escape sequences in URIs; they must be passed untouched. [CTE escape sequences](#escape-sequences), however, must be interpreted and the converted string passed along.

 * `|u http://x.y.z?pipe=\||` decodes to `http://x.y.z?pipe=|`,  which the upper layers interpret as `http://x.y.z?pipe=|`
 * `|u http://x.y.z?pipe=%7c|` decodes to `http://x.y.z?pipe=%7c`,  which the upper layers interpret as `http://x.y.z?pipe=|`


### Custom Binary

Custom binary data is encoded like a `u8x` array (as hex encoded byte elements).

**Example**:

    |cb 04 f6 28 3c 40 00 00 40 40|
    = binary data representing a custom "cplx" struct
      {
          type:uint8 = 4
          real:float32 = 2.94
          imag:float32 = 3.0
      }


### Custom Text

Custom text data is encoded as a string-like array. Custom text can contain [escape sequences](#escape-sequences), which must be processed before being passed to the custom decoder that will interpret it.

**Example**:

    |ct cplx(2.94+3i)|


### String

Strings can be quoted or unquoted.

#### Quoted String

A quoted string encloses the string contents within double-quote delimiters (for example: `"a string"`). All characters leading up to the closing double-quote (including whitespace) are considered part of the string sequence. A quoted string must not contain control characters, non-printable characters, double-quotes (`"`) or backslash (`\`) unless encoded as [escape sequences](#escape-sequences). Whitespace characters CR, LF, and TAB are allowed, but care should be taken as different text editors might swap LF / CRLF, or tabs / spaces. See [human editability](cte-specification.md#human-editability)).

**Example**:

    "Line 1\nLine 2\nLine 3"

#### Unquoted String

Strings must normally be double-quote delimited, but this rule can be relaxed if they are [unquoted-safe](ce-structure.md#unquoted-safe-string).

**Example**:

```
c1 [
  "some-text"
]
```

"some-text" is unquoted-safe, so this document could also be written as:

```
c1 [
  some-text
]
```



Container Types
---------------

### List

A list begins with an opening square bracket `[`, contains whitespace separated contents, and finishes with a closing square bracket `]`.

**Example**:

    [1 two 3.1 {} @null]


### Map

A map begins with an opening curly brace `{`, contains whitespace separated key-value pairs, and finishes with a closing curly brace `}`.

Map entries are split into key-value pairs using the equals `=` character and optional whitespace. Key-value pairs must be separated from each other using whitespace. A key without a paired value is invalid.

**Example**:

    {
        1 = alpha
        2 = beta
        "a map" = {one=1 two=2}
    }


### Markup

The CTE encoding of a markup container is similar to XML, except:

 * There are no end tags. All data is contained within the begin `<`, content begin `:`, and end `>` characters.
 * Comments are encoded using `/*` and `*/` instead of `<!--` and `-->`, and can be nested.
 * [Unquoted strings](#unquoted-string) are allowed in markup names and attribute values.
 * Non-string types can be used in tag names and attribute key-value pairs (under the same rules as [map](#map) keys and values).

#### Markup Structure

| Section    | Delimiter  | Type                      | Required |
| ---------- | ---------- | ------------------------- | -------- |
| Tag name   | `<`        | [Keyable](#keyable-types) | Y        |
| Attributes | whitespace | [Map](#map)               |          |
| Contents   | `:`        | [List](#list)             |          |
| End        | `>`        |                           | Y        |

Attributes and contents are optional. There must be whitespace between the container name and the attributes section (if present), and there can optionally be whitespace adjacent to the begin, contents, and end delimiters.

Illustration of markup encodings:

| Attributes | Children | Example                                                |
| ---------- | -------- | ------------------------------------------------------ |
|     N      |    N     | `<br>`                                                 |
|     Y      |    N     | `<div id=fillme>`                                      |
|     N      |    Y     | `<span:Some text here>`                                |
|     Y      |    Y     | `<ul id=mylist style=boring: <li:first> <li:second> >` |

The contents section is in string processing mode whenever it's not processing a sub-container or comment (initiated by an unescaped `<` or `//` or `/*`).

##### Container End

The markup container ends when an unescaped `>` character is encountered while processing the attributes or contents.

##### Content String

Content strings can contain [escape sequences](#escape-sequences), which must be processed before applying the structural rules for content strings.

**Example**:

```
c1
<View:
    <Image src=|u images/avatar-image.jpg|>
    <Text id=HelloText:
        Hello! Please choose a name!
    >
    // <HRule style=thin>
    <TextInput id=NameInput style={height=40 borderColor=gray} OnChange="\.@@
        HelloText.SetText("Hello, " + NameInput.Text + "!")
    @@":
        Name me!
    >
>
```



Pseudo-Objects
--------------

### Marker

A marker sequence consists of the following, with no whitespace in between:

 * `&` (the marker initiator)
 * A [marker ID](ce-structure.md#marker-id) (unquoted if a string)
 * `:` (the marker separator)
 * The marked value

Example:

    [
        &remember_me:"Pretend that this is a huge string"
        &1:{a = 1}
    ]

The string `"Pretend that this is a huge string"` is marked with the ID `remember_me`, and the map `{a=1}` is marked with the ID `1`.


### Reference

A reference begins with the reference initiator (`$`), followed immediately (with no whitespace) by either a [marker ID](ce-structure.md#marker-id) or a [URI](#uri).

Example:

    {
        some_object = {
            my_string = &big_string:"Pretend that this is a huge string"
            my_map = &1:{
                a = 1
            }
        }

        reference_to_string = $big_string
        reference_to_map = $1
        reference_to_local_doc = $|u common.cte|
        reference_to_remote_doc = $|u https://somewhere.com/my_document.cbe?format=long|
        reference_to_local_doc_marker = $|u common.cte#legalese|
        reference_to_remote_doc_marker = $|u https://somewhere.com/my_document.cbe?format=long#examples|
    }


### Metadata Map

Metadata maps are encoded in the same manner as [regular maps](#map), except that they are enclosed within parenthesis `(`, `)`.

**Example**:

    c1
    // Metadata for the entire document
    (
        _ct = 2017.01.14-15:22:41/Z
        _mt = 2019.08.17-12:44:31/Z
    )
    {
        records = [
            // Metadata for "ABC Corp" record
            (
                _ct = 2019.05.14-10:22:55/Z
                _t = [longtime_client big_purchases]
            )
            {
                client = "ABC Corp"
                amount = 10499.28
                due = 2020.05.14
            }
        ]
    }


### Comment

Comments can be written in single-line or multi-line form.

#### Single Line Comment

A single line comment begins at the sequence `//` and continues until the next linefeed (u+000a) is encountered.

#### Multiline Comment

A multiline comment begins at the sequence `/*` and is terminated by the sequence `*/`. Multiline comments support nesting, meaning that further `/*` sequences inside the comment will start subcomments that must also be terminated by their own `*/` sequence. No processing of the comment contents other than detecting comment begin and comment end is peformed.

**Note**: Commenting out strings or markup contents containing the sequences `/*` or `*/` could potentially cause parse errors because the parser won't have any contextual information about the sequences, and will simply treat them as "comment begin" and "comment end". This edge case could be mitigated by pre-emptively escaping all occurrences of `/*` and `*/` that don't represent comment delimiters:

**Example**:

```
c1
// Comment before top level object
{
    // Comment before the "name" object.
    // And another comment.
    "name" = "Joe Average" // Comment after the "Joe Average" object.

    "email" = // Comment after the "email" key.
    /* Multiline comment with nested comment inside
      |u mailto:joe@average.org|
      /* Unlike in C, nested multiline
         comments are allowed */
    */
    |u mailto:someone@somewhere.com|

    "data" // Comment after data
    =
    // Comment before some binary data (but not inside it)
    |u8x 01 02 03 04 05 06 07 08 09 0a|
}
```


Other Types
-----------

### Null

Null is encoded as `@null`



Letter Case
-----------

A CTE document must be entirely in lower case, except in the following situations:

 * Strings, string-like arrays, and comments can contain uppercase characters. Case must be preserved.
 * [Marker IDs](ce-structure.md#marker-id) can contain uppercase characters. Case must be preserved.
 * [Time zones](#time-zones) are case sensitive, and usually contain both uppercase and lowercase characters.

Everything else, including hexadecimal digits, exponents, and escape sequences, must be lower case.

A decoder must accept data regardless of letter-case because humans will inevitably break these rules, and rejecting a document on such grounds would result in poor UX.

An encoder must output letter case in accordance with this specification.



Whitespace
----------

Structural whitespace (whitespace not encoded into a string or string-like object) must be ignored by a CTE parser. Any number of structural whitespace characters can occur in a sequence.


### Valid Whitespace Characters

While there are many characters classified as "whitespace" within the Unicode set, only the following are valid structural whitespace characters:

| Code Point | Name            |
| ---------- | --------------- |
| u+0009     | horizontal tab  |
| u+000a     | line feed       |
| u+000d     | carriage return |
| u+0020     | space           |


### Whitespace **can** occur:

 * Around an object.
 * Around array and container delimiters (`|`, `[`, `]`, `{`, `=`, `}`, `<`, `:`, `>`)

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `| u8x 01   02 03   04 |` is equivalent to `|u8x 01 02 03 04|`
 * `{ 1="one" 2 = "two" 3= "three" 4 ="four"}` is equivalent to `{1="one" 2="two" 3="three" 4="four"}`


### Whitespace **must** occur:

 * Between the [version specifier](#version-specifier) and the first object.
 * Between the end-of-string identifier and the beginning of the data in a [verbatim sequence](#verbatim-sequence).
 * Between a typed array element type specifier and the array contents, and between typed array elements.
 * Between values in a [list](#list) (`["one""two"]` is invalid).
 * Between key-value pairs in a [map](#map), [metadata map](#metadata-map), or [markup attributes](#attributes-section) (`{1="one"2="two"}` is invalid).
 * Between a markup's [tag name](#markup-tag-name) and its [attributes](#attributes-section) (if attributes are present).


### Whitespace **must not** occur:

 * Before the [version specifier](#version-specifier).
 * Between a marker or reference initiator and its [marker ID](ce-structure.md#marker-id) (`& 1234` and `$ |u mydoc.cbe|` are invalid).
 * Between a [marker ID](ce-structure.md#marker-id) and the object it marks (`&123: xyz` is invalid).
 * Splitting a time value (`2018.07.01-10 :53:22.001481/Z` is invalid).
 * Splitting a numeric value (`0x3 f`, `9. 41`, `3 000`, `9.3 e+3`, `- 1.0` are invalid). Use the numeric whitespace character (`_`) instead.
 * Splitting named values: (`@t rue`, `@ null`, `@i nf`, `@ nan` are invalid).



Pretty Printing
---------------

Pretty printing is the act of laying out structural whitespace in a CTE document so that it is easier for humans to parse. This section specifies how CTE documents should be pretty printed.


#### Right Margin

The right margin (maximum column before breaking an object into multiple lines) should be kept "reasonable". "Reasonable" is difficult to define because it depends in part on the kind of data the document contains, and the container depth.

In general, 120 columns should always be considered reasonable, with larger margins depending on the kind and depth of data the document is likely to contain.


#### Indentation

The indentation string should be configurable, with a default of 4 spaces (u+0020).


#### Lists

Objects in a list should be placed on separate lines.
```
[
    |u https://www.imdb.com/title/tt0090605/|
    |u https://www.imdb.com/title/tt1029248/|
]
```

If a list is empty, the closing `]` should be on the same line.
```
[]
```

Short lists containing small objects may be placed entirely on one line.
```
[a b c d]
```


#### Maps

There should be a space between keys, values and the `=` in key-value pairs, and each key-value pair should be on a separate line.
```
{
    aliens = |u https://www.imdb.com/title/tt0090605/|
    moribito = |u https://www.imdb.com/title/tt1029248/|
}
```

If a map is empty, the closing `}` should be on the same line.
```
{}
```

Small maps containing small objects may be placed entirely on one line. In such a case, omit the spaces around the `=`.
```
{a=b c=d}
```


#### Metadata Map

The object that the metadata map refers to should follow it after a space.
```
{
    (x=y) a = b

    c = (x=y) d

    (
        x = 1
        y = 2
        z = 3
    ) e = f

    g = (
        x = 1
        y = 2
        z = 3
    ) h
}
```


#### Markup

The closing `>` should only be on a different line if there are contents.
```
<a>

<a:
    contents
>
```

The attributes section should be entirely on the same line as the tag name if it's not too long.
```
<a x=y>

<a x=y:
    contents
>
```

If the attributes section is too long, the overflow should be broken up into multiple indented lines.
```
<img src=|u http://somereallylongdomainname.likereallylong.com/images/2.jpg|
    width=50 height=50 border-left=10 units=px>
```


#### Strings

Strings should use [continuations](#continuation) if the line is getting too long.
```
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

Typed arrays should be broken up into multiple indented lines if the line is getting too long.
```
|u16x aa5c 5e0f e9a7 b65b 3572 96ec da16 6496 6133 5aa1 687f 9ce0 4d10 a39e 3bd3
      bf96 ad12 e64b 298f e137 a99f 5fb8 a8ca e8e7 0595 bc2f 4b64 8b0e 895d ebe7
      fb59 fdb0 1d93 5747 239d b16f 7d9c c48b 5581 13ba 19ca 6f3b 4ba9|
```


#### Comments

Comments should have one space (u+0020) after the comment opening sequence. `/* */` style comments should also have a space before the closing sequence.
```
// abc

/* abc */
```

Long comments should be broken up to fit within the right margin.
```
{
    /* When a comment gets too long to fit within the right margin, place
       the overflow on a separate lines, adjusting the indent to keep
       everything aligned. */
}
```

The object following a comment should be on a different line.
```
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
