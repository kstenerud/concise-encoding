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
 * Supports the most commonly used data types.



Contents
--------

 * [Request for Comments](#request-for-comments)
 * [Specifications](#specifications)
 * [Comparison to Other Formats](#comparison-to-other-formats)
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

| Type          | Concise Encoding | XML | JSON | BSON | CBOR | Messagepack | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ------------- | ---------------- | --- | ---- | ---- | ---- | ----------- | --------- | ----------- | ------ | ----- |
| Null          |        Y         |     |  Y   |  Y   |  Y   |      Y      |     Y     |             |        |   Y   |
| Integer       |        Y         |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Boolean       |        Y         |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Binary Float  |        Y         |     |      |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Decimal Float |        Y         |     |  Y   |  Y   |  Y   |             |           |             |        |       |
| Timestamp     |        Y         |     |      |  Y   |  Y   |      Y      |     Y     |             |        |   Y   |
| Bytes         |        Y         |     |      |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| String        |        Y         |  Y  |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| URL           |        Y         |     |      |      |      |             |           |             |        |       |
| List          |        Y         |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |   Y   |
| Map           |        Y         |     |  Y   |  Y   |  Y   |      Y      |     Y     |      Y      |   Y    |       |
| Markup        |        Y         |  Y  |      |      |      |             |           |             |        |       |
| Metadata      |        Y         |     |      |      |      |             |           |             |        |       |
| Comment       |        Y         |     |      |      |      |             |           |             |        |       |
| Reference     |        Y         |     |      |      |  Y   |             |           |             |        |       |


#### Feature Comparison

| Type                   | Concise Encoding | XML | JSON | BSON | CBOR | Messagepack | Protobufs | Flatbuffers | Thrift | ASN.1 |
| ---------------------- | ---------------- | --- | ---- | ---- | ---- | ----------- | --------- | ----------- | ------ | ----- |
| Int Max Size (bits)    |     infinite     |     |  52  |  64  |  64  |     64      |    64     |     64      |   64   |  64   |
| Float Max Size (bits)  |     infinite     |     |  64  | 128  |  64  |     64      |    64     |     64      |   64   |  64   |
| Time Zones             |        Y         |     |      |      |      |             |           |             |        |       |
| Subsecond Precision    |       ns         |     |      |  ns  |   -  |     ns      |    ns     |             |        |  ns   |
| Gregorian Time Fields  |        Y         |     |      |      |      |             |           |             |        |       |
| Endianness             |        L         |     |      |   L  |   B  |      B      |     L     |      L      |    B   |   B   |
| Versioning             |        Y         |     |      |      |      |             |           |             |        |       |
| Non-string map keys    |        Y         |     |      |      |      |             |           |             |        |       |
| 1:1 Bin/Txt Compatible |        Y         |     |      |      |      |             |           |             |        |       |

* **Time Zones**: Timestamps support time zones.
* **Gregorian Time Fields**: Time values use Gregorian fields rather than monotonic types like seconds.
* **Endianness**: B=big, L=little. The most popular modern CPUs use little endian, so formats using this byte order can be more efficiently encoded/decoded.
* **Versioning**: Documents are versioned to match a specification version.
* **1:1 Bin/Txt Compatible**: All types in the binary format match 1:1 to the same type in the text format.



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

 * [C Implementation](https://github.com/kstenerud/c-cbe)
 * [Go Implementation](https://github.com/kstenerud/go-cbe)

### Concise Text Encoding

 * [Go Implementation](https://github.com/kstenerud/go-cte)



License
-------

Copyright (C) Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
