[Concise Encoding](https://concise-encoding.org/)
=================================================

The friendly data format for human and machine. Ad-hoc, secure, with 1:1 compatible twin binary and text formats and rich type support.

 * **Edit text, transmit binary.** Humans love text. Machines love binary. With Concise Encoding, conversion is 1:1 and seamless.
 * **Secure By Design.** Precise decoding behavior means no poisoned data, no privilege escalations, and no DOSing!
 * **Rich type support.** Boolean, integer, float, string, bytes, time, URI, UUID, list, map, markup, and more!


Contents
--------

* [Introduction](#introduction)
* [Motivation](#motivation)
* [Specifications](#specifications)
* [Implementations](#implementations)
* [Remaining Tasks](#remaining-tasks)
* [Comparison to Other Formats](#comparison-to-other-formats)
* [Examples](#examples)
* [License](#license)



Introduction
------------

Today's data formats present us with a dilemma: Use text based formats that are bloated and slow, or use binary formats that humans can't read. Wouldn't it be nice to have the benefits of both and the drawbacks of neither?

**Concise Encoding** is the next step in the evolution of ad-hoc hierarchical data formats, aiming to represent data securely in a power, bandwidth, and human friendly way.



Motivation
----------

### We solved the datacomm problems of the 90s

The text-based formats that became popular in the mid-to-late 90s were a breath of fresh air after the expensive, proprietary, complicated, difficult to use binary formats of the time. Text formats and protocols such as HTML, HTTP, XML, and JSON simplified data communications greatly, and contributed to the flourishing of the web ecosystem.

### But at what cost?

Unfortunately, the simplicity of text came at a cost. Text encodings are bulky and CPU-intensive compared to binary encodings, usually taking 10x longer to parse or produce. Text parsing is also inherently complex, and that complexity translates into more bugs in your data codec (including security issues).

These formats' lack of types gave rise to a new set of problems: When everyone has to come up with their own methods to shoehorn common data types into the restricted types of these formats, interoperability is seriously impaired, mistakes are common, and security risks ever-present.

These formats also played fast and loose in their specifications, resulting in much variance in implementation behaviors. This makes them inherently insecure, and although that wasn't such a problem in the first decade of the 2000s, the hostile world of today is full of highly motivated mercenary, ransomware, and state actors who will go to extreme lengths to [compromise your system](ce-structure.md#security-and-limits). Can you afford the risk?

Lack of versioning is also a problem. Without a version specifier, changes to the specification range from difficult to impossible, leaving the format riddled with deprecation holes, and incapable of adapting to industry changes or new security threats.

### Solving the problems of today

The older formats can't be fixed or updated to deal with today's issues. We **need** a format designed for our modern world:

 * **Secure**: As a tightly specified format, Concise Encoding doesn't suffer from the [security problems](ce-structure.md#security-and-limits) that the more loosely defined formats do. Everything is done **one** way only, leaving less of an attack surface.
 * **Efficient**: As a twin binary/text format, Concise Encoding retains the [text-based](cte-specification.md) ease-of-use of the old text formats, but is stored and transmitted in the simpler and smaller [binary form](cbe-specification.md), making it more secure, easier on the energy bill, and easier on the planet.
 * **Versatile**: Supports [all common types](#natively-supported-types) natively. 90% of users won't need any form of customisation.
 * **Future-proof**: As a versioned format, Concise Encoding can respond to a changing world without degenerating into deprecations and awkward encodings or painting itself into a corner.



Specifications
--------------

 * [Concise Encoding Structure](ce-structure.md)
 * [Concise Text Encoding (CTE)](cte-specification.md)
 * [Concise Binary Encoding (CBE)](cbe-specification.md)
 * [Design Document](design.md)



Implementations
---------------

 * [Go](https://github.com/kstenerud/go-concise-encoding) (reference implementation)
 * [Enctool](https://github.com/kstenerud/enctool) A tool for playing with and converting between formats



Remaining Tasks
---------------

 * [Open comments](https://github.com/kstenerud/concise-encoding/issues) on the format itself (ongoing)
 * Finish the [reference implementation](https://github.com/kstenerud/go-concise-encoding) (95%)
 * ABNF/EBNF for the [CTE format](cte-specification.md)
 * [Schema design](ces-specification.md)
 * Schema reference implementation
 * Release version 1



Comparison to Other Formats
---------------------------

Concise encoding is an ad-hoc format, so it shares more in common with XML, JSON & CBOR than it does with Protobufs, Flatbuffers & Cap'n proto. Ad-hoc data doesn't require a schema, but as a result it can't always be as compact as the schema-based encodings because it must record type data. Then again, ad-hoc data allows more compression opportunities than schema-based encodings can, which Concise encoding takes advantage of.

#### Natively supported types

| Type | Concise | [XML](https://www.w3.org/XML) | [JSON](https://www.json.org) | [BSON](http://bsonspec.org) | [CBOR](https://cbor.io) | [Messagepack](https://msgpack.org) | [Cap'n](https://capnproto.org) | [Protobufs](https://developers.google.com/protocol-buffers) | [Flatbuffers](https://google.github.io/flatbuffers) | [Thrift](https://thrift.apache.org) | [ASN.1](https://www.itu.int/en/ITU-T/asn1/Pages/introduction.aspx) | [Ion](https://amzn.github.io/ion-docs) |
| ------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- | --- |
| Boolean       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| Integer       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| Binary Float  |    Y    |     |      |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| Decimal Float |    Y    |     |  Y   |  Y   |  Y   |             |       |           |             |        |       |  Y  |
| NaN, Infinity |    Y    |     |      |      |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| UUID          |    Y    |     |      |  Y   |  Y   |             |       |           |             |        |   Y   |     |
| Timestamp     |    Y    |     |      |  Y   |  Y   |      Y      |       |     Y     |             |        |   Y   |  Y  |
| Resource ID   |    Y    |  Y  |      |      |      |             |       |           |             |        |       |     |
| String        |    Y    |  Y  |  Y   |  Y   |  Y   |      Y      |       |     Y     |      Y      |   Y    |   Y   |  Y  |
| Bytes         |    Y    |     |      |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| List          |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |  Y  |
| Map           |    Y    |     |  Y   |  Y   |  Y   |      Y      |       |     Y     |      Y      |   Y    |       |     |
| Edge          |    Y    |     |      |      |      |             |       |           |             |        |       |     |
| Node          |    Y    |     |      |      |      |             |       |           |             |        |       |     |
| Markup        |    Y    |  Y  |      |      |      |             |       |           |             |        |       |     |
| Typed Arrays  |    Y    |     |      |      |  Y   |             |   Y   |     Y     |      Y      |   Y    |   Y   |     |
| Reference     |    Y    |     |      |      |  Y   |             |       |           |             |        |       |     |
| Remote Ref    |    Y    |     |      |      |      |             |       |           |             |        |       |     |
| Comment       |    Y    |  Y  |      |      |      |             |       |           |             |        |       |     |
| Constant      |    Y    |     |      |      |      |             |       |           |             |        |       |     |
| Null          |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |             |        |   Y   |  Y  |
| Media         |    Y    |     |      |      |      |             |       |           |             |        |       |     |
| Custom        |    Y    |     |      |      |      |             |       |           |             |        |   Y   |  Y  |

#### Features

| Type | Concise | [XML](https://www.w3.org/XML) | [JSON](https://www.json.org) | [BSON](http://bsonspec.org) | [CBOR](https://cbor.io) | [Messagepack](https://msgpack.org) | [Cap'n](https://capnproto.org) | [Protobufs](https://developers.google.com/protocol-buffers) | [Flatbuffers](https://google.github.io/flatbuffers) | [Thrift](https://thrift.apache.org) | [ASN.1](https://www.itu.int/en/ITU-T/asn1/Pages/introduction.aspx) | [Ion](https://amzn.github.io/ion-docs) |
| ----------------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- | --- |
| Int Max Size (bits)     |   inf   |     | inf  |  64  |  64  |     64      |   64  |    64     |     64      |   64   |  64   | inf |
| Float Max Size (bits)   |   inf   |     | inf  | 128  |  64  |     64      |   64  |    64     |     64      |   64   |  64   | inf |
| Subsecond Precision     |   ns    |     |      |  ns  |   *  |     ns      |       |    ns     |             |        |  ns   |  ns |
| Endianness              |    L    |     |      |   L  |   B  |      B      |    L  |     L     |      L      |    B   |   B   |  B  |
| Ad-hoc                  |    Y    |  Y  |   Y  |   Y  |   Y  |      Y      |       |           |      Y      |        |       |  Y  |
| Non-string map keys     |    Y    |     |      |   Y  |   Y  |      Y      |       |     Y     |             |        |       |     |
| Size Optimization       |    Y    |     |      |      |   Y  |      Y      |    Y  |           |             |        |   Y   |     |
| Cyclic References       |    Y    |     |      |      |   Y  |             |       |           |             |        |       |     |
| Time Zones              |    Y    |     |      |      |   Y  |             |       |           |             |        |       |     |
| Gregorian Time Fields   |    Y    |     |      |      |   Y  |             |       |           |             |        |       |  Y  |
| 1:1 Bin/Txt Compatible  |    Y    |     |      |      |      |             |       |           |             |        |       |  Y  |
| Versioning              |    Y    |     |      |      |      |             |       |           |             |        |       |  *  |

* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian, and so little endian formats can be more efficiently encoded/decoded.
* **Ad-hoc**: Supports ad-hoc data (does not require a schema).
* **Size Optimization**: Encoding is designed such that the more common types & values use less space.
* **Cyclic References**: Cyclic (recursive) data structures can be represented using references.
* **Time Zones**: Timestamps support time zones.
* **Gregorian Time Fields**: Time values use Gregorian fields rather than monotonic types like seconds.
* **1:1 Bin/Txt Compatible**: All types in the binary format match 1:1 to the same type in the text format.
* **Versioning**: Documents are versioned to match a specification version. Ion supports versioning in the binary format only.


### Amazon Ion

Amazon's [Ion](https://amzn.github.io/ion-docs) is the only other format that has twin text/binary variants. Unfortunately, it repeats a number of mistakes from the past:

* The text format doesn't have a version specifier, so any future changes to the text format will break existing codecs and documents.
* Uses ISO 8601, which has bad defaults (local timezone) and doesn't support real time zones (only offsets). ISO 8601 is also too big for what it does and non-free, resulting in inconsistent implementations (and thus security problems).
* Doesn't support chunking (so you must always know the full length before you start encoding).
* Lists must be prefixed with their length, so you must know the length before you start encoding. Converting a list from the text format to binary format would require loading the entire list into memory first.
* Only supports "struct" encoding, so you can't have real maps (for example using integers or dates as keys).
* Doesn't support arrays.
* Doesn't support recursive data or references.
* Doesn't have a URL or UUID type.
* Integer, date, and decimal float encodings are inefficient.
* Allows multiple sections, each with their own version declaration as a feature for mixing specification versions in the same document. This means that ALL codecs must support ALL versions forever, increasing bloat and attack surface.



Examples
--------

All examples are valid [Concise Text Encoding](cte-specification.md) documents that can be transparently 1:1 converted to/from [Concise Binary Encoding](cbe-specification.md)

#### Numeric Types

```cte
c1
{
    "boolean"       = true
    "binary int"    = -0b10001011
    "octal int"     = 0o644
    "decimal int"   = -10000000
    "hex int"       = 0xfffe0001
    "very long int" = 100000000000000000000000000000000000009
    "decimal float" = -14.125
    "hex float"     = 0x5.1ec4p+20
    "very long flt" = 4.957234990634579394723460546348e+100000
    "not-a-number"  = nan
    "infinity"      = inf
    "neg infinity"  = -inf
}
```

#### String and String-Like

```cte
c1
{
    "string" = "Strings support escape sequences: \n \t \51F415"
    "url"    = @"https://example.com/"
    "email"  = @"mailto:me@somewhere.com"
}
```

#### Other Basic Types

```cte
c1
{
    "uuid"      = f1ce4567-e89b-12d3-a456-426655440000
    "date"      = 2019-07-01
    "time"      = 18:04:00.948/Europe/Prague
    "timestamp" = 2010-07-15/13:28:15.415942344
    "null"      = null
    "media"     = |application/x-sh 23 21 2f 62 69 6e 2f 73 68 0a 0a
                  65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a|
}
```

#### Containers

```cte
c1
{
    "list"          = [1 2.5 "a string"]
    "map"           = {"one"=1 2="two" "today"=2020-09-10}
    "bytes"         = |u8x 01 ff de ad be ef|
    "int16 array"   = |i16 7374 17466 -9957|
    "uint16 hex"    = |u16x 91fe 443a 9c15|
    "float32 array" = |f32 1.5e10 -8.31e-12|
}
```

#### Markup

```cte
c1
{
    "main-view" = <View,
        <Image "src"=@"img/avatar-image.jpg">
        <Text "id"="HelloText",
            Hello! Please choose a name!
        >
        // OnChange contains code which might have problematic characters.
        // Use verbatim sequences (\.IDENTIFIER ... IDENTIFIER) to handle this.
        <TextInput "id"="NameInput" "style"={"height"=40 "color"="gray"} "OnChange"="\.@@
            NameInput.Parent.InsertRawAfter(NameInput, '<Image "src"=@"img/check.svg">')
            HelloText.SetText("Hello, " + NameInput.Text + "!")
            @@",
            Name me!
        >
    >
}
```

#### References

```cte
c1
{
    // Entire map will be referenced later as $id1
    "marked object" = &id1:{
        "recursive" = $id1
    }
    "ref1" = $id1
    "ref2" = $id1

    // Reference pointing to part of another document.
    "outside ref" = $"https://xyz.com/document.cte#some_id"
}
```

#### Graphs

```cte
c1
//
// The weighted graph:
//
//     b
//    /|\
//   4 1 1
//  /  |  \
// a-3-c-4-d
//
{
    "vertices" = [
        &a:{}
        &b:{}
        &c:{}
        &d:{}
    ]
    "edges" = [
        @($a {"weight"=4 "direction"="both"} $b)
        @($a {"weight"=3 "direction"="both"} $c)
        @($b {"weight"=1 "direction"="both"} $c)
        @($b {"weight"=1 "direction"="both"} $d)
        @($c {"weight"=4 "direction"="both"} $d)
    ]
}
```

#### Trees

```cte
//
// The tree:
//
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

#### Constants

```cte
c1
{
    /* Given: Actual type and value of "eggshell" and
     *        "navy-blue" have been defined in a schema
     */
    "wall color" = #eggshell
    "door color" = #navy-blue
}
```

#### Custom Types

```cte
c1
{
    // Custom types are user-defined, with user-supplied codecs.
    "custom text"   = |ct cplx(2.94+3i)|
    "custom binary" = |cb 04 f6 28 3c 40 00 00 40 40|
}
```



License
-------

Copyright 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
