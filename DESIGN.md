Design Document
===============

This document describes the design choices that went into Concise Encoding, and where it fits among existing data formats.



Contents
--------

- [Design Document](#design-document)
  - [Contents](#contents)
  - [Mission Statement](#mission-statement)
  - [Problem Space](#problem-space)
  - [Concise Encoding's Positioning](#concise-encodings-positioning)
  - [Primary Goals](#primary-goals)
    - [Security](#security)
    - [Simplicity](#simplicity)
    - [Ad-Hoc](#ad-hoc)
    - [Efficiency](#efficiency)
    - [Human Friendly](#human-friendly)
    - [Low Friction](#low-friction)
  - [Design Choices](#design-choices)
    - [Twin Formats](#twin-formats)
    - [Opinionated](#opinionated)
    - [Byte Oriented](#byte-oriented)
    - [Little Endian](#little-endian)
    - [Avoiding Turing Completeness](#avoiding-turing-completeness)
    - [Signature Byte 0x81](#signature-byte-0x81)
      - [Example: Encoding CBE into a QR Code](#example-encoding-cbe-into-a-qr-code)
    - [Versioned Documents](#versioned-documents)
    - [Short Array Forms](#short-array-forms)
    - [Date Formats](#date-formats)
    - [Human Readability](#human-readability)
    - [Zero Copy](#zero-copy)
    - [Array Chunking](#array-chunking)
      - [Zero Chunk](#zero-chunk)
    - [Padding](#padding)
    - [LEB128](#leb128)
    - [Binary and Decimal Floats](#binary-and-decimal-floats)
    - [Escape Sequences](#escape-sequences)
      - [Continuation](#continuation)
      - [Unicode Escape Sequences](#unicode-escape-sequences)
      - [Verbatim Sequence](#verbatim-sequence)
    - [NUL support](#nul-support)
    - [Whitespace](#whitespace)
    - [Numeric Whitespace](#numeric-whitespace)
    - [Radix Point](#radix-point)
    - [Numeric Bases](#numeric-bases)
    - [Media](#media)
    - [Recursive Data](#recursive-data)
    - [CBE Type Codes](#cbe-type-codes)
      - [Small Integers](#small-integers)
      - [Fixed Size Integers](#fixed-size-integers)
      - [Booleans](#booleans)



Mission Statement
-----------------

Concise Encoding is a secure, ad-hoc data format designed for ease-of-use, human-friendliness, and small encoding size.



Problem Space
-------------

Current data encoding formats are becoming liabilities due to their security problems stemming from:

- **Under-specification**, resulting in incompatible codecs or codecs with subtle behavioral differences that can be exploited.
- **Missing types**, requiring custom secondary codecs (amplifying the compatibility and security problem).
- **Lack of versioning**, so the format can't be updated without breakage when a new threat emerges (and in some cases can't be updated at all).

Threat actors have become much more sophisticated and organized over the past decade, to the point where *security has become the dominant issue* nowadays.

**We need a data format we can trust.**



Concise Encoding's Positioning
------------------------------

Concice Encoding is a general purpose, ad-hoc encoding format that can be added to a project to solve the majority of data storage or transmission issues quickly and efficiently with low friction.

Other formats support advanced features like partial reads, zero-copy structs, random access, finite-time decoding/encoding, etc. And those are great, but I'd consider them specialized features with many trade-offs; they should only be used after careful evaluation of your requirements (which you often won't fully know up front).

Concise Encoding doesn't compete with these because more often than not you won't ever need them. The philosophy here is to keep things simple for as long as possible.

**Reach for CE first, then switch *when* and *if* you actually need it.**



Primary Goals
-------------

### Security

The older formats are no longer secure enough for today's world. Incomplete and vague specifications result in implementation differences that hackers are taking advantage of *today*. As new vulnerabilities are discovered, the existing formats can't be changed to mitigate them.

Any new format **must** be designed to focus on security **first**:

- It must be **tightly specified**.
- It must be **changeable** without breaking existing implementations and documents.
- It must have a **small attack surface**.

**An insecure format has no place in the modern world.**


### Simplicity

For a data format to be adopted, it must be easy to understand (even by non-technical people), and easy to implement. If it's difficult to read and understand, no one will want to use it. If it's too complicated, no one will want to implement it.

Simple is also secure, because simpler software will have less bugs.

**Simple means less bugs and easier maintenance.**


### Ad-Hoc

Defining your data types and structure up front is actually a specialized case. It should be done only AFTER you've proven your model and know exactly what the data you're sending will look like, AND profiling demonstrates that your marshalling code is on the critical path.

Ad-hoc formats tend to be easier to work with, easier to maintain, and are good enough for most purposes.

Schemas tend to happen later as well, because most people aren't interested in creating one until it's absolutely necessary (once their requirements and problem space are fully understood).

**Ad-hoc is often all you need.**


### Efficiency

We're sending too much data nowadays for unnecessary bloat to be acceptable. Transmission size should be as small as possible, but a balance must be reached between size and complexity.

**We can't afford to keep encoding as text.**


### Human Friendly

A format that's difficult for humans to read and write is a format that humans won't want to use.

**If humans can't read it, humans won't use it.**


### Low Friction

Many formats today require a bunch of extra steps before they can be used to transmit data, including:

- Extra code generation or compilation steps.
- Special format "descriptor" files whose syntax one must first learn.
- Missing fundamental data types, requiring hand-rolled secondary codecs and special encoding methods to piggyback data onto them. This is the 21st century! **Base64 should NOT still be a thing**!

These issues add friction that distracts people from their actual business with minutiae they shouldn't even have to be worrying about.

Using the perfect format from your program should ideally be a matter of:

1. Import the library.
2. Call `encode` to write your data structure or `decode` to fill your data structure.
3. The end.

**Easy to use. Easy to debug. Peace of mind.**


Design Choices
--------------

### Twin Formats

The only way to get compactness AND human friendliness is to develop twin binary and text formats that can be transparently converted to each other.

99% of cases will be storing, reading, and sending binary data.

The text format would cover the other 1% of cases (which are still important):

- prototyping
- initial data loads
- debugging
- auditing
- logging
- visualizing
- configuration

**In short**: the binary format is for the majority of cases, and the text format is for any case where a human needs to get involved.


### Opinionated

No optional features or extensions: You can't count on a particular implementation supporting a particular extension or optional feature, so everything ends up regressing to the minimal set, which complicates the format for effectively no gain.

Configuration options (i.e. behavior that is not optional to implement, but is user-configurable) that affect codec behavior on a functional level must have sane, intelligent defaults. Otherwise they become different behaviors in unconfigured codecs, which increases the attack surface of your application.

There should ideally be only one way to do each thing.


### Byte Oriented

Every data format strikes a balance between encoded size and encoding complexity. The bigger the encoded size, the less efficiently it transmits data. The more complex the format, the more CPU power it takes to encode and decode, and the more bugs implementations will have.

CBE tends more towards the simplicity side to:

- Reduce CPU overhead
- Minimize bugs and security issues
- Encourage developers to build implementations

Complex bitfields are kept to a minimum, and all values end on a byte boundary.


### Little Endian

All major architectures today use little endian byte ordering, so it makes sense to transmit data in little endian as well. The war is over, and little endian won.


### Avoiding Turing Completeness

Turing completeness is a desirable aspect for a programming language, but not for a data format.

There is always a temptation to add a little processing ability here and there to cover some of the endless edge cases such as:

* Repetitive data
* Dependent data
* Custom types that can be processed with zero knowledge
* Dynamic data manipulation

We can always find some reason to add extra processing abilities to a data format to make it more powerful, but **this temptation must be resisted**! Once you cross the threshold of Turing completeness (and usually well before), your format becomes a security liability due to the complexity explosion and the [halting problem](https://en.wikipedia.org/wiki/Halting_problem).


### Signature Byte 0x81

All CBE documents start with the signature byte 0x81, followed by the version. Why was 0x81 chosen?


0x81 is either invalid outright as a first byte in all modern text formats, or cannot be followed by a byte < 0x40.

 * In [UTF-8](https://en.wikipedia.org/wiki/UTF-8), 0x80-0xBF are continuation bytes, and therefore are invalid as the first byte of a character.
 * 0x80 (PAD), 0x81 (HOP), and 0x99 (SGI) were never actually included in the [C1 set](https://en.wikipedia.org/wiki/C0_and_C1_control_codes) of [ISO 2022](https://en.wikipedia.org/wiki/ISO/IEC_2022), and are thus invalid for all compliant character sets ([EUC](https://en.wikipedia.org/wiki/Extended_Unix_Code), [ISO 8859](https://en.wikipedia.org/wiki/ISO/IEC_8859))
 * The byte sequences [`80 00`] to [`81 3f`] are invalid in Shift-JIS-like character sets ([Shift JIS](https://en.wikipedia.org/wiki/Shift_JIS), [BIG5](https://en.wikipedia.org/wiki/Big5), [GB18030](https://en.wikipedia.org/wiki/GB_18030)).
 * 0x81, 0x8D, 0x8F, 0x90, and 0x9D are undefined in [Windows codepage 1252](https://en.wikipedia.org/wiki/Windows-1252#Character_set) (most ISO 8859-1 decoders are actually CP-1252 decoders).

Thus, 0x81 is invalid for all major character sets in the next 63 Concise Encoding versions (from 0x01 to 0x3f).

This "invalid" property can be used to indicate the presence of binary data on mediums originally designed for text.

#### Example: Encoding CBE into a QR Code

[QR codes](https://en.wikipedia.org/wiki/QR_code) support multiple different encoding modes, including one misleadingly called "binary" that's actually ISO 8859-1 (so QR codes are basically text-only). If the data were to start with a byte such as 0x81, interpreting it as ISO 8859-1 would result in an invalid character. A smart decoder could take this as an indication that a different format is encoded within, and use the first byte as a selector for what format it actually is (such as 0x81 for CBE).

Such a technique makes it possible to encode complex ad-hoc data structures into QR codes, giving the previously text-only QR codes new superpowers!

For example, consider the following transport and storage data:

```cte
c1 {
    "temperature range" = [-20 5]
    "hazards" = [
        "pressurized"
        "flammable"
        "fragile"
    ]
    "max tilt degrees" = 15
    "perishes after" = 2022-12-05
}
```

This CTE document encodes into a 105 byte CBE document due to the many text fields, but if we used a schema we could replace well-known text keys and values with numeric enumerations:

**Fictional Schema**:

* 0 = schema ID and version adherence: **(fourCC-style integer: "TSS" + version)**
* 1 = temperature range: **(list of two integers)**
* 2 = hazards: **(list of enumerated integers)**:
  - 0 = corrosive
  - 1 = photoreactive
  - ...
  - 4 = pressurized
  - ...
  - 6 = flammable
  - ...
  - 19 = fragile
* ...
* 4 = max tilt degrees: **(integer)**
* ...
* 9 = perishes after: **(date)**

**Document (CTE)**:

```cte
c1 {
    0 = 0x54535301 // Transport and Storage Schema version 1 ("TSS" + 1)
    1 = [-20 5]    // Temperature range from -20 to 5
    2 = [          // Hazards:
        4             // Pressurized
        6             // Flammable
        19            // Fragile
    ]
    4 = 15         // Max 15 degrees tilt
    9 = 2022-12-05 // Perishes after Dec 5, 2022
}
```

Because integers from -100 to 100 encode into a single byte in CBE, you can achieve tremendous space savings using them as map keys. Doing so would reduce the CBE encoded size to 28 bytes, producing the document [81 00 79 00 6c 01 53 53 54 01 7a ec 05 7b 02 7a 04 06 13 7b 04 0f 09 99 85 59 00 7b], which we then encode into the QR code:

    ██████████████      ██      ██  ██  ██████████████
    ██          ██    ██        ████    ██          ██
    ██  ██████  ██  ██    ██    ██      ██  ██████  ██
    ██  ██████  ██    ██      ██  ██    ██  ██████  ██
    ██  ██████  ██    ██    ██      ██  ██  ██████  ██
    ██          ██      ██████    ██    ██          ██
    ██████████████  ██  ██  ██  ██  ██  ██████████████
                    ██        ████                    
    ██████  ██████████    ██  ██  ██  ████      ██    
    ██  ██    ██    ████    ██  ██  ████  ██  ████  ██
          ██  ████████  ██  ██████  ██  ██  ██████████
      ██  ██  ██      ██████  ████            ██  ██  
    ████        ██  ██  ██  ██  ██  ██████████  ██  ██
            ████  ██████████  ██  ██    ██    ██  ██  
    ██  ██████  ██  ██                    ██    ██  ██
      ██████████      ██  ██  ██      ██████          
    ██  ██  ██████      ████    ██████████████      ██
                    ██    ████      ██      ██  ████  
    ██████████████  ██  ██    ██  ████  ██  ██  ██    
    ██          ██  ██  ██████████  ██      ████  ██  
    ██  ██████  ██  ████    ██  ██  ██████████  ████  
    ██  ██████  ██        ██            ██    ██      
    ██  ██████  ██  ██    ██    ██████    ████████  ██
    ██          ██  ████████    ████    ██  ██    ██  
    ██████████████  ██    ██    ██  ██  ██  ██████████

A smart QR code reader could then scan, detect the first byte 0x81 (which is invalid ISO 8859-1), switch to CBE decoding and reverse the whole process.

Full example with working code available [here](https://www.technicalsourcery.net/posts/qr-superpowers/).

### Versioned Documents

Versioned documents are essential to keeping a format clean, secure, and current. In the past, data formats have not done this, and then at some point they've had to change things to deal with new situations or security vulnerabilities that invariably come up over time. In the best case, this leads to "deprecated" types that can't be re-purposed because it would break existing codecs:

| Code | Description                          |
| ---- | ------------------------------------ |
| 1    | Float: 64-bit                        |
| 2    | String: ASCII (deprecated)           |
| 3    | String: ISO 8859-1 (deprecated)      |
| 4    | String: UTF-8                        |
| 5    | Date: ISO 8601 (deprecated)          |
| 6    | Date: RFC3339                        |
| 7    | DB Pointer (deprecated)              |
| 8    | Symbol (deprecated)                  |
| 9    | Embedded CSV (deprecated)            |
| A    | Auto-execute Javascript (deprecated) |

In the worst case, the format is inherently insecure and cannot be fixed (for example JSON).

When documents are versioned to a specification, designers are free to change anything (including the very structure of the document aside from the version specifier), with no danger to legacy systems: A document beginning with [0x81 0x01] is only decodable by a CBE version 1 decoder, and a document beginning with [0x81 0x02] is only decodable by a CBE version 2 decoder.


### Short Array Forms

Arrays normally have a length field following the type field, but for very small arrays this starts to take up a significant amount of overhead. In a 2-byte string, the total overhead (type code + length) is 50% of the complete object, and for a 1-byte string it's a whopping 66%!

For array types that can commonly occur with very short lengths, CBE has "short forms", whereby the length is encoded into the type field iteslf:

| Code | Meaning          |
| ---- | ---------------- |
| 0x80 | Empty string     |
| 0x81 | String length 1  |
| 0x82 | String length 2  |
| ...  | ...              |
| 0x8f | String length 15 |


### Date Formats

ISO 8601 was considered and rejected. The format is too big and complicated, has bad defaults, and is missing important things such as real time zones.

RFC3339 was also considered, but rejected because it's a format designed to store values in the past (which is all that's needed for its use case), and also is missing time zones. It's good at what it was designed for, but not as a general purpose format.

A general purpose time format requires:

- True time zones
- Gregorian time fields
- Human friendly BC dates
- Sane defaults


### Human Readability

Human readability is of paramount importance in the text format. Codec speed does not matter in this case because all critical data encoding and transmission will happen in the more efficient binary format.

Text is for humans only, and so human friendliness is the most important consideration when designing the text format.

The data formats in the text encoding are designed to be as human readable as possible without introducing parser ambiguity. Ideally, even non-technical people can read and edit documents with little difficulty.


### Zero Copy

Zero-copy is a desirable aspect of a data format because it allows the receiving system to access data directly from the received document rather than having to make a separate copy first.

The binary format is designed to support zero-copy as much as is possible in an ad-hoc format:

- Multibyte data types are stored in little endian format (except for UUID, which is big endian as per the spec).
- Standard format and sizing is available for the most common types (integers, binary floats, strings, arrays).
- Strings can be artificially zero-terminated either while encoding (by inserting a [zero chunk](#zero-chunk)), or while decoding (by caching and then overwriting the next object's type code).
- Encoders can insert [padding](#padding) bytes such that multibyte values end up aligned when the document is read into memory by a decoder.


### Array Chunking

Arrays in the binary format are encoded using a chunking mechanism:

    [chunk 1] [data 1] [chunk 2] [data 2] ...

This accommodates situations where you don't yet know what the length of the array data will be.

In the chunking scheme, each chunk has a length field and a "continuation bit" that decides whether more chunks will follow the current one. Once the continuation bit is 0, the current chunk is the last one.

In the most common case, the array is sent in only one chunk containing the entire array length and a continuation bit of 0.

#### Zero Chunk

A zero chunk is a chunk header that specifies a length of 0 and a continuation of 0. The zero chunk is needed for the case where you've been progressively sending array chunks with continuation 1, but have suddenly hit the end of your source data and need to terminate the array somehow.

The encoding of the zero chunk is 0x00 by design due to the usefulness of 0x00 as a string termination value in many languages (and thus useful for setting up string data from the encoder side for [zero copy](#zero-copy))


### Padding

Padding allows an encoder to adjust the alignment of encoded data in the document, which is useful for making documents [zero copy](#zero-copy) friendly.


### LEB128

[LEB128](https://en.wikipedia.org/wiki/LEB128) is used for the following data fields in the binary format:

- [Version specifier](cbe-specification.md#version-specifier)
- [Multibyte integer length field](cbe-specification.md#variable-width)
- [Array chunk header](cbe-specification.md#chunk-header)

While LEB128 is infinitely extensible, it has a constant overhead loss of 1/8, so it's only used in headers with length fields and in data unlikely to exceed a single group. This gives flexibility in the rare cases where it's needed, while keeping CPU and space overhead low on the whole.


### Binary and Decimal Floats

Most of the problems engineers encounter when dealing with floating point values stem from the inexact conversion between base 10 and base 2 exponents. Every time a base 2 float value with a fractional component is expressed as base 10 or vice versa, it loses precision (with a small number of exceptions). To keep this conversion loss to a minimum, floating point values can be stored in CE documents either as decimal floats or as binary floats.

I envision a day when we finally see widespread adoption of IEEE-754 (2008), and binary float falls out of favor, thus closing off this entire class of unnecessary problems.


### Escape Sequences

The following escape sequences are supported:

| Sequence (`\` + ...) | Interpretation              |
| -------------------- | --------------------------- |
| `t`                  | horizontal tab (u+0009)     |
| `n`                  | linefeed (u+000a)           |
| `r`                  | carriage return (u+000d)    |
| `"`                  | double quote (u+0022)       |
| `*`                  | asterisk (u+002a)           |
| `/`                  | slash (u+002f)              |
| `\`                  | backslash (u+005c)          |
| `_`                  | non-breaking space (u+00a0) |
| `-`                  | soft-hyphen (u+00ad)        |
| u+000a               | [continuation](#continuation) |
| u+000d               | [continuation](#continuation) |
| `0` - `9`            | [Unicode sequence](#unicode-escape-sequences) |
| `.`                  | [verbatim sequence](#verbatim-sequence) |

Reasoning:

* `\r`, `\n`, `\t`, `\_`, `\-` : To support common whitespace and utility characters.
* `\"`, `\*`, `\/`, `\\`: To avoid ambiguity with delimiter characters.
* [Continuation](#continuation)
* [Unicode sequence](#unicode-escape-sequences)
* [Verbatim Sequence](#verbatim-sequence)

Other escape sequences present in various formats and languages were evaluated but rejected for being archaic or of limited to no use:

| Sequence (`\` + ...) | Interpretation                  |
| -------------------- | ------------------------------- |
| `a`                  | Alert (Beep, Bell)              |
| `b`                  | Backspace                       |
| `e`                  | Escape character                |
| `f`                  | Formfeed Page Break             |
| `v`                  | Vertical Tab                    |
| `'`                  | Apostrophe                      |
| `?`                  | Question mark                   |
| `000`                | Octal byte representation       |
| `x00`                | Hexadecimal byte representation |
| `N{name}`            | Named Unicode character         |

#### Continuation

Continuations are a convenience for representing long sequences of text that don't have many line breaks. For example:

```cte
    "The only people for me are the mad ones, the ones who are mad to live, \
     mad to talk, mad to be saved, desirous of everything at the same time, \
     the ones who never yawn or say a commonplace thing, but burn, burn, burn \
     like fabulous yellow roman candles exploding like spiders across the stars."
```

The above string is interpreted as a single line:

    The only people for me are the mad ones, the ones who are mad to live, mad to talk, mad to be saved, desirous of everything at the same time, the ones who never yawn or say a commonplace thing, but burn, burn, burn like fabulous yellow roman candles exploding like spiders across the stars.

#### Unicode Escape Sequences

Unicode escaping is a mess. We currently have two clumsy escape methods to represent unicode codepoints:

- `\uxxxx` to represent the first 65536 codepoints using a sequence of 4 hexadecimal digits.
- `\Uxxxxxxxx` to represent all possible codepoints using a sequence of 8 hexadecimal digits.

This is just silly. Not only does it complicate things and encourage user error (i.e. accidentally using the wrong case), it also bloats strings unnecessarily (Unicode codepoints only go to 1fffff, so you automatically waste two or more bytes every time you want to encode a codepoint > 65536).

Since Concise Encoding dosn't suffer from the C legacy of by-now-extinct [octal escape sequences](https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences), it can use a better Unicode escaping mechanism.

Unicode codepoint escape sequences consist of the following:

- The `\` character
- The `+` character
- Any number of hexadecimal digits, representing the hex value of the codepoint.
- The `.` character, to terminate the sequence.

Examples:

- `\+0.` = NUL (0x00)
- `\+c.` = FF (0x0c)
- `\+7f.` = DEL (0x7f)
- `\+101.` = ā (0x101)
- `\+2191.`  = ↑ (0x2191)
- `\+1f415.` = 🐕 (0x1f415)

Advantages:

- One encoding method for all codepoints.
- It can be adopted into other languages (no other language currently defines `\+` as a string escape sequence).
- You only need to encode as many digits as are actually needed to represent the codepoint (no need for zero padding).
- It can handle all existing and future Unicode codepoints (i.e. we'll never have to change this escape scheme, ever).

#### Verbatim Sequence

The only way to avoid byte stuffing (escaping) in a text format and get a true verbatim chunk of string data is to adopt a [here document](https://en.wikipedia.org/wiki/Here_document) style escape sequence.

In the text format, [verbatim sequences](cte-specification.md#verbatim-sequence) are designed with human readers in mind, and so shorter sentinel sequences are preferred (unlike in HTTP). Usually 1-3 characters is enough (e.g. `"\.## some text##"`).

Whitespace was chosen as the initial sentinel terminator because it can never itself be a sentinel character, and also because in some circumstances it's more convenient to have the verbatim sequence on the same line, while in others it's preferable to start on a new line:

```cte
c1 {
    "script" = "\.@@@
#!/usr/bin/env sh
set -evx

. ci/get-nprocessors.sh

if [ -x /usr/bin/getconf ]; then
    NPROCESSORS=$(/usr/bin/getconf _NPROCESSORS_ONLN)
else
    NPROCESSORS=2
fi

mkdir build || true
cd build
cmake -Dgtest_build_samples=ON \
      -Dgtest_build_tests=ON \
      -Dgmock_build_tests=ON \
      -Dcxx_no_exception=$NO_EXCEPTION \
      -Dcxx_no_rtti=$NO_RTTI \
      -DCMAKE_COMPILER_IS_GNUCXX=$COMPILER_IS_GNUCXX \
      -DCMAKE_CXX_FLAGS=$CXX_FLAGS \
      -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
      ..
make
CTEST_OUTPUT_ON_FAILURE=1 make test
@@@"
}
```

In the above example, using a newline terminator is preferable because:

* The verbatim sequence looks and reads just like the original shell script.
* It's easier to paste in an updated script.


### NUL support

NUL is a troublesome character in many languages, but it's still a valid codepoint and must be supported somehow. On the other hand, it's an exploitable security hole in systems that are unprepared for it.

As a compromise, decoders convert NUL to [`c0 80`] by default in order to sidestep the NUL termination problem.


### Whitespace

Only space, tab, carriage return, and linefeed are accepted as structural whitespace in the text format. This is to ensure that only characters that can be easily input on all keyboards are allowed.

Line endings can be LF or CRLF to accommodate the two major encoding methods in use today: Microsoft (CRLF), and everyone else (LF).


### Numeric Whitespace

Underscore (`_`) is used for numeric whitespace, as is done in most programming languages. Characters such as `,` and `.` are unsuitable because many countries use them for opposite purposes (as a radix point / as a grouping separator).


### Radix Point

The text format accepts both `.` and `,` as a radix point in floating point numbers to match how they're used in various countries around the world.


### Numeric Bases

To support the most common numeric representations, the text format supports the following bases:

| Example   | Base        | Number Type      |
| --------- | ----------- | ---------------- |
| `0x10110` | Binary      | Integer          |
| `0o755`   | Octal       | Integer          |
| `12345`   | Decimal     | Integer or float |
| `0x4f9a`  | Hexadecimal | Integer or float |


### Media

Embedded media is such a common occurrence that it makes sense to include it in a modern format. Media types are [clearly defined by IANA](https://www.iana.org/assignments/media-types/media-types.xhtml), and the infrastructure for handling embedded media already exists. Media simply consists of a media type (so that receivers know how to interpret the data), and the media payload itself.

**Note**: Media metadata does *not* include a "file name", because such information is only needed to store the payload separately on a filesystem. A file can contain media, but the media itself is not a file.


### Recursive Data

Recursive data structures are very useful and powerful, yet potentially very dangerous. Concise Encoding opts to support recursive structures, with the caveat that they are opt-in (to avoid inadvertently opening a security hole).


### CBE Type Codes

#### Small Integers

Integer is one of the most commonly used data types, and small values are much more common than large values in the wild (with the majority falling between -100 and 100).

Small integer types in CBE are encoded as the type codes themselves, such that one can simply cast the type code to a signed 8-bit integer. The other types take the remaining type code space 101 to 127 and -101 to -128. This keeps the encoding size down for the most commonly encountered values.

#### Fixed Size Integers

The type codes for the integer types were chosen such that the lowest bit of the type code is always 0 for positive values and 1 for negative values, unofficially acting like a sign bit.

#### Booleans

The boolean type codes were chosen such that the low bit of the type code is 0 for false and 1 for true.
