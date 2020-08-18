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
* [Comparison to Other Formats](#comparison-to-other-formats)
* [Example](#example)
* [License](#license)



Introduction
------------

Today's data formats present us with a dilemma: Use text based formats that are bloated and slow, or use binary formats that humans can't read. Wouldn't it be nice to have the benefits of both, and none of the drawbacks?

**Concise Encoding** is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a power, bandwidth, and human friendly way.

#### Supported Types

| Type          | Description                                             |
| ------------- | ------------------------------------------------------- |
| **Nil**       | No data (NULL)                                          |
| **Boolean**   | True or false                                           |
| **Integer**   | Positive or negative, arbitrary size                    |
| **Float**     | Binary or decimal floating point, arbitrary size        |
| **UUID**      | [RFC-4122 UUID](https://tools.ietf.org/html/rfc4122)    |
| **Time**      | Date, time, or timestamp, arbitrary size                |
| **URI**       | [RFC-3986 URI](https://tools.ietf.org/html/rfc3986)     |
| **String**    | UTF-8 string, arbitrary length                          |
| **Bytes**     | Array of octets, arbitrary length                       |
| **List**      | List of objects                                         |
| **Map**       | Mapping keyable objects to other objects                |
| **Markup**    | Presentation data, similar to XML                       |
| **Reference** | Points to previously defined objects or other documents |
| **Metadata**  | Data about data                                         |
| **Comment**   | Arbitrary comments about anything, nesting supported    |
| **Custom**    | User-defined data type                                  |



Specifications
--------------

 * [CTE condensed description (read this first)](cte-condensed.md)
 * [Concise Text Encoding (CTE)](cte-specification.md)
 * [Concise Binary Encoding (CBE)](cbe-specification.md)
 * [Common Generic Metadata](common-generic-metadata.md)
 * [Design Document](design.md)



Implementations
---------------

 * [Go](https://github.com/kstenerud/go-concise-encoding)



Comparison to Other Formats
---------------------------

Concise encoding is an ad-hoc format, so it shares more in common with XML, JSON & CBOR than it does with Protobufs, Flatbuffers & Cap'n proto. Ad-hoc data doesn't require a schema, but as a result it can't always be as compact as the schema-based encodings because it must record type data. Then again, ad-hoc data allows more compression opportunities than schema-based encodings can, which Concise encoding takes advantage of.

#### Natively supported types

| Type          | Concise | XML | JSON | BSON | CBOR | Messagepack | Cap'n | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- |
| Null          |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |             |        |   Y   |
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
| Reference     |    Y    |     |      |      |  Y   |             |       |           |             |        |       |
| Markup        |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| Comment       |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| URL           |    Y    |  Y  |      |      |      |             |       |           |             |        |       |
| Metadata      |    Y    |     |      |      |      |             |       |           |             |        |       |

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

* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian, and so little endian formats can be more efficiently encoded/decoded.
* **Ad-hoc**: Supports ad-hoc data (does not require a schema).
* **Zero-copy**: Supports [zero-copy](https://en.wikipedia.org/wiki/Zero-copy) operations.
* **Size Optimization**: Encoding is designed such that the more common types & values use less space.
* **Cyclic References**: Cyclic (recursive) data structures can be represented using references.
* **Time Zones**: Timestamps support time zones.
* **Gregorian Time Fields**: Time values use Gregorian fields rather than monotonic types like seconds.
* **1:1 Bin/Txt Compatible**: All types in the binary format match 1:1 to the same type in the text format.
* **Versioning**: Documents are versioned to match a specification version.



Example
-------

Here's an example of the text format in action. Everything here is 1:1 compatible with the binary format.

```
c1
// _ct is the creation time, in this case referring to the entire document
(_ct = 2019-9-1/22:14:01)
{
    /* Comments look very C-like, except:
       /* Nested comments are allowed! */
    */
    // Notice that there are no commas in maps and lists
    (metadata_about_a_list = "something interesting about a_list")
    a_list           = [1 2 "a string"]
    map              = {2=two 3=3000 1=one}
    string           = "A string value"
    boolean          = @true
    "binary int"     = -0b10001011
    "octal int"      = 0o644
    "regular int"    = -10000000
    "hex int"        = 0xfffe0001
    "decimal float"  = -14.125
    "hex float"      = 0x5.1ec4p20
    uuid             = @f1ce4567-e89b-12d3-a456-426655440000
    date             = 2019-7-1
    time             = 18:04:00.940231541/E/Prague
    timestamp        = 2010-7-15/13:28:15.415942344/Z
    nil              = @nil
    bytes            = |u8x 10 ff 38 9a dd 00 4f 4f 91|
    url              = u"https://example.com/"
    email            = u"mailto:me@somewhere.com"
    1.5              = "Keys don't have to be strings"
    long-string      = `ZZZ
A backtick induces verbatim processing, which in this case will continue
until three Z characters are encountered, similar to how here documents in
bash work.
You can put anything in here, including double-quote ("), or even more
backticks (`). Verbatim processing stops at the end sequence, which in this
case is three Z characters, specified earlier as a sentinel.ZZZ
    marked_object    = &tag1:{
                                description = "This map will be referenced later using #tag1"
                                value = -@inf
                                child_elements = @nil
                                recursive = #tag1
                            }
    ref1             = #tag1
    ref2             = #tag1
    outside_ref      = #u"https://somewhere.else.com/path/to/document.cte#some_tag"
    // The markup type is good for presentation data
    html_compatible  = (xml-doctype=[html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" u"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"])
                       <html xmlns=u"http://www.w3.org/1999/xhtml" xml:lang=en |
                         <body|
                           Please choose from the following widgets:
                           <div id=parent style=normal ref-id=1 |
                             /* Here we use a backtick to induce verbatim processing.
                              * In this case, "##" is chosen as the ending sequence
                              */
                             <script| `##
                               document.getElementById('parent').insertAdjacentHTML('beforeend', '<div id="idChild"> content </div>');
                             ##>
                           >
                         >
                       >
}
```



License
-------

Copyright 2018 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
