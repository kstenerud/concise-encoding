// Copyright 2021 Karl Stenerud
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

lexer grammar CTELexer;

@members {

/* Verbatim sequences are beyond the normal capabilities of Antlr, so we must
 * build additional support in the target language to:
 *  - Record the verbatim sentinel value for later use
 *  - Check to see if we are at the closing sentinel value yet
 *  - Consume the closing sentinel value
 *
 * For example, given the verbatim sequence: \.@@ some text@@
 *  - First, record the sentinel "@@" and skip the required whitespace char
 *  - Next, consume the text "some text" while looking for the closing sentinel
 *  - Finally, consume the closing sentinel "@@"
 */

type LexerContext interface {
	RecordVerbatimSentinel(text string)
	IsAtVerbatimSentinel(stream antlr.CharStream) bool
	IsSentinelChar(stream antlr.CharStream) bool
}

type CTELexerContext struct {
	verbatimSentinel string
	verbatimIndex    int
}

func (_this *CTELexerContext) RecordVerbatimSentinel(text string) {
	_this.verbatimSentinel = text
}

func (_this *CTELexerContext) IsAtVerbatimSentinel(stream antlr.CharStream) bool {
    for n := 0; n < len(_this.verbatimSentinel); n++ {
		if stream.LA(n+1) != int(_this.verbatimSentinel[n]) {
		  return false;
		}
	  }
  
	  _this.verbatimIndex = 0;
	  return true;
  }

func (_this *CTELexerContext) IsSentinelChar(stream antlr.CharStream) bool {
    index := _this.verbatimIndex;
    if index >= len(_this.verbatimSentinel) {
      return false;
    }
    _this.verbatimIndex++;
    return stream.LA(1) == int(_this.verbatimSentinel[index]);
}

type CTEContextualInterpreter struct {
	antlr.ILexerATNSimulator
	CTELexerContext
}

func NewContextualCTELexer(input antlr.CharStream) *CTELexer {
	lexer := NewCTELexer(input)
	lexer.Interpreter = &CTEContextualInterpreter{ILexerATNSimulator: lexer.Interpreter}
	return lexer
}

func recordVerbatimSentinel(lexer *CTELexer) {
	lexer.Interpreter.(LexerContext).RecordVerbatimSentinel(lexer.GetText())
}

func isAtVerbatimSentinel(lexer *CTELexer) bool {
	return lexer.Interpreter.(LexerContext).IsAtVerbatimSentinel(lexer.GetInputStream())
}

func isSentinelChar(lexer *CTELexer) bool {
	return lexer.Interpreter.(LexerContext).IsSentinelChar(lexer.GetInputStream())
}

}

// ============================================================================

// ============
// Initial Mode
// ============

VERSION: C CTE_VERSION -> mode(MODE_NORMAL);

CTE_VERSION: '1';


// =============
mode MODE_NORMAL;
// =============

WSL: WHITESPACE_NL;
COMMENT_LINE:  LINE_COMMENT;
COMMENT_BLOCK: BLOCK_COMMENT;

LIST_BEGIN:        '[';
LIST_END:          ']';
MAP_BEGIN:         '{';
MAP_OR_RECORD_END: '}';
KV_SEPARATOR:      '=';
NODE_BEGIN:        '(';
EDGE_OR_NODE_END:  ')';

NULL:  N U L L;
TRUE:  T R U E;
FALSE: F A L S E;

PINT_BIN: PREFIX_BIN DIGITS_BIN;
NINT_BIN: NEG PINT_BIN;
PINT_DEC: DIGITS_DEC;
NINT_DEC: NEG PINT_DEC;
PINT_OCT: PREFIX_OCT DIGITS_OCT;
NINT_OCT: NEG PINT_OCT;
PINT_HEX: PREFIX_HEX DIGITS_HEX;
NINT_HEX: NEG PINT_HEX;

FLOAT_DEC: FLOAT_D;
FLOAT_HEX: FLOAT_H_PREFIX;
FLOAT_INF: INF;
FLOAT_NINF: NEG INF;
FLOAT_NAN:  NAN;
FLOAT_SNAN: SNAN;

DATE: DATE_PORTION ('/' TIME_PORTION)?; 
TIME: TIME_PORTION; 

VALUE_UID: UID;

STRING_BEGIN: '"' -> pushMode(MODE_STRING);
RREF_BEGIN:   '$"' -> pushMode(MODE_STRING);

