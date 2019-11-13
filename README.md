Concise Binary Encoding
=======================

Data communications have become needlessly bloated, wasting bandwidth and power (de)serializing and transmitting data in text formats. The age of plentiful bandwidth and processing power is coming to an end, and energy efficiency in software is fast becoming a serious cost, battery, heat dissipation, and environmental issue. We need to meet these challenges with new data formats that are not only versatile, but also compact, computationally simple to process, and human readable.

SGML and friends marked a paradigm shift in the 90s, displacing many of the obscure and complex proprietary data formats of the time. The simplicity, human readability, and machine-parseability of HTML and XML helped the worldwide web and many other technologies flourish. These text formats were bigger and more CPU intensive than the binary formats, but the availability of fast internet and cheap computing kicked those issues far down the road.

JSON was a major improvement over XML, reducing bloat and boilerplate, and more closely modeling the actual data types and structures used in real-world programs. Unfortunately, since JSON was originally designed to be directly ingested by a Javascript engine (now considered a security risk), it lacked many fundamental data types & value ranges and was poorly defined, leading to ambiguity, incompatibility, and tricky edge cases.

Various JSON-inspired binary formats emerged as the costs of text formats added up, but they suffered from many of the same limitations as JSON, and added needless complexity with proprietary and obscure data types, or difficult (i.e. expensive) to decode fields. It was also difficult or impossible for humans to view and edit these binary documents, limiting their utility.

Concise Encoding is the next step in the evolution of ad-hoc hierarchical data formats, aiming to support 80% of data use cases in a power and bandwidth friendly way:

 * Completely redesigned from the ground up to balance human readability, encoded size, and codec complexity.
 * Split into two formats: [binary-based CBE](https://github.com/kstenerud/concise-binary-encoding) and [text-based CTE](https://github.com/kstenerud/concise-text-encoding).
 * 1:1 type compatibility between formats, allowing transparent conversion between CBE and CTE. You can use the more efficient binary format for data interchange and storage, and convert to/from text only when a human needs to be involved.
 * Nowadays, little endian is ubiquitous, and so fixed-length multi-byte data fields in CBE are stored in little endian byte order to avoid extra byte swaps in the most popular hardware.
 * CBE and CTE are fully specified, eliminating ambiguities and covering edge cases.
 * Documents and specifications are versioned to support future expansion.
 * Support for metadata and comments.
 * Support for the most commonly used data types.



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
