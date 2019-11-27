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