MARKER:    '&' IDENTIFIER ':';
REFERENCE: '$' IDENTIFIER;

RECORD_TYPE_END: '>';

RID_BEGIN:         '@"' -> pushMode(MODE_STRING);
EDGE_BEGIN:        '@(';
RECORD_TYPE_BEGIN: '@' IDENTIFIER '<';
RECORD_BEGIN:      '@' IDENTIFIER '{';

ARRAY_TYPE_I8:         '@' I '8'    '[' -> pushMode(MODE_ARRAY_I);
ARRAY_TYPE_I8B:        '@' I '8'  B '[' -> pushMode(MODE_ARRAY_I_B);
ARRAY_TYPE_I8O:        '@' I '8'  O '[' -> pushMode(MODE_ARRAY_I_O);
ARRAY_TYPE_I8X:        '@' I '8'  X '[' -> pushMode(MODE_ARRAY_I_X);
ARRAY_TYPE_I16:        '@' I '16'   '[' -> pushMode(MODE_ARRAY_I);
ARRAY_TYPE_I16B:       '@' I '16' B '[' -> pushMode(MODE_ARRAY_I_B);
ARRAY_TYPE_I16O:       '@' I '16' O '[' -> pushMode(MODE_ARRAY_I_O);
ARRAY_TYPE_I16X:       '@' I '16' X '[' -> pushMode(MODE_ARRAY_I_X);
ARRAY_TYPE_I32:        '@' I '32'   '[' -> pushMode(MODE_ARRAY_I);
ARRAY_TYPE_I32B:       '@' I '32' B '[' -> pushMode(MODE_ARRAY_I_B);
ARRAY_TYPE_I32O:       '@' I '32' O '[' -> pushMode(MODE_ARRAY_I_O);
ARRAY_TYPE_I32X:       '@' I '32' X '[' -> pushMode(MODE_ARRAY_I_X);
ARRAY_TYPE_I64:        '@' I '64'   '[' -> pushMode(MODE_ARRAY_I);
ARRAY_TYPE_I64B:       '@' I '64' B '[' -> pushMode(MODE_ARRAY_I_B);
ARRAY_TYPE_I64O:       '@' I '64' O '[' -> pushMode(MODE_ARRAY_I_O);
ARRAY_TYPE_I64X:       '@' I '64' X '[' -> pushMode(MODE_ARRAY_I_X);

ARRAY_TYPE_U8:         '@' U '8'    '[' -> pushMode(MODE_ARRAY_U);
ARRAY_TYPE_U8B:        '@' U '8'  B '[' -> pushMode(MODE_ARRAY_U_B);
ARRAY_TYPE_U8O:        '@' U '8'  O '[' -> pushMode(MODE_ARRAY_U_O);
ARRAY_TYPE_U8X:        '@' U '8'  X '[' -> pushMode(MODE_ARRAY_U_X);
ARRAY_TYPE_U16:        '@' U '16'   '[' -> pushMode(MODE_ARRAY_U);
ARRAY_TYPE_U16B:       '@' U '16' B '[' -> pushMode(MODE_ARRAY_U_B);
ARRAY_TYPE_U16O:       '@' U '16' O '[' -> pushMode(MODE_ARRAY_U_O);
ARRAY_TYPE_U16X:       '@' U '16' X '[' -> pushMode(MODE_ARRAY_U_X);
ARRAY_TYPE_U32:        '@' U '32'   '[' -> pushMode(MODE_ARRAY_U);
ARRAY_TYPE_U32B:       '@' U '32' B '[' -> pushMode(MODE_ARRAY_U_B);
ARRAY_TYPE_U32O:       '@' U '32' O '[' -> pushMode(MODE_ARRAY_U_O);
ARRAY_TYPE_U32X:       '@' U '32' X '[' -> pushMode(MODE_ARRAY_U_X);
ARRAY_TYPE_U64:        '@' U '64'   '[' -> pushMode(MODE_ARRAY_U);
ARRAY_TYPE_U64B:       '@' U '64' B '[' -> pushMode(MODE_ARRAY_U_B);
ARRAY_TYPE_U64O:       '@' U '64' O '[' -> pushMode(MODE_ARRAY_U_O);
ARRAY_TYPE_U64X:       '@' U '64' X '[' -> pushMode(MODE_ARRAY_U_X);

