[Concise Encoding](https://concise-encoding.org/)
=================================================

The friendly data format for human and machine. Think JSON, but with 1:1 compatible twin binary and text formats and rich type support.

 * **Edit text, transmit binary.** Humans love text. Machines love binary. With Concise Encoding, conversion is 1:1 and seamless.
 * **Rich type support.** Boolean, integer, float, string, bytes, time, URI, UUID, list, map, markup, metadata, and more!
 * **Plug and play.** No schema needed. No special syntax files. No code generation. Just import and go.


Contents
--------

* [Introduction](#introduction)
* [Specifications](#specifications)
* [Implementations](#implementations)
* [Remaining Tasks](#remaining-tasks)
* [Comparison to Other Formats](#comparison-to-other-formats)
* [Examples](#examples)
* [License](#license)



Introduction
------------

Today's data formats present us with a dilemma: Use text based formats that are bloated and slow, or use binary formats that humans can't read. Wouldn't it be nice to have the benefits of both, and none of the drawbacks?

**Concise Encoding** is the next step in the evolution of ad-hoc hierarchical data formats, aiming to represent data in a power, bandwidth, and human friendly way.

#### Supported Types

| Type                                   | Description                                             |
| -------------------------------------- | ------------------------------------------------------- |
| [Boolean](#numeric-types)              | True or false                                           |
| [Integer](#numeric-types)              | Positive or negative, arbitrary size                    |
| [Float](#numeric-types)                | Binary or decimal floating point, arbitrary size        |
| [UUID](#other-basic-types)             | [RFC-4122 UUID](https://tools.ietf.org/html/rfc4122)    |
| [Time](#other-basic-types)             | Date, time, or timestamp, arbitrary size                |
| [Resource ID](#string-and-string-like) | URL, URI, IRI, etc                                      |
| [String](#string-and-string-like)      | UTF-8 string, arbitrary length                          |
| [Typed Array](#containers)             | Array of fixed-width type, arbitrary length             |
| [List](#containers)                    | List of objects                                         |
| [Map](#containers)                     | Mapping keyable objects to other objects                |
| [Markup](#containers)                  | Presentation data, similar to XML                       |
| [Relationship](#relationships)         | Semantic relationship data compatible with [RDF](https://www.w3.org/2001/sw/wiki/RDF) |
| [Reference](#references)               | Points to objects defined elsewhere                     |
| [Metadata](#metadata)                  | Data about data                                         |
| [Comment](#relationships)              | Arbitrary comments about anything, nesting supported    |
| [Custom](#custom-types)                | User-defined data type                                  |
| [NA](#other-basic-types)               | Not Available (data missing)                            |



Specifications
--------------

 * [Concise Encoding Structure](ce-structure.md)
 * [Concise Text Encoding (CTE)](cte-specification.md)
 * [Concise Binary Encoding (CBE)](cbe-specification.md)
 * [Common Generic Metadata](common-generic-metadata.md)
 * [Design Document](design.md)



Implementations
---------------

 * [Go](https://github.com/kstenerud/go-concise-encoding) (reference implementation)



Remaining Tasks
---------------

 * [Open comments](https://github.com/kstenerud/concise-encoding/issues) on the format itself (ongoing)
 * Finish the [reference implementation](https://github.com/kstenerud/go-concise-encoding) (90%)
 * ABNF/EBNF for the [CTE format](cte-specification.md) (0%)
 * [Schema design](ces-specification.md) (2%)
 * Schema reference implementation (0%)
 * Release version 1



Comparison to Other Formats
---------------------------

Concise encoding is an ad-hoc format, so it shares more in common with XML, JSON & CBOR than it does with Protobufs, Flatbuffers & Cap'n proto. Ad-hoc data doesn't require a schema, but as a result it can't always be as compact as the schema-based encodings because it must record type data. Then again, ad-hoc data allows more compression opportunities than schema-based encodings can, which Concise encoding takes advantage of.

#### Natively supported types

| Type          | Concise | XML | JSON | BSON | CBOR | Messagepack | Cap'n | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- |
| Null          |    Y*   |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |             |        |   Y   |
| Integer       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Boolean       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Binary Float  |    Y    |     |      |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Decimal Float |    Y    |     |  Y   |  Y   |  Y   |             |       |           |             |        |       |
| UUID          |    Y    |     |      |  Y   |  Y   |             |       |           |             |        |   Y   |
| Timestamp     |    Y    |     |      |  Y   |  Y   |      Y      |       |     Y     |             |        |   Y   |
| Bytes         |    Y    |     |      |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| String        |    Y    |  Y  |  Y   |  Y   |  Y   |      Y      |       |     Y     |      Y      |   Y    |   Y   |
| List          |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Map           |    Y    |     |  Y   |  Y   |  Y   |      Y      |       |     Y     |      Y      |   Y    |       |
| Relationship  |    Y    |  Y  |  Y*  |      |      |             |       |           |             |        |       |
| Reference     |    Y    |     |      |      |  Y   |             |       |           |             |        |       |
| Markup        |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| Comment       |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| Resource ID   |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| Metadata      |    Y    |     |      |      |      |             |       |           |             |        |       |
| Custom        |    Y    |     |      |      |      |             |       |           |             |        |   Y   |

#### Features

| Type                    | Concise | XML | JSON | BSON | CBOR | Messagepack | Cap'n | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ----------------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- |
| Int Max Size (bits)     |   inf   |     | inf  |  64  |  64  |     64      |   64  |    64     |     64      |   64   |  64   |
| Float Max Size (bits)   |   inf   |     | inf  | 128  |  64  |     64      |   64  |    64     |     64      |   64   |  64   |
| Subsecond Precision     |   ns    |     |      |  ns  |   *  |     ns      |       |    ns     |             |        |  ns   |
| Endianness              |    L    |     |      |   L  |   B  |      B      |    L  |     L     |      L      |    B   |   B   |
| Ad-hoc                  |    Y    |  Y  |   Y  |   Y  |   Y  |      Y      |       |           |      Y      |        |       |
| Non-string map keys     |    Y    |     |      |   Y  |   Y  |      Y      |       |     Y     |             |        |       |
| Zero-copy               |    Y    |     |      |      |   Y  |      Y      |    Y  |           |      Y      |        |   Y   |
| Size Optimization       |    Y    |     |      |      |   Y  |      Y      |    Y  |           |             |        |   Y   |
| Cyclic References       |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| Time Zones              |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| Gregorian Time Fields   |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| 1:1 Bin/Txt Compatible  |    Y    |     |      |      |      |             |       |           |             |        |       |
| Versioning              |    Y    |     |      |      |      |             |       |           |             |        |       |

* **Null**: Concise Encoding doesn't actually support null, but it has the more restricted NA (not available) type.
* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian, and so little endian formats can be more efficiently encoded/decoded.
* **Ad-hoc**: Supports ad-hoc data (does not require a schema).
* **Zero-copy**: Supports [zero-copy](https://en.wikipedia.org/wiki/Zero-copy) operations.
* **Size Optimization**: Encoding is designed such that the more common types & values use less space.
* **Cyclic References**: Cyclic (recursive) data structures can be represented using references.
* **Time Zones**: Timestamps support time zones.
* **Gregorian Time Fields**: Time values use Gregorian fields rather than monotonic types like seconds.
* **1:1 Bin/Txt Compatible**: All types in the binary format match 1:1 to the same type in the text format.
* **Versioning**: Documents are versioned to match a specification version.



Examples
--------

All examples are valid [Concise Text Encoding](https://github.com/kstenerud/concise-encoding/blob/master/cte-specification.md) documents that can be transparently 1:1 converted to/from [Concise Binary Encoding](https://github.com/kstenerud/concise-encoding/blob/master/cbe-specification.md)

#### Numeric Types

```cte
c1
{
    boolean       = @true
    binary-int    = -0b10001011
    octal-int     = 0o644
    decimal-int   = -10000000
    hex-int       = 0xfffe0001
    very-long-int = 100000000000000000000000000000000000009
    decimal-float = -14.125
    hex-float     = 0x5.1ec4p20
    very-long-flt = 4.957234990634579394723460546348e1000000000
    not-a-number  = @nan
    infinity      = @inf
    neg-infinity  = -@inf
}
```

#### String and String-Like

```cte
c1
{
    unquoted-str = no_quotes-needed
    quoted-str   = "Quoted strings support escapes: \n \t \27f"
    url          = |r https://example.com/|
    email        = |r mailto:me@somewhere.com|
}
```

#### Other Basic Types

```cte
c1
{
    uuid          = @f1ce4567-e89b-12d3-a456-426655440000
    date          = 2019-07-01
    time          = 18:04:00.940231541/E/Prague
    timestamp     = 2010-07-15/13:28:15.415942344/Z
    na            = @na
    na_for_reason = @na:"file not found"
}
```

#### Containers

```cte
c1
{
    list          = [1 2.5 "a string"]
    map           = {one=1 2=two today=2020-09-10}
    bytes         = |u8x 01 ff de ad be ef|
    int16-array   = |i16 7374 17466 -9957|
    uint16-hex    = |u16x 91fe 443a 9c15|
    float32-array = |f32 1.5e10 -8.31e-12|
}
```

#### Markup

```cte
c1
{
    main-view = <View,
        <Image src=|r images/avatar-image.jpg|>
        <Text id=HelloText,
            Hello! Please choose a name!
        >
        // OnChange contains code which might have problematic characters.
        // Use verbatim sequences (\.IDENTIFIER ... IDENTIFIER) to handle this.
        <TextInput id=NameInput style={height=40 borderColor=gray} OnChange="\.@@
            NameInput.parent.InsertRawAfter(NameInput, "<Image src=|r images/checkmark.svg|>")
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
    marked_object = &id1:{
        recursive = $id1
    }
    ref1        = $id1
    ref2        = $id1

    // Reference pointing to part of another document.
    outside_ref = $|r https://xyz.com/document.cte#some_id|
}
```

#### Metadata

```cte
c1
// Metadata about the entire document
(
    created     = 2019-9-1/22:14:01
    description = "A demonstration"
    version     = "1.1.0"
){
    children = (
        // Metadata about the list of children
        description = "Homer's children"
    )[Bart Lisa Maggie]
}
```

#### Relationships

```cte
c1 (
    // Marked base resource identifiers used for concatenation.
    // (Stored in metadata because they themselves aren't part of the data)
    rdf = [
        &people:|r https://springfield.gov/people#|
        &e:|r https://example.org/|
    ]
){
    // Map-encoded relationships (the map is the subject)
    $people:homer_simpson = {

        /* $e refers to |r https://example.org/|
         * $e:wife concatenates to |r https://example.org/wife|
         */
        $e:wife = $people:marge_simpson

        // Multiple relationship objects
        $e:regrets = [
            $firing
            $forgotten_birthday
        ]
    }

    // Relationship containers (@[subject predicate object])
    rdf-statements = [
        &marge_birthday:@[$people:marge_simpson $e:birthday 1956-10-01]
        &forgotten_birthday:@[$people:homer_simpson $e:forgot $marge_birthday]
        &firing:@[$people:montgomery_burns $e:fired $people:homer_simpson]

        // Multiple relationship subjects
        @[[$firing $forgotten_birthday] $e:contribute $e:marital_strife]
    ]
}
```

#### Constants

```cte
c1
{
    /* Given: Actual type and value of "eggshell" and
     *        "navy-blue" have been defined in a schema
     */
    wall_color = #eggshell
    door_color = #navy-blue
}
```

#### Custom Types

```cte
c1
{
    // Custom types are user-defined, with user-supplied codecs.
    custom-text   = |ct cplx(2.94+3i)|
    custom-binary = |cb 04 f6 28 3c 40 00 00 40 40|
}
```



License
-------

Copyright 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
