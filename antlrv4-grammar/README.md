CTE Grammar for ANTLR 4
=======================

This is the initial, lightly tested [CTE](../cte-specification.md) grammar for Antlr v4.

TODO:
- Define the proper character ranges (see the bottom of [CTELexer.g4](CTELexer.g4))
- Figure out how to do [verbatim sequences](../cte-specification.md#verbatim-sequence) in ANTLR.
- Hook it up to the full test suite and verify that it is correct.
- Description of how the parsing works (what to expect, what text needs to be stripped inside the visitor methods, what needs manual intervention).
- Basic parser example maybe? Or just integrate it into [go-concise-encoding](https://github.com/kstenerud/go-concise-encoding/)?
