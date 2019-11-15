Request for Comments
====================

This is a request for comments regarding the [CBE](https://github.com/kstenerud/concise-binary-encoding/blob/master/cbe-specification.md) and [CTE](https://github.com/kstenerud/concise-text-encoding/blob/master/cte-specification.md) prototype specifications. It is not an IETF RFC, but rather an actual **request** for **comments** from the community at large in order to work out bugs, inconsistencies, and oversights in the specifications. Version 2 should ideally come out in a few decades, not in a few months ;-)

The purpose of CBE and CTE is to provide type-compatible binary and text formats capable of storing semi-structured hierarchical data, similar to what was accomplished with XML and JSON. CBE and CTE endeavor to directly support the most common data types, while also avoiding the complexities that sank other formats in the past. What CBE/CTE aims for is a good balance between types, expressivity, and complexity in a way that supports 80% of use cases. CBE further endeavors to do so in a compact binary representation that is easy to encode and decode.



Remaining TODO
--------------

While the specifications are mostly done, the reference implementations aren't completely finished yet. As I implement them, I'll probably discover more things that need to be updated or further refined in the specs.

#### TODO:

* Update CBE implementations to latest specification
* Implement CTE in `C` and `go` (partially complete)



How to Comment
--------------

To start a comment and discussion, please [open an issue in the Concise Encoding repository](https://github.com/kstenerud/concise-encoding/issues). Discussion will happen in the issue's comments and is open to all.



Thank You
---------

I really appreciate all constructive criticism. These encoding formats are intended to be used by all, but I can only effectively visualize my own use cases unless I'm given other scenarios to consider, so it's important that you get involved and share your ideas!
