Concise Binary Encoding and Concise Text Encoding
=================================================

A general purpose, compact representation of semi-structured hierarchical data, in machine readable and human readable formats.


Goals
-----

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Minimal complexity

#### Goals: Concise Binary Encoding (CBE):

  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow the most common systems to read directly off the wire
  * Balanced space and computation efficiency

#### Goals: Concise Text Encoding (CTE):

  * Human readable format
  * Type compatible with CBE



Specifications
--------------

 * [Concise Binary Encoding](cbe-specification.md)
 * [Concise Text Encoding](cte-specification.md)



Supported Types
---------------


### Scalar Types

| Type     | Description                                          |
| -------- | ---------------------------------------------------- |
| Boolean  | True or false                                        |
| Integer  | Signed two's complement, from 8 to 128 bits          |
| Float    | IEEE 754 floating point, from 32 to 128 bits         |
| Decimal  | IEEE 754 densely packed decimal, from 32 to 128 bits |
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

| Type     | Description                                            |
| -------- | ------------------------------------------------------ |
| Empty    | Denotes the absence of data                            |
| Padding  | Used to align data in a CPU friendly manner (CBE Only) |



License
-------

Specifications released under Creative Commons Attribution 4.0 International Public License.
Reference implementation released under MIT License.
