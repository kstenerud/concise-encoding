Concise Text Encoding
=====================

Concise Text Encoding (CTE) is a general purpose, human friendly, compact representation of semi-structured hierarchical data. CTE is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a human friendly way:

 * There are two formats: [binary-based CBE](cbe-specification.md) and text-based CTE.
 * 1:1 type compatibility between formats. Use the more efficient binary format for data interchange and storage, and transparently convert to/from text only when a human needs to be involved.
 * Documents and specifications are versioned to support future expansion.
 * Supports metadata and comments.
 * Supports references to other parts of the document or to other documents.
 * Supports the most commonly used data types:

| Type              | Example                        |
| ----------------- | ------------------------------ |
| Nil               | `@nil`                         |
| Boolean           | `@true`                        |
| Integer           | `-1_000_000_000_000_000`       |
| Float             | `4.8255`                       |
| Time              | `2019-7-15/18:04:00/E/Rome`    |
| String            | `"A string"`                   |
| URI               | `u"http://example.com?q=1"`    |
| Bytes             | `h"f1 e2 d3 c4 b5 a6 97 88"`   |
| List              | `[1 2 3 4]`                    |
| Map               | `{one=1 two=2}`                |
| Markup            | `<span style=bold\| Blah blah>` |
| Metadata Map      | `(_id=12345)`                  |
| Marker/Reference  | `*a_ref "something"`, `#a_ref` |
| Comment           | `// A comment`                 |
| Multiline Comment | `/* A comment */`              |



Version
-------

Version 1 (prerelease)



Contents
--------

