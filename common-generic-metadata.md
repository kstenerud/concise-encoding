Common Generic Metadata
=======================

Metadata is useful for understanding the context surrounding the data being transmitted, but only if the receiving party knows what the metadata means.

This document describes a set of predefined metadata key names and types for use in generic data encoding schemes. A common set of names and types helps promote interoperability when dealing with data from disparate or unknown systems.



Terms
-----

The following terms have specific meanings in this specification:

| Term         | Meaning                                                                                                               |
| ------------ | --------------------------------------------------------------------------------------------------------------------- |
| Must (not)   | If this directive is not adhered to, the document or implementation is invalid/non-conformant.                        |
| Should (not) | Every effort should be made to follow this directive, but the document/implementation is still valid if not followed. |
| May (not)    | It is up to the implementation to decide whether to do something or not.                                              |



General Rules
-------------

 * Implementations should make use of these metadata keys whenever possible to maximize interoperability between systems.

 * Implementations are not required to implement all metadata keys; they may ignore the keys they're not interested in.

 * All string-based metadata keys beginning with underscore (`_`) are reserved. Only [predefined keys](#predefined-keys) may begin with an underscore.



Predefined Keys
---------------

The following predefined metadata keys should be used for the specified type of information. Implementations must accept both long and short versions of the keys they implement.

| Long Key                | Short Key | Type                | Contents                                                           |
| ----------------------- | --------- | ------------------- | ------------------------------------------------------------------ |
| `_access_time`          | `_at`     | Timestamp           | Last access time                                                   |
| `_attributes`           | `_a`      | Map<String:Any>     | Attributes (free-form)                                             |
| `_copyright`            | `_cr`     | String or URI       | Holder of copyright over data (Name or URI)                        |
| `_creation_time`        | `_ct`     | Timestamp           | Creation time                                                      |
| `_creator`              | `_c`      | List<String or URI> | Creator(s) of the data                                             |
| `_data_type`            | `_dt`     | String              | https://www.iana.org/assignments/media-types/media-types.xhtml     |
| `_description`          | `_d`      | String              | Description (free-form)                                            |
| `_human_language`       | `_hl`     | String              | Human language, as an ISO 639 alpha-2 or alpha-3 code              |
| `_identifier`           | `_id`     | Any                 | Identifier (free-form)                                             |
| `_language`             | `_l`      | String              | The [programming language](#language-name) that generated the data |
| `_license`              | `_li`     | URI                 | Pointer to the license for this data                               |
| `_modification_time`    | `_mt`     | Timestamp           | Last modification time                                             |
| `_origin`               | `_o`      | List<String or URI> | Origin(s) of this data                                             |
| `_schema`               | `_s`      | URI                 | [Schema](#schema) describing how to interpret the data             |
| `_specification`        | `_sp`     | URI                 | Human-readable specification about the data                        |
| `_tags`                 | `_ta`     | List<String>        | Set of tags associated with this data (free-form)                  |
| `_type`                 | `_t`      | String              | Name of the type/class/struct that this structure represents       |

All other string-based metadata keys beginning with `_` are reserved for future expansion, and must not be used as metadata keys for any purpose.

TODO: `_schema_format` to specify the format of the schema?


Definitions
-----------


### Language Name

The programming language name must match a name from the [Linquist languages list](https://github.com/github/linguist/blob/master/lib/linguist/languages.yml), converted to lowercase, with spaces replaced by dashes (`-`).

Example:

    ...
    Visual Basic:
      type: programming
    ...

Language name = "Visual Basic". Converted to lowercase, spaces replaced by dashes: `visual-basic`


### Schema

This must be a machine-readable schema file that is suitable for the data format of the object that follows.

**Note:** No schema format has been defined yet for Concise Encoding.



Example
-------

An example of metadata in a [CTE](cte-specification.md) document:

    v1
    // Metadata for the entire document
    (
        _ct = 2017.01.14-15:22:41/Z
        _mt = 2019.08.17-12:44:31/Z
        _at = 2019.09.14-09:55:00/Z
        _l = cobol
    )
    {
        ( _o = [u"https://all-your-data-are-belong-to-us.com"] )
        records = [
            // Metadata for "ABC Corp" record
            (
                _t = CUSREC
                _ct = 2019.05.14-10:22:55/Z
                _ta = ["longtime client" "big purchases"]
            )
            {
                client = "ABC Corp"
                amount = 10499.28
                due = 2020.05.14
            }

            // Metadata for "XYZ Corp" record
            (
                _t = CUSREC
                _ct=2019.02.30-09:00:01/Z
                _mt=2019.08.17-12:44:31/Z
            )
            {
                client = "XYZ Corp"
                amount = 3994.01
                due = 2020.08.30
            }
        ]

        ( _s = u"schemas-r-us.com/schemas/v4/super-duper.schema" )
        external_packet = h"934f3ba6433000000b14"
    }



License
-------

Copyright (c) 2019 Karl Stenerud. All rights reserved.

Distributed under the Creative Commons Attribution License: https://creativecommons.org/licenses/by/4.0/legalcode
License deed: https://creativecommons.org/licenses/by/4.0/
