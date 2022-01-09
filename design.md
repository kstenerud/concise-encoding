Design Document
===============

    Hindsight is easier than foresight.



Mission Statement
-----------------

Concise Encoding is a secure, ad-hoc data format designed for ease-of-use, human-friendliness, and small encoding size.


Primary Goals
-------------

### Secure

The older formats are no longer secure enough for today's world. Incomplete and vague specifications result in implementation differences that hackers are taking advantage of TODAY. As new vulnerabilities are discovered, the existing formats can't be changed to mitigate them.

Any new format MUST be designed to focus on security FIRST:
- It must be tightly specified.
- It must be changeable without breaking existing implementations and documents.
- It must avoid optional features in order to keep the attack surface small.

### Simple

For a data format to be adopted, it must be easy to understand (even by non-technical people), and easy to implement. If it's difficult to use, no one will want to use it, and if it's too complicated, no one will want to implement it.

Simple is also secure, because simpler software will have less bugs.

### Ad-Hoc

Defining your data types and structure up front is actually a specialized case. It should be done only AFTER you've proven your model and know exactly what the data you're sending will look like, AND profiling demonstrates that your marshalling code is on the critical path. Until then, ad-hoc formats are MUCH easier to work with, easier to maintain, and are good enough for most purposes. Schemas should happen later as well, because most people aren't interested in creating one until it's absolutely necessary.

### Compact

We're sending too much data nowadays for unnecessary bloat to be acceptable.

### Human Friendly

A format that's difficult for humans to read and write is a format that humans don't like to use.

### Low Friction

Many formats today require a bunch of extra steps before they can be used to transmit data, including:
- Extra code generation or compilation steps.
- Special format "descriptor" files whose syntax one must first learn.
- Missing fundamental data types, requiring hand-rolled secondary codecs and special encoding methods to piggyback data onto them. We're well into the 21st century; **base64 should NOT be a thing**!

These issues add friction that distracts people from their actual business with minutiae they don't even want to be worrying about.

Using the perfect format from your program should be a matter of:

1. Import the library.
2. Call `encode` to write your data structure or `decode` to fill your data structure.
3. The end.


Design Choices
--------------

### Twin Formats

The only way to get compactness AND human friendliness is to develop twin binary and text formats that can be converted to each other.

99% of cases will be storing, reading, and sending binary data.

The text format would cover the other 1% of cases (which are still important):
- prototyping
- initial data loads
- debugging
- auditing
- logging
- visualizing
- configuration

Basically, the binary format is for the majority of cases, and the text format is for any case where a human needs to get involved.

### Opinionated

No optional behaviors or extensions.

You can't count on a particular implementation supporting a particular extension, so everything ends up regressing to the minimal set.

Optional behaviors (that must be supported by all codecs) complicate the format, and generally don't add much value in return.

There should be one way to do each thing.

### Byte Oriented

Every data format strikes a balance between encoded size and encoding complexity. The bigger the encoded size, the less efficiently it transmits data. The more complex the format, the more CPU power it takes to encode and decode, and the more bugs implementations will have.

CBE tends more towards the simplicity side to:
- Reduce CPU overhead
- Minimize bugs and security issues
- Encourage developers to build implementations

Complex bitfields are kept to a minimum, and all values end on a byte boundary.

### Avoid Turing Completeness

Turing completeness is a desirable aspect of a programming language, but not for a data format.

The temptation is always there to add a little processing ability here and there to cover some of the endless edge cases:

* Repetitive data
* Dependent data
* Custom types that can be processed with zero knowledge
* Dynamic data manipulation

There's always some reason to add extra processing abilities to a data format, but **this temptation must be resisted**! Once you cross the threshold of Turing completeness (and often even before), your format becomes a security liability.

### Signature Byte 0x83

All CBE documents start with the signature byte 0x83, followed by the version. Why was 0x83 chosen?

The low nybble `3` was chosen to match the low-nybble of the signature character `c` (hex code `0x63`) in CTE documents.

The high nybble `8` was chosen for its useful property:

Most text formats leave 0x80-0x9f unassigned as first bytes (notably ISO 8859 and UTF-8), and so a document that begins with such a byte is never valid text. This property can be exploited to encode binary data onto mediums designed for text formats.

#### Example: Encoding CBE into a QR Code

