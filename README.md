Concise Binary Encoding
=======================

A general purpose, machine-readable, compact representation of semi-structured hierarchical data.

Designed with the following points of focus:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow native reading directly off the wire
  * Compact data in ways that don't affect reading directly of the wire
  * Balanced space and computation efficiency
  * Minimal complexity


Supported Types
---------------

Many common data types and structures are supported:


### Scalar Types

Binary, stored in little endian byte order.

  * **Boolean**: True or false.
  * **Integer**: Always signed, two's complement, in widths from 8 to 128 bits.
  * **Float**: IEEE 754 floating point, in widths from 32 to 128 bits.
  * **Decimal**: IEEE 754 decimal, in widths from 64 to 128 bits, DPD encoding.
  * **Time**: UTC-based date + time with precision to the second, millisecond or microsecond.


### Array Types

Array types can hold multiple scalar values of the same type and size.

  * **Array**: Array of a scalar type
  * **Bitfield**: Packed "Array" of bits
  * **String**: Array of UTF-8 encoded characters without BOM


### Container Types

Containers can store any type, including other containers. They can also contain mixed types.

  * **List**: A list of any type, including mixed types
  * **Map**: Can use any scalar or array types for keys, and all types for values


### Other Types

  * **Empty**: Denotes the absence of data.
  * **Padding**: Invisible field used to align data in a CPU friendly manner.



See [the specification](cbe-specification.md) for more details.


Specification released under Creative Commons Attribution 4.0 International Public License.
Reference implementation released under MIT License.