ARRAY_TYPE_F16:        '@' F '16'   '[' -> pushMode(MODE_ARRAY_F);
ARRAY_TYPE_F16X:       '@' F '16' X '[' -> pushMode(MODE_ARRAY_F_X);
ARRAY_TYPE_F32:        '@' F '32'   '[' -> pushMode(MODE_ARRAY_F);
ARRAY_TYPE_F32X:       '@' F '32' X '[' -> pushMode(MODE_ARRAY_F_X);
ARRAY_TYPE_F64:        '@' F '64'   '[' -> pushMode(MODE_ARRAY_F);
ARRAY_TYPE_F64X:       '@' F '64' X '[' -> pushMode(MODE_ARRAY_F_X);

ARRAY_TYPE_UID:        '@' U I D '['    -> pushMode(MODE_ARRAY_UID);
ARRAY_TYPE_BIT:        '@' B '['        -> pushMode(MODE_ARRAY_BIT);

ARRAY_TYPE_CUSTOM_BIN: '@' CUSTOM_TYPE '[' -> pushMode(MODE_BYTES);
ARRAY_TYPE_CUSTOM_TXT: '@' CUSTOM_TYPE '"' -> pushMode(MODE_STRING);
ARRAY_TYPE_MEDIA_BIN:  '@' MEDIA_TYPE '['  -> pushMode(MODE_BYTES);
ARRAY_TYPE_MEDIA_TXT:  '@' MEDIA_TYPE '"'  -> pushMode(MODE_STRING);


// ==============
mode MODE_ARRAY_I;
// ==============

ARRAY_I_ELEM_B:  NEG? PREFIX_BIN DIGITS_BIN;
ARRAY_I_ELEM_O:  NEG? PREFIX_OCT DIGITS_OCT;
ARRAY_I_ELEM_H:  NEG? PREFIX_HEX DIGITS_HEX;
ARRAY_I_ELEM_D:  NEG? DIGITS_DEC;
ARRAY_I_END:     ARRAY_END -> popMode;
ARRAY_I_WSL:     WHITESPACE_NL;


// ==============
mode MODE_ARRAY_U;
// ==============

ARRAY_U_ELEM_B:  PREFIX_BIN DIGITS_BIN;
ARRAY_U_ELEM_O:  PREFIX_OCT DIGITS_OCT;
ARRAY_U_ELEM_H:  PREFIX_HEX DIGITS_HEX;
ARRAY_U_ELEM_D:  DIGITS_DEC;
ARRAY_U_END:     ARRAY_END -> popMode;
ARRAY_U_WSL:     WHITESPACE_NL;


// ==============
mode MODE_ARRAY_F;
// ==============

ARRAY_F_ELEM_D:  FLOAT_OR_INT_D;
ARRAY_F_ELEM_H:  FLOAT_OR_INT_H_PREFIX;
ARRAY_F_NAN:     NAN;
ARRAY_F_SNAN:    SNAN;
ARRAY_F_INF:     INF;
ARRAY_F_NINF:    NEG INF;
ARRAY_F_END:     ARRAY_END -> popMode;
ARRAY_F_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_F_X;
// ================

ARRAY_F_X_ELEM:    FLOAT_OR_INT_H_NOPREFIX;
ARRAY_F_X_NAN:     NAN;
ARRAY_F_X_SNAN:    SNAN;
ARRAY_F_X_INF:     INF;
ARRAY_F_X_NINF:    NEG INF;
ARRAY_F_X_END:     ARRAY_END -> popMode;
ARRAY_F_X_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_I_B;
// ================

ARRAY_I_B_ELEM:    NEG? DIGITS_BIN;
ARRAY_I_B_END:     ARRAY_END -> popMode;
ARRAY_I_B_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_I_O;
// ================

ARRAY_I_O_ELEM:    NEG? DIGITS_OCT;
ARRAY_I_O_END:     ARRAY_END -> popMode;
ARRAY_I_O_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_I_X;
// ================

ARRAY_I_X_ELEM:    NEG? DIGITS_HEX;
ARRAY_I_X_END:     ARRAY_END -> popMode;
ARRAY_I_X_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_U_B;
// ================

ARRAY_U_B_ELEM:    DIGITS_BIN;
ARRAY_U_B_END:     ARRAY_END -> popMode;
ARRAY_U_B_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_U_O;
// ================

ARRAY_U_O_ELEM:    DIGITS_OCT;
ARRAY_U_O_END:     ARRAY_END -> popMode;
ARRAY_U_O_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_U_X;
// ================

