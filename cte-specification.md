Concise Text Encoding
=====================

Work in progress...

An object may be one of the following:

  * Empty
  * Boolean
  * Number
  * Time
  * String
  * List
  * Map
  * Array

Containers may contain other objects.


Data Types
----------


### Empty

Represents the absence of data.

    empty


### Boolean

Represents true or false

    true
    false


### Number

#### Integer

##### Base 2

    b10001011

##### Base 8

    o644

##### Base 10

    1000

##### Base 16

    h7ff0


#### Float

    123.45


#### Decimal

    d1000.55


### Time

    1985-10-26T08:22:16.900142


### String

    "A string"

#### Escapes

  * \\
  * \"
  * \r
  * \n
  * \t
  * \000
  * \xff
  * \uffff


### List

    (object, object, object)

### Map

    [key: value, key: value]

### Array

    i32(1000, 2000, 3000)

#### Types

  * b
  * i8
  * i16
  * i32
  * i64
  * i128
  * f32
  * f64
  * f128
  * d64
  * d128
  * t
