Design Document
===============

    The thing about foresight is that it's a lot harder than hindsight.



Mission Statement
-----------------

Concise Encoding exists to provide an efficient mechanism (in terms of power, bandwidth, and convenience) for storing, transmitting, reading, and editing (by humans) ad-hoc semi-structured hierarchical data.



Goals
-----

 * Small size when storing or transmitting data
 * Low power requirements when processing lots of data
 * Human readability and editability
 * Native support for the most common data types and structures (enough to cover 80% of use cases)



High Level Design
-----------------

### Versioning

All documents are versioned. It's a mistake to assume that your creation will never need revisions, and differentiating versions after the fact ranges from difficult to impossible.


### Twin Formats

Concise Encoding is composed of two parallel formats: one binary and one text. This was the only way to preserve human readability and editability while also maintaining processing and bandwidth efficiency along the critical path. The idea is to store and transmit in binary, and only convert to text as-needed when a human gets involved in the process. For this to work, the binary and text formats need to be 1:1 type compatible so that they can be transparently converted on-demand.


### Choice of Data Types

The biggest challenge of a text format is figuring out how to concisely and uniquely represent all of the different data types in a convenient, human readable way that still remains easily parseable by a computer. Limiting the number of data types helps a lot, but that can run counter to the goal of covering 80% of use cases. After many revisions, I settled on the following types:

 * Nil
 * Boolean
 * Integer (arbitrary length)
 * UUID
 * Float (binary and arbitrary length decimal)
 * Time (date, time, timestamp, to the nanosecond, with time zone)
 * String
 * URI
 * Bytes
 * List
 * Map
 * Markup
 * Reference
 * Metadata
 * Comment
 * Custom

The most fundamental container types are the list and the map. Internally, almost every structure is composed of these two.

URIs and references are important because linking amplifies the power and expressivity of any medium.

Comments are necessary because people will build complicated data structures that need explanations.

Metadata is important because we can't be sure that negotiations made prior to sending a document will provide enough context to make sense of the data.

Markup is important because XML-style parent-child data structures have become the de-facto standard for storing presentation data.



Type-Specific Concerns
----------------------

### Containers

Containers can contain mixed types. Not all languages support this, but for such cases it's better to limit the kinds of data you send rather than limit the format itself.

Containers are ordered by default. It's easier to start with tighter constraints and relax them on demand than to go the other way around.


### Nil

The `nil` type perpetuates the null problem (null is a problematic type in general). However, it's ubiquitous enough that I felt compelled to include it.


### Time

Time zones can be represented as area/location or as latitude/longitude. I've included the latter to cover situations that area/location can't (mostly for the case of politically unstable areas). There are few latitude/longitude time zone databases in use today, but it's my hope that this will soon change.


### Arrays

Array types are limited to octet-based types because arrays are just an optimization of the list. For strings and binary blobs, this optimization makes sense. For larger data types, it starts to bloat the format without contributing much to the "80% of use cases" goal.


### Float

