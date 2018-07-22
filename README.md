Concise Binary Encoding and Concise Text Encoding
=================================================

A general purpose, compact representation of semi-structured hierarchical data, in a machine readable format, and in a compatible human readable format.

#### Designed with the following points of focus:

  * General purpose encoding for a large number of applications
  * Supports the most common data types
  * Supports hierarchical data structuring
  * Minimal complexity

#### Concise Binary Encoding (CBE) adds the following design points:

  * Binary format to minimize parsing costs
  * Little endian byte ordering to allow native reading directly off the wire
  * Data is compacted in ways that don't affect reading directly off the wire
  * Balanced space and computation efficiency


Supported Types
---------------


### Scalar Types

Binary, stored in little endian byte order.

  * **Boolean**: True or false.
  * **Integer**: Always signed, two's complement, in widths from 8 to 128 bits.
  * **Float**: IEEE 754 binary floating point, in widths from 32 to 128 bits.
  * **Decimal**: IEEE 754 decimal floating point, in widths from 64 to 128 bits.
  * **Time**: Date + time, with precision to the second, millisecond, or microsecond.


### Array Types

Array types can hold multiple scalar values of the same type and size.

  * **Array**: Array of a scalar type.
  * **String**: Array of UTF-8 encoded bytes.


### Container Types

Containers can store any type, including other containers. They can also contain mixed types.

  * **List**: A list of any type, including mixed types
  * **Map**: Can use any scalar or array types for keys, and all types for values


### Other Types

  * **Empty**: Denotes the absence of data.



See [the CBE specification](cbe-specification.md) and [the CTE specification](cte-specification.md) for more details.



License
-------

Specifications released under Creative Commons Attribution 4.0 International Public License.
Reference implementation released under MIT License.
