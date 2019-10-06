Request for Comments
====================

The thing about foresight is that it's a lot harder than hindsight.

This is a request for comments regarding the [CBE](https://github.com/kstenerud/concise-binary-encoding/blob/master/cbe-specification.md) and [CTE](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md) prototype specifications. It is not an IETF RFC, but rather an actual **request** for **comments** from the community at large in order to work out bugs, inconsistencies, and oversights in the specifications. Version 2 should ideally come out in a few decades, not in a few months ;-)

The purpose of CBE and CTE is to provide type-compatible binary and text formats capable of storing semi-structured hierarchical data, similar to what was accomplished with XML and JSON. CBE and CTE endeavor to directly support the most common data types, while also avoiding the complexities that sank other formats in the past. What CBE/CTE aims for is a good balance between types, expressivity, and complexity in a way that supports 80% of use cases. CBE further endeavors to do so in a compact binary representation that is easy to encode and decode.



Contents
--------

* [Remaining TODO](#remaining-todo)
* [Structural Concerns](#structural-concerns)
* [Data Types](#data-types)
  - [Numeric Types](#numeric-types)
  - [Temporal Types](#temporal-types)
  - [Array Types](#array-types)
  - [Container Types](#container-types)
  - [Metadata Types](#metadata-types)
  - [Other Types](#other-types)
  - [More Types?](#more-types)
* [Maximums](#maximums)
* [Anything Else?](#anything-else?)
* [How to Comment](#how-to-comment)
* [Thank You](#thank-you)



Remaining TODO
--------------

While the specifications are mostly done, the reference implementations aren't completely finished yet. As I implement them, I'll probably discover more things that need to be updated or further refined in the specs (more likely in CTE than in CBE).

#### TODO:

* Update the `go` implementation of CBE to match the latest spec
* Implement CTE in `C` and `go` (most likely this will be done using Ragel)



Structural Concerns
-------------------

The structure of CBE and CTE were designed to balance codec complexity and encoded data size.


### Object Separators

CTE uses whitespace rather than commas to separate objects. There don't seem to be any downsides to this, but if you notice problems, please let me know!


### Byte-Oriented

The binary format must be byte-oriented. Sub-byte data tends to invite too much complexity, to the point that the bug count and the algorithmic complexity outweighs the marginal transmission size benefits. Some sub-byte data is allowed for specific data type payloads, but this is kept to a minimum.


### Variable Sized Data Types

The first prototype of CBE used only fixed length data, and although it was simple to parse and generate, it also tended to be wasteful of space when considering the most common data values that are used in the real world:

* Floating point values rarely need to use many significant digits or exponential range.
* Dates and times are usually within 50 years of 2000.
* Sub-second precision requirements vary greatly.
* Numbers in the range -100 to 100 are more common than those outside of that range.
* Integral types usually have many of the upper bits cleared.

CBE takes advantage of these data tendencies to reduce the effective size of the binary representation in real-world use cases.

CBE incorporates the following technologies for its variable sized data types:

* [Variable Length Quantity](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)
* [Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)
* [Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md)

CTE incorporates the following technologies:

* [Safe64 Encoding](https://github.com/kstenerud/safe-encoding/blob/master/safe64-specification.md)
* [Safe85 Encoding](https://github.com/kstenerud/safe-encoding/blob/master/safe85-specification.md)



Data Types
----------

The data types were chosen based on what is most commonly used in the industry. The idea is to support 80% of use cases.

Data types must be 1:1 compatible between CBE and CTE.


### Numeric Types

Most data is numeric, so the majority of the compression techniques are focused on these types.

#### Boolean

True and false, encoded such that the low bit of the type field is 0 for false and 1 for true.

#### Integer

Integers are the most commonly used types. CBE is structured such that the most commonly used integer values (-100 to 100) are encoded directly in the type field, taking the least amount of space. Various compression techniques allow other integer values to be represented in less bytes than their original sizes.

The sign is recorded in the type field rather than in the data portion. This makes it easier to represent unsigned integers, while also expanding the range of negative integers at no extra cost (it uses the same number of type codes as would be required to support both signed and unsigned integers).

Positive integer type fields always have a 0 in the LSB, and negative integer type fields always have a 1 in the LSB.

#### Decimal floating point number

This should be the default floating point type. We're effectively using decimal floating point in all text-based formats already. CBE formalizes this for binary formats, and also can often encode them in less space than their ieee754 counterparts.

#### Binary floating point number

The 32 and 64 bit ieee754 binary floating point types should be avoided unless absolutely necessary. They waste space, and don't map very well to base-10. These types exist only to support systems that would otherwise break from decimal rounding.


### Temporal Types

Temporal types are currently a mess in the industry. The only standard of note is ISO8601, which unfortunately leaves too many things ambiguous. For binary formats, there is effectively no decent standard.

* Date
* Time
* Timestamp

Question: Should the date type include timezone data? Is that important enough to care about (for example, exactly when the transition from April 5th to April 6th occurs)?

#### Time Zones

There are two ways to record time zones: `zone/area` and `location`. Location based time zone data is still uncommon, although I've seen Google using it. My hope is that systems will eventually catch up and offer location based timezone databases for general use. Until then, the location based time zones will likely be mostly unused.


### Array Types

"Array" types are made up of arrays of bytes. These are primarily for binary blobs and for textual data.

#### Bytes

Bytes is the catch-all if your data type is not available. It's recommended that any encoded data be written in little endian byte order.

Bytes can be represented in CTE using `hex`, `safe64`, and `safe85` encoding. The safe encoding schemes were developed because of problems with existing mainstream encoders:

* There are too many incompatible versions of the various encoding methods, using the same names. This makes it confusing and will lead to errors and incompatibilities in implementations.
* The choice of alphabets for many of the existing implementations don't work well in modern text processing environments (HTML, XML, URIs, source code, etc). If more than two characters of the alphabet require escaping, the format is unsuitable.
* All existing implementations make use of padding, which is wasteful, unnecessary, and doesn't even live up to its promise of end-of-data detection.
* The whitespace rules in existing implementations make no sense. Whitespace should be allowed anywhere.

#### String

Strings are UTF-8 encoded. I've listed all the string rules I can think of in the specifications, but there may be edge cases I haven't thought of that could cause problems.

There are fewer escape sequences allowed compared to what's normally available in string literals for most programming languages. Many escape sequences (for example `\v`, `\b`, `\a`) don't make much sense in modern times.

Unquoted strings are allowed in CTE as long as they don't clash with existing keywords. This technically opens up the possibility of erroneously encoding for example the boolean `false` when you intended to encode a string, but I think the benefits outweigh the risks.

#### URI

URIs are generally useful and versatile, so they make sense as a first-class data type.

#### More Array Types?

Theoretically, an array is a lot like a list, except that it is restricted to a single type, which allows it to be stored more compactly (maybe - it depends on the actual values stored vs the compressed representation). Is this enough of a potential benefit to warrant adding explicit arrays for each scalar type? If it were allowed, the CTE representation might get ugly. There would need to be some kind of prefix to say what kind of data is stored within the array, and there'd be no actual benefit in the text format over using a list.

Other string-based array types may prove useful. Please suggest any that should be added!


### Container Types

Although there are plenty of container types, they all tend to be representable as tree and/or list structures, and can be trivially reconstructed from these base types. Unless I've missed something important, there are only three container types that are actually needed:

* List
* Unordered Map (key ordering isn't important)
* Ordered Map (key ordering is important)

Since a set operates like lists with extra restrictions (no duplicates allowed, ordering is unimportant), it's not included as a type. Applications can apply the extra restrictions internally without too much trouble.

Maps are most commonly unordered, and so there's a need for both unordered and ordered map types.

#### More Container Types?

Are there other fundamental container types that I've missed? Let me know!


### Metadata Types

Metadata types have either been missing entirely, or were confusing to use in existing formats. I've tried to structure things such that metadata is useable anywhere most people would want to use it.

#### Comment

Comments behave mostly in the same way a comment would in source code. Because of the history of how comments have been used, one cannot reliably associate a comment to a particular object in a document (does it refer to the next object? The previous object? The set of objects that follow? How many objects does it refer to?). Comments are therefore not linked to any particular object in a document.

Question: Do the character restrictions in comments make sense?

#### Metadata Map

A Metadata map always refers to the object directly following it (even if that object is another metadata map, in which case you have meta-metadata). Metadata maps work just like ordered maps, and can contain any object (including more metadata maps if you wish).

#### Special Metadata Map Keys

The intent for special metadata keys is to codify a standard way of representing the most common types of metadata found in data structures, files, etc. There are [so many metadata standards](https://en.wikipedia.org/wiki/Metadata_standard) that it's difficult to come up with a standardized set that is general enough for ad-hoc data formats like CBE and CTE. "Creation Time", for example, is a lot more generally useful than "Aspect Ratio". The question is where to draw the line. I doubt that we're ever going to be able to answer this to everyone's satisfaction, but it doesn't hurt to try. The initial (incomplete) list is:

| Key                  | Short Key | Type          | Contents          |
| -------------------- | --------- | ------------- | ----------------- |
| `_creation_time`     | `_ct`     | Timestamp     | Creation time     |
| `_modification_time` | `_mt`     | Timestamp     | Modification time |
| `_access_time`       | `_at`     | Timestamp     | Last access time  |
| `_tags`              | `_t`      | List          | Set of tags       |
| `_attributes`        | `_a`      | Unordered Map | Attributes        |

It might be a better idea to standardize these keys in a separate specification, which would allow the set of accepted keys to change without requiring a new version of CBE and CTE every time it happens. This would also allow for better interoperability with other formats.


### Other Types

#### Nil

The `nil` type will be controversial due to the problems inherent in allowing a null type, but so many languages and structures make use of it that I felt compelled to add it. Implementations can choose whether it's better to use `nil` or to simply omit the field.

#### Padding

I'm ambivalent to the usefulness of the padding type. Since it only consumes one type code, and there are still 10 unassigned codes in the format, I'm leaving it in. It can always be removed in a future version if it's found to be unused in practice.


### More Types?

Are there more types that should be considered?



Maximums
--------

The only hard maximum in CTE and CBE is the maximum container depth. It's left to the implementations to decide upon maximum document byte length, maximum number of objects, etc. This feels like a good compromise position considering that "ridiculous" documents seem to be rather uncommon in the JSON world, despite it having no official restrictions at all.



Anything Else?
--------------

Is there anything else that should be looked at? Are there more efficient ways of storing the data without increasing complexity too much?



How to Comment
--------------

To start a comment and discussion, please [open an issue in the CBE repository](https://github.com/kstenerud/concise-binary-encoding/issues). Discussion will happen in the issue's comments and is open to all.



Thank You
---------

I really appreciate all constructive criticism. These encoding formats are intended to be used by all, but I can only effectively visualize my own use cases unless I'm given other scenarios to consider, so it's important that you get involved and share your ideas!
