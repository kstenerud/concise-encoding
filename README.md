Concise Encoding
================

Data communications have become too bloated and inefficient. The open, text based formats that supplanted the proprietary ones in the 90s are becoming a liability due to their size, codec complexity, and limited type support. And while there now are open binary formats, they lack what made text formats so successful: *human readability and editability*. We're left with the choice between a text format (which wastes power and bandwidth), and a binary format (which can't be edited).

Data format efficiency is becoming a serious cost, battery, heat, and environmental issue, so binary formats are an inevitability. The challenge is to come up with something versatile, compact, computationally simple to process, and also human editable.

**Concise Encoding** is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a power, bandwidth, and human friendly way:

 * Split into two formats: [binary-based CBE](cbe-specification.md) and [text-based CTE](cte-specification.md).
 * 1:1 type compatibility between formats. Use the more efficient binary format for data interchange and storage, and transparently convert to/from text only when a human needs to be involved.
 * Fully specified, eliminating ambiguities and covering edge cases.
 * Documents and specifications are versioned to support future expansion.
 * Supports metadata and comments.
 * Supports references to other parts of the document or to other documents.
 * Supports the most commonly used data types:

| Type      | Description                                         |
| --------- | --------------------------------------------------- |
| Nil       | No data                                             |
| Boolean   | True or false                                       |
| Integer   | Positive or negative, of arbitrary size             |
| Float     | Decimal or binary floating point of arbitrary size  |
| Time      | Date, time, or timestamp, of arbitrary size         |
| String    | UTF-8 string of arbitrary size                      |
| URI       | [RFC-3986 URI](https://tools.ietf.org/html/rfc3986) |
| Bytes     | Array of octets of arbitrary length                 |
| List      | List of objects                                     |
| Map       | Maps keyable objects to other objects               |
| Markup    | Data structure similar to XML                       |
| Reference | References a previously defined object              |
| Metadata  | Data about other data                               |
| Comment   | User definable comment                              |



Contents
--------

 * [Request for Comments](#request-for-comments)
 * [Specifications](#specifications)
 * [Comparison to Other Formats](#comparison-to-other-formats)
 * [Example](#example)
 * [Supported Types](#supported-types)
 * [Design](#design)
 * [Implementations](#implementations)
 * [License](#license)



Request for Comments
--------------------

This prototype specification is now [open to comments from the public](request-for-comments.md). Please join the discussion!



Specifications
--------------

 * [Binary (CBE)](cbe-specification.md)
 * [Text (CTE)](cte-specification.md)
 * [Common Generic Metadata](common-generic-metadata.md)



Comparison to Other Formats
---------------------------

#### Type Comparison

| Type          | Concise | XML | JSON | BSON | CBOR | Messagepack | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ------------- | ------- | --- | ---- | ---- | ---- | ----------- | --------- | ----------- | ------ | ----- |
| Null          |    Y    |     |  Y   |  Y   |  Y   |      Y      |     Y     |             |        |   Y   |
| Integer       |    Y    |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Boolean       |    Y    |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Binary Float  |    Y    |     |      |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Decimal Float |    Y    |     |  Y   |  Y   |  Y   |             |           |             |        |       |
| Timestamp     |    Y    |     |      |  Y   |  Y   |      Y      |     Y     |             |        |   Y   |
| Bytes         |    Y    |     |      |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| String        |    Y    |  Y  |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| List          |    Y    |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Map           |    Y    |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |       |
| Reference     |    Y    |     |      |      |  Y   |             |           |             |        |       |
| Markup        |    Y    |  Y  |      |      |      |             |           |             |        |       |
| Comment       |    Y    |  Y  |      |      |      |             |           |             |        |       |
| URL           |    Y    |  Y  |      |      |      |             |           |             |        |       |
| Metadata      |    Y    |     |      |      |      |             |           |             |        |       |


#### Feature Comparison

| Type                    | Concise | XML | JSON | BSON | CBOR | Messagepack | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ----------------------- | ------- | --- | ---- | ---- | ---- | ----------- | --------- | ----------- | ------ | ----- |
| Int Max Size (bits)     |   inf   |     | inf  |  64  |  64  |     64      |    64     |     64      |   64   |  64   |
| Float Max Size (bits)   |   inf   |     | inf  | 128  |  64  |     64      |    64     |     64      |   64   |  64   |
| Subsecond Precision     |   ns    |     |      |  ns  |   *  |     ns      |    ns     |             |        |  ns   |
| Endianness              |    L    |     |      |   L  |   B  |      B      |     L     |      L      |    B   |   B   |
| Non-string map keys     |    Y    |     |      |   Y  |   Y  |      Y      |     Y     |             |        |       |
| Zero-copy               |    Y    |     |      |      |   Y  |      Y      |           |      Y      |        |   Y   |
| Size Optimization       |    Y    |     |      |      |   Y  |      Y      |           |             |        |       |
| Cyclic References       |    Y    |     |      |      |   Y  |             |           |             |        |       |
| Time Zones              |    Y    |     |      |      |   Y  |             |           |             |        |       |
| Gregorian Time Fields   |    Y    |     |      |      |   Y  |             |           |             |        |       |
| 1:1 Bin/Txt Compatible  |    Y    |     |      |      |      |             |           |             |        |       |
| Versioning              |    Y    |     |      |      |      |             |           |             |        |       |

* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian, so formats using this byte order can be more efficiently encoded/decoded.
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
    long-string      = `@@@
A backtick induces verbatim processing, which in this case will continue
until three @ characters are encountered, similar to how here documents in
bash work.
You can put anything in here, including double-quote ("), or even more
backticks (`). Verbatim processing stops at the end sequence, which in this
case is three @ characters, specified earlier as a sentinel.@@@
    marked_object    = *tag1 {
    	                         description = "This map will be referenced later using #tag1"
    	                         value = -@inf
    	                         child_elements = @nil
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


Supported Types
---------------

### Numeric Types

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| Boolean       | True or false                                              |
| Integer       | Positive or negative integer of arbitrary size             |
| Decimal Float | Decimal exponent based floating point of arbitrary size    |
| Binary Float  | IEEE754 compatible binary floating point                   |


### Temporal Types

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| Date          | Date with unlimited year range                             |
| Time          | Time with time zone and precision to the nanosecond        |
| Timestamp     | Combined date and time                                     |


### Array Types

Array types refer to arrays of octets, encoded in specific ways.

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| Bytes         | Array of binary data                                       |
| String        | Array of UTF-8 encoded characters                          |
| URI           | Universal Resource Identifier                              |


### Container Types

Containers can hold other objects, including other containers. Contents can be of any type, including mixed types. Map keys can be of any type except containers, nil, and NaN.

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| List          | Ordered collection of objects                              |
| Map           | Ordered mapping of key objects to value objects            |
| Markup        | Name, attributes, contents (similar to XML structures)     |


### Metadata Types

Metadata types describe other data.

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| Metadata      | A map containing metadata about the object that follows it |
| Comment       | A UTF-8 encoded comment string                             |


### Other Types

| Type          | Description                                                |
| ------------- | ---------------------------------------------------------- |
| Nil           | Denotes the absence of data                                |
| Reference     | References another object in the document or externally    |
| Padding       | Optional data alignment type (CBE only)                    |



Design
------

 * [Design document](design.md)



Implementations
---------------

These are works-in-progress

### Concise Binary Encoding

 * [Go Implementation](https://github.com/kstenerud/go-cbe)

### Concise Text Encoding

 * [Go Implementation](https://github.com/kstenerud/go-cte)



License
-------

Copyright (C) Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
