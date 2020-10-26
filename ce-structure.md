Concise Encoding - Structural Specification
===========================================

Concise Encoding is a general purpose, human and machine friendly, compact representation of semi-structured hierarchical data.

It is composed of 1:1 type compatible [text](cte-specification.md) and [binary](cbe-specification.md) formats, both of which follow the same structural and content rules laid out in this document. Examples are given in the [text](cte-specification.md) format for clarity.



Version
-------

Version 1 (prerelease)



Contents
--------

* [Terms](#terms)
* [Structure](#structure)
* [Version Specifier](#version-specifier)
* [Numeric Types](#numeric-types)
  - [Boolean](#boolean)
  - [Integer](#integer)
  - [Floating Point](#floating-point)
  - [UUID](#uuid)
* [Temporal Types](#temporal-types)
  - [Date](#date)
  - [Time](#time)
  - [Timestamp](#timestamp)
  - [Time Zones](#time-zones)
    - [Area/Location](#arealocation)
    - [Global Coordinates](#global-coordinates)
* [Array Types](#array-types)
  - [String-like Arrays](#string-like-arrays)
    - [String](#string)
    - [URI](#uri)
  - [Typed Array](#typed-array)
  - [Custom Types](#custom-types)
    - [Binary Encoding](#binary-custom-type)
    - [Text Encoding](#text-custom-type)
* [Container Types](#container-types)
  - [List](#list)
  - [Map](#map)
  - [Markup](#markup)
* [Pseudo-Objects](#pseudo-objects)
  - [Marker](#marker)
  - [Reference](#reference)
  - [Metadata Map](#metadata-map)
  - [Comment](#comment)
  - [Padding](#padding)
  - [Constant](#constant)
* [Other Types](#other-types)
  - [Null](#null)
* [Unquoted-Safe String](#unquoted-safe-string)
  - [Confusable Characters](#confusable-characters)
* [Equivalence](#equivalence)
* [Document Limits](#document-limits)
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

A Concise Encoding document is a binary or text encoded document containing data arranged in an ad-hoc hierarchical fashion. Data is stored serially, and can be progressively read or written.

Documents begin with a [version specifier](#version-specifier), followed by a top-level object. To store multiple values in a document, use a [container](#container-types) as the top-level object and store other objects within that container.

    [version specifier] [object]

The top-level object can be preceded by [pseudo-objects](#pseudo-objects), but must itself be a real object of any type.

**Examples**:

 * Empty document (CTE version 1): `c1 @null`
 * Document containing the top-level integer value 1000: `c1 1000`
 * Document containing a top-level list: `c1 [string1 string2 string3]`
 * Document with metadata referring to the top-level object: `c1 (a=b) some-string-as-top-level-object`



Version Specifier
-----------------

The version specifier is composed of a 1-byte type identifier (`c` for CTE, 0x03 for CBE) followed by a version number, which is an unsigned integer representing which version of this specification the document adheres to.

**Example**:

 * CTE version 1: `c1`



Numeric Types
-------------

### Boolean

Supports the values true and false.


### Integer

Integer values can be positive or negative, and can be represented in various bases and sizes. An implementation may alter base and size when encoding/decoding as long as the numeric value remains the same.


### Floating Point

A floating point number is composed of a coefficient and an exponent, and can be binary or decimal. In a decimal floating point number, the exponent represents 10 to the power of the exponent value, whereas in a binary floating point number the exponent represents 2 to the power of the exponent value. Concise Encoding supports both decimal and binary floating point numbers in various sizes, configurations, and notations.

Binary floating point values represent ieee754 binary floating point for 32-bit and 64-bit sizes, and [bfloat](https://software.intel.com/sites/default/files/managed/40/8b/bf16-hardware-numerics-definition-white-paper.pdf) for 16-bit. Following ieee754-2008 recommendations, the most significant bit of the significand field of an ieee754 binary NaN (not-a-number) value is defined as the "quiet" bit. When set, the NaN is quiet. When cleared, the NaN is signaling.

    s 1111111 1xxxxxxxxxxxxxxxxxxxxxxx = Quiet NaN (binary float32)
    s 1111111 0xxxxxxxxxxxxxxxxxxxxxxx = Signaling NaN (binary float32)

An implementation may discard NaN information other than its signaling/quiet status.

An implementation may alter the type and storage size of a floating point value when encoding/decoding as long as the final numeric value remains the same.

#### Special Floating Point Values

Both decimal and binary floating point numbers have representations for the following special values:

* Two kinds of zero: +0 and -0
* Two kinds of infinities: +∞ and -∞
* Two kinds of NaN (not-a-number): Signaling and quiet


### UUID

The 128-bit universally unique identifier must be structured according to the [rfc4122](https://tools.ietf.org/html/rfc4122) definition of the UUID string representation (in CTE) or binary representation (in CBE).



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

 * Year, month, and day fields must not be 0 (counting starts at 1).
 * The year value must be negative to represent BC dates, and positive to represent AD dates. The Anno Domini system has no zero year (there is no 0 BC or 0 AD), so the year values `0` and `-0` are invalid.
 * The year field must contain the full year (no abbreviations).

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
 * Since a time by itself has no date component, time zone data must be interpreted as if it were "today". This means that time zones which are not offsets (such as `Etc/GMT+1`) might be interpreted differently on different dates for political reasons (such as daylight savings).
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

A time zone refers to the political designation of a location having a specific time offset from UTC at a particular time. Time zones are in a constant state of flux, and can change at any time for many reasons. There are two ways to denote a time zone: by area/location, and by global coordinates.

**Note**: If the time zone is unspecified, it is assumed to be `Zero` (UTC).

#### Area/Location

The area/location method is the more human-readable of the two, but might not be precise enough for certain applications. Time zones are partitioned into areas containing locations, and are written in the form `Area/Location`. These areas and locations are specified in the [IANA time zone database](https://www.iana.org/time-zones). Area/Location timezones are case-sensitive because they tend to be implemented that way on most platforms.

**Note**: Some locations might be split into sub-components for disambiguation (for example `America/Indiana/Petersburg`, which has area `America` and location `Indiana/Petersburg`).

See "[Theory and pragmatics of the tz code and data](https://data.iana.org/time-zones/tzdb-2020b/theory.html)" for more information.

##### Abbreviated Areas

Since there are only a limited number of areas in the database, the following abbreviations can be used in the area portion of the time zone to save space:

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

The following special pseudo-areas can also be used. They do not contain a location component.

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

An array represents a contiguous sequence of fixed length elements. The type of the array determines how its contents are interpreted.

There are three kinds of array representations in Concise Encoding:

 * String-like arrays, which contain UTF-8 data. String-like array lengths are counted in bytes.
 * [Typed arrays](#typed-array), whose contents represent elements of a particular type. A typed array's length is counted in elements, not bytes.
 * Custom types, which represent custom data that only a custom codec designed for them will understand. Custom type lengths are counted in bytes, and are encoded either in the style of a uint8 array for custom binary, or a string-like array for custom text.


### String-like Arrays

#### String

An array of UTF-8 encoded bytes. The NUL character (u+0000) must be supported, but because NUL can be a troublesome character on many platforms, its use in documents is discouraged.

Strings must always resolve to complete, valid UTF-8 sequences when fully decoded (in CTE, validation must occur after decoding all escape sequences).

##### Line Endings

Line endings can be encoded as LF only (u+000a) or CR+LF (u+000d u+000a) to maintain compatibility with editors on various popular platforms. However, for data transmission, the canonical format is LF only. Decoders must accept all encodings as input, but encoders should only output LF when the destination is a foreign or unknown system.

#### URI

The Uniform Resource Identifier must be structured according to [RFC 3986](https://tools.ietf.org/html/rfc3986).


### Typed Array

A typed array encodes an array of values of a fixed type and size. In a CBE document, the array elements will all be adjacent to each other, allowing large amounts of data to be easily copied between the stream and your internal structures.

Fixed width types boolean, signed/unsigned integer (8-64 bit), binary float (16-64 bit), and UUID can be stored in typed arrays. For other types, use a [list](#list).

Array elements can be any of the representations allowed for the specified type and size.

**Examples**:

    |u8x 9f 47 cb 9a 3c|
    |f32 1.5 0x4.f391p100 30 9.31e-30|
    |i16 0b1001010 0o744 1000 0xffff|
    |uu 3a04f62f-cea5-4d2a-8598-bc156b99ea3b 1d4e205c-5ea3-46ea-92a3-98d9d3e6332f|
    |b 1 1 0 1 0|


### Custom Types

There are some situations where a custom data type is preferable to the standard types. The data might not otherwise be representable, or it might be too bulky using standard types, or you might want the data to map directly to/from memory structs for performance reasons.

Custom types restrict interoperability to implementations that understand the types, and should only be used as a last resort. An implementation that encounters a custom type it doesn't recognize must report the problem to the user and substitute [null](#null).

Custom type implementations should provide both a binary and a text encoding, with the binary encoding preferred for CBE documents, and the text encoding preferred for CTE documents. When both binary and text forms of a custom type are provided, they must be 1:1 convertible to each other without data loss.

**Note**: Although custom types are encoded as "array types", the interpretation of their contents is user-defined, and they might not represent an array at all.

#### Binary Custom Type

A uint8 array value representing a user-defined custom data type. The interpretation of the octets is implementation defined, and must be understood by both sending and receiving parties. To reduce cross-platform confusion, data should be represented in little endian byte order.

**Example**:

    [93 12 04 f6 28 3c 40 00 00 40 40]
    = binary data representing an imaginary custom "cplx" struct
      {
          type:uint8 = 4
          real:float32 = 2.94
          imag:float32 = 3.0
      }

#### Text Custom Type

A string-like array value representing a user-defined custom data type. The interpretation of the string is implementation defined, and must be understood by both sending and receiving parties.

**Example**:

    |ct cplx(2.94+3i)|



Container Types
---------------

### List

A sequential list of objects. Lists can contain any mix of any type, including other containers.

By default, a list is ordered and allows duplicate values. Different rules can be set using a schema.

**Example**:

    [1 two 3.1 {} @null]


### Map

A map associates key objects with value objects. Keys can be any mix of any [keyable type](#keyable-types). Values can be any mix of any type, including other containers.

Map entries are stored as key-value pairs. A key without a paired value is invalid.

By default, a map is unordered and does not allow duplicate keys. Different rules can be set using a schema.

Keys must be unique across numeric data types. For example, the following keys are considered duplicates:

 * Integer value 2000
 * Floating point value 2000.0

**Note**: The string value "2000" is not numeric, and would not be considered a duplicate of 2000 or 2000.0.

#### Keyable types

Only certain types can be used as keys in map-like containers:

* [Numeric types](#numeric-types), except for NaN (not-a-number)
* [Temporal types](#temporal-types)
* [Strings](#string)
* [URI](#uri)
* [References](#reference) (only if the referenced value is keyable)

Null must not be used as a key.

**Example**:

    {
        1 = alpha
        2 = beta
        "a map" = {one=1 two=2}
    }


### Markup

A markup container stores data in a hierarchical style similar to XML. Each element is composed of a name, an optional map of attributes, and an optional list of contents.

    [name] [optional attributes] [optional contents]

 * Name is a [keyable type](#keyable-types), and is case-insensitive if it's a string.
 * Attributes behaves like a [map](#map)
 * The contents section behaves similarly to a [list](#list), except that it can only contain:
   - [Content strings](#content-string)
   - [Comments](#markup-comment)
   - Other markup containers

Markup containers are best suited for presentation data. For regular data, maps and lists are better.

Illustration of markup encodings:

| Attributes | Contents | Example                                                    |
| ---------- | -------- | ---------------------------------------------------------- |
|     N      |    N     | `<br>`                                                     |
|     Y      |    N     | `<div id=fillme>`                                          |
|     N      |    Y     | `<span:Some text here>`                                    |
|     Y      |    Y     | `<ul id=mylist style=boring: <li:first> <li:second> >`     |

##### Content String

In content strings, leading and trailing whitespace is not considered significant. Whitespace sequences between printable sequences in a content string are considered equivalent to a single space character (u+0020). Whitespace at the beginning or end of a string can be ignored (trimmed).

Implementations may alter whitespace in content strings for aesthetic reasons so long as at least one whitespace character remains between printable sequences.

##### Entity Reference

The Concise Encoding formats don't interpret [entity references](https://en.wikipedia.org/wiki/SGML_entity); they are treated as regular text.

#### Example

```
c1
<View;
    <Image src=|u images/avatar-image.jpg|>
    <Text;
        Hello! Please choose a name!
    >
    /* <HRule style=thin> */
    <TextInput id=name style={height=40 borderColor=gray}; Name me! >
>
```



Pseudo-Objects
--------------

Pseudo-objects add additional context or metadata to another object or to the document, or affect the interpreted structure of the document in some way.

Pseudo-objects can be placed anywhere a full object (not an array element) can be placed, but **referring** or **invisible** pseudo-objects do not themselves constitute objects. For example, `(begin-map) ("a key") (referring pseudo-object) (end-container)` is not valid, because a referring pseudo-object doesn't represent a real object, and therefore doesn't complete the key-value pair for "a key".

Like regular objects, pseudo-objects must not appear before the [version specifier](#version-specifier), and must not appear after the top-level object.

### Pseudo-object Properties

 * **Referring** pseudo-objects refer to the next object following at the current container level. This will either be a real object, or a visible pseudo-object
 * **Invsible** pseudo-objects are effectively invisible to referring pseudo-objects, and are skipped over when searching for the object that is being referred to by a referring pseudo-object.

Only a **non-referring** _and_ **visible** pseudo-object can stand in for a real object.

### Marker

A marker is a **referring**, **invisible** pseudo-object that tags the next object with a [marker ID](#marker-id), such that it can be referenced from another part of the document (or from a different document).

    [marker id] [marked object]

#### Marker ID

A marker ID is a unique (to the document) identifier for marked objects. A marker ID can be one of:

1. A positive integer from 0 to 18446744073709551615 (up to 64 bits)
2. An [unquoted-safe string](#unquoted-safe-string) with a max length of 30 Unicode characters.

#### Rules:

 * A marker cannot mark an object in a different container level. For example: `(begin-list) (marker ID) (end-list) (string)` is invalid.
 * Marker IDs must be unique in the document; duplicate marker IDs are invalid.
 * Marker IDs must be compared case-insensitive.

#### Example:

    [
        &remember_me:"Pretend that this is a huge string"
        &1:{a = 1}
    ]

The string `"Pretend that this is a huge string"` is marked with the ID `remember_me`, and the map `{a=1}` is marked with the ID `1`.


### Reference

A reference is a **non-referring**, **visible** pseudo-object that acts as a stand-in for an object that has been [marked](#marker) elsewhere in this or another document. This could be useful for repeating or cyclic data. Unlike other pseudo-objects, references can be used just like regular objects (for example, `(begin-map) ("a key") (reference) (end-container)` is valid).

A reference is followed by either a [marker ID](#marker-id) or a [URI](#uri).

Rules:

 * A reference with a [local marker ID](#marker-id) must refer to another object marked elsewhere in the same document (local reference).
 * A reference used as a map key must refer to a [keyable type](#keyable-types).
 * A URI reference must not be used as a map key.
 * Forward references within a document are allowed.
 * Recursive references are allowed.
 * A reference with a URI must point to:
   - Another CBE or CTE document (using no fragment section, thus referring to the entire document)
   - A marker ID inside another CBE or CTE document, using the fragment section of the URI as an ID

**Note**: Implementations should define security rules for following URI references; blindly following an unknown URI is dangerous.

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

A metadata map is a **referring**, **visible** pseudo-object containing keyed values which are to be associated with the object following the metadata map.

Metadata is data about the data, which might or might not be of interest to a consumer of the data. Implementations must allow the user to choose whether to receive or ignore metadata maps.

Metadata can refer to other metadata (meta-metadata).

#### Metadata Keys

Keys in metadata maps follow the same rules as for [regular maps](#map), except that all string typed keys beginning with the underscore `_` character are reserved for predefined keys, and must only be used in accordance with the [Common Generic Metadata specification](common-generic-metadata.md).

Predefined metadata keys should be used where possible to maximize interoperability between systems.

#### Metadata Example

    c1
    // Metadata for the entire document
    (
        _ct = 2017.01.14-15:22:41/Z
        _mt = 2019.08.17-12:44:31/Z
        _at = 2019.09.14-09:55:00/Z
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

A comment is a **non-referring**, **invisible**, list-style pseudo-object that can only contain strings or other comment containers (to support nested comments).

Although comments are not _referring_ pseudo-objects, they tend to unofficially refer to what follows in the document, similar to how comments are used in source code.

Comments do not support escape sequences. Character sequences that look like escape sequences in comments are not processed (they are passed through verbatim).

Whitespace in a comment string is treated the same as in [markup content strings](#content-string).

Implementations must allow the user to choose whether to receive or ignore comments.

**Note**: Comments cannot occur inside of [typed arrays](#type-array).

#### Comment String Character Restrictions

The following characters are explicitly allowed:

 * Horizontal Tab (u+0009)
 * Linefeed (u+000a) - discarded in single line comments
 * Carriage Return (u+000d) - discarded in single line comments

The following characters are disallowed if they aren't in the above allowed section:

 * Control characters (such as u+0000 to u+001f, u+007f to u+009f).
 * Line breaking characters (such as u+2028, u+2029).

The following characters are allowed if they aren't in the above disallowed section:

 * UTF-8 printable characters
 * UTF-8 whitespace characters

The following character sequences must not be put into comment strings because they are comment delimiters in CTE:

* `/*`
* `*/`

#### Example

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


### Padding

Padding is **non-referring** and **invisible**. The padding type has no semantic meaning; its only purpose is for memory alignment. The padding type can occur any number of times before a CBE type field. A decoder must read and discard padding types. An encoder may add padding between objects to help larger data types fall on an aligned address for faster direct reads from the buffer on the receiving end.

Padding is only available for CBE documents.



### Constant

Constants are named values that have been defined in a schema. Constants are **non-referring** and **visible**. A CTE decoder must look up the constant name in the schema and use the value it maps to. CTE encoders must use constant names where specified by the schema.

A constant's name must be an [unquoted-safe string](#unquoted-safe-string).

Constants are only available in CTE documents; CBE documents aren't meant for human consumption, and store the actual value only.



Other Types
-----------

### Null

Denotes the absence of data.

**Note**: Null is a [contentious value in computer science](https://en.wikipedia.org/wiki/Null_pointer), and should be used with caution.



Unquoted-Safe String
--------------------

In certain contexts, string data must be restricted to characters that are safe to put in CTE documents without requiring special sentinel values such as double-quotes (`"`) or escape sequences. Such strings are called "unquoted-safe".

An unquoted-safe string must adhere to the following rules:

 * The string must not contain characters from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, numerals 0-9, underscore (`_`), dash (`-`), and dot (`.`).
 * The string must not begin with a character from u+0000 to u+007f, with the exception of lowercase a-z, uppercase A-Z, and underscore (`_`).
 * The string must not contain line breaks, whitespace, control, reserved, unassigned, private, or unprintable characters.
 * The string must not be empty.
 * The string's CTE representation must not contain escape sequences.
 * Any characters that look [confusingly similar to printable characters from Unicode range 0000-007f](#confusable-characters) are subject to the same rules as their lookalikes (for example, u+ff15 `５` is disallowed as a first character, and u+2039 `‹` is disallowed entirely in an unquoted-safe string).

**Examples**:

These are unquoted-safe strings:

    twenty-five
    value.next
    _underscore
    _150
    飲み物
    contains－dash－lookalikes

These are not unquoted-safe strings:

    String with whitespace
    disallowed*symbol
    contains-star-＊-lookalike
    .begins-with-a-dot
    ．begins-with-a-dot-lookalike


### Confusable Characters

The following is a mostly complete list of [Unicode characters](https://unicode.org/charts) found to be confusingly similar to symbols and numerals from 0000-007f.

**Note**: This list is not guaranteed complete! Use it as a guide only.

| Character | Lookalikes (codepoints)                                                 |
| --------- | ----------------------------------------------------------------------- |
| `0`-`9`   | 00b2, 00b3, 00b9, 2488-249b, ff10-ff19, 10931, 1d7ce-1d7ff, 1f100-1f10a |
| `!`       | 01c3, 203c, 2048, 2049, 2d51, fe15, fe57, ff01                          |
| `"`       | 02ba, 02ee, 201c, 201d, 201f, 2033, 2034, 2036, 2037, 2057, 3003, ff02  |
| `#`       | fe5f, ff03                                                              |
| `$`       | fe69, ff04                                                              |
| `%`       | 2052, fe6a, ff05                                                        |
| `&`       | fe60, ff06                                                              |
| `'`       | 00b4, 02b9, 02bb, 02bc, 02bd, 02ca, 02c8, 0374, 2018-201b, 2032, 2035, a78b, a78c, fe10, fe50, ff07, 10107, 1d112 |
| `(`       | 2474-2487, 249c-24b5, fe59, ff08                                        |
| `)`       | fe5a, ff09                                                              |
| `*`       | 204e, 2055, 2217, 22c6, 2b51, fe61, ff0a                                |
| `+`       | fe62, ff0b                                                              |
| `,`       | 02cc, 02cf, 0375, ff0c, 10100                                           |
| `-`       | 02c9, 2010-2015, 2212, 23af, 23bb, 23bc, 23e4, 23fd, fe58, fe63, ff0d, ff70, 10110, 10191, 1d116 |
| `.`       | fe52, ff0e                                                              |
| `/`       | 2044, 2215, 27cb, 29f8, 3033, ff0f, 1d10d                               |
| `:`       | 02f8, 205a, 2236, a789, fe13, fe30, fe55, ff1a, 1d108                   |
| `;`       | 037e, fe14, fe54, ff1b                                                  |
| `<`       | 00ab, 02c2, 3111, 2039, 227a, 2329, 2d66, 3008, fe64, ff1c, 1032d       |
| `=`       | a78a, fe66, ff1d, 10190, 16fe3                                          |
| `>`       | 00bb, 02c3, 203a, 227b, 232a, 3009, fe65, ff1e                          |
| `?`       | 2047-2049, fe16, fe56, ff1f                                             |
| `@`       | fe6b, ff20                                                              |
| `[`       | fe5d, ff3b, 1d115                                                       |
| `\`       | 2216, 27cd, 29f5, 29f9, 3035, fe68, ff3c                                |
| `]`       | fe5e, ff3d                                                              |
| `^`       | ff3e                                                                    |
| `_`       | 02cd, 23bd, ff3f                                                        |
| `` ` ``   | 02cb, fe11, fe45, fe46, fe51, ff40                                      |
| `{`       | fe5b, ff5b, 1d114                                                       |
| `\|`       | 00a6, 01c0, 2223, 2225, 239c, 239f, 23a2, 23a5, 23aa, 23ae, 23b8, 23b9, 23d0, 2d4f, 3021, fe31, fe33, ff5c, ffdc, ffe4, ffe8, 1028a, 10320, 10926, 10ce5, 10cfa, 1d100, 1d105, 1d1c1, 1d1c2 |
| `}`       | fe5c, ff5d                                                              |
| `~`       | 2053, 223c, 223f, 301c, ff5e                                            |



Equivalence
-----------

There are many things to consider when determining if two Concise Encoding documents are equivalent. This section helps clear up possible confusion.

**Note**: Equivalence is relaxed unless otherwise specified.


### Relaxed Equivalence

Relaxed equivalence is concerned with the question: Does the data destined for machine use come out essentially the same, even if there are some minor structural and type differences?

#### Numeric Types

Numeric values (integers and floats) do not have to be of the same type or size in order to be equivalent. For example, the 32-bit float value 12.0 is equivalent to the 8-bit integer value 12. So long as they can resolve to the same value without data loss, they are equivalent.

**Note**: In contrast to ieee754 rules, two floating point values ARE considered equivalent if they are both NaN, so long as they are both the same kind of NaN (signaling or quiet). Only the quiet bit is considered when comparing NaN values.

#### Custom Types

By default, custom types are compared byte-by-byte, with no other consideration to their contents. Custom text values cannot be compared with custom binary values unless additional contextual information is provided by the schema that the receiver can understand.

#### Strings

Strings are considered equivalent if their contents are equal. Comparisons are case sensitive unless otherwise specified by the schema.

#### Arrays

Arrays must contain the same number of elements, and those elements must be equivalent.

The equivalence rules for numeric types also extends to numeric arrays. For example, the 16-bit unsigned int array `1 2 3`, 32-bit integer array `1 2 3`, and 64-bit float array `1.0 2.0 3.0` are equivalent under relaxed equivalence.

#### Containers

Containers must be of the same type. For example, a map is never equivalent to a metadata map.

Containers must contain the same number of elements, and their elements must be equivalent, with one exception: for map-like containers, keys mapping to null are considered equivalent to the same type of map where the key is not present.

By default, list types must be compared ordered and map types unordered, unless their ordering was otherwise specified by the schema.

#### Markup Contents

Extraneous whitespace in a markup contents section is elided before comparison. Comparisons are case sensitive unless otherwise specified by the schema.

#### Null

Two null values are considered equivalent.

#### Comments

Comments are ignored when testing for relaxed equivalence.

#### Padding

Padding is always ignored when testing for equivalence.


### Strict Equivalence

Strict equivalence concerns itself with differences that can still technically have an impact on how the document is interpreted, even if the chances are low:

* Comments are compared, but are trimmed of leading and trailing whitespace before comparison. Comparisons are case sensitive unless otherwise specified by the schema.
* Keys mapping to null values are not equivalent to maps where the key is absent.
* Arrays must be of the same type to be considered equivalent.



Document Limits
---------------

Implementations should enforce limits to the documents they'll accept in order to protect against malicious payloads. These may include things such as:

 * Maximum object count overall
 * Maximum object count in a container
 * Maximum array length
 * Maximum total document size in bytes
 * Maximum container depth (from the top level to the most deeply nested container)
 * Maximum coefficient size in integer and fp numbers (significant digits)

It's impossible to prescribe what limits should be reasonable for all decoders, because different systems will have different constraints, and system capabilities increase as technologies improve. Decoders must either make known what their limits are, or provide the user with a way to configure them (with reasonable defaults). A decoder is free to discard documents that threaten to exceed its resources.

As an illustration, for a general purpose decoder the following default limits should give a reasonable balance (in 2020):

| Metric                 | Limit     |
| ---------------------- | --------- |
| Document size in bytes | 10GB      |
| Overall object count   | 1 billion |
| Container depth        | 1000      |
| Coefficient size       | 100       |



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