ARRAY_U_X_ELEM:    DIGITS_HEX;
ARRAY_U_X_END:     ARRAY_END -> popMode;
ARRAY_U_X_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_UID;
// ================

ARRAY_UID_ELEM:    UID;
ARRAY_UID_END:     ARRAY_END -> popMode;
ARRAY_UID_WSL:     WHITESPACE_NL;


// ================
mode MODE_ARRAY_BIT;
// ================

ARRAY_BIT_BITS:    BIT+;
ARRAY_BIT_END:     ARRAY_END -> popMode;
ARRAY_BIT_WSL:     WHITESPACE_NL -> skip;


// ============
mode MODE_BYTES;
// ============

BYTES_ELEM:    BYTE_HEX;
BYTES_END:     ARRAY_END -> popMode;
BYTES_WS:      WHITESPACE_NL;


// =============
mode MODE_STRING;
// =============

STRING_END:      '"' -> popMode;
STRING_ESCAPE:   '\\' -> pushMode(MODE_STRING_ESCAPE);
STRING_CONTENTS: CHAR_QUOTED_STRING;


// ====================
mode MODE_STRING_ESCAPE;
// ====================

VERBATIM_INIT:  '.' -> mode(MODE_VERBATIM);
CODEPOINT_INIT: '[' -> mode(MODE_CODEPOINT);
CONTINUATION:   [\r\n] CHAR_WS* -> popMode;
ESCAPE_CHAR:    ([rRnNtT"*_] | '-' | '/' | '\\') -> popMode;


// ===============
mode MODE_VERBATIM;
// ===============

VERBATIM_SENTINEL: (CHAR_VERBATIM_SENTINEL+ {recordVerbatimSentinel(this)}) -> mode(MODE_VERBATIM_SEPARATOR);


// =========================
mode MODE_VERBATIM_SEPARATOR;
// =========================

VERBATIM_SEPARATOR: ([ \t] | LINE_END) -> mode(MODE_VERBATIM_CONTENTS);


// ========================
mode MODE_VERBATIM_CONTENTS;
// ========================

VERBATIM_EMPTY:    ( {isSentinelChar(this)}? CHAR_VERBATIM_SENTINEL )+ -> popMode;
VERBATIM_CONTENTS: ( {!isAtVerbatimSentinel(this)}? . )+ -> mode(MODE_VERBATIM_END);


// ===================
mode MODE_VERBATIM_END;
// ===================

VERBATIM_END: ( {isSentinelChar(this)}? CHAR_VERBATIM_SENTINEL )+ -> popMode;


// ================
mode MODE_CODEPOINT;
// ================

CODEPOINT: HEX+ ']' -> popMode;


// =========
// Fragments
// =========

fragment WHITESPACE_NL: CHAR_WS+;

fragment A: [aA];
fragment B: [bB];
fragment C: [cC];
fragment D: [dD];
fragment E: [eE];
fragment F: [fF];
fragment I: [iI];
fragment L: [lL];
fragment N: [nN];
fragment O: [oO];
fragment P: [pP];
fragment R: [rR];
fragment S: [sS];
fragment T: [tT];
fragment U: [uU];
fragment X: [xX];

fragment ARRAY_END: ']';
fragment IDENTIFIER: CHAR_IDENTIFIER+;
fragment MEDIA_TYPE: CHAR_MEDIA_TYPE_FIRST CHAR_MEDIA_TYPE_NEXT* '/' CHAR_MEDIA_TYPE_NEXT+;
fragment CUSTOM_TYPE: DEC+;

fragment PREFIX_BIN:    '0' B;
fragment PREFIX_OCT:    '0' O;
fragment PREFIX_HEX:    '0' X;
fragment FRACTION_DEC:  RADIX DIGITS_DEC;
fragment FRACTION_HEX:  RADIX DIGITS_HEX;
fragment RADIX:         '.';
fragment NEG:           '-';
fragment INF:           [Ii] [Nn] [Ff];
fragment NAN:           [Nn] [Aa] [Nn];
fragment SNAN:          [Ss] [Nn] [Aa] [Nn];
fragment BIT:           [0-1];
fragment OCT:           [0-7];
fragment DEC:           [0-9];
fragment HEX:           [0-9a-fA-F];
fragment DIGITS_BIN:    BIT ('_'* BIT)*;
fragment DIGITS_OCT:    OCT ('_'* OCT)*;
fragment DIGITS_DEC:    DEC ('_'* DEC)*;
fragment DIGITS_HEX:    HEX ('_'* HEX)*;
fragment EXPONENT_DEC:  E [+\-]? DIGITS_DEC;
fragment EXPONENT_HEX:  P [+\-]? DIGITS_DEC;

fragment BYTE_HEX: HEX HEX;

fragment UID: HEX HEX HEX HEX HEX HEX HEX HEX '-'
              HEX HEX HEX HEX '-'
              HEX HEX HEX HEX '-'
              HEX HEX HEX HEX '-'
              HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX
            ;

fragment FLOAT_D:                 NEG? DIGITS_DEC
                                  ( FRACTION_DEC
                                  | EXPONENT_DEC
                                  | FRACTION_DEC EXPONENT_DEC
                                  );
fragment FLOAT_OR_INT_D:          NEG? DIGITS_DEC
                                  ( FRACTION_DEC
                                  | EXPONENT_DEC
                                  | FRACTION_DEC EXPONENT_DEC
                                  )?;
fragment FLOAT_H_PREFIX:          NEG? PREFIX_HEX DIGITS_HEX
                                  ( FRACTION_HEX
                                  | EXPONENT_HEX
                                  | FRACTION_HEX EXPONENT_HEX
                                  );
fragment FLOAT_OR_INT_H_PREFIX:   NEG? PREFIX_HEX DIGITS_HEX
                                  ( FRACTION_HEX
                                  | EXPONENT_HEX
                                  | FRACTION_HEX EXPONENT_HEX
                                  )?;
fragment FLOAT_H_NOPREFIX:        NEG? DIGITS_HEX
                                  ( FRACTION_HEX
                                  | EXPONENT_HEX
                                  | FRACTION_HEX EXPONENT_HEX
                                  );
fragment FLOAT_OR_INT_H_NOPREFIX: NEG? DIGITS_HEX
                                  ( FRACTION_HEX
                                  | EXPONENT_HEX
                                  | FRACTION_HEX EXPONENT_HEX
                                  )?;

fragment DATE_PORTION: NEG? DIGITS_DEC '-' DEC DEC? '-' DEC DEC?;
fragment TIME_PORTION: DEC DEC? ':' DEC DEC ':' DEC DEC (RADIX DEC DEC? DEC? DEC? DEC? DEC? DEC? DEC? DEC?)? TIME_ZONE?;
fragment TIME_ZONE:    TZ_AREALOC | TZ_LATLONG | TZ_OFFSET;
fragment TZ_AREALOC:   '/' CHAR_AREA_LOC_FIRST CHAR_AREA_LOC_NEXT*;
fragment TZ_LATLONG:   '/' NEG? DEC DEC? (RADIX DEC DEC?)?
                       '/' NEG? DEC DEC? DEC? (RADIX DEC DEC?)? 
                       ;
fragment TZ_OFFSET:    [+-] DEC DEC DEC DEC;

fragment CHAR_WS: [ \t\n\r];
fragment LINE_END: '\n' | '\r\n';

fragment CHAR_IDENTIFIER: [\p{Cf}\p{L}\p{M}\p{N}_.-];

fragment CHAR_AREA_LOC_FIRST: [A-Z];
fragment CHAR_AREA_LOC_NEXT:  [a-zA-Z0-9_-] | '.' | '/' | '+';

// https://datatracker.ietf.org/doc/html/rfc2045#section-5.1
fragment CHAR_MEDIA_TYPE_FIRST: [a-zA-Z];
fragment CHAR_MEDIA_TYPE_NEXT:  [a-zA-Z0-9!#$%&'*+.^_`|~] | '{' | '}' | '-';

// TODO
// fragment CHAR_QUOTED_STRING: ~[\u0000-\u0008\u000b\u000c\u000e-\u001f"\\]; // TODO
fragment CHAR_QUOTED_STRING: [\p{Cf}\p{L}\p{M}\p{N}\p{P}\p{S}\p{Z}\u0009\u000a\u000d];
// char_string = char_cte ! ('"' | '\\' | delimiter_lookalikes);

fragment CHAR_VERBATIM_SENTINEL: [\p{L}\p{M}\p{N}\p{P}\p{S}];

fragment LINE_COMMENT:  '//' .*? LINE_END;
fragment BLOCK_COMMENT: '/*' ('/'*? BLOCK_COMMENT | ('/'* | '*'*) ~[/*])*? '*'*? '*/';
