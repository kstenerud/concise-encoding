Concise Encoding
================

Data communications have become bloated and inefficient. The open, text based formats that supplanted proprietary ones in the 90s have become a liability due to their size, codec inefficiency, and limited type support. While there are now open binary formats, they lack the human readability and editability that made the text formats so successful. We're forced to choose between a wasteful text format and an unreadable binary format. What's needed is something with the benefits of both and the drawbacks of neither.

**Concise Encoding** is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a power, bandwidth, and human friendly way:

 * Split into two formats: [binary-based CBE](cbe-specification.md) and [text-based CTE](cte-specification.md).
 * 1:1 type compatibility between formats. Use the more efficient binary format for data interchange and storage, and transparently convert to/from text only when a human needs to be involved.
 * Fully specified, eliminating ambiguities and covering edge cases.
 * Specifications and documents are versioned to support future expansion.
 * Support for recursive/cyclic data.
 * Support for the most commonly used data types:

| Type      | Description                                             |
| --------- | ------------------------------------------------------- |
| Nil       | No data (NULL)                                          |
| Boolean   | True or false                                           |
| Integer   | Positive or negative, arbitrary size                    |
| Float     | Binary or decimal floating point, arbitrary size        |
| Time      | Date, time, or timestamp, arbitrary size                |
| URI       | [RFC-3986 URI](https://tools.ietf.org/html/rfc3986)     |
| String    | UTF-8 string, arbitrary length                          |
| Bytes     | Array of octets, arbitrary length                       |
| List      | List of objects                                         |
| Map       | Mapping keyable objects to other objects                |
| Markup    | Presentation data, similar to XML                       |
| Reference | Points to previously defined objects or other documents |
| Metadata  | Data about data                                         |
| Comment   | Arbitrary comments about anything, nesting supported    |



Specifications
--------------

 * [Binary (CBE)](cbe-specification.md)
 * [Text (CTE)](cte-specification.md)
 * [Common Generic Metadata](common-generic-metadata.md)
 * [Design Document](design.md)



Implementations
---------------

**Note:** These are works-in-progress!

#### Concise Binary Encoding

 * [Go Implementation](https://github.com/kstenerud/go-cbe)

#### Concise Text Encoding

 * [Go Implementation](https://github.com/kstenerud/go-cte)



Comparison to Other Formats
---------------------------

Concise encoding is an ad-hoc format, so it shares more in common with XML, JSON & CBOR than it does with Protobufs, Flatbuffers & Cap'n proto. Ad-hoc data doesn't require a schema, but as a result it can't always be as compact as the schema-based encodings because it must record type data. Then again, ad-hoc data allows more compression opportunities than schema-based encodings can, which Concise encoding takes advantage of.

#### Types

| Type          | Concise | XML | JSON | BSON | CBOR | Messagepack | Cap'n | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ------------- | ------- | --- | ---- | ---- | ---- | ----------- | ----- | --------- | ----------- | ------ | ----- |
| Null          |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |             |        |   Y   |
| Integer       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Boolean       |    Y    |     |  Y   |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Binary Float  |    Y    |     |      |  Y   |  Y   |      Y      |   Y   |     Y     |      Y      |   Y    |   Y   |
| Decimal Float |    Y    |     |  Y   |  Y   |  Y   |             |       |           |             |        |       |
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
| Ad-hoc                  |    Y    |  Y  |   Y  |   Y  |   Y  |      Y      |       |           |             |        |       |
| Non-string map keys     |    Y    |     |      |   Y  |   Y  |      Y      |       |     Y     |             |        |       |
| Zero-copy               |    Y    |     |      |      |   Y  |      Y      |    Y  |           |      Y      |        |   Y   |
| Size Optimization       |    Y    |     |      |      |   Y  |      Y      |    Y  |           |             |        |   Y   |
| Cyclic References       |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| Time Zones              |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| Gregorian Time Fields   |    Y    |     |      |      |   Y  |             |       |           |             |        |       |
| 1:1 Bin/Txt Compatible  |    Y    |     |      |      |      |             |       |           |             |        |       |
| Versioning              |    Y    |     |      |      |      |             |       |           |             |        |       |

* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian. Formats using this byte order can be more efficiently encoded/decoded.
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
       Note: Markup comments use <* and *> (shown later).
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
    date             = 2019-7-1
    // Note: Time zones can also be latitude/longitude based. For example,
    //       18:04:00.940231541/50.07/14.43 would reference the same time zone.
    time             = 18:04:00.940231541/E/Prague
    timestamp        = 2010-7-15/13:28:15.415942344/Z
    nil              = @nil
    bytes            = h"10ff389add004f4f91"
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
    marked_object    = &tag1 {
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
                             <* Here we use a backtick to induce verbatim processing.
                                In this case, "##" is chosen as the ending sequence *>
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
