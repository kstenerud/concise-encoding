Concise Binary Encoding
=======================

Many ad-hoc hierarchical data encoding schemes exist today, but the genre has yet to reach its full potential.

JSON was a major improvement over XML, reducing bloat and boilerplate, and more closely modeling the actual data types and data structures used in real-world programs. Many JSON-inspired binary formats later emerged, with varying levels of compatibility.

Unfortunately, since JSON was originally designed to be transparently interpreted by a Javascript engine (now considered a security risk), it lacked many fundamental data types & value ranges and was poorly defined, leading to incompatibility, ambiguity, and tricky edge cases with no clear solution. The binary formats suffered similar problems, and also tended to add many uncommon types that bloated them unnecessarily.

Concise Binary Encoding (CBE) is the next step in the evolution of ad-hoc hierarchical data formats, aiming to address the shortfalls of the current generation:

 * 1:1 type compatiblility between the binary and text formats. Converting between CBE and [CTE](https://github.com/kstenerud/concise-text-encoding) is transparent, allowing you to use the much smaller and energy efficient binary format for data interchange and storage, converting to/from text only when and where a human needs to be involved.
 * Native support for the most commonly used data types. Concise Encoding aims to support 80% of data use cases natively.
 * Support for metadata and comments.
 * Completely redesigned from the ground up to balance user readability, encoded size, and codec complexity.
 * The formats are fully specified, eliminating ambiguities and covering edge cases, thus facilitating compatibility between implementations and reducing complexity.
 * Documents and specifications are versioned to support future expansion.
 * Easy to parse (very few sub-byte fields).
 * Binary format to minimize transmission costs.
 * Little endian byte ordering where possible to allow the most common systems to read directly off the wire.



Request for Comments
--------------------

This prototype specification is now [open to comments from the public](https://github.com/kstenerud/concise-encoding/blob/master/request-for-comments.md). Please join the discussion!



Supported Types
---------------

CBE aims to natively support 80% of data use cases. To this end, it provides the following fundamental data types:


### Numeric Types

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| Boolean       | True or false                                             |
| Integer       | Positive or negative integer of arbitrary size            |
| Decimal Float | Decimal exponent based floating point of arbitrary size   |
| Binary Float  | IEEE754 compatible binary floating point                  |


### Temporal Types

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| Date          | Date with unlimited year range                            |
| Time          | Time with time zone and precision to the nanosecond       |
| Timestamp     | Combined date and time                                    |


### Array Types

Array types refer to arrays of octets, encoded in specific ways.

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| Bytes         | Array of binary data                                      |
| String        | Array of UTF-8 encoded characters                         |
| URI           | Universal Resource Identifier                             |


### Container Types

Containers can hold other objects, including other containers. Contents can be of any type, including mixed types. Map keys can be of any type except containers, nil, and NaN.

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| List          | Ordered collection of objects                             |
| Map           | Ordered mapping of key objects to value objects           |


### Metadata Types

Metadata types describe other data.

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| Metadata      | Arbitrary typed metadata about the object that follows it |
| Comment       | A UTF-8 encoded comment string                            |


### Other Types

| Type          | Description                                               |
| ------------- | --------------------------------------------------------- |
| Nil           | Denotes the absence of data                               |
| Padding       | Optional data alignment type                              |



Specification
-------------

 * [Concise Binary Encoding](cbe-specification.md)



Implementations
---------------

 * [C implementation](https://github.com/kstenerud/c-cbe)
 * [Go implementation](https://github.com/kstenerud/go-cbe)



License
-------

Copyright (C) Karl Stenerud. All rights reserved.

Specifications released under [Creative Commons Attribution 4.0 International Public License](LICENSE.md).