* [Terms](#terms)
* [Structure](#structure)
  - [Human Editability](#human-editability)
  - [Version Specifier](#version-specifier)
  - [Maximum Depth](#maximum-depth)
  - [Maximum Length](#maximum-length)
  - [Line Endings](#line-endings)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Floating Point](#floating-point)
    - [Base-10 Notation](#base-10-notation)
    - [Base-16 Notation](#base-16-notation)
    - [Floating Point Rules](#floating-point-rules)
    - [Infinity and Not a Number](#infinity-and-not-a-number)
  - [Numeric Whitespace](#numeric-whitespace)
* [Temporal Types](#temporal-types)
  - [Temporal Type Notes](#temporal-type-notes)
  - [Time Zones](#time-zones)
    - [Area/Location](#arealocation)
    - [Global Coordinates](#global-coordinates)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
* [Array Types](#array-types)
  - [String](#string)
    - [Quoted Sequence](#quoted-sequence)
    - [Verbatim Sequence](#verbatim-sequence)
    - [Unquoted String](#unquoted-string)
  - [URI](#uri)
  - [Bytes](#bytes)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
    - [Markup Structure](#markup-structure)
    - [Container Name](#container-name)
    - [Attributes Section](#attributes-section)
    - [Contents Section](#contents-section)
    - [Container End](#container-end)
    - [Content String](#content-string)
    - [Entity References](#entity-references)
    - [Doctype](#doctype)
    - [Style Sheets](#style-sheets)
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
* [Named Values](#named-values)
* [Letter Case](#letter-case)
* [Whitespace](#whitespace)
* [Invalid Encodings](#invalid-encodings)
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

A CTE document is a UTF-8 encoded text document containing data arranged in an ad-hoc hierarchical fashion.

The document begins with a [version specifier](#version-specifier), followed by an object. Multiple objects can be stored in the document by using a container as the top-level object.

    [version specifier] [object]

Whitespace is used to separate elements in a container. In maps, the key and value portions of a key-value pair are separated by an equals character (`=`) and possible whitespace. The key-value pairs themselves are separated by whitespace.

#### Example

    v1
    // _ct is the creation time, in this case referring to the document
    (_ct = 2019.9.1-22:14:01)
    {
        // A comment
        /* A multiline
           comment */
        (metadata_about_a_list = "something interesting about a_list")
        a_list           = [1 2 "a string"]
        map              = {2=two 3=3000 1=one}
        *tagged original = "Something we'd like to reference later"
        boolean          = @true
        "binary int"     = -0b10001011
        "octal int"      = 0o644
        "regular int"    = -10000000
        "hex int"        = 0xfffe0001
        "decimal float"  = -14.125
        "hex float"      = 0x5.1ec4p20
        date             = 2019-7-1
        time             = 18:04:00.940231541/E/Prague
        timestamp        = 2010-7-15/13:28:15.415942344/Z
        nil              = @nil
        bytes            = h"10ff389add004f4f91"
        url              = u"https://example.com/"
        email            = u"mailto:me@somewhere.com"
        1                = "Keys don't have to be strings"
        ref1             = #tagged
        ref2             = #tagged
        markup           = (xml-doctype=[html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" u"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"])
                           <html xmlns=u"http://www.w3.org/1999/xhtml" xml:lang=en |
                             <body|
                               Widgets:
                               <div id=parent style=normal ref-id=1 |
                                 <script| `##
                                   document.getElementById('parent').insertAdjacentHTML('beforeend', '<div id="idChild"> content </div>');
                                 ##>
                               >
                             >
                           >
    }

The top-level object can also be a non-container type, for example:

    v1 "A single string object"


### Human Editability

A CTE document must be editable by a human. This means that it must contain only valid UTF-8 characters and sequences that can actually be viewed, entered and modified in a UTF-8 capable text editor. Unicode runes that have no width or visibility or direct input method, or are permanently marked as non-characters, must not be present in the document. Unpaired surrogates are not allowed.

In the spirit of human editability:

 * Implementations and document creators should avoid easily confused or otherwise difficult to use characters, especially in identifiers.
 * Implementations should avoid outputting characters that editors tend to convert automatically, in places where those characters have significance. The tab character and line ending characters come to mind.
 * Line lengths should be kept within reasonable amounts in order to avoid excessive horizontal scrolling in an editor.
 * Implementations should convert structural line endings to the operating system's native format when saving a document to disk. See: [line endings](#line-endings)
 * If a certain character is likely to be confusing or problematic, it's encouraged to use an escape sequence instead.

**Note:** Problematic characters can be escaped in certain contexts.


### Version Specifier

All CTE documents must begin with a version specifier, which must not be preceded by whitespace. In other words, the very first byte of a CTE document must be `v` (0x76).

The version specifier is the lowercase letter `v` followed immediately by a number representing the version of this specification that the document adheres to (there must not be whitespace between the `v` and the number). The version specifier must be followed by whitespace to separate it from the rest of the document.

Note: Because CBE places the version as the first byte in a document, the version value 118 is invalid. If 118 were allowed, it would clash with CTE when differentiating the file type by its contents because CTE uses `v` (0x76, or 118) as its first byte.

#### Examples

    v1
    {
        a = 1
    }

Or:

    v1 "this is a string"


### Maximum Depth

Since nested objects (in containers such as maps and lists) are possible, it is necessary to impose an arbitrary depth limit to insure interoperability between implementations. For the purposes of this spec, that limit is 1000 levels of nesting from the top level container to the most nested object (inclusive), unless a different maximum depth is established between parties.


### Maximum Length

Maximum lengths (max list length, max map length, max array length, max total objects in document, max byte length, etc) are implementation defined, and should be negotiated beforehand. A decoder is free to discard documents that threaten to exceed its resources.


### Line Endings

Line endings can be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for data transmission, the canonical format is LF only. Decoders must accept all encodings as input, but encoders should only output LF when the destination is a foreign or unknown system.



Numeric Types
-------------

### Boolean

Supports the values `@true` and `@false`.

#### Example

    @true
    @false


### Integer

Integer values can be positive or negative, and can be represented in various bases. Negative values are prefixed with a dash `-` as a sign character. Values must be written in lower case.

Integers can be specified in base 2, 8, 10, or 16. Bases other than 10 must be prefixed:

| Base | Name        | Digits           | Prefix | Example      | Decimal Equivalent |
| ---- | ----------- | ---------------- | ------ | ------------ | ------------------ |
|   2  | Binary      | 01               | `0b`   | `-0b1100`    | -12                |
|   8  | Octal       | 01234567         | `0o`   | `0o755`      | 493                |
|  10  | Decimal     | 0123456789       |        | `900000`     | 900000             |
|  16  | Hexadecimal | 0123456789abcdef | `0h`   | `0xdeadbeef` | 3735928559         |


### Floating Point

A floating point number is composed of a whole part and a fractional part, separated by a dot `.`, with an optional exponential portion. Negative values are prefixed with a dash `-`.

    1.0
    -98.413

#### Base-10 Notation

The exponential portion of a base-10 number is denoted by the lowercase character `e`, followed by the signed size of the exponent (using `+` for positive and `-` for negative). The exponent's sign character may be omitted if it's positive. The exponent portion is a signed base-10 number representing the power-of-10 to multiply the significand by. Values must be normalized (only one digit to the left of the decimal point).

 * `6.411e+9` = 6411000000
 * `6.411e9` = 6411000000
 * `6.411e-9` = 0.000000006411

There is no maximum number of significant digits or exponent digits, but care should be taken to ensure that the receiving end will be able to store the value. 64-bit ieee754 floating point values, for example, can store up to 16 significant digits.

#### Base-16 Notation

Base-16 floating point numbers allow 100% accurate representation of ieee754 binary floating point values. They begin with `0x`, and the exponential portion is denoted by the lowercase character `p`. The exponential portion is a signed base-10 number representing the power-of-2 to multiply the significand by. The exponent's sign character may be omitted if it's positive. Values should be normalized unless the source ieee754 value being represented is subnormal.

 * `0xa.3fb8p+42` = a.3fb8 x 2 ^ 42
 * `0x1.0p0` = 1

Base-16 notation should only be used to support legacy systems that can't handle decimal rounded values. Decimal floating point values tend to be smaller, and also avoid the false precision of binary floating point values. [More info](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md#how-much-precision-do-you-need)

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

**Values with exponential notation must be normalized (one digit, non-zero, to the left of the dot):**

| Invalid      | Valid        |
| ------------ | ------------ |
| `22e+50`     | `2.2e+51`    |
| `508.44e+10` | `5.0844e+12` |
| `-1000e+5`   | `-1.0e+8`    |
| `65.0e-20`   | `6.5e-21`    |
| `0.5e10`     | `5.0e9`      |
| `0x1f.33p+1` | `0x1.f33p+5` |

#### Infinity and Not a Number

The following are special floating point values:

 * `@inf`: Infinity
 * `-@inf`: Negative Infinity
 * `@nan`: Not a Number (quiet)
 * `@snan`: Not a Number (signaling)


### Numeric Whitespace

The `_` character can be used as "numeric whitespace" when encoding numeric values.

Rules:

 * Numeric values of any type can contain any amount of whitespace at any point after the first digit and before the last digit.
 * Special named values `@nan`, `@snan`, and `@inf` must not contain whitespace.

| Value       | Valid Whitespace     | Invalid Whitespace | Notes                                         |
| ----------- | -------------------- | ------------------ | --------------------------------------------- |
| `1000000`   | `1_000_000`          | `_1_000_000`       | `_1_000_000` would be interpreted as a string |
| `1000000`   | `1_000_000`          | `1_000_000_`       | `1_000_000_` would cause a decoding error     |
| `-7.4e+100` | `-7_._4__e_+___100`  | `-_7.4e+100`       |                                               |
| `@nan`      | `@nan`               | `@n_an`            |                                               |
| `-@inf`     | `-@inf`              | `-_@inf`           |                                               |

Numeric whitespace characters must be ignored when decoding numeric values.



Temporal Types
--------------

The temporal types deal with time.


### Temporal Type Notes

#### Field Width

Fields other than year can be pre-padded with zeros (`0`) up to their maximum allowed digits for aesthetic reasons if desired. Minutes and seconds must always be padded to 2 digits.

Field values must never be abbreviated (the year 19 refers to 19 AD, not 2019).

#### The Year Field

The year field can be any number of digits, and can be positive (representing AD dates) or negative (representing BC dates). Negative (BC) years are prefixed with a dash character (`-`). The year must always be written in full, and must not be abbreviated.

Note: The Anno Domini system has no zero year (there is no 0 BC or 0 AD), and so the year values `0` and `-0` are invalid. Many date systems internally use the value 0 to represent 1 BC and offset all BC dates by 1 for mathematical continuity, but in interchange formats it's better to avoid exposing potentially confusing internal details, and so the year value 0 is invalid.

#### Date Structure

All dates are structured according to the Gregorian calendar, containing a month and day-of-month.

#### Dates prior to 1582

Dates prior to the introduction of the Gregorian calendar in 1582 must be written according to the proleptic Gregorian calendar.


### Time Zones

A time zone refers to the political designation of a location having a specific time offset from UTC during a particular time period. Time zones are in a constant state of flux, and can change at any time for all sorts of reasons. There are two ways to denote a time zone: by area/location, and by global coordinates.

If the time zone is unspecified, it is assumed to be `Zero` (UTC).


#### Area/Location

The area/location method is the more human-readable of the two, but might not be precise enough for certain applications. Time zones are partitioned into areas containing locations, and are written in the form `Area/Location`. These areas and locations are specified in the [IANA time zone database](https://www.iana.org/time-zones). Area/Location timezones are case-sensitive because they tend to be implemented that way on most platforms.

##### Abbreviated Areas

Since there are only a limited number of areas in the database, the following abbreviations can be used in the area portion of the time zone:

| Area         | Abbreviation |
| ------------ | ------------ |
| `Africa`     | `F`          |
| `America`    | `M`          |
| `Antarctica` | `N`          |
| `Arctic`     | `R`          |
| `Asia`       | `S`          |
| `Atlantic`   | `T`          |
| `Australia`  | `U`          |
| `Etc`        | `C`          |
| `Europe`     | `E`          |
| `Indian`     | `I`          |
| `Pacific`    | `P`          |

##### Special Areas

The following special psuedo-areas can also be used. They do not contain a location component.

| Area    | Abbreviation | Meaning            |
| ------- | ------------ | ------------------ |
| `Zero`  | `Z`          | Alias to `Etc/UTC` |
| `Local` | `L`          | "Local" time zone, meaning that the accompanying time value is to be interpreted as if in the time zone of the observer. |

##### Examples

 * `E/Paris`
 * `America/Vancouver`
 * `Etc/UTC` == `Zero` == `Z`
 * `L`


#### Global Coordinates

The global coordinates method uses the global position to hundredths of degrees, giving a resolution of about 1km at the equator. Locations are written as longitude and latitude, separated by a slash character (`/`). Negative values are prefixed with a dash character (`-`), and the period character (`.`) is used as a decimal separator.

This method has the advantage of being unambiguous, which can be useful for areas that are in an inconsistent political state at a particular time. The disadvantage is that it's not easily decodable by humans.

##### Examples

 * `51.60/11.11`
 * `-13.53/-172.37`


### Date

Refers to a particular date without specifying a time during that day.

A date is made up of the following fields, separated by a dash character (`-`):

| Field | Mandatory | Min Value | Max Value | Max Digits |
| ----- | --------- | --------- | --------- | ---------- |
| Year  |     Y     |         * |         * |          * |
| Month |     Y     |         1 |        12 |          2 |
| Day   |     Y     |         1 |        31 |          2 |

#### Examples

 * `2019-8-5`: August 5, 2019
 * `5081-03-30`: March 30, 5081
 * `-300-12-21`: December 21, 300 BC (proleptic Gregorian)



### Time

Represents a time of day without specifying a particular date.

A time is made up of the following fields:

| Field        | Mandatory | Separator | Min Value | Max Value | Max Digits |
| ------------ | --------- | --------- | --------- | --------- | ---------- |
| Hour         |     Y     |           |         0 |        23 |          2 |
| Minute       |     Y     |    `:`    |         0 |        59 |          2 |
| Second       |     Y     |    `:`    |         0 |        60 |          2 |
| Subseconds   |     N     |    `.`    |         0 | 999999999 |          9 |
| Time Zone    |     N     |    `/`    |         - |         - |          - |

#### Notes

 * Hours are always written according to the 24h clock (21:00, not 9:00 PM).
 * Minutes and seconds must always be padded to 2 digits.
 * Seconds go to 60 to support leap seconds.
 * Since there is no date component, time zone data must be interpreted as if it were "today", and so the time might not remain constant should the political situation at that time zone change at a later date (except when using `Etc/GMT+1`, etc).
 * If the time zone is unspecified, it is assumed to be `Zero` (UTC).

#### Examples

 * `9:04:21`: 9:04:21 UTC
 * `23:59:59.999999999`: 23:59:59 and 999999999 nanoseconds UTC
 * `12:05:50.102/Z`: 12:05:50 and 102 milliseconds UTC
 * `4:00:00/Asia/Tokyo`: 4:00:00 Tokyo time
 * `17:41:03/-13.54/-172.36`: 17:41:03 Samoa time
 * `9:00:00/L`: 9:00:00 local time


### Timestamp

A timestamp combines a date and a time, separated by a slash character (`/`).

#### Examples

 * `2019-01-23/14:08:51.941245`: January 23, 2019, at 14:08:51 and 941245 microseconds, UTC
 * `1985-10-26/01:20:01.105/M/Los_Angeles`: October 26, 1985, at 1:20:01 and 105 milliseconds, Los Angeles time
 * `5192-11-01/03:00:00/48.86/2.36`: November 1st, 5192, at 3:00:00, at whatever is in the place of Paris at that time



Array Types
-----------

An "array" for the purposes of this spec represents a contiguous sequence of octets. The array type determines how those octets are to be interpreted.

Arrays begin with an encoding type, followed by the data enclosed within double-quotes (`"`) (no whitespace between the encoding type and the opening double-quote). [Strings](#string), are an exceptional case, and are handled differently.


### String

An array of UTF-8 encoded bytes. Strings must not contain BOM (u+feff), NUL (u+0000), or escape sequences that evaluate to those.

Strings must always resolve to complete, valid unicode sequences (for example, no unpaired surrogates) when fully decoded (i.e. after evaluating all escape sequences).

Unlike other array types, strings are not prefixed with an encoding type.

Strings are delimited using:

 * [A quoted sequence](#quoted-sequence)
 * [A verbatim sequence](#verbatim-sequence)
 * [Nothing at all](#unquoted-string)

#### Quoted Sequence

A quoted sequence encloses the string contents within double-quote delimiters (for example: `"a string"`). All characters leading up to the closing double-quote (including whitespace) are considered part of the string sequence, except where special processing occurs as a result of an escape sequence.

The backslash character (`\`) initiates an escape sequence inside a doube-quote enclosed sequence. The following escape sequences are allowed, and must be in lower case:

| Sequence            | Interpretation                  |
| ------------------- | ------------------------------- |
| `\`, u+000a         | continuation                    |
| `\`, u+000d, u+000a | continuation                    |
| `\\`                | backslash (u+005c)              |
| `\"`                | double quote (u+0022)           |
| `\r`                | carriage return (u+000d)        |
| `\n`                | linefeed (u+000a)               |
| `\t`                | horizontal tab (u+0009)         |
| `\x01` - `\xff`     | one octet, hexadecimal notation |
| `\u0001` - `\uffff` | unicode character               |

Escape sequences aid [human editability](#human-editability).

Example:

```
    "This string has a bare newline right here:
Everything except for escape sequences are read as-is, until the closing double-quote."
```

##### Continuation

A continuation causes the decoder to ignore all whitespace characters until it reaches the next printable character. For example:

```
    "The only people for me are the mad ones, the ones who are mad to live, mad to talk, \
    mad to be saved, desirous of everything at the same time, the ones who never yawn or \
    say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles \
    exploding like spiders across the stars."
```

The above string must be interpreted as:

```
The only people for me are the mad ones, the ones who are mad to live, mad to talk, mad to be saved, desirous of everything at the same time, the ones who never yawn or say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles exploding like spiders across the stars.
```

Note that a continuation doesn't have to be preceded by whitespace; it just generally looks nicer to do it that way.

#### Verbatim Sequence

A verbatim sequences is a section of a string that must not be interpreted in any way (no special interpretation of whitespace, character sequences, escape sequences, backticks etc) until the specified end sequence is encountered. The result must be a valid [string](#string).

A verbatim sequence is composed of the following:

 * Backtick (`` ` ``).
 * An end-of-string identifier, which is a sequence of printable, non-whitespace characters (in accordance with [human editability](#human-editability)).
 * A single whitespace sequence (either: SPACE `u+0020`, TAB `u+0009`, LF `u+000a`, or CR/LF `u+000d u+000a`).
 * The string contents.
 * Another instance of the end-of-string identifier to mark the end of the string.

Example:

```
`::: A verbatim string is not constrained like normal strings are.
It can contain problematic characters like ", `, \ and such without problems.

The `\` at the end of this line is not a continuation: \

Three colons (`:`) are being used to mark the end-of-string in this case,
so we can't use that exact character sequence in the string contents.

Whitespace, including newlines and "leading" whitespace, is also read verbatim.
        For example, this line really is indented 8 spaces.:::
```

#### Unquoted String

Strings must normally be delimited, but this rule can be relaxed if:

 * The string does not begin with a character from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, and underscore (`_`).
 * The string does not contain characters from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, numerals 0-9, underscore (`_`), dash (`-`), plus (`+`), period (`.`), colon (`:`), and slash (`/`).
 * The string does not end with a character from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, numerals 0-9, and underscore (`_`).
 * The string does not contain unicode characters or sequences that would be mistaken by a human reader for symbol characters in the u+0000 to u+007f range (``!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~``).
 * The string does not contain escape sequences or whitespace or line breaks or unprintable characters.
 * The string is not empty (`""`).

For example, these cannot be unquoted strings:

    "String with spaces"
    "String\twith\ttabs\nand\nnewlines"
    "[special-chars]"
    "ends-with-a-dash-"

These can be unquoted strings:

    string.two-25
    std:value
    _contains_underscores
    _150
    飲み物


### URI

Uniform Resource Identifier, structured in accordance with [RFC 3986](https://tools.ietf.org/html/rfc3986). Instances of the double-quote character(`"`), control characters, whitespace characters, line break characters, and any [problematic characters](#human-editability) must be percent-encoded.

URIs have the encoding type `u`.

#### Examples

    u"https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top"

    u"mailto:John.Doe@example.com"

    u"urn:oasis:names:specification:docbook:dtd:xml:4.1.2"

    u"https://example.com/percent-encoding/?double-quote=%22"


### Bytes

An array of octets. This data type should only be used as a last resort if the other data types cannot represent the data you need. To maximize cross-platform compatibility, multibyte data types stored within a binary blob should be represented in little endian byte order whenever possible.

The encoded contents can contain whitespace (CR, LF, TAB, SPACE) at any point. Implementations should use whitespace to keep line lengths reasonable (assume that humans will be reading and editing the document).

The supported encoding types are:

| Type      | Prefix | Bloat | Features                        |
| --------- | ------ | ----- | ------------------------------- |
| Hex       |    h   | 2.0   | Human readable                  |
| Base64url |    b   | 1.33  | Smaller size, compresses better |


#### Hex Encoding

Hex format encodes each byte into two characters (4 bits per character), using a lowercase hex alphabet (0-9, a-f). This is the most convenient method for human input of binary data.

Example:

    v1
    {
        ws_at_8  = h"39 12 82 e1 81 39 d9 8b 39 4c 63 9d 04 8c"

        ws_at_16 = h"1f48 ae45 63ff"

        large    = h"89504e470d0a1a0a0000000d4948445200000190000001900806000000
                     80bf36cc0000800049444154789cecbd09781cd599effd76b75a8b6559
                     926d6c831ddb60639b2d102060c06699400281c93224f91226ebc7bd73
                     6f6e76924c"
    }


#### Base64url Encoding

In CTE documents, base64url encoding uses the [RFC 4648](https://tools.ietf.org/html/rfc4648) url-safe alphabet (using `-` and `_`), with the following special rules:

 * Padding must not be used (the end delimiter `"` is enough to mark the end of the data stream).
 * Whitespace characters (CR, LF, TAB, SPACE) can occur anywhere in the stream to help align the contents in the document and keep line lengths down (the idea is to make it easy to open and modify a CTE document in your average text editor).

Example:

    v1
    {
        data = b"iVBORw0KGgoAAAANSUhEUgAAAZAAAAGQCAYAAACAvzbMAACAAElEQVR4nOy9CX
                 gc1Znv_Xa3WotlWZJtbIMd22Bjmy0QIGDAZplAAoHJMiT5Eibrx71zb252kkzu
                 zTKZPElmyJchIRMuWQkJSYBJyAaY"
    }



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

A list is ordered by default unless otherwise understood between parties (for example via a schema), or the user has specified that order doesn't matter. Other characteristics (such as non-duplicate data in the case of sets) must be a-priori understood between parties.

A list begins with an opening square bracket `[`, whitespace separated contents, and finally a closing bracket `]`.

Note: While this spec allows mixed types in lists, not all languages do. Use mixed types with caution.

#### Example

    [1 two 3.1 {} @nil]


### Map

A map associates objects (keys) with other objects (values). Keys can be any mix of scalar or array types. A key must not be a container type, the `@nil` type, or any value that evaluates to NaN (not-a-number). Values can be any mix of any type, including other containers.

A map is ordered by default unless otherwise negotiated between parties (for example via a schema), or the user has specified that order doesn't matter.

All keys in a map must resolve to a unique value, even across data types. For example, the following keys would clash:

 * `2000`
 * `2000.0`

Map entries are split into key-value pairs using the equals `=` character and optional whitespace. Key-value pairs must be separated from each other using whitespace. A key without a paired value is invalid.

A map begins with an opening curly brace `{`, whitespace separated key-value pairs, and finally a closing brace `}`.

Note: While this spec allows mixed types in maps, not all languages do. Use mixed types with caution.

#### Example

    {
        1 = alpha
        2 = beta
        "a map" = {one=1 two=2}
    }


### Markup

A markup container stores XML-style data, which is essentially a map of attributes followed by a list of content strings and other markup containers. Markup containers are best suited to presentation. For regular data, maps and lists are better.

The CTE encoding of a markup container is similar to XML, except:

 * There are no end tags. All data is contained within the begin `<`, content begin `|`, and end `>` characters.
 * Comments use `<*` and `*>` instead of `<!--` and `-->`. Comments can also be nested.
 * CTE uses less commonly occurring characters for escape initiators (for example, `\` instead of `&`).
 * Quotes are not actually required anywhere in a CTE markup container, depending on how you name things.
 * Non-string types can be stored in a markup container.

#### Markup Structure

A markup container structure in CTE consists of the following:

 * Markup container begin character (`<`).
 * Optional whitespace.
 * Container name.
 * Optional attributes section:
   - Mandatory whitespace.
   - A map of attributes (e.g. `x=1 y=5 z=true`).
 * Optional contents section:
   - Optional whitespace.
   - Contents-begin character (`|`).
   - A list of contents.
 * Optional whitespace.
 * Markup container end character (`>`).

Examples:

 * No attributes,  no children: `<br>`
 * Has attributes, no children: `<div id=fillme>`
 * No attributes, has children: `<span|Some text here>`
 * Has attributes and children: `<ul id=mylist style=boring | <li|first> <li|second> >`

Although it may look a little different, the internal structure is the same, and can easily be converted to/from XML & HTML.

#### Container Name

The container name follows the same rules as a [map key](#map). It's recommended to use values that are compatible with XML names, or can be automatically string-converted to such. See: [human editability](#human-editability).

#### Attributes Section

The attributes section behaves like a [map](#map). It's recommended to use values that are compatible with XML attribute names and values, or can be automatically string-converted to such. See: [human editability](#human-editability).

#### Contents Section

The contents section behaves similarly to a [list](#list), except that it can only contain [content strings](#content-string) and markup containers. The contents section is in string processing mode whenever it's not processing a sub-container (initiated by an unescaped `<` character).

#### Container End

The markup container ends when an unescaped `>` character is encountered while processing a [content string](#content-string). There are no separate "end tags" or slash characters like in XML.

#### Content String

A content string works similarly to the text content inside of an XML tag (such as `<a>text content</a>`).

Within a content string, a [verbatim sequence](#verbatim-sequence) can be started at any time using the backtick (`` ` ``) character. The verbatim contents of the sequence are appended to the growing content string. Literal backtick values outside of a verbatim sequence must be escaped.

The following escape sequences are recognised inside of a content string (except during [verbatim sequence](#verbatim-sequence) processing):

| Sequence            | Interpretation              |
| ------------------- | --------------------------- |
| `` \` ``            | backtick (u+0060)           |
| `\<`                | less-than (u+003c)          |
| `\>`                | greater-than (u+003e)       |
| `\\`                | backslash (u+005c)          |
| `\_`                | non-breaking space (u+00a0) |
| `\u0001` - `\uffff` | unicode character           |

#### Entity References

Entity references in CTE work the same as they do in XML and HTML, except that the standard escape character (`\`) is used instead of the ampersand (`&`) to initiate a reference (e.g. `\gt;` instead of `&gt;`).

#### Doctype

Use a [metadata map](#metadata-map) entry to specify a doctype:

```
( xml-doctype=[html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" u"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"] )
```

#### Style Sheets

Use a [metadata map](#metadata-map) entry to specify an XML style sheet:

```
( xml-stylesheet={type=text/xsl href=my-stylesheet.xsl} )
```


#### Example

```
v1
(xml-doctype=[html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" u"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"])
<html xmlns=u"http://www.w3.org/1999/xhtml" xml:lang=en |
    <body|
    <div id=parent style=normal ref-id=1 |
      <script| `##
        document.getElementById('parent').insertAdjacentHTML('beforeend', '<div id="idChild"> content </div>');
      ##>
        Here is some text <span style=highlighted| with highlighted text> and more text.
        <br>
        <ul|
            <li id=item_a | Item A>
            <li id=item_b | Item B>
            <li id=item_c | Item C>
        >

        <* MathML: ax^2 + bx + c *>
        <mrow |
          <mi|a> <mo|\InvisibleTimes;> <msup| <mi|x> <mn|2> >
          <mo|+> <mi|b> <mo|\InvisibleTimes;> <mi|x>
          <mo|+> <mi|c>
        >
    >
  >
>
```

This can easily be auto-converted to XML or HTML:

```
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
  <body>
    <div id="parent" style="normal" ref-id="1">
      <script>
      //<![CDATA[
        document.getElementById('parent').insertAdjacentHTML('beforeend', '<div id="idChild"> content </div>');
      //]]>
      </script>
        Here is some text <span style="highlighted">with highlighted text</span> and more text.
        <br/>
        <ul>
            <li id="item_a">Item A</li>
            <li id="item_b">Item B</li>
            <li id="item_c">Item C</li>
        </ul>

        <!-- MathML: ax^2 + bx + c -->
        <mrow>
          <mi>a</mi> <mo>&InvisibleTimes;</mo> <msup><mi>x</mi> <mn>2</mn> </msup>
          <mo>+</mo> <mi>b</mi> <mo>&InvisibleTimes;</mo> <mi>x</mi>
          <mo>+</mo> <mi>c</mi>
        </mrow>
    </div>
  </body>
</html>
```



Metadata
--------

Metadata is data about the data. It describes whatever data follows it in a document, which might or might not necessarily be of interest to a consumer of the data. For this reason, decoders are free to ignore and discard metadata if they so choose. Senders and receivers should negotiate beforehand how to react to metadata.

Metadata must only be placed in front of another object. It cannot be placed at the end of a document, or before the version specifier. A CTE document containing only metadata and no real objects (for example `v1 (a=1)`) is invalid.


### Metadata Association

Metadata objects are pseudo-objects that can be placed anywhere a real object can be placed, but do not count as objects themselves. Instead, metadata is associated with the object that follows it. For example:

    {"a key" = (some_metadata=500) "a value"}

In this case, the metadata refers to the value `"a value"`, but the actual data for purposes of decoding the map is `{"a key" = "a value"}`.

    { "a key" = (some_metadata=500) }

This map is invalid, because it resolves to `{"a key"}`, where no value is associated with the key (the metadata doesn't count).

Metadata can also refer to other metadata, for example:

    { (metadata_outer=1) (metadata_inner=1) "a key" = "a value" }

In this case, `metadata_outer` refers to `metadata_inner`, and `metadata_inner` refers to the string `"a key"`. The actual map is `{"a key" = "a value"}`.

#### Exception: Comments

The metadata association rules do not apply to [comments](#comment). Comments stand entirely on their own, and do not officially refer to anything, nor can any other metadata refer to a comment (i.e. comments are invisible to other metadata). This is to keep their usage consistent with how comments are used in existing languages.

    { (metadata_outer=1) (metadata_inner=1) /* a comment */ "a key" = "a value" }

In this case, `metadata_inner` still refers to `"a key"`, not `a comment`, and `a comment` doesn't officially refer to anything.


### Metadata Map

A metadata map contains keyed values which are associated with the object that follows the metadata map. Metadata maps are delimited by parentheses: `(` and `)`.

Keys in metadata maps follow the same rules as for regular maps, except that all string typed keys beginning with the underscore `_` character are reserved for predefined keys, and must only be used in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).

Implementations should make use of predefined metadata keys whenever possible to maximize interoperability between systems.

Example:

    v1
    // Metadata for the entire document
    (
        _ct = 2017.01.14-15:22:41/Z
        _mt = 2019.08.17-12:44:31/Z
        _at = 2019.09.14-09:55:00/Z
    )
    {
        ( _o=[u"https://all-your-data-are-belong-to-us.com"] )
        records = [
            // Metadata for "ABC Corp" record
            (
                _ct = 2019.05.14-10:22:55/Z
                _t = ["longtime client" "big purchases"]
            )
            (
                client = "ABC Corp"
                amount = 10499.28
                due = 2020.05.14
            )
            // Metadata for "XYZ Corp" record
            (
                _ct=2019.02.30-09:00:01/Z
                _mt=2019.08.17-12:44:31/Z
            )
            {
                client = "XYZ Corp"
                amount = 3994.01
                due = 2020.08.30
            }
        ]
    }


### Comment

Comments are user-defined string metadata equivalent to comments in a source code document. Comments do not officially refer to other objects, although conventionally they tend to refer to what follows in the document, be it a single object, a series of objects, a distant object, or they might even be entirely standalone. This is similar to how source code comments are used in most programming languages.

Comment contents must contain only complete and valid UTF-8 sequences. Escape sequences in comments are not interpreted (they are passed through verbatim).

Comments can be written in single-line or multi-line form. The single-line form starts with a double slash `//` and ends at a newline. The multi-line form starts with the sequence `/*` and ends with the sequence `*/`. They operate similarly to how comments operate in C-like languages, except that nested multiline comments are allowed.

Note: Comments must not be placed before the [version specifier](#version-specifier).

#### Comment Character Restrictions

The following characters are explicitly allowed:

 * Horizontal Tab (u+0009)
 * Linefeed (u+000a) - discarded in single line comments
 * Carriage Return (u+000d) - always ignored and discarded

The following characters are disallowed if they aren't in the above allowed section:

 * Control characters (such as u+0000 to u+001f, u+007f to u+009f).
 * Line breaking characters (such as u+2028, u+2029).
 * Byte order mark.

The following characters are allowed if they aren't in the above disallowed section:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

#### Example

    v1
    // Comment before top level object
    {
        // Comment before the "name" object.
        // And another comment.
        "name" = "Joe Average" // Comment after the "Joe Average" object.
        "email" = // Comment after the "email" key.
        /* Multiline comment with nested comment inside
          u"mailto:joe@average.org"
          /* Unlike in C, nested multiline
             comments are allowed */
        */
        u"mailto:someone@somewhere.com"
        "data" // Comment after data
        =
        //
        // Comment before some binary data (but not inside it)
        h"01 02 03 04 05 06 07 08 09 0a"
    }
    // Comments at the
    // end of the document.



Other Types
-----------

### Nil

Denotes the absence of data. Some languages implement this as the `null` value.

Note: Use nil judiciously and sparingly, as some languages might have restrictions on how and if it can be used.

#### Example

    @nil


### Marker

A marker tags an object in the document with a [tag value](#tag-value) such that it can be referenced in another part of the document. The next object following the marker is associated with the marker's tag value. Markers are not objects themselves, and are for all intents and purposes invisible to all other objects (they don't count as values in collections, for example).

A marker begins with the marker initiator (`*`), followed immediately (with no whitespace) by a [tag value](#tag-value).

Rules:

 * A marker cannot mark a comment; instead it would mark the next non-comment object.
 * A marker must not be followed by another marker (even with comments in between); markers must reference objects, not other markers.
 * Marker tags are globally unique to the document; duplicate tags are invalid.

Example:

    [ *remember_me "world" *1 {a = 1} ]

#### Tag Value

A tag value is a globally unique (to the document) identifier for marked objects. A tag value can be either a positive integer or an [unquoted string](#unquoted-string).

TODO: Relax tag string restriction to allow start with [0-9]?


### Reference

A reference is a shorthand used in place of an actual object to indicate that it is the same object as the one marked with the given tag value (it's much like a pointer, with the tag value acting as a labeled address). References can be useful for keeping the size down when there is repeating information in your document, or for following DRY principles in a configuration document. One could also use URI references as an include mechanism, whereby parts of a document are stored in separate locations.

A reference begins with the reference initiator (`#`), followed immediately (with no whitespace) by either a [tag value](#tag-value) or a [URI](#uri).

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

Example:

    {
        hello = #remember_me
        my_map = #1
        // references an object in relative file "common.ce", tag "legalese"
        substructure = #u"common.ce#legalese"
        // references the entire document at the specified URL
        my_document = #u"https://somewhere.com/my_document.cbe?format=long"
    }



Named Values
------------

Certain values cannot be expressed other than by their names. These named values are prefixed with an at (`@`) character:

| Value    | Meaning                  |
| -------- | ------------------------ |
| `@nil`   | No data                  |
| `@nan`   | Not a number (quiet)     |
| `@snan`  | Not a number (signaling) |
| `@inf`   | Infinity (signed value)  |
| `@true`  | Boolean true             |
| `@false` | Boolean false            |

Note: Named values must not be broken by whitespace.



Letter Case
-----------

A CTE document must be entirely in lower case, with the following exceptions:

 * String and comment contents: `"A string can contain UPPER CASE. Escape sequences must be lower case: \x3d"`
 * [Time zones](#time-zones) are case sensitive, and contain uppercase characters.
 * Binary and URI array contents can contain uppercase characters where allowed by their respective encoding specifications.

Everything else, including hexadecimal digits, exponents, and escape sequences, must be lower case.



Whitespace
----------

Whitespace characters outside of strings and comments must be ignored by a CTE parser. Any number of whitespace characters can occur in a sequence.


### Valid Whitespace Characters

While there are many characters classified as "whitespace" within the Unicode set, only the following are valid whitespace characters in a CTE document:

| Code Point | Name            |
| ---------- | --------------- |
| U+0009     | horizontal tab  |
| U+000A     | line feed       |
| U+000D     | carriage return |
| U+0020     | space           |


### Whitespace **can** occur:

 * Before an object.
 * After an object.
 * Between array/container openings & closings: `[`, `]`, `{`, `}`
 * Between encoding characters in a byte array, and between array delimiters `"`.

Examples:

 * `[   1     2      3 ]` is equivalent to `[1 2 3]`
 * `h" 01 02 03   0 4 "` is equivalent to `h"01020304"`
 * `{ 1="one" 2 = "two" 3= "three" 4 ="four"}` is equivalent to `{1="one" 2="two" 3="three" 4="four"}`


### Whitespace **must** occur:

 * Between values in a list (`[12"a string"]` is invalid).
 * Between key-value pairs in a map (`{1="one"2="two"3="three"4="four"}` is invalid).


### Whitespace **must not** occur:

 * Before the [version specifier](#version-specifier).
 * Between an array encoding type and the opening double-quote (`h "` is invalid).
 * Between a marker or reference initiator and its tag value (`* 1234` and `# u"mydoc.cbe"` are invalid).
 * Splitting a time value (`2018.07.01-10 :53:22.001481/Z` is invalid).
 * Splitting a numeric value (`3f h`, `9. 41`, `3 000`, `9.3 e+3`, `- 1.0` are invalid). Use the numeric whitespace character (`_`) instead.
 * Splitting named values: (`@t rue`, `@ nil`, `@i nf`, `@n a n` are invalid).


### Whitespace is interpreted literally (not ignored) within a string or comment:

    "This string has spaces
    and a newline, which are all preserved."

    //   Comment whitepsace      is preserved.



Invalid Encodings
-----------------

Invalid encodings must not be used, as they will likely cause problems or even API violations in certain languages. A parser must halt processing when invalid data is detected.

 * A CTE document must not contain the NUL (u+000) character, the BOM (u+feff) character, or any invalid characters.
 * All UTF-8 sequences must be complete and valid (no partial characters, unpaired surrogates, etc).
 * Times must be valid. For example: `2000-2-30`, while technically encodable, is not allowed.
 * Containers must be properly terminated. Extra container endings (`}`, `]`, etc) are invalid.
 * All map keys must have corresponding values. A key with a missing value is invalid.
 * Map keys must not be container types, the `@nil` type, or values the resolve to NaN (not-a-number).
 * Maps must not contain duplicate keys. This includes numeric keys of different types that resolve to the same value.
 * Metadata map keys beginning with `_` must not be used, except in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).
 * Upper case text is not allowed, except as described in section [Letter Case](#letter-case).
 * Whitespace must only occur as described in section [Whitespace](#whitespace).



Version History
---------------

July 22, 2018: First draft



License
-------

Copyright (c) 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
