Concise Binary Encoding
=======================

General purpose, compact representations of semi-structured hierarchical data.

Alternative To:

* JSON
* XML
* BSON
* CBOR
* MessagePack
* Protobuf



Features
--------

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Easy to parse (very few sub-byte fields)
  * Binary format to minimize transmission costs
  * Little endian byte ordering where possible to allow the most common systems to read directly off the wire
  * Balanced space and computation efficiency
  * Minimal complexity
  * Type compatible with [Concise Text Encoding (CTE)](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md)



Supported Types
---------------


### Scalar Types

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| Boolean       | True or false                                         |
| Integer       | Signed two's complement integer                       |
| Float         | IEEE 754 binary floating point                        |
| Decimal       | IEEE 754 decimal floating point                       |


### Temporal Types

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| Date          | Date, with unlimited year range                       |
| Time          | Time, with precision to the nanosecond, and time zone |
| Timestamp     | Combined date and time                                |


### Array Types

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| Bytes         | Array of binary data                                  |
| String        | Array of UTF-8 encoded bytes                          |
| URI           | Universal Resource Identifier                         |


### Container Types

Containers can hold any combination of types, including other containers.

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| List          | A list may containin any types, even mixed            |
| Unordered Map | Scalar or array types for keys, any types for values  |
| Ordered Map   | key-value pairs are explicitly ordered                |


### Metadata Types

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| Metadata Map  | Metadata about an object                              |
| Comment       | A UTF-8 encoded comment string                        |


### Other Types

| Type          | Description                                           |
| ------------- | ----------------------------------------------------- |
| Nil           | Denotes the absence of data                           |
| Padding       | Used to align data in a CPU friendly manner           |



Specification
-------------

 * [Concise Binary Encoding](cbe-specification.md)



Implementations
---------------

TODO: Update these to the latest.

* [C implementation](reference-implementation)
* [Go implementation](https://github.com/kstenerud/go-cbe)



License
-------

Specifications released under Creative Commons Attribution 4.0 International Public License.
Reference implementation released under MIT License.
