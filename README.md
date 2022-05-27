<p align="center"><img width="130" alt="CE Logo" src="img/ce-logo.svg"/><h3 align="center">

Concise Encoding

</h3><p align="center"> The secure data format for a modern world </p><hr/></p>


💡 Solving today's problems
---------------------------

**Times are different from the carefree days that brought us XML and JSON:**

### Security

<ul>

  State actors, criminal organizations and mercenaries are now actively hacking governments, companies and individuals to steal secrets, plant malware, and hold your data hostage.

  The existing ad-hoc data formats are [too loosely defined to be secure](ce-structure.md#attack-vectors), and can't be fixed because they're not versioned.

  **Concise Encoding is designed for security, and is versioned so that it can be updated to handle new threats**.

</ul>

### Efficiency AND ease-of-use

<ul>

  We send so much data now that efficiency is critical, but switching to binary means giving up the ease of text formats.

  **Concise Encoding gives you ease and efficiency with its 1:1 compatible [text](cte-specification.md) and [binary](cbe-specification.md) formats.**

  **`Edit text. Transmit binary.`**

</ul>

### Types

<ul>

  Lack of types forces everyone to add extra encoding steps to send their data, which is buggy, reduces compatibility, and opens new security holes.

  **Concise Encoding supports all of the common types natively**.

</ul>

<br/>

📊 Compared to other formats
----------------------------

### Features

| Type                  | CE | XML | JSON | BSON | CBOR | Protobufs | Thrift | ASN.1 | Ion |
| --------------------- | -- | --- | ---- | ---- | ---- | --------- | ------ | ----- | --- |
| Int Max Size (bits)   | ∞  | ❌  |  53  |  64  |  64  |    64     |   64   |  64   |  ∞  |
| Float Max Size (bits) | ∞  | ❌  |  64  | 128  |  64  |    64     |   64   |  64   |  ∞  |
| Subsecond Precision   | ns | ❌  |  ❌  |  ns  |  ns  |    ns     |   ❌   |  ns   | ns  |
| Ad-hoc                | ✔️  | ✔️   |  ✔️   |  ✔️   |  ✔️   |    ❌     |   ❌   |  ❌   | ✔️   |
| Little Endian         | ✔️  | ❌  |  ❌  |  ✔️   |  ❌  |    ✔️      |   ❌   |  ❌   | ❌  |
| Non-string map keys   | ✔️  | ❌  |  ❌  |  ✔️   |  ✔️   |    ✔️      |   ❌   |  ❌   | ❌  |
| Size Optimization     | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ❌     |   ❌   |  ✔️    | ❌  |
| Cyclic Data           | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ❌     |   ❌   |  ❌   | ❌  |
| Time Zones            | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ❌     |   ❌   |  ❌   | ❌  |
| Bin + Txt             | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ✔️    | ✔️   |
| Versioned             | ✔️  | ✔️   |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ⚠️   |

 * **Ad-hoc**: Supports ad-hoc data (does not require a schema).
 * **Little Endian**: Uses little-endian (modern CPUs use little endian, making little endian formats more efficient).
 * **Size Optimization**: The most common types and values use less space.
 * **Cyclic Data**: Supports cyclic (recursive) data structures.
 * **Time Zones**: Time types support real time zones.
 * **Bin + Txt**: Has twin formats, and the binary format is 1:1 compatible with the text format.
 * **Versioned**: Documents are versioned to the specification they adhere to. (Ion supports versioning in the binary format only).

### Type Support

| Type          | CE | XML | JSON | BSON | CBOR | Protobufs | Thrift | ASN.1 | Ion |
| ------------- | -- | --- | ---- | ---- | ---- | --------- | ------ | ----- | --- |
| Boolean       | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Integer       | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Binary Float  | ✔️  | ❌  |  ❌  |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Decimal Float | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ❌     |   ❌   |  ❌   | ✔️   |
| NaN, Infinity | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Universal ID  | ✔️  | ❌  |  ❌  |  ✔️   |  ✔️   |    ❌     |   ❌   |  ✔️    | ❌  |
| Timestamp     | ✔️  | ❌  |  ❌  |  ✔️   |  ✔️   |    ✔️      |   ❌   |  ✔️    | ✔️   |
| Resource ID   | ✔️  | ✔️   |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| String        | ✔️  | ✔️   |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Bytes         | ✔️  | ❌  |  ❌  |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| List          | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ✔️   |
| Map           | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ✔️    |  ❌   | ❌  |
| Edge          | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| Node          | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| Typed Arrays  | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ✔️      |   ✔️    |  ✔️    | ❌  |
| Reference     | ✔️  | ❌  |  ❌  |  ❌  |  ✔️   |    ❌     |   ❌   |  ❌   | ❌  |
| Remote Ref    | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| Comment       | ✔️  | ✔️   |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| Null          | ✔️  | ❌  |  ✔️   |  ✔️   |  ✔️   |    ✔️      |   ❌   |  ✔️    | ✔️   |
| Media         | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ❌   | ❌  |
| Custom        | ✔️  | ❌  |  ❌  |  ❌  |  ❌  |    ❌     |   ❌   |  ✔️    | ✔️   |


<br/>

📚 Specifications and Code
--------------------------

### Specifications

 * [🧬 Concise Encoding Structure](ce-structure.md) (describes the structure and rules that both formats follow)
 * [📡 Concise Binary Encoding (CBE)](cbe-specification.md) (describes the binary format encoding)
 * [👥 Concise Text Encoding (CTE)](cte-specification.md) (describes the text format encoding)

**Note**: Most applications will only need the [binary format](cbe-specification.md). The [text format](cte-specification.md) is only required in places where a human must get involved, and this can often be handled by a simple [command-line tool](https://github.com/kstenerud/enctool).

### Design

* [📐 Design Document](DESIGN.md) explains the design choices behind Concise Encoding.

### Grammar

 * [🔡 ANTLRv4 grammar for CTE](antlrv4-grammar)

### Implementations

 * [⚙️ Go Implementation](https://github.com/kstenerud/go-concise-encoding) (reference implementation)

### Tools

 * [🛠️ Enctool](https://github.com/kstenerud/enctool) A tool for converting between formats


<br/>

⚠️ Draft Specification
----------------------

Although Concise Encoding is nearing a release, it's currently a **draft** specification and thus subject to change. Please use a version of `0` for now to avoid compatibility issues with existing documents when version 1 is released.

**Note**: When version 1 is released, `0` will no longer be a valid version number.


<br/>

📌 Examples
-----------

All examples are valid [Concise Text Encoding](cte-specification.md) documents that can be transparently 1:1 converted to/from [Concise Binary Encoding](cbe-specification.md).

#### Numeric Types

```cte
c1
{
    "boolean"       = true
    "binary int"    = -0b10001011
    "octal int"     = 0o644
    "decimal int"   = -10000000
    "hex int"       = 0xfffe0001
    "very long int" = 100000000000000000000000000000000000009
    "decimal float" = -14.125
    "hex float"     = 0x5.1ec4p+20
    "very long flt" = 4.957234990634579394723460546348e+100000
    "not-a-number"  = nan
    "infinity"      = inf
    "neg infinity"  = -inf
}
```

#### String and String-Like

```cte
c1
{
    "string" = "Strings support escape sequences: \n \t \51F415"
    "url"    = @"https://example.com/"
    "email"  = @"mailto:me@somewhere.com"
}
```

#### Other Basic Types

```cte
c1
{
    "uuid"      = f1ce4567-e89b-12d3-a456-426655440000
    "date"      = 2019-07-01
    "time"      = 18:04:00.948/Europe/Prague
    "timestamp" = 2010-07-15/13:28:15.415942344
    "null"      = null
    "media"     = |m application/x-sh 23 21 2f 62 69 6e 2f 73 68 0a 0a
                    65 63 68 6f 20 68 65 6c 6c 6f 20 77 6f 72 6c 64 0a|
}
```

#### Containers

```cte
c1
{
    "list"          = [1 2.5 "a string"]
    "map"           = {"one"=1 2="two" "today"=2020-09-10}
    "bytes"         = |u8x 01 ff de ad be ef|
    "int16 array"   = |i16 7374 17466 -9957|
    "uint16 hex"    = |u16x 91fe 443a 9c15|
    "float32 array" = |f32 1.5e10 -8.31e-12|
}
```

#### References

```cte
c1
{
    // Entire map will be referenced later as $id1
    "marked object" = &id1:{
        "recursive" = $id1
    }
    "ref1" = $id1
    "ref2" = $id1

    // Reference pointing to part of another document.
    "outside ref" = $"https://xyz.com/document.cte#some_id"
}
```

#### Graphs

```cte
c1
/* The weighted graph:
 *
 *     b
 *    /|\
 *   4 1 1
 *  /  |  \
 * a-3-c-4-d
 *
 */
{
    "vertices" = [
        &a:{}
        &b:{}
        &c:{}
        &d:{}
    ]
    "edges" = [
        @($a {"weight"=4 "direction"="both"} $b)
        @($a {"weight"=3 "direction"="both"} $c)
        @($b {"weight"=1 "direction"="both"} $c)
        @($b {"weight"=1 "direction"="both"} $d)
        @($c {"weight"=4 "direction"="both"} $d)
    ]
}
```

#### Trees

```cte
c1
/* The tree:
 *
 *       2
 *      / \
 *     5   7
 *    /   /|\
 *   9   6 1 2
 *  /   / \
 * 4   8   5
 *
 */
(2
    (7
        2
        1
        (6
            5
            8
        )
    )
    (5
        (9
            4
        )
    )
)
```

Notice how when rotated 90°, it resembles the tree it represents:

![Rotated](img/tree-rotated.svg)

#### Custom Types

```cte
c1
{
    // Custom types are user-defined, with user-supplied codecs.
    "custom text"   = |ct cplx(2.94+3i)|
    "custom binary" = |cb 04 f6 28 3c 40 00 00 40 40|
}
```
