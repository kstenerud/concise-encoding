Concise Binary Encoding and Concise Text Encoding
=================================================

A general purpose, compact representation of semi-structured hierarchical data, in a machine readable format, and in a compatible human readable format.

#### Designed with the following goals:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Minimal complexity

#### Concise Binary Encoding (CBE) adds the following design goals:

  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow native reading directly off the wire
  * Balanced space and computation efficiency


Supported Types
---------------


### Scalar Types

Binary, stored in little endian byte order.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Boolean  | True or false                                        |
| Integer  | Signed two's complement, from 8 to 128 bits          |
| Float    | IEEE 754 floating point, from 32 to 128 bits         |
| Decimal  | IEEE 754 densely packed decimal, from 64 to 128 bits |
| Time     | Date & time, to the microsecond                      |


### Array Types

Array types can hold multiple scalar values of the same type and size.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Array    | Array of a scalar type                               |
| Bitfield | Packed "array" of bits                               |
| String   | Array of UTF-8 encoded bytes (no BOM)                |


### Container Types

Containers can store any type, including other containers and mixed types.

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| List     | A list containing any types                          |
| Map      | Scalar or array types for keys, any types for values |


### Other Types

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Empty    | Denotes the absence of data                          |
| Padding  | Used to align data in a CPU friendly manner.         |



See [the CBE specification](cbe-specification.md) and [the CTE specification](cte-specification.md) for more details.



License
-------

Specifications released under Creative Commons Attribution 4.0 International Public License.
Reference implementation released under MIT License.
