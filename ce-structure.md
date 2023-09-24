Concise Encoding - Structural Specification
===========================================


Version
-------

Version 0 (prerelease)



This Document
-------------

This document describes the logical structure of the Concise Encoding formats and the data types they support.

 * The Concise Binary Encoding (CBE) format is described in [cbe-specification.md](cbe-specification.md).
 * The Concise Text Encoding (CTE) format is described in [cte-specification.md](cte-specification.md).



Contents
--------

- [Concise Encoding - Structural Specification](#concise-encoding---structural-specification)
  - [Version](#version)
  - [This Document](#this-document)
  - [Contents](#contents)
  - [Terms and Conventions](#terms-and-conventions)
  - [Introduction](#introduction)
  - [Design Goals](#design-goals)
    - [Why Two Formats?](#why-two-formats)
  - [Schema](#schema)
  - [Formal Representation](#formal-representation)
  - [Document Structure](#document-structure)
  - [Version Header](#version-header)
    - [Prerelease Version](#prerelease-version)
  - [Object Categories](#object-categories)
    - [Concrete Objects](#concrete-objects)
    - [Intangible Objects](#intangible-objects)
  - [Data Objects](#data-objects)
  - [Numeric Types](#numeric-types)
    - [Boolean](#boolean)
    - [Integer](#integer)
    - [Floating Point](#floating-point)
      - [Supported Bases](#supported-bases)
      - [Decimal Floating Point](#decimal-floating-point)
      - [Binary Floating Point](#binary-floating-point)
      - [Value Ranges](#value-ranges)
      - [Special Floating Point Values](#special-floating-point-values)
      - [NaN Payload](#nan-payload)
    - [UID](#uid)
  - [Temporal Types](#temporal-types)
    - [General Rules](#general-rules)
    - [Date](#date)
    - [Time](#time)
    - [Timestamp](#timestamp)
    - [Time Zones](#time-zones)
      - [Area/Location](#arealocation)
        - [Abbreviated Areas](#abbreviated-areas)
        - [Special Areas](#special-areas)
      - [Global Coordinates](#global-coordinates)
      - [UTC](#utc)
      - [UTC Offset](#utc-offset)
  - [String Types](#string-types)
    - [NUL Character](#nul-character)
    - [String](#string)
    - [Resource Identifier](#resource-identifier)
  - [Array Types](#array-types)
  - [Container Types](#container-types)
    - [Container Properties](#container-properties)
      - [Ordering](#ordering)
      - [Duplicates](#duplicates)
    - [List](#list)
    - [Map](#map)
      - [Keyable types](#keyable-types)
    - [Records](#records)
      - [Record](#record)
      - [Record Validation](#record-validation)
    - [Node](#node)
    - [Edge](#edge)
      - [Example](#example)
  - [Other Types](#other-types)
    - [Media](#media)
    - [Null](#null)
  - [Custom Types](#custom-types)
    - [Custom Type Code](#custom-type-code)
    - [Custom Type Forms](#custom-type-forms)
  - [Pseudo-Objects](#pseudo-objects)
    - [Reference](#reference)
      - [Local Reference](#local-reference)
        - [Recursive References](#recursive-references)
      - [Remote Reference](#remote-reference)
  - [Invisible Objects](#invisible-objects)
    - [Comment](#comment)
    - [Padding](#padding)
  - [Structural Objects](#structural-objects)
    - [Record Type](#record-type)
    - [Marker](#marker)
    - [Identifier](#identifier)
      - [Identifier Rules](#identifier-rules)
  - [Empty Document](#empty-document)
  - [Unrepresentable Values](#unrepresentable-values)
    - [Lossy Conversions](#lossy-conversions)
      - [Binary and Decimal Float Conversions](#binary-and-decimal-float-conversions)
    - [Problematic Values](#problematic-values)
  - [Equivalence](#equivalence)
    - [Boolean Equivalence](#boolean-equivalence)
    - [Integer and Float Equivalence](#integer-and-float-equivalence)
      - [Special Float Value Equivalence Follows ieee754](#special-float-value-equivalence-follows-ieee754)
    - [Custom Type Equivalence](#custom-type-equivalence)
    - [String Type Equivalence](#string-type-equivalence)
    - [Array Equivalence](#array-equivalence)
    - [Container Equivalence](#container-equivalence)
    - [Null Equivalence](#null-equivalence)
    - [Comment Equivalence](#comment-equivalence)
    - [Padding Equivalence](#padding-equivalence)
  - [Error Processing](#error-processing)
  - [Security and Limits](#security-and-limits)
    - [Attack Vectors](#attack-vectors)
      - [Induced Data Loss](#induced-data-loss)
      - [Default Type Conversions](#default-type-conversions)
      - [Induced Omission](#induced-omission)
      - [Key Collisions](#key-collisions)
      - [Deserialization Complexity](#deserialization-complexity)
      - [Payload Size](#payload-size)
    - [Mitigations: Concise Encoding Codecs](#mitigations-concise-encoding-codecs)
      - [Validation](#validation)
      - [User-Controllable Limits](#user-controllable-limits)
    - [Mitigations: Application Guidelines](#mitigations-application-guidelines)
  - [Appendix A: List of Codec Options](#appendix-a-list-of-codec-options)
    - [Mandatory Options](#mandatory-options)
    - [Mandatory User-controllable limits](#mandatory-user-controllable-limits)
    - [Recommended Options](#recommended-options)
    - [Schema Options](#schema-options)
  - [Appendix B: Recording Time](#appendix-b-recording-time)
    - [Absolute Time](#absolute-time)
    - [Fixed Time](#fixed-time)
    - [Floating Time](#floating-time)
    - [Where each kind fits best](#where-each-kind-fits-best)
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
 * Some explanations will include [Dogma notation](https://dogma-lang.org/) notation for illustrative purposes only (they will be inexact because this document doesn't describe actual encodings).
 * Sample Concise Encoding data will usually be given in [CTE format](cte-specification.md) for clarity and human readability.


Introduction
------------

Concise Encoding is a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data. It consists of two parallel and seamlessly convertible data formats: a **binary format** [(Concise Binary Encoding - or CBE)](cbe-specification.md) for size and transmission efficiency, and a **text format** [(Concise Text Encoding - or CTE)](cte-specification.md) for human readability.



Design Goals
------------

**Primary Goals:**

 * It must be machine and transmission efficient (low transmission cost, low energy cost, low complexity)
 * It must be easy for a human to parse and produce.
 * It must be secure (secure defaults, no implicit behavior, one way to do each thing, reject bad data by default).
 * It must be future proof (allow changes and additions to the spec with no negative impact to existing implementations).
 * It must be simple to use (no special compilation or code generation steps, no descriptor files, etc).
 * It must support the most commonly used types and the funtamental types natively.
 * It must support ad-hoc data structures and progressive document building.
 * It must support unlimited numeric value ranges.
 * Schemas must be optional.

**Secondary Goals:**

 * It must support cyclic data.
 * It must be zero-copy friendly.
 * It must use little endian byte ordering wherever possible.
 * It must be identifiable within the first few bytes of the document.
 * It must support inter-document linking.
 * It must support partial recovery of damaged/incomplete documents.



### Why Two Formats?

The first two primary goals (ease for machines, ease for humans) cannot be satisfied by a single format. A binary format is easy for a machine and hard for a human. A text format is easy for a human and hard for a machine. Twin formats make it easy and efficient for machines to read and write data (as binary), and _also_ provide a way for humans to read and write that same data (as text).

Data should ideally be stored and transmitted in a binary format, and only converted to/from text in the uncommon cases where a human needs to be involved (modifying data, configuring, debugging, etc). In fact, most applications won't even need to concern themselves with the text format at all; a simple standalone command-line tool to convert between CBE and CTE (for example [enctool](https://github.com/kstenerud/enctool)) is usually enough for a human to examine and modify the CBE data that your application uses.

![Example binary and text data flow](img/dataflow.svg)



Schema
------

Schema research and development is still very much in its infancy. There are currently many schema languages in various stages of maturity, but they tend to focus on specific technologies, formats and use cases, which limits their utility in a general purpose data format:

- [XML DTD](https://www.w3.org/XML/1998/06/xmlspec-report-v20.htm)
- [XML Schema](https://www.w3.org/XML/Schema)
- [RelaxNG](https://relaxng.org/)
- [JSON Schema](https://json-schema.org/)
- [TypeSchema](https://typeschema.org/)
- [JTD](https://datatracker.ietf.org/doc/html/rfc8927)
- [AsyncAPI](https://www.asyncapi.com/)
- [OpenAPI](https://www.openapis.org/)

Although many of these schema technologies could be used to validate CE documents, their limitations would constrain the data and structural design.

The current frontrunner and most agnostic language so far is [CUE](https://cuelang.org/), and although even CUE doesn't fully support all CE types and structural possibilities, it shows the most promise and extensibility.

Therefore, [CUE](https://cuelang.org/) is the preferred schema language for validating Concise Encoding documents.



Formal Representation
---------------------

Formal specification clauses are written in the [Dogma](https://dogma-lang.org/) metalanguage. Many of the text descriptions in this specification will be augmented with Dogma rules for clarity.

Whenever any textual description seems ambiuguous or unclear, please refer to the formal Dogma documents (and also, please [open an issue](https://github.com/kstenerud/concise-encoding/issues)):

* [CBE Dogma](cbe.dogma)
* [CTE Dogma](cte.dogma)



Document Structure
------------------

Data in a Concise Encoding document is arranged in an ad-hoc hierarchical fashion, and can be progressively read or written.

Documents begin with a [version header](#version-header), followed by possible [intangible](#intangible-objects) objects (which at this point in the document can also include [record types](#record-type)), and then ultimately followed by one (and only one) top-level [data object](#data-objects).

Once the top-level data object is fully decoded, the document is considered finished.

```dogma
document = version_header
         & intangible_object*
         & data_object
         ;
```

**Specific Use Cases**:

 * To store multiple [data objects](#data-objects) in a document, use a [container](#container-types) as the top-level object.
 * To represent an [empty document](#empty-document), use [null](#null) as the top-level object.

**Examples**:

 * A document (CE version 1) containing a list with the integers 1, 2, 3:
   - In [CTE](cte-specification.md): `c1 [1 2 3]`
   - In [CBE](cbe-specification.md): [`81 01 9a 01 02 03 9b`]
 * An empty document (CE version 1):
   - In [CTE](cte-specification.md): `c1 null`
   - In [CBE](cbe-specification.md): [`81 01 7d`]



Version Header
--------------

Concise Encoding is versioned, meaning that every Concise Encoding document contains the version of the Concise Encoding specification it adheres to. This ensures that any future incompatible changes to the format will not break existing implementations.

A version header consists of a signature byte - 0x63 (`c`) or 0x43 (`C`) for CTE, 0x81 for CBE - followed by the version number, which is an unsigned integer representing the [version of this specification](#version) that the document adheres to.

```dogma
version-header = signature_byte & unsigned_integer;
```

**Example**:

 * [CTE](cte-specification.md) version 1: the character sequence `c1`
 * [CBE](cbe-specification.md) version 1: the byte sequence [`81 01`]


### Prerelease Version

During the pre-release phase, all documents **SHOULD** use version `0` so as not to cause potential compatibility problems once V1 is released. After release, version 0 will be permanently retired and considered invalid (there shall be no backwards compatibility to the prerelease spec).



Object Categories
-----------------

Concise Encoding documents support many kinds of objects, the combinations of which offer rich expressivity. The object types are roughly split into the following categories based on their purpose:


### Concrete Objects

Concrete objects either are data, or point to data.

 * [Data objects](#data-objects) contain actual data.
 * [Pseudo-objects](#pseudo-objects) serve as stand-ins for [data objects](#data-objects).

Wherever a concrete object can be placed, any number of intangible objects can be placed before it.

```dogma
concrete-object = intangible_object* & (data_object | pseudo_object);
```

### Intangible Objects

Intangible objects are either meta-information or structural helpers, but do not themselves count as actual data when structurally interpreting the document (i.e. they cannot fill containers).

 * [Invisible objects](#invisible-objects) provide helper functionality such as comments and byte alignment, but don't affect the document structure or data.
 * [Structural objects](#structural-objects) provide support for linkages beween parts of the document and ways to reduce redundancy, but are not themselves complete data objects.

```dogma
intangible-object = invisible_object | structural_object;
```


Data Objects
------------

Data objects hold the actual data of a document, and consist mainly of containers and primitives:

 * [Numeric Types](#numeric-types)
 * [Temporal Types](#temporal-types)
 * [String Types](#string-types)
 * [Array Types](#array-types)
 * [Container Types](#container-types)
 * [Other Types](#other-types)

```dogma
data-object = numeric_type
            | temporal_type
            | string_type
            | array_type
            | container_type
            | other_type
            ;
```


Numeric Types
-------------

Numeric types represent the basic scalar numeric types present in most computer systems.

An implementation **MAY** alter the type and/or storage size of integer and floating point values when encoding/decoding as long as the resulting value can be converted back to the original value without data loss (although [NaN payloads](#nan-payload) are treated more leniently).

**Note**: The Concise Encoding format itself places no bounds on the range of most numeric types, but implementations (being bound by language, platform, and physical limitations) **MUST** [decide which ranges to accept](#user-controllable-limits). It's important that all chosen limits are kept consistent across all participating systems in order to mitigate potential [security holes](#security-and-limits).


### Boolean

Supports the values true and false.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    true
    false
]
```


### Integer

"Integer" in Concise Encoding refers to the integer type as it is used in programming languages (in other words, 0 is included in its definition).

Integer values **CAN** be positive or negative, and **CAN** be represented in various bases (in [CTE](cte-specification.md#integer)) and sizes.

**Note**: Although negative zero (`-0`) can technically be encoded using certain integer encodings in [CBE](cbe-specification.md#fixed-width-integer) (and it is valid to do so), this value is actually a [floating point](#floating-point) type.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    42
    0xff
    -1000000000000000000000000000000000000000000000000000
]
```


### Floating Point

A floating point number is conceptually composed of a whole part and a fractional part (forming a significand), multiplied by a base raised to an exponent:

    value = whole.fractional × baseᵉˣᵖᵒⁿᵉⁿᵗ

Internally, most systems combine the whole and fractional parts into a single integer significand value, with the exponent adjusted to compensate:

    value = significand × baseᵉˣᵖᵒⁿᵉⁿᵗ

#### Supported Bases

Concise Encoding supports the two most common bases in use: 10 (decimal) and 2 (binary).

#### Decimal Floating Point

In a decimal floating point number, the exponent represents 10 to the power of the exponent value (for example 3.814 x 10⁵⁰).

Decimal floating point **SHOULD** be the preferred method for storing floating point values because of the rounding issues associated with binary floating point when converting to/from human-friendly forms.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    -2.81
    4.195342e-10000
]
```

#### Binary Floating Point

In a binary floating point number, the exponent represents 2 to the power of the exponent value (for example 7.403 x 2¹⁵).

Binary floating point is provided mainly to support legacy and specialized systems (to ensure that no further rounding occurs when transmitting the values). Concise Encoding supports three types of binary floating point values:

 * [16-bit bfloat](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)
 * [32-bit ieee754 binary](https://en.wikipedia.org/wiki/Single-precision_floating-point_format)
 * [64-bit ieee754 binary](https://en.wikipedia.org/wiki/Double-precision_floating-point_format)

Following [ieee754-2008 recommendations](https://en.wikipedia.org/wiki/IEEE_754#Binary), the most significant bit of the significand field of a binary NaN (not-a-number) value is defined as the "quiet" bit. When set, the NaN is quiet. When cleared, the NaN is signaling.

    s 1111111 1xxxxxxxxxxxxxxxxxxxxxxx = float32 quiet NaN
    s 1111111 0xxxxxxxxxxxxxxxxxxxxxxx = float32 signaling NaN (if payload is not all zeroes)

**Notes**:

 * Concise Encoding does **not** require implementations to preserve NaN payloads other than the signaling/quiet status.
 * Be careful not to set the rest of the bits of an ieee754 binary float or bfloat payload to all zeroes on a signaling NaN, because such an encoding signifies [infinity, not NaN](https://en.wikipedia.org/wiki/Single-precision_floating-point_format#Exponent_encoding).

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    0xa.3fb8p+42 // a.3fb8 x 2⁴²
    0x1.0p0      // 1
]
```

#### Value Ranges

Floating point types support the following ranges:

| Type    | Significant Digits | Exponent Range |
| ------- | ------------------ | -------------- |
| Binary  | up to 15.95        | -1022 to 1023  |
| Decimal | ∞                  | ∞              |

**Notes**:

 * Binary floats are limited to what is representable by their respective types (bfloat16, ieee754 float32, ieee754 float64).
 * Although decimal floats technically have unlimited range, implementations will suffer performance issues after a point, and thus require pragmatic and homogenous [limit enforcement](#user-controllable-limits).

#### Special Floating Point Values

Both decimal and binary floating point numbers have representations for the following special values:

* Two kinds of zero: +0 and -0
* Two kinds of infinities: +∞ and -∞
* Two kinds of NaN (not-a-number): signaling and quiet

#### NaN Payload

An implementation **MUST** preserve the signaling/quiet status of a NaN, and **MAY** discard the rest of the NaN payload information (this actually becomes necessary when converting to [CTE](cte-specification.md)). Applications **SHOULD NOT** attach special meaning to NaN payloads (other than the quiet bit) because differences in implementations and possible implicit type conversions at the receiving end could potentially pose a [security risk](#security-and-limits).


### UID

A universal identifier. This identifier is designed to be unique across all identifiers in the universe.

Concise encoding uses [rfc4122 UUID](https://tools.ietf.org/html/rfc4122) (and future updates to rfc4122) as the universal identifier implementation.

**Example (in [CTE](cte-specification.md))**:

```cte
c1
123e4567-e89b-12d3-a456-426655440000
```



Temporal Types
--------------

Temporal types are some of the most difficult to implement and use correctly. A thorough understanding of how time works physically, politically, conventionally, and socially gets you halfway there. But even then, it's still a veritable minefield that can be your system's undoing if you haven't planned **very** carefully for [the purposes you need to keep time for](#appendix-b-recording-time), and the implications thereof.

Temporal types are represented using the [Gregorian calendar](https://en.wikipedia.org/wiki/Gregorian_calendar) (or the [proleptic Gregorian calendar](https://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar) for dates prior to 15 October 1582) and a [24h clock](https://en.wikipedia.org/wiki/24-hour_clock). Temporal types can have precision to the nanosecond, and also support [leap years](https://en.wikipedia.org/wiki/Leap_year) and [leap seconds](https://en.wikipedia.org/wiki/Leap_second).


### General Rules

 * The `year` field **CANNOT** be 0 (The [Anno Domini](https://en.wikipedia.org/wiki/Anno_Domini) system has no zero year, meaning there is no 0 BC or 0 AD).
 * The sign of the `year` field signifies the era (negative for BC, positive for AD).
 * The `year` field always represents the full year (abbreviations are not allowed).
 * The `month` and `day` fields start counting at 1 (they **CANNOT** be 0).
 * The `day` field **MUST** be valid for the specified month according to the [Gregorian calendar](https://en.wikipedia.org/wiki/Gregorian_calendar#Description).
 * The `day` field **CAN** go up to 29 in Feburary when accommodating a [leap year](https://en.wikipedia.org/wiki/Leap_year).
 * The `hour`, `minute`, `second`, and `subsecond` fields start counting at 0.
 * The `hour` field represents the [24h clock](https://en.wikipedia.org/wiki/24-hour_clock) hour value (there is no AM or PM).
 * The `second` field **CAN** go up to 60 when accommodating a [leap second](https://en.wikipedia.org/wiki/Leap_second).
 * If the time zone is omitted, it is assumed to be `Zero` (aka "Zulu" or "[GMT](https://en.wikipedia.org/wiki/Greenwich_Mean_Time)" or "[UTC](https://en.wikipedia.org/wiki/UTC%C2%B100:00)").


### Date

Represents a date without specifying a time of day or time zone.

A date is made up of the following fields:

| Field | Mandatory | Min Value | Max Value |
| ----- | --------- | --------- | --------- |
| Year  |     Y     |        -∞ |        +∞ |
| Month |     Y     |         1 |        12 |
| Day   |     Y     |         1 |        31 |

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    2019-08-05  // August 5, 2019
    15081-03-30 // March 30, 15081
    70-01-01    // January 1st, year 70 (proleptic Gregorian) - *not* 1970
    -300-12-21  // December 21, 300 BC (proleptic Gregorian)
]
```


### Time

Represents a time of day without specifying a particular date.

A time is made up of the following fields:

| Field      | Mandatory | Min Value | Max Value |
| ---------- | --------- | --------- | --------- |
| Hour       |     Y     |         0 |        23 |
| Minute     |     Y     |         0 |        59 |
| Second     |     Y     |         0 |        60 |
| Subseconds |     N     |         0 | 999999999 |
| Time Zone  |     N     |           |           |

**Notes**:

 * Since a time by itself has no date component, its time zone data **MUST** be interpreted as if it were "today". This means that location-based time zones like `America/Seattle` or `48.86/2.36` or `Local` (as opposed to UTC-offsets like `Zero` or `Etc/GMT+1`) might result in a different absolute time when read on different dates due to political time shifts (such as daylight savings).

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    23:59:59.999999999      // 23:59:59 and 999999999 nanoseconds UTC
    12:05:50.102            // 12:05:50 and 102 milliseconds UTC
    4:00:00/Asia/Tokyo      // 4:00:00 Tokyo time
    17:41:03/-13.54/-172.36 // 17:41:03 Samoa time
    9:00:00/Local           // 9:00:00 local time
]
```


### Timestamp

A timestamp combines a date and a time.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    2019-01-23/14:08:51.941245                  // January 23, 2019, at 14:08:51 and 941245 microseconds, UTC
    1985-10-26/01:20:01.105/America/Los_Angeles // October 26, 1985, at 1:20:01 and 105 milliseconds, Los Angeles time
    5192-11-01/03:00:00/48.86/2.36              // November 1st, 5192, at 3:00:00, at whatever will be in the place of Paris at that time
]
```


### Time Zones

A time zone refers to the political designation of a location as having a specific time offset from UTC during a particular time period. Time zones are in a continual state of flux, and could change at any time for all sorts of reasons.

Time zone data can be denoted in the following ways:

 * [Area/Location](#arealocation)
 * [Global Coordinates](#global-coordinates)
 * [UTC](#utc)
 * [UTC Offset](#utc-offset)

#### Area/Location

Area/location is the more human-readable method, but might not be precise enough for certain applications. Time zones are partitioned into areas containing locations, and are written in the form `Area/Location`. These areas and locations are specified in the [IANA time zone database](https://www.iana.org/time-zones). Area/Location timezones have a minumum length of 1 character and a maximum length of 127 **bytes** (not characters). They are also case-sensitive because they tend to be implemented that way on most platforms.

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

A decoder **MUST NOT** blindly pass abbreviated area names to the application because the application's time implementation may not understand them. Convert the time data to a host-supported format first.

##### Special Areas

The following special pseudo-areas **CAN** also be used. They do not contain a location component.

| Area    | Abbreviation | Meaning            |
| ------- | ------------ | ------------------ |
| `Zero`  | `Z`          | Alias to `Etc/UTC` |
| `Local` | `L`          | "Local" time zone, meaning that the accompanying time value is to be interpreted as if in the time zone of the observer. |

A decoder **MUST NOT** blindly pass these special areas to the application because the application's time implementation may not understand them. Convert the time data to a host-supported format first.

**Examples**:

 * `Europe/Paris`
 * `E/Paris`
 * `America/Vancouver`
 * `Etc/UTC` == `Zero` == `Z`
 * `L`

#### Global Coordinates

The global coordinates method represents the location's global latitudinal and longitudinal position to a precision of hundredths of degrees, giving a resolution of about 1km at Earth's equator.

This method has the advantage of being temporally unambiguous, which could be useful for areas that are in an inconsistent political state at a particular time such that area/location cannot be reliably determined. The disadvantage is that it's not as easily recognizable to humans.

**Examples (in [CTE](cte-specification.md))**:

 * `50.45/30.52` (Kyiv, near Independence Square)
 * `-13.53/-172.37` (Samoa)

#### UTC

If the time zone is unspecified, it is assumed to be `Zero` (UTC). Placing all past event time values in the UTC time zone has the advantage of more compact and unambiguous time storage, which makes comparisons and other operations much easier and reduces bugs.

UTC time **SHOULD NOT** be used for future or periodic/repeating time values (see [appendix B: recording time](#appendix-b-recording-time)] for an explanation).

#### UTC Offset

Time offset is recorded as an offset (+ or -) from UTC, recorded in hours and minutes (to a maximum of `+2359` and a minimum of `-2359`). This format is not an actual time zone because it doesn't contain location data and therefore cannot account for political shifts (such as daylight savings). In fact, it can't convey any useful information beyond what UTC time can (other than a vague notion of where the value might have originated, and roughly what time of day it was).

Use of UTC offset is discouraged except as a means of interfacing with legacy systems.

UTC offsets **SHOULD NOT** be used for future or periodic/repeating time values (see [appendix B: recording time](#appendix-b-recording-time) for an explanation).

**Examples (in [CTE](cte-specification.md))**:

 * `+0530`
 * `-0100`



String Types
------------

String types contain [UTF-8 encoded](https://en.wikipedia.org/wiki/UTF-8) string data. All [Unicode](https://en.wikipedia.org/wiki/Unicode) codepoints except for surrogates and those marked permanently as noncharacters are allowed:

```dogma
string_type = char_string*;
char_string = unicode(C,L,M,N,P,S,Z);
```

String types **MUST** always resolve to complete, valid UTF-8 sequences when fully decoded. Furthermore, all codepoints **MUST** be encoded in the shortest possible UTF-8 encoding (with the sole exception of allowing [`c0 80`] for the NUL codepoint in some cases - [see below](#nul-character)).

The following types use string-style encoding and follow string encoding rules:

 * [String](#string)
 * [Resource Identifier](#resource-identifier)
 * [Remote Reference](#remote-reference)
 * [Custom Type (text form)](#custom-type-forms)


### NUL Character

The NUL character (U+0000) is allowed in string types, but because it is problematic on so many platforms, the folllowing special rules apply:

 * On platforms that **do not** support NUL in strings (for example in C strings), decoders **MUST** stuff all NULs encountered in string data (convert every instance of [`00`] to [`c0 80`]).
 * On all platforms, encoders **MUST NOT** output stuffed NULs in string data (they **MUST** convert every instance of [`c0 80`] to [`00`] in string data before output).


### String

A standard UTF-8 string.

**Example (in [CTE](cte-specification.md))**:

```cte
c1 "I'm just a boring string."
```

### Resource Identifier

A resource identifier is a text-based universally unique identifier that can be resolved by a machine to point to a resource. Resource identifier validation is done according to [the URL standard](https://url.spec.whatwg.org).

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    @"https://x.com/"
    @"mailto:nobody@nowhere.com"
]
```



Array Types
-----------

An array represents a contiguous sequence of identically typed fixed length elements (like a space-optimized [list](#list) where every element is the same type). The type of the array determines the size of its elements and how its contents are interpreted.

In a [CBE document](cbe-specification.md), the array elements will all be adjacent to each other, allowing large amounts of data to be efficiently copied between the stream and your internal structures.

```dogma
array = array_element*;
```

The following element types are supported in primitive arrays. For other types or mixed types, use a [list](#list).

| Type                 | Element Sizes (bits) |
| -------------------- | -------------------- |
| Bit                  | 1                    |
| Unsigned Integer     | 8, 16, 32, 64        |
| Signed Integer       | 8, 16, 32, 64        |
| BFloat               | 16                   |
| IEEE754 Binary Float | 32, 64               |
| UID                  | 128                  |

**Note**: The following types also use array-style encoding (with byte elements), but are not considered arrays:

 * [Media](#media) encapsulates other data formats with well-known media types (which can thus be automatically passed by the application to an appropriate codec). Elements of a media "array" are always bytes, regardless of the actual data the bytes represent.
 * [Custom types (binary form)](#custom-type-forms) represent custom data structures that only a custom codec designed for them will understand. Elements of a custom type "array" are always bytes, regardless of the actual data the bytes represent.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
[
    @u8x[9f 47 cb 9a 3c]
    @f32[1.5 0x4.f391p100 30 9.31e-30]
    @i16[0b1001010 0o744 1000 0x7fff]
    @uid[3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f]
    @b[1 1 0 1 0]
]
```



Container Types
---------------

Container types hold collections of other objects.


### Container Properties

#### Ordering

If a container is ordered, the order in which objects are placed in the container matters. Ordered containers that contain [equivalent](#equivalence) objects but in a different order are **not** [equivalent](#equivalence).

#### Duplicates

For list-like containers, a duplicate means any object that is [equivalent](#equivalence) to another object already present in the list. Note that due to proccessing cost, containers within a list are never considered equivalent for the purpose of duplicate checking.

For map-like containers, a duplicate means any key-value pair whose key is [equivalent](#equivalence) to another key already present in the map, regardless of what that key's associated value is.

An implementation **MUST** disregard the type and size of integers and floats when comparing numeric types. If they can be converted to one another without data loss, they are potential duplicates. For example, the 16-bit integer value `2000`, the 64-bit integer value `2000`, and the 32-bit float value `2000.0` are all considered duplicates. The string value `"2000"`, however, is _not_ a duplicate because it's not a number (it's a string).

Ordering and duplication policies in [lists](#list) and [maps](#map) **CAN** be set by a schema, per-instance and globally.


### List

A sequential list of objects. List elements **CAN** be any [concrete object](#concrete-objects) (including other containers), and do not have to be all the same type.

```dogma
list = object*;
```

By default, a list is [ordered, and allows duplicate values](#container-properties).

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

A map associates key objects with value objects. Every key **MUST** be a [keyable type](#keyable-types), and keys do not have to all be the same type. Values **CAN** be any [concrete object](#concrete-objects) (including other containers), and do not have to be all the same type.

Map entries are stored as key-value pairs. Every key in the map **MUST** have a matching value.

```dogma
map = (keyable-type & concrete-object)*;
```

By default, a map is [unordered, and does not allow duplicate keys](#container-properties).

#### Keyable types

Only the following data types are allowed as keys in map-like containers:

* [Boolean](#boolean)
* [Integer](#integer) (**note**: not [`-0`](#special-floating-point-values), which is actually a float)
* [Universal ID](#uid)
* [Date, time, timestamp](#temporal-types)
* [String](#string)
* [Resource identifier](#resource-identifier)
* [Local reference](#local-reference) (only if the referenced value is keyable)

```dogma
keyable_type = boolean | integer | uid | date | time | timestamp | string | resource_id;
```

**Example (in [CTE](cte-specification.md))**:

```cte
c1
{
    1 = "alpha"
    2 = "beta"
    "a map" = {"one"=1 "two"=2}
    2000-01-01 = "New millenium"
}
```


### Records

Records split [map-like](#map) data into two parts: a [record type](#record-type) which defines what keys will be present, and a [record](#record) which references the record type and provides the matching values.

    Record type:  @<key1 key2 key3 ...>
    Record:       @{val1 val2 val3 ...}
    -------------------------------------------------
    Produces Map: {key1=val1 key2=val2 key3=val3 ...}

Records offer a more efficient way to encode payloads containing many instances of the same data structures by removing the need to write their map keys over and over. For tabular data this can reduce the payload size by 30-50% or more.

#### Record

A record builds a [map](#map) from a [record type](#record-type) by assigning the values from the record to the keys from the record type.

A record contains the [identifier](#identifier) of the [record type](#record-type) to build from, followed by a series of values that will be assigned in-order to the keys from the record type. [Null](#null) values in a record **MUST** be treated as "no data provided for this field"; it's up to the application to decide the appropriate action.

```dogma
record = identifier & concrete_object*;
```

 * Records are always [ordered, and **CAN** contain duplicates](#container-properties).
 * A record that references an undefined [record type](#record-type) identifier is an [error condition](#error-processing).
 * The record **MUST** define the same number of values as there are keys in the [record type](#record-type). A mismatch is an [error condition](#error-processing).

**Example (in [CTE](cte-specification.md))**:

```cte
c1
@vehicle<"make" "model" "drive" "sunroof">
@phone<"make" "model" "storage">
{
    "year end" = 2018
    "vehicles" = [
        @vehicle{"Ford"       "Explorer"   "4wd" true }
        @vehicle{"Toyota"     "Corolla"    "fwd" false}
        @vehicle{"Honda"      "Civic"      "fwd" false}
        @vehicle{"Alfa Romeo" "Giulia 952" "awd" true }
    ]
    "phones" = [
        @phone{"Apple"  "iPhone XS"   67108864}
        @phone{"Google" "Pixel 3 XL" 134217728}
    ]
}
```

The above document is equivalent to:

```cte
c1
{
    "year end" = 2018
    "vehicles" = [
        {
            "make" = "Ford"
            "model" = "Explorer"
            "drive" = "4wd"
            "sunroof" = true
        }
        {
            "make" = "Toyota"
            "model" = "Corolla"
            "drive" = "fwd"
            "sunroof" = false
        }
        {
            "make" = "Honda"
            "model" = "Civic"
            "drive" = "fwd"
            "sunroof" = false
        }
        {
            "make" = "Alfa Romeo"
            "model" = "Giulia 952"
            "drive" = "awd"
            "sunroof" = true
        }
    ]
    "phones" = [
        {
            "make" = "Apple"
            "model" = "iPhone XS"
            "storage" = 67108864
        }
        {
            "make" = "Google"
            "model" = "Pixel 3 XL"
            "storage" = 134217728
        }
    ]
}
```

#### Record Validation

A record and record type **MUST** be validated together as the [map](#map) they produce through their combination.


### Node

A node is the basic building block for unweighted directed graphs. It consists of:

 * A value (any data object or pseudo-object).
 * An [ordered](#container-properties) collection of zero or more children (directionality is always from the node to its children).

If a child is not of type node, it is treated as though it were the value portion of a node with no children.

```dogma
node = concrete_object & (node | concrete_object)*;
```

**Hint**: If the graph is cyclic, use [references](#reference) to nodes to represent the cycles.

Nodes are recorded in a **depth-first**, **node-right-left** order, which ensures that the [CTE](cte-specification.md) representation looks like the actual tree structure it describes when rotated 90 degrees clockwise.

**Example (in [CTE](cte-specification.md))**:

```cte
c1
// The tree:
//       2
//      / \
//     5   7
//    /   /|\
//   9   6 1 2
//  /   / \
// 4   8   5
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

When rotated 90 degrees clockwise, one can recognize the tree structure this represents:

![tree rotated](img/tree-rotated.svg)


### Edge

An edge describes a relationship between vertices in a graph. It is composed of three parts:

 * A **source**, which is the first vertex of the edge being described. This will most commonly be either a [reference](#reference) to an existing object, or a [resource ID](#resource-identifier). This **MUST NOT** be [null](#null) or a [reference](#local-reference) to null.
 * A **description**, which describes the relationship (edge) between the source and destination. This implementation-dependent object can contain information such as weight, directionality, or other application-specific data. If the edge has no properties, use [null](#null).
 * A **destination**, which is the second vertex of the edge being described. This **MUST NOT** be [null](#null) or a [reference](#local-reference) to null.

If any of these parts are missing, it is an [error condition](#error-processing).

```dogma
edge        = source & description & destination;
source      = not_null;
description = concrete_object;
destination = not_null;
not_null    = intangible_object*
            & ( numeric_type
              | temporal_type
              | string_type
              | array_type
              | container_type
              | pseudo_object
              )
            ;
```

Directionality is by default from the source to the destination unless the description or schema specifies otherwise.

Because graphs can take so many forms, there is no default interpretation for edges. Both sender and receiver **MUST** have a common understanding of what the graph edges represent, and how to interpret the data. For directed graphs with no edge descriptions, it's better to use [nodes](#node) instead.

#### Example

Edges can describe many kinds of graphs, even [RDF-style](https://en.wikipedia.org/wiki/Resource_Description_Framework) edges of a semantic graph (where the source, description and destination represent the subject, predicate and object of the semantic relationships):

```cte
c1
[
    @(
        @"https://springfield.gov/people#homer_simpson"
        @"https://example.org/wife"
        @"https://springfield.gov/people#marge_simpson"
    )
    @(
        @"https://springfield.gov/people#homer_simpson"
        @"https://example.org/employer"
        @"https://springfield.gov/employers/nuclear_power_plant"
    )
]
```

From the above data, we understand that Homer Simpson's wife is Marge Simpson, and that Homer Simpson's employer is the nuclear power plant.

More complex graph data can be succinctly represented by mixing in other types such as [references](#reference), [lists](#list), and [maps](#map).

```cte
c1
{
    @"https://springfield.gov/people#homer_simpson" = {
        @"https://mypredicates.org/wife" = @"https://springfield.gov/people#marge_simpson"
        @"https://mypredicates.org/regrets" = [
            $firing
            $forgotten_birthday
        ]
        @"https://mypredicates.org/troubles" = $troubles
    }

    "graph edges" = [
        &marge_birthday:@(
            @"https://springfield.gov/people#marge_simpson"
            @"https://mypredicates.org/birthday"
            1956-10-01
        )
        &forgotten_birthday:@(
            @"https://springfield.gov/people#homer_simpson"
            @"https://mypredicates.org/forgot"
            $marge_birthday
        )
        &firing:@(
            @"https://springfield.gov/people#montgomery_burns"
            @"https://mypredicates.org/fired"
            @"https://springfield.gov/people#homer_simpson"
        )

        // Multiple subjects
        &troubles:@(
            [$firing $forgotten_birthday]
            @"https://mypredicates.org/contribute"
            @"https://myobjects.org/marital_strife"
        )
    ]
}
```



Other Types
-----------

### Media

A media object contains a [media type](http://www.iana.org/assignments/media-types/media-types.xhtml) and a series of bytes representing the media's data.

```dogma
media = media_type & byte*;
```

The media object's internal encoding is not the concern of a Concise Encoding codec; CE merely sees the data as a sequence of bytes along with an associated media type.

The media type **MUST** be validated according to the rules of [rfc6838](https://www.rfc-editor.org/rfc/rfc6838.html#section-4.2). An invalid media type is an [error condition](#error-processing).

**Notes**:

 * An _unrecognized_ media type is **not** an [error condition](#error-processing); it is the application layer's job to decide such things.
 * [Multipart types](https://www.iana.org/assignments/media-types/media-types.xhtml#multipart) are **not** supported.

**Example (in [CTE](cte-specification.md))**:

```cte
c1
@application/x-sh[23 21 2f 62 69 6e 2f 73 68 0a 0a 65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a]
```

Which is the shell script:

```sh
#!/bin/sh

echo hello world
```


### Null

In programming languages, `null` most commonly denotes a [null reference](https://en.wikipedia.org/wiki/Null_pointer). This is **not** the purpose of `null` in Concise Encoding!

Like in [data query and manipulation languages](https://en.wikipedia.org/wiki/Null_(SQL)), Concise Encoding uses `null` to denote the absence of data ("no data at this field or index"). Its purpose is to support data operations that would otherwise be difficult to specify.

Uses for `null` in common operations:

| Operation | Data Source | Meaning when a field value = `null`                         |
| --------- | ----------- | ----------------------------------------------------------- |
| Create    | Client      | Do not create this field (overrides any default value).     |
| Read      | Server      | This field has been removed since the specified checkpoint. |
| Update    | Client      | Remove this field.                                          |
| Delete    | Client      | Match records where this field is absent.                   |
| Fetch     | Client      | Match records where this field is absent.                   |

Null is often used in [data records](#record) because every field in a record entry must have something specified (even if just to say "no data specified for this field"). For example:

```cte
c1
// Define the "Employee" record type:
@Employee<"name" "department" "parking stall">
[
    // Add some employee records:
    @Employee{ "John Marcos" "Marketing"   34}
    @Employee{ "Judy McGill" "Executive"    5}
    // Jane works from home, and uses guest parking when at the office
    @Employee{ "Jane Morgan" "Sales"     null}
]
```

Architecturally speaking, think twice, then think again, before deciding that null is necessary for your application. Like `goto`, it has a _very narrow_ use case but tends to be used far too liberally.



Custom Types
------------

There are some situations where a custom data type is preferable to the standard types. The data might not otherwise be representable, or it might be too bulky using standard types, or you might want the data to map directly to/from memory structs for performance reasons.

Adding custom types restricts interoperability to only those implementations that understand the types, and **SHOULD** only be used as a last resort. An implementation that encounters a custom type it doesn't know how to decode **MUST** treat it as an [error condition](#error-processing).

**Note**: Although custom types are encoded into bytes or string data, the interpretation of their contents is user-defined, and very likely won't represent an array or string value at all.

### Custom Type Code

All custom type values **MUST** have an associated unsigned integer "custom type" code. This code uniquely differentiates each type from all other types being used in the current document. The definition of which type codes refer to which data types **MUST** be agreed upon by both sending and receiving sides (for example via a schema).

A custom type code **MUST** be an unsigned integer in the range of 0 to 0xffffffff (inclusive).

### Custom Type Forms

Custom types can be represented in binary and textual form, where the binary form is encoded as a series of bytes, and the textual form is a structured textual representation.

```dogma
custom_binary = type_code & byte*;
custom_text   = type_code & string;
```

[CBE](cbe-specification.md) documents only support the binary form. [CTE](cte-specification.md) documents support both the binary and textual forms. CTE encoders **MUST** convert any custom binary form to its matching textual form whenever the text form is available.

Custom type implementations **MUST** provide at least a binary form, and **SHOULD** also provide a textual form. When both binary and textual forms of a custom type are provided, they **MUST** be 1:1 convertible to each other without data loss.

**Example**:

Suppose we wanted to encode a fictional "complex number" type:

    typedef complex
    {
        real:      float32
        imaginary: float32
    }

In our conceptual structure, we could represent complex numbers using something like `REAL + IMAGINARY`, where `REAL` and `IMAGINARY` are float32s.

For our textual encoding scheme, we could use the mathematical notation, such as `2.94+3i`.

For our binary encoding scheme, we could write the two float32 values directly:

    78 56 34 12 78 56 34 12 <-- in little endian byte order
    |---------| |---------|
       real      imaginary

**Note**: It's a good idea to store multibyte primitive binary types in little endian byte order since that's what all modern CPUs use natively.

In this example, we'll assign the custom type code 99 to our complex number type (assume that we've stated this in our schema). Therefore, a complex number such as 2.94 + 3i would be represented as follows:

Our data:

 * Type: `99`
 * Real: `2.94`, float32 bit pattern `0x403c28f6`, in little endian `f6 28 3c 40`
 * Imaginary: `3`, float32 bit pattern `0x40400000`, in little endian `00 00 40 40`

In [CTE](cte-specification.md):

 * Binary form:  `@99[f6 28 3c 40 00 00 40 40]`
 * Textual form: `@99"2.94+3i"`

In [CBE](cbe-specification.md):

 * Binary form: [`92 63 10 f6 28 3c 40 00 00 40 40`]
 * Textual form not supported in CBE



Pseudo-Objects
--------------

Pseudo-objects are not [data objects](#data-objects) themselves, but rather are stand-ins for [data objects](#data-objects).

Pseudo-objects **CAN** be placed anywhere a [data object](#data-objects) can be placed, except inside the contents of an [array](#array-types) or other types that use array-style encoding such as [media](#media) and [custom binary](#custom-types) (for example, `@u8x[11 22 $myref 44]` is an [error condition](#error-processing).


### Reference

A reference acts as a stand-in for another object in the current document or another document. It functions much like a pointer or reference would in a programming language, offering ways to:

 * Reduce repetitive information.
 * Encode cyclic data (recursive references).

#### Local Reference

A local reference contains the marker [identifier](#identifier) of an object that has been [marked](#marker) elsewhere in the current document.

```dogma
local_reference = identifier;
```

 * Recursive references (reference causing a cyclic graph) are supported only if the implementation has been [configured to accept them](#recursive-references).
 * Forward references (reference to an object marked later in the document) are supported.
 * A local reference **MUST** point to a valid [marked object](#marker) that exists in the current document. A reference with an invalid or undefined marker ID is an [error condition](#error-processing).
 * A local reference used as a map key **MUST** refer to a marked [keyable object](#keyable-types).

##### Recursive References

Because cyclic graphs are potential denial-of-service attack vectors to a system unprepared to handle such data, implementations **MUST** provide a configuration **OPTION** to enable recursive references, and this option **MUST** default to disabled.

When support is disabled, a recursive local reference is an [error condition](#error-processing).

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
{
    "some object" = {
        "some string" = &my_string_ref:"This is my string"
        "some map" = &my_map_ref:{
            "a" = 1
        }
    }

    "forward reference" = $later_obj_ref
    "reference to string" = $my_string_ref
    "reference to map" = $my_map_ref

    "a later object" = &later_obj_ref:{
        "x" = 2
        "recursive reference" = $later_obj_ref
    }
}
```

#### Remote Reference

A remote reference refers to an object in another document. It acts like a [resource ID](#resource-identifier) that describes how to find the referenced object in an outside document. Remote reference validation is done according to [the URL standard](https://url.spec.whatwg.org).

 * A remote reference **MUST** point to either:
   - Another Concise Encoding document (using no fragment section, thus referring to the top-level object in the document)
   - A [marked object](#marker) inside of another Concise Encoding document, using the fragment section to specify the [marker identifier](#marker) of the object in the document being referenced.
 * A remote reference **MUST NOT** be used as a map key because there's no way to know if it refers to a keyable type without actually following the reference (which would slow down evaluation and poses a security risk).
 * Because remote links pose security risks, implementations **MUST NOT** follow remote references unless explicitly configured to do so. If an implementation provides a configuration option to follow remote references, it **MUST** default to disabled.
 * If automatic remote reference following is enabled, a remote reference that doesn't resolve to a valid Concise Encoding document or valid [marker identifier](#marker) inside the document is an [error condition](#error-processing).

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
{
    "ref to doc on filesystem" = $"some_document.cbe"
    "ref to marked obj in local doc" = $"some_document.cbe#widgets" // Refers to the marked object "widgets" in document some_document.cbe
    "ref to remote doc" = $"https://somewhere.com/my_document.cbe"
    "ref to marked obj in remote doc" = $"https://somewhere.com/my_document.cbe#widgets"
}
```



Invisible Objects
-----------------

Invisible objects are "invisible" to the structure of the document (they have no semantic relevance, and could be removed without affecting the document's structure or data). They provide utility functionality for convenience when building a document.

Invisible objects may not be available in both text and binary formats.

Invisible objects **CANNOT** be used as real objects, and **CANNOT** be marked or referenced; they are completely invisible to the document structure.


### Comment

A comment is a string-like invisible object that provides extra information for human readers in CTE documents. CBE documents do not support comments (meaning that comments will be lost when converting from CTE to CBE).

CTE supports two forms of comments:

 * Single-line comments, which end at the line end (`// a comment`).
 * Multi-line comments, which can span multiple lines of text, and support nesting (`/* a comment /* nested */ */`).

Comments are allowed anywhere in a CTE document where a real object would be allowed, except inside [arrays](#array-types) or other types that use array-style encoding such as [media](#media) and [custom binary](#custom-types) (i.e. `@u8[1 2 /* a comment */ 3]` is invalid).

**Note**: CBE **CANNOT** encode comments, so they **MUST** be discarded when converting from CTE to CBE.

**Examples (in [CTE](cte-specification.md))**:

```cte
c1
// Comment before top-level object
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

    // Comment before the end of the top-level object (the map), but not after!
}
```


### Padding

Padding is an invisible object used for aligning data in a CBE document, and has no actual meaning.

The padding type **CAN** occur any number of times where a [CBE type field](cbe-specification.md#type-field) is valid.

**Note**: CTE **CANNOT** encode padding, so it **MUST** be discarded when converting from CBE to CTE.



Structural Objects
------------------

Structural objects exist purely in support of the document structure itself. They have no meaning outside of the document.

Structural objects do not represent data, and **CANNOT** be [marked](#marker).


### Record Type

A record type provides instructions for a decoder to build [records](#record) from, defining what keys will be present for any records that use it.

Record Types **CAN ONLY** occur at the top level of the document between the [version specifier](#version-header) and the [top-level object](#document-structure). A record type occuring anywhere else in the document is an [error condition](#error-processing).

A record type contains a unique (to the current document) type [identifier](#identifier), followed by a series of keys that will be present in any records created from it.

```dogma
record_type = identifier & keyable_type*;
```

**Notes**:

 * Record types **MUST** always be [ordered](#container-properties).
 * Record types default to not allowing duplicate keys.
 * Record type keys **MUST** be [keyable types](#keyable-types), and **CANNOT** be [references](#reference).


### Marker

A marker assigns a unique (to the current document) marker [identifier](#identifier) to another object, which can then be [referenced](#reference) from elsewhere the document (or from a different document using [remote references](#remote-reference)).

```dogma
marked-object = identifier & concrete_object;
```

A marker **CAN ONLY** be attached to a [data object](#data-objects) (e.g. `&my_marker1:&my_marker2:"abc"` and `&my_marker1:$my_marker2` are [error conditions](#error-processing)).

Other objects **CANNOT** be placed between a marker and the object it marks, regardless of type (e.g. `&my_id:/*comment*/123456` is an [error condition](#error-processing)).

**Example (in [CTE](cte-specification.md))**:

```cte
c1
[
    &remember_me:"Remember this string"
    &1:{"a" = 1}
]
```

The string `"Remember this string"` is marked with the ID `remember_me`, and the map `{"a"=1}` is marked with the ID `1`.


### Identifier

Identifiers provide the linkage mechanism between objects.

Identifiers are always an integral part of another type, and thus **CANNOT** exist on their own, and **CANNOT** be preceded by [pseudo-objects](#pseudo-objects) or [invisible objects](#invisible-objects) (e.g. `&/*comment*/mymarker:"Marked string"` is an [error condition](#error-processing)).

Implementations **MUST** provide a configuration **OPTION** to set the maximum length allowed for an identifier, and it **MUST** default to 1000 bytes.

#### Identifier Rules

 * It **MUST** be a valid UTF-8 string containing only characters of Unicode categories Cf, L, M, N, or characters '_', '.', or '-'.
 * It **MUST** begin with either a letter, number, or an underscore '_' (and therefore **CANNOT** be empty).
 * Comparisons are **case sensitive**.
 * Identifier definitions **MUST** be unique to an identifier type in the current document. So for example the [marker](#marker) ID "a" will not clash with the [record type](#record-type) ID "a", but a document **CANNOT** contain two [markers](#marker) with ID "a" or two [record types](#record-type) with ID "a".

```dogma
identifier             = char_identifier_first & char_identifier_next*;
char_identifier_first  = unicode(L,N) | '_';
char_identifier_next   = unicode(Cf,L,M,N) | '_' | '.' | '-';
```



Empty Document
--------------

An empty document is signified by using [null](#null) as the [top-level object](#document-structure):

* In CBE: [`81 01 7d`]
* In CTE: `c1 null`



Unrepresentable Values
----------------------

Although Concise Encoding strives to support the most common and fundamental information types, there's no guarantee that all values of all types will be representable on a particular platform.

Decoders are given a lot of leeway in how they represent a document's data after decoding in order to minimize these sorts of problems, but there will sometimes be situations where there is no type available that can represent the value. If a value in a Concise Encoding document cannot be represented in the designated type on the destination platform or in any acceptable substitute type without inducing data loss (other than type information), it **MUST** be processed according to the [lossy conversion rules](#lossy-conversions).


### Lossy Conversions

If, after decoding and storing a value, it is no longer possible to encode it back into the exact same original bit pattern due to data loss, the conversion is considered to be "lossy".

**Lossy conversions that MUST always be allowed**:

 * Loss of [NaN payload data](#nan-payload), except for the quiet/signaling status which **MUST** be preserved.

**Lossy conversions that MUST NOT be allowed at all**:

 * String character substitution or omission
 * Truncation from storing in a type that cannot hold all of the data (except where decided based on configuration - see below)

**Lossy conversions that MUST be decided based on configuration**:

 * Loss of floating point coefficient accuracy from conversion between binary and decimal float types
 * Loss of floating point coefficient precision from storing in a smaller float type
 * Loss of subsecond precision due to temporal type mismatch or platform capabilities
 * Conversion from a real [time zone](#time-zones) to a [UTC offset](#utc-offset)

Implementations **MUST** provide a configuration **OPTION** for each configurable lossy conversion that can occur on its platform, and each option **MUST** default to disabled.

Disallowed or disabled lossy conversions are [error conditions](#error-processing).

#### Binary and Decimal Float Conversions

Binary and decimal float values can rarely be converted to each other without data loss, but conversions are sometimes necessary:

 * The destination platform might not support the requested type.
 * The record being decoded into might only have a partially compatible type.

Conversion between binary and decimal float values **MUST** be done using a method that effectively produces the same result in the destination type as the following algorithm would:

1. Convert the source value to its string-based decimal float encoding using the standard library.
2. Convert the string value into the destination type using the standard library.

Where conversion between binary float and decimal string representation follows one of the commonly accepted conversion algorithms present in most standard libraries, for example:

 * Jerome T. Coonen: "An Implementation Guide to a Proposed Standard for Floating-Point Arithmetic." Computer, Vol. 13, No. 1, January 1980, pp. 68-79
 * Guy. L. Steele Jr. and J. L. White: "How to print floating-point numbers accurately". In proceedings of ACM SIGPLAN '90 Conference on Programming Language Design and Implementation, White Plains, New York, June 1990, pp. 112-126
 * David M. Gay: "Correctly rounded binary-decimal and decimal-binary conversions." Technical Report 90-10, AT&T Bell Laboratories, November 1990.
 * Robert G. Burger and R. Kent Dybvig: "Printing floating-point numbers quickly and accurately." In proceedings of ACM SIGPLAN 1996 conference on Programming Language Design and Implementation, Philadelphia, PA, USA, May 1996, pp. 108-116
 * Guy L. Steele Jr. and Jon L. White: "Retrospective: How to print floating-point numbers accurately." ACM SIGPLAN Notices, Vol. 39, No. 4, April 2004, pp. 372–389
 * Florian Loitsch: "Printing floating-point numbers quickly and accurately with integers." In proceedings of 2010 ACM SIGPLAN Conference on Programming Language Design and Implementation, Toronto, ON, Canada, June 2010, pp. 233-243
 * Marc Andrysco, Ranjit Jhala, and Sorin Lerner: "Printing floating-point numbers: a faster, always correct method." ACM SIGPLAN Notices, Vol. 51, No. 1, January 2016, pp. 555-567
 * Ulf Adams: "Ryū: fast float-to-string conversion." ACM SIGPLAN Notices, Vol. 53, No. 4, April 2018, pp. 270-282

This helps to minimize exploitable behavioral differences between implementations.


### Problematic Values

It's best to think ahead about types and values that might be problematic on the various platforms your application runs on. In some cases, switching to a different type might be enough. In others, a schema limitation might be the better approach. Regardless, applications **SHOULD** always take problematic values and their mitigations into account during the design phase in order to ensure uniform (and thus unexploitable) behavior in all parts of an application.

 * The [Concise Binary Encoding](cbe-specification.md) integer encoding can store the "integer" value `-0` (which is actually a float), but if the destination cannot handle floating point values, it will have to reject the document.
 * Platforms might not be able to handle the NUL character in strings. Please see the [NUL character](#nul-character) section for how to deal with this.
 * Platforms might not support UTF-8 encoding.
 * Platforms might not support the full range of Unicode codepoints.
 * Platforms might have limitations on the size of numeric types.
 * Platform temporal types might not support the same kinds of time zones, or might not support subsecond values to the same resolution.
 * Platforms might not support data with cyclical references.
 * Platforms might not provide some of the less common data types such as [edge](#edge), and [node](#node). Generic types for these could be provided by the codec.
 * The destination structure might not support [references](#reference). In such a case, duplicating the data might be enough (taking care not to exceed the [global object limit](#user-controllable-limits)).
 * Applications **SHOULD** always decide upon a common configuration across all codecs in all platforms they use so that they conform to the same [limits](#security-and-limits).



Equivalence
-----------

When comparing objects to detect duplicate entries in containers, object equivalence is decided according to the following rules:

### Boolean Equivalence

Booleans can only be compared to other booleans. There is no inferred "truthiness" in Concise Encoding.

### Integer and Float Equivalence

Integers and floats do not have to be of the same type or size in order to be equivalent. For example, the 32-bit float value 12.0 is equivalent to the 8-bit integer value 12. So long as they can be round-trip converted to the destination type and back again to the same value without data loss, they are equivalent.

#### Special Float Value Equivalence Follows ieee754

* Infinity values are equivalent if they have the same sign.
* Zero values are equivalent regardless of sign.
* NaN values are never equivalent.

### Custom Type Equivalence

Unless the schema specifies otherwise, custom types are compared byte-by-byte, with no other consideration to their contents. Custom text values **MUST NOT** be compared to custom binary values unless they can both first be converted to a common type that the receiver can compare.

### String Type Equivalence

String types (such as [string](#string), [resource id](#resource-identifier), [remote reference](#remote-reference)) are considered equivalent if their types are the same and their contents are equal after decoding escape sequences, [NUL stuffing](#nul-character), etc. Comparisons are case sensitive unless otherwise specified by the schema.

String types **MUST NOT** be normalized prior to comparison; a different encoding of the same characters is **not** equivalent.

Comparisons are case sensitive unless otherwise specified by the schema.

String types are never equivalent to any other type, even if the byte contents are the same or "look like" the string encoding of something (e.g. `"https://concise-encoding.com/"` is not equivalent to `@"https://concise-encoding.com/"`, and `"2"` is not equivalent to `2`).

### Array Equivalence

Arrays **MUST** contain the same number of elements in the same order, and each element **MUST** be equivalent.

The equivalence rules for integer and float types also extends to numeric arrays. For example, the 16-bit unsigned int array `1 2 3`, 32-bit integer array `1 2 3`, and 64-bit float array `1.0 2.0 3.0` are equivalent.

### Container Equivalence

Containers are never considered equivalent when testing for duplicates, as this would cause too much complexity.

### Null Equivalence

[Null](#null) values are always considered equivalent to each other.

### Comment Equivalence

Comments are always ignored when testing for equivalence.

### Padding Equivalence

Padding is always ignored when testing for equivalence.



Error Processing
----------------

Any error encountered when encoding or decoding a Concise Encoding document **MUST** halt processing and issue diagnostic information about what went wrong.

**Note**: A decoder **MUST NOT** destroy or withhold from the application the already successfully decoded objects from before the error occurreed. It is up to the application to decide what to do with the already-decoded objects (e.g. an application running over an unreliable communications medium might elect to keep the partial data if there's enough of it to be actionable).

Example error types:

 * Improper document structure (mismatched container start/end, etc).
 * Incorrect data types for the current context (map keys, etc).
 * Malformed identifiers.
 * Failed reference lookup.
 * Failed [global limit checks](#user-controllable-limits).
 * Failed string-like object validation.
 * Failed value constraint validation.
 * Unrecognized custom data.
 * Premature end-of-file (incomplete document).



Security and Limits
-------------------

Accepting data from an outside source is always a security risk. The safest approach is to always assume hostile intentions when ingesting data.

Although Concise Encoding supports a wide range of data types and values, any given implementation will have limitations of some sort on their abilities due to the platform, language, system, and performance profiles. This will invariably lead to subtle differences in CE implementations and applications that an attacker might be able to take advantage of if you're not careful.


### Attack Vectors

There are many vectors that attackers could take advantage of when they control the data your system is receiving, the most common of which are [induced data loss](#induced-data-loss), [default type conversions](#default-type-conversions), [field omission](#induced-omission), [key collisions](#key-collisions), [exploitation of algorithmic complexity](#deserialization-complexity), and [payload size](#payload-size).

#### Induced Data Loss

Once the characteristics of a system are known, an attacker could anticipate under what circumstances it will suffer data loss based on the language and technology used, the host it's running on, and the code itself.

The most common sources of deserialization data loss are:

 * On integer overflow: losing the most significant portion of the value.
 * On floating point overflow: losing a portion of the significand and/or exponent, rounding to infinity, or converting to NaN
 * Truncating long arrays/strings
 * Erasing/replacing invalid characters in a string-like object

Any part of your system that allows data loss is a potential security hole, because different parts of your system will likely handle the same loss-inducing data in different ways, and those differences could be exploited by an attacker using specially crafted documents.

As a contrived example, consider a fictional system where the access control subsystem running on platform A leaves bad characters as-is or replaces them with u+fffd, and the storage subsystem running on platform B truncates bad characters. If an attacker is able to send a "change user's group" command with a group of `admin\U+D800` (which would pass access control validation because `admin\U+D800` != `admin`), he could set up an admin user because the storage subsystem truncates bad characters and stores the group `admin\U+D800` as `admin`. The next time that user is loaded, it will be in group `admin`.

Numbers could also suffer data loss depending on how the decoded values are stored internally. For example, attempting to load the value 0x123456789 into a 32-bit unsigned integer would in many languages silently overflow to a result of 0x23456789. Similarly, the value 0x87654321 (2271560481) stored in a 32-bit ieee754 binary float field would be silently truncated to 2271560448, losing precision and changing the effective value because it only has 24 bits available for the siginificand.

#### Default Type Conversions

Default type conversions are a form of data loss, and are specially cited here because they tend to happen silently as a result of how the language they're implemented in works. For example, PHP silently converts string values to 0 when comparing to a number. C and C++ treat all nonzero values as "true", including pointers. Other languages convert the numeric value to a string using the default format and then compare. These differing behaviors could potentially be exploited by an attacker.

Default truncation is also a problem. For example, some systems will truncate out-of-range values to 0, meaning that if an attacker could somehow cause an out of range price (999999999999999999999999999999999999 or whatever), such a system would convert it to 0, giving it away for free!

#### Induced Omission

Omission occurs when a decoder decides to drop bad data rather than store a default value or attempt to "fix" it. When this occurs, the count of objects in containers will change, and critical information might now be missing. This could become a security hole if the fields are preserved by the decoders in some parts of your system but omitted in others.

#### Key Collisions

Key collisions occur when two or more identical keys are present in the same encoded map (assuming the map disallows duplicates). The most common ways systems tend to handle key collisions are:

 * Take the initial key and reject all duplicate keys (choose the first instance)
 * Replace the value for each duplicate encountered (choose the last instance)
 * Reject the key and all associated values (choose nothing)
 * Reject the entire document (abort processing)

The first three approaches are dangerous and lead to security issues. Only rejecting the document is safe (which is what Concise Encoding mandates).

For example, given the map:

    {
        "purchase-ids" = [1004 102062 94112]
        "total" = 91.44
        "total" = 0
    }

As a seller, you'd want your billing system to choose the first instance of "total". As a buyer, you'd much prefer the second!

**Note**: Key collisions could also occur as a result of [data loss](#induced-data-loss) or even [default type conversions](#default-type-conversions):

    {
        "purchase-ids" = [1004 102062 94112]
        "total" = 91.44
        "total\[D800]" = 0
    }

In this case, if the system truncated bad Unicode characters _after_ checking for duplicate keys, it would be vulnerable to exploitation.

#### Deserialization Complexity

Depending on the implementation, some operations could get expensive very quickly the larger the object is, exhibiting O(n²) or sometimes even O(n³) behavior. This is particularly true of "big int" type structures in many languages. Even attempting to deserialize values as small as 10^1000 into a BigInt could DOS some systems.

#### Payload Size

Attackers can exploit payload size limitations (either leveraging differences between implementations, or directly attacking an implementation for overflows and such). For example:

 * Extremely large objects (like a 1TB array).
 * High object count.
 * Deep container depth (attempting to overflow the decoder's stack).


### Mitigations: Concise Encoding Codecs

To mitigate these kinds of security issues, Concise Encoding codecs have the following additional requirements:

#### Validation

All decoded values **MUST** be validated for the following before being passed to the application:

* [Global limits](#user-controllable-limits)
* Content rules (based on type)
* Schema rules (if any)

#### User-Controllable Limits

A codec **MUST** provide at least the following **OPTIONS** to allow the user to control various limits and ranges, with sane defaults to guard against denial-of-service attacks:

| Limit                                | Clarification                            |
| ------------------------------------ | ---------------------------------------- |
| Max document size                    | In bytes                                 |
| Max array size                       | Per array, in bytes                      |
| Max [identifier](#identifier) length | In bytes                                 |
| Max object count                     |                                          |
| Max container depth                  | 0 = [top-level object](#document-structure) cannot contain other objects, 1 = [top-level object](#document-structure) can contain objects (which cannot themselves contain other objects), ... |
| Max integer digits                   |                                          |
| Max float coefficient digits         |                                          |
| Max decimal float exponent digits    | Max binary float exponent digits = `max_decimal_digits × 10 ÷ 3` rounded down. |
| Max year digits                      |                                          |
| Max marker count                     |                                          |
| Max reference count                  |                                          |

**Notes**:

 * An array along with its contents constitute a _single_ object.
 * [Referenced](#reference) containers are **not** double-counted (the reference itself is counted as an object, but the container and contents it references are not counted again, and do not count towards the container depth).
 * See [Appendix A: Mandatory User-controllable limits](#mandatory-user-controllable-limits) for a list of default values.


### Mitigations: Application Guidelines

For application developers, security is a frame of mind. You **SHOULD** always be considering the risks of your architecture, and the size of your attack surface. Here are some general guidelines:

 * Harmonize limits across your entire application surface (all subsystems). The most insidious exploits take advantage of differences in processing.
 * Use a common schema to ensure that your validation rules are consistent across your infrastructure.
 * Treat received values as all-or-nothing. If you're unable to store it in its entirety without data loss, it **SHOULD** be rejected. Allowing data loss means opening your system to key collisions and other exploits.
 * Guard against unintentional default conversions (for example string values converting to 0 or true in comparisons).
 * When in doubt, toss it out. The safest course of action with foreign data is all-or-nothing. Not rejecting the entire document means that you'll have to compromise (either truncating or omitting data), which opens your system to exploitation.



Appendix A: List of Codec Options
---------------------------------

This section collects in one place all codec **OPTIONS** listed elsewhere in the specification.


### Mandatory Options

The following options **MUST** be present in a Concise Encoding codec:

| Option                                | Default    | Section                                             |
| ------------------------------------- | ---------- | --------------------------------------------------- |
| Allow recursive references            | forbidden  | [Recursive References](#recursive-references)       |
| Keep partial document after error     | disabled   | [Truncated Document](#truncated-document)           |

The following lossy conversion options **MUST** be present in a Concise Encoding codec:

| Option                                | Default    | Section                                             |
| ------------------------------------- | ---------- | --------------------------------------------------- |
| Lossy binary decimal float conversion | forbidden  | [Lossy Conversions](#lossy-conversions)             |
| Lossy conversion to smaller float     | forbidden  | [Lossy Conversions](#lossy-conversions)             |
| Subsecond truncation                  | forbidden  | [Lossy Conversions](#lossy-conversions)             |
| Time zone to time offset conversion   | forbidden  | [Lossy Conversions](#lossy-conversions)             |


### Mandatory [User-controllable limits](#user-controllable-limits)

The following options **MUST** be present in a Concise Encoding codec, and their defaults will need to be revisited from time to time based on the capabilities of machines of the era:

| Option                            | Recommended Default (in 2020) |
| --------------------------------- | ----------------------------- |
| Max document size                 | 5 GB (5 × 1024 × 1024 × 1024) |
| Max array size                    | 1 GB (1 × 1024 × 1024 × 1024) |
| Max identifier length             | 1000                          |
| Max object count                  | 1,000,000                     |
| Max container depth               | 1000                          |
| Max integer digits                | 100                           |
| Max float coefficient digits      | 100                           |
| Max decimal float exponent digits | 5                             |
| Max year digits                   | 11                            |
| Max marker count                  | 10,000                        |
| Max reference count               | 10,000                        |


### Recommended Options

The following [CTE](cte-specification.md) options are recommended, but not required:

| Option                                | Default   | Section                                             |
| ------------------------------------- | --------- | --------------------------------------------------- |
| CTE: Integer output format            | base 10   | [Integer](cte-specification.md#integer)             |
| CTE: Binary float output format       | base 16   | [Binary Float](cte-specification.md#floating-point) |
| CTE: Array: integer output format     | base 10   | [Primitive type array encoding](cte-specification.md#primitive-type-array-encoding) |
| CTE: Array: float output format       | base 16   | [Primitive type array encoding](cte-specification.md#primitive-type-array-encoding) |


### Schema Options

The following options **CAN** be set globally and on a per-object basis by a schema (if the schema technology can support it):

| Option                                | Default   | Section                       |
| ------------------------------------- | --------- | ----------------------------- |
| List Ordering                         | ordered   | [List](#container-properties) |
| List Duplicates                       | allowed   | [List](#container-properties) |
| Map Ordering                          | unordered | [Map](#container-properties)  |
| Map Key Duplicates                    | forbidden | [Map](#container-properties)  |
| Record Type Duplicates                | forbidden | [Record Type](#record-type)   |



Appendix B: Recording Time
--------------------------

Time is one of the most difficult data types to get right. Aside from issues of synchronization, leap seconds, data container limitations and such, it's important to choose what **kind** of time you need to store, and the right kind depends on what the purpose of storing the time value is.

There are three main kinds of time:

### Absolute Time

Absolute time is a time that is locked to UTC (or an offset from UTC). It is not affected by daylight savings time, nor will it be affected if an area's time zone changes through political action. Absolute time is best recorded in the UTC time zone, and is mostly useful for events in the past (because the time zone that was in effect when the event happened is forever locked in, no longer affecting the time's interpretation).

### Fixed Time

Fixed time is fixed to a particular place. If the time zone at that place changes, the fixed time's corresponding absolute time will change as well. For example, 12:00 in Toronto on June 1st is equivalent to 11:00 in Toronto on December 1st relative to UTC due to daylight savings. Fixed time is mostly useful for times in the future (such as an appointment in London this coming October 12th) or repeating events. If the expected time zone changes (for example an act of government such as a change to daylight savings), fixed time adapts automatically.

### Floating Time

Floating (or local) time is always relative to the time zone of the observer. If you travel and change time zones, floating time changes time zones with you. If two observers in different time zones observe the same floating time, they will convert that same floating time to different absolute times. An example would be an 8:00 morning workout.


### Where each kind fits best

Use whichever kind of time most succinctly handles your time needs. Don't depend on time zone information as a proxy for a location (that would be depending upon a side effect, which is always brittle). Always store location information separately if it's important information to your system.

| Situation                            | Kind                                            |
| ------------------------------------ | ----------------------------------------------- |
| Log entries and past events          | Absolute                                        |
| Future events                        | Absolute or fixed (it depends on the use case)  |
| Appointments                         | Fixed                                           |
| Your daily schedule                  | Floating                                        |
| Multi-zone shared repeating schedule | Absolute or fixed (it depends on the use case)  |
| Deadlines                            | Usually fixed time, but possibly absolute time. |



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
