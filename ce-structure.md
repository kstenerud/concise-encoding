Concise Encoding - Structural Specification
===========================================

Concise Encoding is a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

It is composed of 1:1 type compatible [text](cte-specification.md) and [binary](cbe-specification.md) formats, both of which follow the same structural and content rules laid out in this document. Examples are given in the [text](cte-specification.md) format for clarity.



Version
-------

Version 0 (prerelease)



Contents
--------

* [Terms](#terms)
* [Versioning](#versioning)
* [Structure](#structure)
* [Document Version Specifier](#document-version-specifier)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Floating Point](#floating-point)
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
  - [String-like Arrays](#string-like-arrays)
    - [String Character Replacement](#string-character-replacement)
    - [NUL](#nul)
    - [Line Endings](#line-endings)
    - [String](#string)
    - [Identifier](#identifier)
    - [Resource Identifier](#resource-identifier)
  - [Typed Array](#typed-array)
  - [Media](#media)
  - [Custom Types](#custom-types)
    - [Binary Encoding](#binary-custom-type)
    - [Text Encoding](#text-custom-type)
* [Container Types](#container-types)
  - [Container Properties](#container-properties)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
  - [Relationship](#relationship)
* [Other Types](#other-types)
  - [Nil](#nil)
* [Pseudo-Objects](#pseudo-objects)
  - [Marker](#marker)
  - [Reference](#reference)
  - [Comment](#comment)
  - [Padding](#padding)
  - [Constant](#constant)
  - [NA](#na)
* [Combined Objects](#combined-objects)
* [Empty Document](#empty-document)
* [Text Safety](#text-safety)
* [Equivalence](#equivalence)
* [Error Processing](#error-processing)
* [Security and Limits](#security-and-limits)
  - [Attack Vectors](#attack-vectors)
    - [Induced Data Loss](#induced-data-loss)
    - [Default Type Conversions](#default-type-conversions)
    - [Induced Omission](#induced-omission)
    - [Key Collisions](#key-collisions)
    - [Deserialization Complexity](#deserialization-complexity)
    - [Other Vulnerabilities](#other-vulnerabilities)
  - [Mitigations: Concise Encoding Codecs](#mitigations-concise-encoding-codecs)
    - [Validation](#validation)
    - [User-Controllable Limits](#user-controllable-limits)
    - [Automatic Rejection](#automatic-rejection)
  - [Mitigations: Application Guidelines](#mitigations-application-guidelines)
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



Versioning
----------

Concise Encoding is versioned, meaning that every Concise Encoding document contains the version of the Concise Encoding specification it adheres to. This ensures that any future incompatible changes to the format will not break existing implementations.

The Concise Encoding version is a single positive integer value, starting at 1.

#### Prerelease Version

During the pre-release phase, all documents **SHOULD** use version `0` so as not to cause potential compatibility problems once V1 is released. After release, version 0 will be permanently retired and considered invalid (there shall be no backwards compatibility to the prerelease spec).



Structure
---------

A Concise Encoding document is a binary or text encoded document containing data arranged in an ad-hoc hierarchical fashion. Data is stored serially, and can be progressively read or written.

Documents begin with a [version specifier](#version-specifier), followed by a top-level object. To store multiple values in a document, use a [container](#container-types) as the top-level object and store other objects within that container. For an [empty document](#empty-document), store [nil](#nil) as the top-level object.

    [version specifier] [object]

The top-level object **CAN** be preceded by [pseudo-objects](#pseudo-objects), but **MUST** itself be a real object of any type.

**Examples**:

 * Empty document: `c1 nil` (in [CBE](cbe-specification.md): [`03 01 7e`])
 * Document containing a comment (pseudo-object) and the top-level integer value 1000: `c1 /* a comment */ 1000`
 * Document containing a top-level list: `c1 [string1 string2 string3]`



Document Version Specifier
--------------------------

The version specifier is composed of a 1-byte type identifier (`c` for CTE, 0x03 for CBE) followed by the [version number](#version), which is an unsigned integer representing the version of this specification that the document adheres to.

**Example**:

 * CTE version 1: `c1`
 * CBE version 1: [`03 01`]



Numeric Types
-------------

The Concise Encoding format itself places no bounds on the range of most numeric types, but implementations (being bound by language, platform, and physical limitations) **MUST** [decide which ranges to accept](#user-controllable-limits).


### Boolean

Supports the values true and false.


### Integer

Integer values **CAN** be positive or negative, and **CAN** be represented in various bases (in [CTE](cte-specification.md)) and sizes. An implementation **MAY** alter base and size when encoding/decoding as long as the final numeric value remains the same.


### Floating Point

A floating point number is composed of a whole part, fractional part, and possible exponent. Floating point numbers **CAN** be binary or decimal. In a decimal floating point number, the exponent represents 10 to the power of the exponent value, whereas in a binary floating point number the exponent represents 2 to the power of the exponent value. Concise Encoding supports both decimal and binary floating point numbers in various sizes, configurations, and notations.

 * Decimal floating point number: 3.814 x 10⁵⁰
 * Binary floating point number:  7.403 x 2¹⁵

Binary floating point values in Concise Encoding adhere to the ieee754 binary floating point standard for 32-bit and 64-bit sizes, and [bfloat](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format) for 16-bit sizes. In [CBE](cbe-specification.md), they are directly stored in these formats. In [CTE](cte-specification.md), they are stored as textual representations that will ultimately be converted into these formats when evaluated by a machine. Following ieee754-2008 recommendations, the most significant bit of the significand field of an ieee754 binary NaN (not-a-number) value is defined as the "quiet" bit. When set, the NaN is quiet. When cleared, the NaN is signaling.

    s 1111111 1xxxxxxxxxxxxxxxxxxxxxxx = Quiet NaN (binary float32)
    s 1111111 0xxxxxxxxxxxxxxxxxxxxxxx = Signaling NaN (binary float32)

An implementation **MUST** preserve the signaling/quiet status of a NaN, and **MAY** discard the rest of the NaN payload information.

An implementation **MAY** alter the type and storage size of a floating point value when encoding/decoding as long as the final numeric value remains the same.

#### Value Ranges

Floating point types support the following ranges:

| Type    | Significant Digits | Exponent Min | Exponent Max |
| ------- | ------------------ | ------------ | ------------ |
| Binary  | 15.95              | -1022        | 1023         |
| Decimal | unlimited          | unlimited    | unlimited    |

Binary floats are limited to what is representable by 64-bit ieee754 binary float.

Although decimal floats technically have unlimited range, most implementations will have performance issues after a point, and thus require pragmatic [limit enforcement](#user-controllable-limits).

#### Special Floating Point Values

Both decimal and binary floating point numbers have representations for the following special values:

* Two kinds of zero: +0 and -0
* Two kinds of infinities: +∞ and -∞
* Two kinds of NaN (not-a-number): Signaling and quiet


### UID

A universal identifier. This identifier is designed to be unique across all identifiers in the universe.

Concise encoding version 1 uses [rfc4122 UUIDs](https://tools.ietf.org/html/rfc4122) as the implementation type.



Temporal Types
--------------

### Date

Represents a date without specifying a time during that day.

Dates in Concise Encoding are represented using the Gregorian calendar. Dates prior to 1582 are represented using the proleptic Gregorian calendar.

A date is made up of the following fields:

| Field | Mandatory | Min Value | Max Value |
| ----- | --------- | --------- | --------- |
| Year  |     Y     |         * |         * |
| Month |     Y     |         1 |        12 |
| Day   |     Y     |         1 |        31 |

 * Year, month, and day fields **MUST NOT** be 0 (counting starts at 1).
 * The year value **MUST** be negative to represent BC dates, and positive to represent AD dates. The Anno Domini system has no zero year (there is no 0 BC or 0 AD), so the year values `0` and `-0` are invalid.
 * The year field **MUST** contain the full year (no abbreviations).

**Examples**:

 * `2019-8-5`: August 5, 2019
 * `5081-03-30`: March 30, 5081
 * `-300-12-21`: December 21, 300 BC (proleptic Gregorian)


### Time

Represents a time of day without specifying a particular date.

A time is made up of the following fields:

| Field      | Mandatory | Min Value | Max Value |
| ---------- | --------- | --------- | --------- |
| Hour       |     Y     |         0 |        23 |
| Minute     |     Y     |         0 |        59 |
| Second     |     Y     |         0 |        60 |
| Subseconds |     N     |         0 | 999999999 |
| Time Zone  |     N     |         - |         - |

 * Hours are always according to the 24h clock (21:00, not 9:00 PM).
 * Seconds go to 60 to support leap seconds.
 * Since a time by itself has no date component, time zone data **MUST** be interpreted as if it were "today". This means that time zones which are not offsets like `Etc/GMT+1` might be interpreted differently on different dates for political reasons (for example daylight savings).
 * If the time zone is unspecified, it is assumed to be `Zero` (UTC).

**Examples**:

 * `23:59:59.999999999`: 23:59:59 and 999999999 nanoseconds UTC
 * `12:05:50.102/Z`: 12:05:50 and 102 milliseconds UTC
 * `4:00:00/Asia/Tokyo`: 4:00:00 Tokyo time
 * `17:41:03/-13.54/-172.36`: 17:41:03 Samoa time
 * `9:00:00/L`: 9:00:00 local time


### Timestamp

A timestamp combines a date and a time.

**Examples**:

 * `2019-01-23/14:08:51.941245`: January 23, 2019, at 14:08:51 and 941245 microseconds, UTC
 * `1985-10-26/01:20:01.105/M/Los_Angeles`: October 26, 1985, at 1:20:01 and 105 milliseconds, Los Angeles time
 * `5192-11-01/03:00:00/48.86/2.36`: November 1st, 5192, at 3:00:00, at whatever will be in the place of Paris at that time


### Time Zones

A time zone refers to the political designation of a location having a specific time offset from UTC at a particular time. Time zones are in a continual state of flux, and could change at any time for many reasons. There are two ways to denote a time zone: by area/location, and by global coordinates.

**Note**: If the time zone is unspecified, it is assumed to be `Zero` (UTC).

#### Area/Location

The area/location method is the more human-readable of the two, but might not be precise enough for certain applications. Time zones are partitioned into areas containing locations, and are written in the form `Area/Location`. These areas and locations are specified in the [IANA time zone database](https://www.iana.org/time-zones). Area/Location timezones have a minumum length of 1 character and a maximum length of 127 bytes, and are also case-sensitive because they tend to be implemented that way on most platforms.

**Note**: Some older IANA time zones (mostly deprecated ones) don't follow the `Area/Location` format (for example `MST`, `PST8PDT`). These **MUST** be supported.

**Note**: Some IANA time zones are split into extra sub-components for disambiguation (for example `America/Indiana/Petersburg`, which has area `America` and location `Indiana/Petersburg`). These **MUST** be supported.

See "[Theory and pragmatics of the tz code and data](https://data.iana.org/time-zones/tzdb-2020b/theory.html)" for more information.

##### Abbreviated Areas

Since there are only a limited number of areas in the database, the following abbreviations **CAN** be used in the area portion of the time zone to save space:

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

The following special pseudo-areas **CAN** also be used. They do not contain a location component.

| Area    | Abbreviation | Meaning            |
| ------- | ------------ | ------------------ |
| `Zero`  | `Z`          | Alias to `Etc/UTC` |
| `Local` | `L`          | "Local" time zone, meaning that the accompanying time value is to be interpreted as if in the time zone of the observer. |

**Examples**:

 * `E/Paris`
 * `America/Vancouver`
 * `Etc/UTC` == `Zero` == `Z`
 * `L`

#### Global Coordinates

The global coordinates method uses the global position to a precision of hundredths of degrees, giving a resolution of about 1km at the equator. Locations are stored as latitude and longitude values representing global coordinates.

This method has the advantage of being temporally unambiguous, which could be useful for areas that are in an inconsistent political state at a particular time such that area/location cannot be reliably determined. The disadvantage is that it's not as easily recognizable to humans.

**Examples**:

 * `51.60/11.11`
 * `-13.53/-172.37`


#### Time Offset

Time offset records only the offset from UTC rather than an actual location. It exists for historical reasons, and its use is discouraged except as a means to interface with legacy systems.

Time offset is recorded as an offset (+ or -) from UTC, recorded in hours and minutes as `±hhmm`.

**Examples**:

 * `+0530`
 * `-0100`


### How to Record Time

Time is one of the most difficult data types to get right. Aside from issues of synchronization, leap seconds, data container limitations and such, it's important to choose the correct **kind** of time to store, and the right kind depends on what the purpose of recording the time is.

There are three main kinds of time:

#### Absolute Time

Absolute time is a time that is fixed relative to UTC (or an offset from UTC). It is not affected by daylight savings time, nor will it be affected if an area's time zone changes through political action. Absolute time is best recorded in the UTC time zone, and is mostly useful for events in the past (because the time zone is now fixed at the time of the event, so it probably no longer matters what specific time zone was in effect).

#### Fixed Time

Fixed time is fixed to a particular place. If the time zone at that place changes, the fixed time's corresponding absolute time will change as well. For example, 12:00 in Toronto on June 1st is equivalent to 11:00 in Toronto on December 1st relative to UTC due to daylight savings. Fixed time is mostly useful for times in the future (such as an appointment in London this coming October 12th). If the expected time zone changes (for example an act of government such as a change to daylight savings), fixed time adapts automatically.

#### Floating Time

Floating (or local) time is always relative to the time zone of the observer. If you travel and change time zones, floating time changes time zones with you. If two observers in different time zones observe the same floating time, they will convert that same floating time to different absolute times. An example would be an 8:00 morning workout.


### When to Use Each Kind

Use whichever kind of time most succinctly and completely handles your time needs. Don't depend on time zone information as a proxy for a location; that's depending on a side effect, which is always brittle. Always store location information separately if it's important.

| Situation                            | Kind                                            |
| ------------------------------------ | ----------------------------------------------- |
| Log entries and past events          | Absolute                                        |
| Future events                        | Absolute or fixed (it depends on the use case)  |
| Appointments                         | Fixed                                           |
| Your daily schedule                  | Floating                                        |
| Multi-zone shared repeating schedule | Absolute or fixed (it depends on the use case)  |
| Deadlines                            | Usually fixed time, but possibly absolute time. |



Array Types
-----------

An array represents a contiguous sequence of fixed length elements. The length of an array is counted in elements (which are not necessarily bytes). The type of the array determines the size of its elements and how its contents are interpreted.

There are three primary kinds of array representations in Concise Encoding:

 * String-like arrays, which contain UTF-8 data. A string-like array's elements are always 8 bits wide, regardless of how many characters the bytes encode (the array length is in bytes, not characters).
 * [Typed arrays](#typed-array), whose contents represent elements of a particular size and type.
 * Custom types, which represent custom data that only a custom codec designed for them will understand. Elements of a custom type array are always considered 8 bits wide (regardless of the actual data the bytes represent), and are encoded either in the style of a uint8 array for custom binary, or a string-like array for custom text.


### String-like Arrays

String-like arrays are arrays of UTF-8 encoded bytes. String-like arrays **MUST** always resolve to complete, valid UTF-8 sequences when fully decoded. A string-like array containing invalid UTF-8 sequences **MUST** be treated as a [data error](#data-errors).

#### String Character Replacement

String-like arrays **CAN** contain characters that require special processing when decoded. This processing **MUST** occur *before* any validation takes place.

Special processing is required:

 * When escape sequences are encountered (in CTE documents only)
 * When a [NUL](#nul) character is encountered

#### NUL

String-like arrays can contain the NUL character (U+0000), which **MUST** be supported by the codec if the underlying implementation (language, platform etc) supports it.

Decoders **MUST** provide the option to automatically replace NUL with the Unicode replacement character (U+FFFD), and this option **MUST** default to enabled. Decoders that cannot support NUL due to language/platform restrictions **MUST** always replace NUL with the Unicode replacement character (U+FFFD).

**Note**: Because NUL is a troublesome character on many platforms, its use in documents is strongly discouraged.

##### Line Endings

Line endings **CAN** be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for data transmission, the canonical format is LF only. Decoders **MUST** accept both line ending types as input, but encoders **SHOULD** only output LF when the destination is a foreign or unknown system.

#### String

A basic UTF-8 string.

#### Identifier

An identifier is a type designed to be unique within a local context. It's implemented as a string with the following additional requirements and restrictions:

 * It **MUST** begin with one of:
   - A letter or numeric character ([base categories "L" and "N" in Unicode](https://unicodebook.readthedocs.io/unicode.html#categories))
   - `_` (underscore)
 * It **MUST ONLY** contain:
   - Letter, numeric, and mark characters ([base categories "L", "M", and "N" in Unicode](https://unicodebook.readthedocs.io/unicode.html#categories))
   - `_` (underscore)
   - `:` (colon)
   - `-` (dash)
   - `.` (period)
 * It **MUST NOT** end with a colon (`:`).
 * Colons **MUST NOT** must not be placed adjacent to each other (`a::b` is invalid).
 * The maximum length is 127 **bytes** (not characters).
 * The minimum length is 1 **character** (identifiers **CANNOT** be empty strings).
 * Comparisons are case insensitive when doing lookups or testing for uniqueness.
 * All identifier declarations (not usages) **MUST** be unique within their context. A duplicate identifier declaration within a particular context is a [structural error](#structural-errors).
 * An identifier **MUST NOT** be [marked](#marker).
 * An identifier **MUST NOT** be the object referred to by a [reference](#reference) or [constant](#constant).
 * An identifier **MUST NOT** be preceded by or consist of [pseudo-objects](#pseudo-objects) (for example, it must not be preceded by a [comment](#comment)).
 * The identifier is not a general type; it's only allowed where specifically noted in this document.

The colon (`:`) character has a special purpose as a **namespace separator**. The exact meaning and function of a namespace is application-specific, however it is most commonly used to mark an identifier's provenance in order to prevent naming collisions. An identifier containing multiple colons is considered to be in a multi-level namespace. An identifier that does not contain any colon characters is considered to be in the *default* namespace.

**Note**: Concise Encoding itself doesn't place any significance on namespaces; it only cares about the uniqueness of an identifier's string representation.

**Examples**:

 * `123`
 * `some_id`
 * `25th`
 * `猫`
 * `foo:bar`

#### Resource Identifier

A resource identifier is a text-based (UTF-8) universally unique identifier that can be resolved by a machine. The most common resource identifier types are [URLs](https://tools.ietf.org/html/rfc1738), [URIs](https://tools.ietf.org/html/rfc3986), and [IRIs](https://tools.ietf.org/html/rfc3987). Validation of the resource ID is done according to its type. If unspecified by a schema, the default resource identifier type is IRI.

Resource IDs **CAN** also be [combined](#combined-objects) with a [string](#string), where the string is appended to the resource ID to create a final combined value. This is normally done in conjunction with [references](#reference) to cut down on repetition.

When combination occurs, validation **MUST** be done in two steps:

* Each side of the combination is individually validated as a string.
* The combined value is validated as a resource ID.

**Examples**:

    @"https://x.com/"             // = https://x.com/
    @"https://x.com/":"something" // = https://x.com/something
    @"https://x.com/":"1234"      // = https://x.com/1234


### Typed Array

A typed array encodes an array of values of a fixed type and size. In a CBE document, the array elements will all be adjacent to each other, allowing large amounts of data to be easily copied between the stream and your internal structures.

The following element types are supported in typed arrays. For other types, use a [list](#list).

| Type                 | Element Sizes (bits) |
| -------------------- | -------------------- |
| Bit                  | 1                    |
| Unsigned Integer     | 8, 16, 32, 64        |
| Signed Integer       | 8, 16, 32, 64        |
| BFloat               | 16                   |
| IEEE754 Binary Float | 32, 64               |
| UID                  | 128                  |

Array elements **CAN** be written using any of the representations allowed for the specified type and size.

**Examples**:

 * `|u8x 9f 47 cb 9a 3c|`
 * `|f32 1.5 0x4.f391p100 30 9.31e-30|`
 * `|i16 0b1001010 0o744 1000 0xffff|`
 * `|u 3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f|`
 * `|b 1 1 0 1 0|`


### Media

The media object encapsulates a foreign media object/file, along with its string [media type](http://www.iana.org/assignments/media-types/media-types.xhtml).

    [media type] [data]

The media object's internal encoding is not the concern of a Concise Encoding codec; CE merely sees its data as a sequence of bytes, and passes it along as such.

A decoder **MUST NOT** attempt to validate the media type beyond checking the allowed character range per [rfc2045](https://tools.ietf.org/html/rfc2045). An unrecognized media type is not a decoding error.

**Note**: [Multipart types](https://www.iana.org/assignments/media-types/media-types.xhtml#multipart) are not supported, as there's no unified way to unambiguously represent them as a single byte stream.

**Example**:

```cte
|application/x-sh 23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a|
```

Which is the shell script:

```sh
#!/bin/sh

echo hello world
```


### Custom Types

There are some situations where a custom data type is preferable to the standard types. The data might not otherwise be representable, or it might be too bulky using standard types, or you might want the data to map directly to/from memory structs for performance reasons.

Custom types restrict interoperability to implementations that understand the types, and **SHOULD** only be used as a last resort. An implementation that encounters a custom type it doesn't know how to decode **MUST** report it as a [data error](#data-errors).

Custom type implementations **SHOULD** provide both a binary and a text encoding, with the binary encoding preferred for CBE documents, and the text encoding preferred for CTE documents. When both binary and text forms of a custom type are provided, they **MUST** be 1:1 convertible to each other without data loss.

**Note**: Although custom types are encoded as "array types", the interpretation of their contents is user-defined, and they might not represent an array at all.

#### Binary Custom Type

A uint8 array value representing a user-defined custom data type. The interpretation of the octets is user-defined, and is only decodable by receivers that know how to decode it. To reduce cross-platform confusion, data **SHOULD** be represented in little endian byte order.

**Example**:

    |cb 93 12 04 f6 28 3c 40 00 00 40 40|

Which is binary data representing a fictional example custom "cplx" struct

      {
          type:uint8 = 4
          real:float32 = 2.94
          imag:float32 = 3.0
      }

#### Text Custom Type

A string-like array value representing a user-defined custom data type. The interpretation of the string is user-defined, and is only decodable by receivers that know how to decode it.

**Example**:

    |ct cplx(2.94+3i)|



Container Types
---------------

Container types hold collections of other objects. 


### Container Properties

#### Ordering

If a container is ordered, the order in which objects are placed in the container matters. Ordered containers that contain equivalent objects but in a different order are NOT equivalent.

#### Duplicates

For list-like containers, a duplicate means any object that is equivalent to another object already present in the list.

For map-like containers, a duplicate means any key-value pair whose key is equivalent to another key already present in the map, regardless of what the key's associated value is.

The testing of integer and float values for duplicates transcends the data type when the value is convertible without loss. For example, the integer value `2000` and the float value `2000.0` are considered duplicates. The string value `"2000"`, however, would not be a duplicate.

If a container disallows duplicates, duplicate entries are [structural errors](#structural-errors).


### List

A sequential list of objects. Lists **CAN** contain any mix of any type, including other containers.

By default, a list is ordered and allows duplicate values. Different rules **CAN** be set using a schema.

**Example**:

```cte
c1 [
    1
    "two"
    3.1
    {}
    nil
]
```


### Map

A map associates key objects with value objects. Keys **CAN** be any mix of any [keyable type](#keyable-types). Values **CAN** be any mix of any type, including other containers.

Map entries are stored as key-value pairs. A key without a paired value is invalid.

By default, a map is unordered and does not allow duplicate keys. Different rules **CAN** be set using a schema.

#### Keyable types

Only certain types **CAN** be used as keys in map-like containers:

* [Numeric types](#numeric-types), except for NaN (not-a-number)
* [Temporal types](#temporal-types)
* [Strings](#string)
* [Resource identifiers](#resource-identifier)
* [References](#reference) (only if the referenced value is keyable)

[Nil](#nil) and [NA](#na) **MUST NOT** be used as a key.

**Example**:

```cte
c1 {
    1 = "alpha"
    2 = "beta"
    "a map" = {one=1 two=2}
    2000-01-01 = "New millenium"
}
```


### Markup

Markup is a specialized data structure composed of a name, a map (containing attributes), and a list of contents that **CAN** contain string data and child nodes (popularized in XML). Markup containers are generally best suited for presentation data. For regular data, maps and lists are usually better.

    [name] [attributes (**OPTIONAL**)] [contents (**OPTIONAL**)]

 * Name is an [identifier](#identifier).
 * The attributes section behaves like a [map](#map). An attribute key **MUST NOT** be empty (string, resource ID).
 * The contents section behaves similarly to a [list](#list), except that it **MUST** only contain:
   - [Content strings](#content-string)
   - [Comments](#comment)
   - Other markup containers

Illustration of markup encodings:

| Attributes | Contents | Example (CTE format)                                           |
| ---------- | -------- | -------------------------------------------------------------- |
|     N      |    N     | `<br>`                                                         |
|     Y      |    N     | `<div "id"="fillme">`                                          |
|     N      |    Y     | `<span,Some text here>`                                        |
|     Y      |    Y     | `<ul "id"="mylist" "style"="boring", <li,first> <li,second> >` |

##### Content String

Content strings behave much like regular strings, except that:

 * Leading and trailing collapsible whitespace **MUST** be removed from each line in the string before being passed on from the decoder.
 * If the last line ends in collapsible whitespace and is not otherwise empty, the collapsible whitespace at the end **MUST** be converted to a single space (U+0020).

For the purposes of this rule:

 * Collapsible whitespace is a sequence containing only the TAB (U+0009) and SPACE (U+0020) characters.
 * A line is a sequence of characters delimited by either a line breaking sequence [(Unicode classes BK, CR, LF, NL)](http://www.unicode.org/reports/tr14/#Table1), or the beginning or end of the content string.
 * Leading collapsible whitespace is collapsible whitespace that occurs between the beginning of the line and the first non-collapsible whitespace character.
 * Trailing collapsible whitespace is collapsible whitespace that occurs between the last non-collapsible whitespace character and the end of the line.

CTE encoders **MAY** add leading collapsible whitespace to content strings for aesthetic purposes.

**Note**: At the application level, content strings generally follow the same rules as [HTML content strings](https://www.w3.org/TR/html4/struct/text.html#h-9.1), but this is beyond the scope of Concise Encoding.

**Note**: Concise Encoding doesn't interpret [entity references](https://en.wikipedia.org/wiki/SGML_entity); they **MUST** be treated as regular text by decoders and never converted.

**Example**:

```cte
c1 <View;
    <Image "src"=@"images/avatar-image.jpg">
    <Text,
        Hello! Please choose a name!
    >
    /* <HRule "style"="thin"> */
    <TextInput "id"="name" "style"={"height"=40 "borderColor"="gray"}, Name me! >
>
```


### Relationship

A relationship is a container-like structure for making statements about resources in the form of subject-predicate-object triples (like in [RDF](https://en.wikipedia.org/wiki/Resource_Description_Framework)). Relationships form edges between nodes ([resources](#resource) or values) to build a semantic graph. Local resources are anonymous by default, but **CAN** be made addressable by [marking them](#marker).

A relationship is composed of the following three components (in order):

 * Subject, which **MUST** be a [resource](#resource) or a [list](#list) of [resources](#resource), and **MUST NOT** be [nil](#nil) or [na](#na)
 * Predicate, which **MUST** be a [resource identifier](#resource-identifier) that represents a semantic predicate, and **MUST NOT** be [nil](#nil) or [na](#na)
 * Object, which **CAN** be any value.

#### Resource

A resource is one of:

 * a [map](#map) (entity)
 * a [relationship](#relationship)
 * a [resource identifier](#resource-identifier)

#### Maps as Relationships

[Maps](#map) **CAN** also be used to represent relationships because they are natural relationship structures (where the map itself is the subject, the key is the predicate, and the value is the object). In Concise Encoding, the key-value pairs of a map are only considered [relationships](#relationship) if their types match the requirements for the predicate and object of a relationship.

Using maps to represent relationships is a common tactic to make the document more concise and the graph structure easier to follow, but there's no way to make relationships expressed as key-value pairs addressable (and thus they **MUST NOT** be used as resources). This is generally not a problem because few relationships actually need to be used as resources in real-world applications.

**Note**: Concise Encoding itself provides no mechanism to enforce the concept of a map specifically representing relationships. Such interpretations can only be enforced at the application or schema level.

**Examples**:

At their most basic, relationships are simply 3-component statements containing a subject, a predicate, and an object:

```cte
c1 [
    (@"https://springfield.gov/people#homer_simpson" @"https://example.org/wife" @"https://springfield.gov/people#marge_simpson")
    (@"https://springfield.gov/people#homer_simpson" @"https://example.org/employer" @"https://springfield.gov/employers/nuclear_power_plant")
]
```

Using the full resource identifier is tedious, but we could use [markers](#marker) and [combination](#combined-objects) to make things more manageable:

```cte
c1 {
    "resources" = [
        &people:@"https://springfield.gov/people#"
        &employers:@"https://springfield.gov/employers/"
        &mp:@"https://mypredicates.org/"
    ]
    "statements" = [
        ($people:"homer_simpson" $mp:"wife" $people:"marge_simpson")
        ($people:"homer_simpson" $mp:"employer" $employers:"nuclear_power_plant")
    ]
}
```

We could also use map syntax to model most relationships, which often makes the graph more clear to a human reader:

```cte
c1 {
    "resources" = [
        &people:@"https://springfield.gov/people#"
        &employers:@"https://springfield.gov/employers/"
        &mp:@"https://mypredicates.org/"
    ]
    "relationships" = {
        $people:"homer_simpson" = {
            $mp:"wife" = $people:"marge_simpson"
            $mp:"employer" = $employers:"nuclear_power_plant"
        }
    }
}
```

With map syntax, there's no way to mark relationships. When relationship marking is needed, use standard relationship statements:

```cte
c1 {
    "resources" = [
        &people:@"https://springfield.gov/people#"
        &mp:@"https://mypredicates.org/"
        &mo:@"https://myobjects.org/"
    ]

    "relationships" = {
        $people:"homer_simpson" = {
            $mp:"wife" = $people:"marge_simpson"
            $mp:"regrets" = [
                $firing
                $forgotten_birthday
            ]
        }
    }

    "relationship statements" = [
        &marge_birthday:($people:"marge_simpson" $mp:"birthday" 1956-10-01)
        &forgotten_birthday:($people:"homer_simpson" $mp:"forgot" $marge_birthday)
        &firing:($people:"montgomery_burns" $mp:"fired" $people:"homer_simpson")

        // Multiple relationship subjects
        ([$firing $forgotten_birthday] $mp:"contribute" $mo:"marital_strife")
    ]
}
```

**Note**: If the previous document were published at `https://mysite.org/data.cte`, all [markers](#marker) would be accessible using fragments:

 * `https://mysite.org/data.cte#marge_birthday`
 * `https://mysite.org/data.cte#forgotten_birthday`
 * `https://mysite.org/data.cte#firing`



Other Types
------------

### Nil

Nil (also known as "Null") represents the intentional absence of data. It is most commonly used in update operations to indicate the clearing of a field. For read and create operations, it's better to simply omit empty fields altogether.

**Note**: Do not use nil to indicate an error condition; use [NA](#na) instead.



Pseudo-Objects
--------------

Pseudo-objects stand-in for real objects, add additional context or meaning to another object, or affect the interpreted structure of the document in some way.

Pseudo-objects **CAN** be placed anywhere a full object can be placed, with the following additional restrictions:

 * Pseudo-objects **MUST NOT** be placed before or used as an [identifier](#identifier) (for example, `&#myconst:"something"` and `&/*comment*/:"something"` are invalid).
 * Pseudo-objects **MUST NOT** be placed inside a [typed array's] contents (for example, `|u8x 11 22 #myconst 44|`, `|u8x 11 22 $myref 44|`, and `|u8 1 2 /*comment*/ 3 4|` are invalid).
 * Pseudo-objects **MUST NOT** be used as the right-side of [combined objects](#combined-objects) (for example, `#myconst:$myref`, `&myref1:$myref2`, and `@"http://x.com/":#myconst` are invalid).

**Note**: Like real objects, pseudo-objects **MUST NOT** appear before the [version specifier](#version-specifier), and **MUST NOT** appear after the top-level object.


### Marker

A marker is a [combined](#combined-object) pseudo-object that assigns a marker ID to another object, which can then be [referenced](#reference) in another part of the document (or from a different document).

    [marker id] [marked object]

#### Marker ID

The marker ID is an [identifier declaration](#identifier), except with the additional restriction that it cannot contain a colon (`:`) since it would clash with the id-value seprator in CTE.

Marker IDs are declared identifiers, and therefore **MUST** be unique within the current document.

#### Rules

 * A marker construct **MUST NOT** contain other pseudo-objects (i.e. no markers-to-markers, markers-to-references, or comments between left-side and right-side).
 * A marker **CANNOT** mark an object in a different container level. For example: `(begin-list) (marker ID) (end-list) (string)` is invalid.

**Example**:

```cte
c1 [
    &remember_me:"Pretend that this is a huge string"
    &1:{a = 1}
]
```

The string `"Pretend that this is a huge string"` is marked with the ID `remember_me`, and the map `{a=1}` is marked with the ID `1`.


### Reference

A reference acts as a stand-in for an object that has been [marked](#marker) elsewhere in this or another document. This could be useful for repeating or cyclic data.

References can take one of two forms:

 * Local reference, where the [identifier](#identifier) is a reference to an object marked by a [marker ID](#marker) elsewhere in the current document.
 * Remote reference, where the [resource ID](#resource-id) is a reference to another document or to a [marker ID](#marker) in another document.

#### Rules

 * Recursive references are allowed.
 * References **CAN** refer to objects [marked](#marker) later in the document (forward references).
 * A reference used as a map key **MUST** refer to a [keyable type](#keyable-types).
 * A resource identifier reference **MUST NOT** be used as a map key (because there's no way to know if it refers to a keyable type without following the reference).
 * A resource identifier reference **MUST** point to either:
   - Another CBE or CTE document (using no fragment section, thus referring to the entire document)
   - A marker ID inside another CBE or CTE document, using the fragment section of the resource identifier as the local marker ID

A decoder **MUST NOT** automatically follow resource ID references because they pose a security risk. Applications **SHOULD** define security rules for following resource identifier references.

**Example**:
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

A comment is a list-style pseudo-object that **MUST** only contain strings or other comment containers (to support nested comments).

Comments do not support escape sequences. Character sequences that look like escape sequences in comments **MUST NOT** be processed (they are passed through verbatim).

Whitespace in a comment string is treated the same as in [markup content strings](#content-string).

Comments are valid in places where a real object would be valid, but they are specifically disallowed:

 * Between the left object and the `=` in a key-value pair (`{ a /* comment */ = 1 }` is invalid).
 * Between left and right side of a [combined object](#combined-objects) (`@"http://x.com/"/*comment*/:1234` and `&1234:/*comment*/"something"` are invalid).
 * Before an [identifier](#identifier) (`&/*comment*/1234` and `</*comment*/View>` are invalid).

Also, like all objects, comments **CANNOT** be placed:

 * Inside of typed arrays (`|u8x 11 22 /* comment */ 33 44|` is invalid).
 * Before the [version specifier](#version-specifier) (`/* comment */ c1 {}` is invalid).
 * After the top-level object (`c1 {} /* comment */` is invalid).

Implementations **MUST** provide a configuration option to ignore comments (meaning that comments will be skipped over without passing their contents along), and **MUST** default to ignoring.

#### Comment String Character Restrictions

The following characters are allowed in comments:

 * Horizontal Tab (u+0009)
 * Linefeed (u+000a)
 * Carriage Return (u+000d)
 * UTF-8 [text-safe](#text-safety) printable characters
 * UTF-8 [text-safe](#text-safety) whitespace characters

The following character sequences **MUST NOT** be put into comment strings because they are comment delimiters in CTE:

* `/*`
* `*/`

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

    "data" =
    // Comment before some binary data (but not inside it)
    |u8x 01 02 03 04 05 06 07 08 09 0a|
}
```


### Padding

Padding is a completely invisible type available only in CBE, whose sole purpose is data alignment.

The padding type **CAN** occur any number of times where a CBE type field is valid (padding doesn't share the [additional restrictions of other pseudo-objects](#pseudo-objects)).

A CBE decoder **MUST** consume and discard padding. A CBE encoder **MAY** add padding between objects to cause other data to fall on an aligned address for faster direct reads from the buffer on the receiving end.


### Constant

Constants are custom named values that have been defined in a schema. They can be used in the same fashion as the normal specification-defined named values (such as `true`, `nan`, `nil`, etc). Constants are only available to CTE documents. CBE documents must store the actual value instead.

A constant's name is an [identifier](#identifier).

A constant **MUST** be defined in a schema to represent a real object, **not** a pseudo-object or pseudo-object combination.

CTE decoders **MUST** look up (in the schema) all constants encountered in a CTE document and use the values they refer to. A lookup failure is a [data error](#data-errors).

CTE encoders **MUST** use constant names where required by the schema.


### NA

NA ("Not Available") is a pseudo-object that indicates missing data (data that was expected be there but is not for some reason). The [combined](#combined-objects) reason field (which **MUST** be a real object) describes why the data is missing.

    [NA] [Reason]

NA implies an error condition. For intentional "no-data", use [nil](#nil).

Decoders **MUST** provide a configuration option to choose whether NA is treated as a [structural error](#structural-errors) or a [data error](#data-errors), and **MUST** default to structural error.

**NA MUST NOT be used as**:

 * A [map key](#map).
 * An [identifier](#identifier).
 * The media type field in a [media object](#media).
 * A content string in a [markup container](#markup) or [comment](#comment).
 * The predicate in a [relationship](#relationship).

**Some possible reasons for NA**:

 * The system doesn't have the requested data.
 * Rules prevent the data from being provided.
 * There was an error while fetching or computing the data.
 * The data could not be provided in the requested form.

**Examples**:

 * `na:"Insufficient privileges"` (not available, with a text reason)
 * `na:404` (not available for reason code 404)
 * `na:nil` (not available for unknown reason)



Combined Objects
----------------

Combined objects are objects that are composed of two sub-objects.

    [left-object] combined-with [right-object]

The effect of this combination could be to concatenate one object to another (for example [resource IDs](#resource-id)), assign metadata to an object (for example [markers](#marker), or to clarify meaning (for example [NA](#na)).

**Note**: In CTE, combining is indicated by the `:` character. In CBE, the combination is implied by the type field.

### Rules

To keep the complexity down, combined objects have the following rules:

 * Combined objects are considered a single object. This means for example that in `&123:@"http://example.com/":"999"`, the marker ID `123` refers to `http://example.com/999`, not `http://example.com/`.
 * Combined objects **MUST NOT** be further combined (`@"http://x.com/":string:string` is invalid).
 * [Pseudo-objects](#pseudo-objects) are not allowed on the right-side of a combination. (`@"http://x.com/":$ref-to-string` and `na:/*comment*/string` are invalid).
 * Only the following types **CAN** be the left-side of a combination:

| Left Type                   | Allowed Right Side Types | Combining is Mandatory |
| --------------------------- | ------------------------ | ---------------------- |
| [Resource ID](#resource-id) | String                   | No                     |
| [Marker](#marker)           | Any Real Object          | Yes                    |
| [NA](#na)                   | Any Real Object          | Yes                    |

**Example**:

```cte
c1 {
    resources = [
        &ex:@"http://example.com/"     // "ex" refers to http://example.com/
    ]
    urls = [
        @"http://example.com/":path        // http://example.com/path
        @"http://example.com/":"long/path" // http://example.com/long/path
        $ex:path                            // http://example.com/path
        &long-path:$ex:"long/path"          // "long-path" refers to http://example.com/long/path
        $long-path:"a/b/c/d"                // http://example.com/long/path/a/b/c/d
    ]
}
```



Empty Document
--------------

An empty document is signified by using the [Nil](#nil) type as the top-level object:

* In CBE: [`03 01 7e`]
* In CTE: `c1 nil`



Text Safety
-----------

Because Concise Encoding is a twin format (text and binary), every character printed in a CTE document **MUST** be editable by a human. Such human-editable characters are considered "text safe".

The following Unicode codepoints are text-unsafe:

 * Codepoints in general category `C`, except for TAB (u+0009), LF (u+000a), and CR (u+000d)
 * Codepoints in categories `Zl` and `Zp`

Text-unsafe characters **MUST NOT** appear in their raw form in a CTE document. If the type allows escape sequences, such characters **MUST** be represented as escape sequences in a CTE document.



Equivalence
-----------

There are many things to consider when determining if two Concise Encoding documents are equivalent. This section helps clear up possible confusion.

**Note**: Equivalence is relaxed unless otherwise specified.


### Relaxed Equivalence

Relaxed equivalence is concerned with the question: Does the data destined for machine use come out essentially the same, even if there are some minor structural and type differences?

#### Numeric Types

Numeric values (integers and floats) do not have to be of the same type or size in order to be equivalent. For example, the 32-bit float value 12.0 is equivalent to the 8-bit integer value 12. So long as they resolve to the same effective value without data loss after type coercion, they are equivalent.

Infinities with the same sign are considered equivalent.

**Note**: In contrast to ieee754 rules, two floating point values ARE considered equivalent if they are both NaN so long as they are both the same kind of NaN (signaling or quiet).

#### Custom Types

Unless the schema specifies otherwise, custom types are compared byte-by-byte, with no other consideration to their contents. Custom text values **MUST NOT** be compared with custom binary values unless the receiver understands the custom type.

#### Strings

Strings are considered equivalent if their contents are equal. Comparisons are case sensitive unless otherwise specified by the schema.

#### Arrays

Arrays **MUST** contain the same number of elements, and those elements **MUST** be equivalent.

The equivalence rules for numeric types also extends to numeric arrays. For example, the 16-bit unsigned int array `1 2 3`, 32-bit integer array `1 2 3`, and 64-bit float array `1.0 2.0 3.0` are equivalent under relaxed equivalence.

#### Containers

Containers **MUST** be of the same type. For example, a map is never equivalent to a list.

Containers **MUST** contain the same number of elements, and their elements **MUST** be equivalent.

By default, list types **MUST** be compared ordered and map types unordered, unless their ordering was otherwise specified by the schema.

#### Markup Contents

Extraneous whitespace in a markup contents section is elided before comparison. Comparisons are case sensitive unless otherwise specified by the schema.

#### NA

[NA](#na) values are always considered equivalent to each other regardless of the reasons.

#### Nil

[Nil](#nil) values are always considered equivalent to each other.

#### Comments

Comments are ignored when testing for relaxed equivalence.

#### Padding

Padding is always ignored when testing for equivalence.


### Strict Equivalence

Strict equivalence concerns itself with differences that could still technically have an impact on how the document is interpreted, even if the chances are low:

* Objects **MUST** be of the same type and size.
* Comments are compared, but extraneous whitespace is elided before comparison. Comparisons are case sensitive unless otherwise specified by the schema.



Error Processing
----------------

Errors are an inevitable part of the decoding process. This section lays out how to handle errors. There are two major kinds of decoding errors:

### Structural Errors

Structural errors are the kinds of errors that imply or cause a malformed document structure, affect lookups, or hit a limit that stops the object from being ingested. This could be due to things such as:

 * Improper document structure (mismatched container start/end, etc).
 * Incorrect data types for the current context (map keys, object combinations, etc).
 * Malformed identifiers.
 * Failed reference lookup.
 * Failed [global limit checks](#user-controllable-limits).

A decoder **MUST** stop processing and issue a diagnostic when a structural error occurs.

### Data Errors

Data errors affect the reliability of a particular object, but don't compromise confidence in the decoder's ability to continue decoding the rest of the document. Some examples are:

 * Failed string-like object validation.
 * Failed value constraint validation.
 * Unrecognized custom data.

A decoder **MUST** allow the user or schema to decide what to do when a data error occurs, with a default of halting processing and issuing a diagnostic.



Security and Limits
-------------------

Accepting data from an outside source is always a security risk. The safest approach is to always assume hostile intentions when ingesting data.

Although Concise Encoding supports a wide range of data types and values, any given implementation will have limitations of some sort on their abilities due to the platform, language, system, and performance profiles. This will inevitably lead to subtle differences in CE implementations and applications that an attacker might be able to take advantage of if you're not careful.


### Attack Vectors

There are many vectors that attackers could take advantage of when they control the data your system is receiving, the most common of which are induced data loss, field omission, key collisions, and exploitation of algorithmic complexity.

#### Induced Data Loss

Once the characteristics of a system are known, an attacker could anticipate under what circumstances it will suffer data loss based on the language and technology used, the host it's running on, and the code itself.

The most common sources of deserialization data loss are:

 * Cutting off the upper part of an integer on overflow
 * Rounding off the lower part of a binary float on overflow
 * Rounding to infinity or converting to NaN
 * Truncating long arrays/strings
 * Erasing/replacing invalid characters in a string-like object

Any point in your system that allows data loss is a potential security hole, because different parts of your system will likely handle the same loss-inducing data in different ways, and those differences could be exploited by an attacker using specially crafted documents.

As a contrived example, consider a fictional system where the access control subsystem running on platform A leaves bad characters as-is or replaces them with u+fffd, and the storage subsystem running on platform B truncates bad characters. If an attacker is able to send a "create user" or "change user" command with a group of `admin\U+D800` (which would pass access control validation because `admin\U+D800` != `admin`), he could set up an admin user because the storage subsystem truncates bad characters and stores the group `admin\U+D800` as `admin`. The next time that user is loaded, it will be in group `admin`.

Numbers could also suffer data loss depending on how the decoded values are stored internally. For example, attempting to load the value 0x123456789 into a 32-bit unsigned integer would in many languages silently overflow to a result of 0x23456789. Similarly, the value 0x87654321 (2271560481) stored in a 32-bit ieee754 binary float field would be silently truncated to 2271560448, losing precision and changing the effective value because it only has 24 bits available for the siginificand.

#### Default Type Conversions

Default type conversions are a form of data loss, and are specially cited here because they tend to happen silently as a result of how the language they're implemented in works. For example, PHP silently converts string values to 0 when comparing to a number. C and C++ treat all nonzero values as "true", including pointers. Other languages convert the numeric value to a string using the default format and then compare. These differing behaviors could potentially be exploited by an attacker.

Default truncation is also a problem. For example, some systems will truncate out-of-range values to 0, meaning that if an attacker could somehow cause an out of range price (999999999999999999999999999999999999 or whatever), such a system would convert it to 0, giving it away for free!

#### Induced Omission

Omission occurs when a decoder decides to drop bad data rather than store a default value or attempt to "fix" it. When this occurs, the count of objects in containers will change, and critical information might now be missing. This could become a security hole if the fields are preserved by the decoders in some parts of your system but omitted in others.

#### Key Collisions

Key collisions occur when two or more identical keys are present in the same encoded map. The most common ways systems tend to handle key collisions are:

 * Take the initial key and reject all duplicate keys (choose the first instance)
 * Replace the value for each duplicate encountered (choose the last instance)
 * Reject the key and all associated values (choose nothing)
 * Reject the entire document (abort processing)

The first three approaches are dangerous and lead to security issues. Only rejecting the document is safe (which is what Concise Encoding mandates).

For example, given the map:

```
{
    purchase-ids = [1004 102062 94112]
    total = 91.44
    total = 0
}
```

As a seller, you'd want your billing system to choose the first instance of "total". As a buyer, you'd much prefer the second!

**Note**: Key collisions could also occur as a result of [data loss](#data-loss) or even [default type conversions](#default-type-conversions):

```
{
    purchase-ids = [1004 102062 94112]
    total = 91.44
    total\U+D800 = 0
}
```

In this case, if the system truncated bad Unicode characters after checking for duplicate keys, it would be vulnerable to exploitation.

#### Deserialization Complexity

Depending on the implementation, some operations could get expensive very quickly the larger the object is, exhibiting O(n²) or sometimes even O(n³) behavior. This is particularly true of "big int" type structures in many languages. Even attempting to deserialize values as small as 10^1000 into a BigInt could DOS some systems.

#### Other Vulnerabilities

Attack documents could threaten a system in other ways, too. For example:

 * Extremely large objects (for example a 1TB array).
 * Documents with too many objects.
 * Documents with too much container depth (attempting to overflow the decoder's stack).


### Mitigations: Concise Encoding Codecs

To mitigate these kinds of security issues, Concise Encoding codecs have the following additional requirements:

#### Validation

All decoded values **MUST** be validated for the following before being passed to the application:

| Validation Type                            | Error Type                       |
| ------------------------------------------ | -------------------------------- |
| [Global limits](#user-controllable-limits) | [Structural](#structural-errors) |
| Content rules (based on type)              | [Data](#data-errors)             |
| Schema rules (if any)                      | Depends on the rule              |

#### User-Controllable Limits

The codec **MUST** allow the user to control various limits and ranges, with sane defaults to guard against DOS attacks:

 * Maximum object count overall
 * Maximum object count in a container
 * Maximum array length in bytes
 * Maximum total document size in bytes
 * Maximum depth from the top level (1) to the most deeply nested object (inclusive)
 * Maximum number of digits allowed in an integer value
 * Maximum number of significant digits allowed in a floating point number
 * Maximum exponent size (in digits) in a floating point number

It's impossible to prescribe what limits are reasonable for all decoders, because different systems will have different constraints, and system capabilities increase as technologies improve. As an illustration, for a general purpose decoder the following defaults would probably give a reasonable balance in 2020:

| Metric                 | Limit     |
| ---------------------- | --------- |
| Document size in bytes | 5 GB      |
| Overall object count   | 1 million |
| Depth                  | 1000      |
| Coefficient size       | 100       |


### Mitigations: Application Guidelines

For application developers, security is a frame of mind. You **SHOULD** always be considering the risks of your architecture, and the size of your attack surface. Here are some general guidelines:

 * Harmonize limits across your entire application surface (all subsystems). The most insidious exploits take advantage of differences in processing.
 * Use a common schema to ensure that your validation rules are consistent across your infrastructure.
 * Treat received values as all-or-nothing. If you're unable to store it in its entirety without data loss, it **SHOULD** be rejected. Allowing data loss means opening your system to key collisions and other exploits.
 * Guard against unintentional default conversions (for example string values converting to 0 or true in comparisons).
 * Never store na or attempt to convert na to a default value. Generate a diagnostic and either store a consistent, deterministic replacement, or more preferably halt processing.
 * When in doubt, toss it out. The safest course of action with foreign data is all-or-nothing. Not rejecting the entire document means that you'll have to compromise, either truncating or omitting data, which opens your system to exploitation.



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