[QR codes](https://en.wikipedia.org/wiki/QR_code) support multiple different encoding modes, including one misleadingly called "binary" that's actually ISO 8859-1 (so QR codes are basically text-only). If the data were to start with a byte such as 0x83, interpreting it as ISO 8859-1 would break. A smart decoder could take this as an indication that a different format is encoded within, and use the first byte as a selector for what format it actually is (such as 0x83 for CBE).

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

This CTE document encodes into a 117 byte CBE document due to the many text fields, but if we used a schema we could replace well-known text keys and values with numeric enumerations:

Schema:
* 0 = schema version adherence: (integer)
* 1 = temperature range: (list of two integers)
* 2 = hazards: (list of enumerated integers):
  - 0 = corrosive
  - 1 = photoreactive
  - ...
  - 4 = pressurized
  - ...
  - 6 = flammable
  - ...
  - 19 = fragile
* ...
* 4 = max tilt degrees: (integer)
* ...
* 9 = perishes after: (date)

Document (CTE):
```cte
c1 {
    0 = 0x54535301 // Transport and Storage Schema (TSS) version 1
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
Because integers from -100 to 100 encode into a single byte in CBE, you can achieve tremendous space savings using them as map keys. Doing so would reduce the CBE encoded size to 24 bytes, producing the document [83 01 79 00 01 01 7a ec 05 7b 02 7a 04 06 13 7b 04 0f 09 99 85 59 00 7b], which we then encode into the QR code:

```
██████████████      ██    ████  ██  ██████████████
██          ██    ██        ████    ██          ██
██  ██████  ██  ██    ██            ██  ██████  ██
██  ██████  ██    ██    ████  ██    ██  ██████  ██
██  ██████  ██    ██    ██████  ██  ██  ██████  ██
██          ██      ████████  ████  ██          ██
██████████████  ██  ██  ██  ██  ██  ██████████████
                ██    ██    ██                    
██████  ██████████      ██  ████  ████      ██    
████████      ██  ██  ████  ██  ██  ██    ████  ██
██  ██      ██      ██████  ██        ████████████
    ████  ██  ████████  ██  ██      ██    ██      
██████  ████████    ████    ██      ██  ██  ██  ██
  ████    ██  ██    ████  ██  ██  ██  ██  ██  ██  
██  ████  ██████    ██          ██  ██  ██  ██  ██
  ██████      ██    ████  ████  ██████  ████    ██
██  ████████████    ██  ██      ██████████  ██  ██
                ████  ████  ██████      ████  ██  
██████████████  ██  ██████  ██  ██  ██  ██████    
██          ██  ██████  ██████████      ████  ██  
██  ██████  ██  ██  ██████  ██  ██████████  ██    
██  ██████  ██    ██  ██  ██  ██      ██████      
██  ██████  ██  ██  ██      ████████  ██  ████  ██
██          ██  ████      ██  ██  ████████    ██  
██████████████  ████████  ██    ██        ██  ████
```

A smart QR code reader could then scan and reverse the process.

Versioned Documents
-------------------

Versioned documents are essential to keeping a format clean and current. In the past, data formats have not done this, and then at some point they've had to change things to deal with new situations or security vulnerabilities that invariably come up over time. In the best case, this leads to "deprecated" types that can't be re-purposed because it would break existing codecs:

| Code | Description                          |
| ---- | ------------------------------------ |
| 1    | Float: 64-bit                        |
| 2    | String: Ascii (deprecated)           |
| 3    | String: UTF-8                        |
| 4    | DB Pointer (deprecated)              |
| 5    | Symbol (deprecated)                  |
| 6    | Embedded CSV (deprecated)            |
| 7    | Auto-execute Javascript (deprecated) |

In the worst case, the format is inherently insecure and cannot be fixed (for example JSON).

When documents are versioned to a specification, designers are free to change anything (including the very structure of the document aside from the version specifier), with no danger to legacy systems: A document beginning with [0x83 0x01] is only decodable by a CBE version 1 decoder, and a document beginning with [0x83 0x02] is only decodable by a CBE version 2 decoder.


Small Integers
--------------

Integer is one of the most commonly used data types, and small values are much more common than large values in the wild (with the majority falling between -100 and 100).

Small integer types in CBE are encoded as their actual two's complement encodings, with the rest of the encoding types taking the unused space 101 to 127 and -101 to -128. This keeps the encoding size down and also simplifies codec implementation.


Short Array Forms
-----------------

Arrays normally have a length field following the type field, but for very small arrays this starts to take up a significant amount of overhead. In a 2-byte string, the total overhead (type code + length) is 50% of the complete object, and for a 1-byte string it's 66%!

For array types that can commonly occur with very short lengths, CBE has "short forms", whereby the length is encoded into the type field iteslf:

| Code | Meaning          |
| ---- | ---------------- |
| 0x80 | Empty string     |
| 0x81 | String length 1  |
| 0x82 | String length 2  |
| ...  | ...              |
| 0x8f | String length 15 |

### Date Formats

ISO 8601 was considered and rejected. The format is too big and complicated, and is missing important things such as real time zones.

RFC3339 was also considered, but rejected because it's a format designed to store values in the past (which is all that's needed for its use case), and also is missing time zones.

A general purpose time format requires:
- True time zones
- Gregorian time fields
- Human friendly BC dates
- Sane defaults

--------------------------------------------------------------

TODO:
- Where this format fits (general purpose)
- Human readability
- some zero-copy support
- little endian
- LEB128
- why decimal floats
- verbatim sequences
- escaped unicode
- NUL
- line endings
- radix char
- media
- array chunking
- zero chunk
- artificial zero termination
- padding
- nbsp and shy
- continuation
- number bases
- numeric whitespace
- recursive data



- Simple to understand and use, even by non-technical people (the text format, I mean).
- Low friction: no extra compilation / code generation steps or special tools or descriptor files needed.
- Ad-hoc: no requirement to fully define your data types up front. Schema or schemaless is your choice (people often avoid schemas until they become absolutely necessary).

Other formats support features like partial reads, zero-copy structs, random access, finite-time decoding/encoding, etc. And those are awesome, but I'd consider them specialized applications with trade-offs that only an experienced person can evaluate (and absolutely SHOULD evaluate).

CE is more of a general purpose tool that can be added to a project to solve the majority of data storage or transmission issues quickly and efficiently with low friction, and then possibly swapped out for a more specialized tool later if the need arises. "First, reach for CE. Then, reach for XYZ once you actually need it."

existing solutions are security holes due to under-specification (causing codec behavior variance), missing types (requiring custom secondary - and usually buggy - codecs), and lack of versioning (so the formats can't be updated). And security is fast becoming the dominant issue nowadays. 