IEEE754 binary floating point is only supported for 32 and 64 bit sizes. This seems a good compromise, as most real-world values are in these sizes already. Decimal float supports arbitrary precision and length, and can often be stored in fewer bytes. See also: [how much precision do you need?](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md#how-much-precision-do-you-need)



Predefined Metadata Keys
------------------------

The intent of predefined metadata keys is to codify a standard way of representing the most common types of metadata found in data structures. There are [so many metadata standards](https://en.wikipedia.org/wiki/Metadata_standard) that it's difficult to come up with a predefined set that is general enough for ad-hoc data formats like CBE and CTE. "Creation Time", for example, is a lot more generally useful than "Aspect Ratio". The question is where to draw the line. I doubt that we're ever going to be able to answer this to everyone's satisfaction, but it doesn't hurt to try.

The current list is [here](common-generic-metadata.md)



Maximums
--------

The only hard maximum is the maximum container depth. It's left to the implementations to decide upon maximum document byte length, maximum number of objects, etc. This feels like a good compromise position considering that "ridiculous" documents seem to be rather uncommon in the JSON world despite it having no official restrictions at all.



Binary Format Design
--------------------

### Goals

 * Small size
 * Simple to process
 * 1:1 type compatible with the text format


### Byte-Oriented

The binary format is byte-oriented. Sub-byte data tends to invite too much complexity, to the point that the defect potential and the algorithmic complexity outweighs the marginal transmission size benefits. Some sub-byte data is allowed for specific data type payloads, but this is kept to a minimum.


### Variable Sized Data Fields

The early prototypes used only fixed length data because it's very simple to encode and decode, but it turned out to be too wasteful. Later revisions took into account how data tends to exist in the real world:

 * Smaller integers are more common than larger integers.
 * Numbers in the range -100 to 100 are more common than those outside of that range.
 * Floating point values rarely need to use many significant digits or exponential range.
 * 99% of dates are within 50 years of 2000.
 * Sub-second precision requirements vary greatly.
 * Short string values are much more common than long string values.

Everything in the binary format (including the layout of type fields) was designed to take advantage of these tendencies. The following technologies were developed/refined specifically for the binary format:

 * [Variable Length Quantity](https://github.com/kstenerud/vlq/blob/master/vlq-specification.md)
 * [Compact Float](https://github.com/kstenerud/compact-float/blob/master/compact-float-specification.md)
 * [Compact Time](https://github.com/kstenerud/compact-time/blob/master/compact-time-specification.md)


### Type Field Design

 * Values from -100 to +100 are encoded directly into the type field, such that they can be decoded directly as 8-bit two's complement values.

 * Integer signs are stored separately, in the type field itself. This allows "unsigned" types to be stored as positive integers, and increases the range of signed integers by 1 bit at no extra cost to the format itself (you'd still need 2 types per integer size to differentiate signed from unsigned).

 * The types are laid out such that all positive integers have a 0 in the LSB of the type field, and all negative integers have a 1 in the LSB of the type field. As well, the boolean `true` and `false` type fields differ only in the lowest bit (1 for true, 0 for false).

 * The selection of integer types allows integers to be stored in 1, 2, 3, 4, 5, and 8+ bytes (including type field). Since smaller integer values are more common than larger ones, this spread captures the widest range without bloating the format too much.

 * There are 16 "convenience" string types, whereby the upper nybble is 8 and the lower nybble is the string length. Short string values are very common in the real world, and losing an extra byte to record the length for every short string adds up fast.

 * A number of type fields have been left unused (reserved) to support future expansion.


### Padding

I'm still not 100% convinced of padding's usefulness. The idea was to allow tightly coupled systems that can anticipate each others' optimal data alignment to shave off even more processing cost in time critical applications.

Another potential use could be putting a series of padding values in as sequence points in a data stream for synchronization purposes.



Text Format Design
------------------

### Goals

Since the binary format already handles the size and efficiency goals, the text format can focus on convenience and readability. It's not enough to just make it "readable" by humans; it must also be pleasing to use, such that humans will actually *want* to use it over other formats.


### Aesthetics

Most of the symbols and structures of the text format have been lifted from popular programming languages, so that humans can feel comfortable and familiar using them.

Whitespace is the object separator because keeping track of commas is annoying.

The whitespace rules are very liberal in general. How you align things visually shouldn't cause the document to be rejected by a parser.

Bare strings are allowed (even though it complicates parsing) because putting everything in double-quotes gets tedious.

Integers and floats can be written in different bases (base 2, 8, 10, and 16 for integers, and base 10 and 16 for floats).

The canonical encoding is entirely in lowercase, with some exceptions where necessary.


### Other

I played with other binary encodings besides hex (base64, base85), but they complicate the format, and since the text format (CTE) is not designed for transmission or storage, reducing the document size is not a concern.

The date format is completely new, designed to avoid the problems with ISO8601.
