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

// TODO: Disallow version 0 once version 1 is released
VERSION: C [01] -> pushMode(NORMAL);


// ========
mode NORMAL;
// ========

WS: WHITESPACE -> skip;

LIST_BEGIN:           '[';
LIST_END:             ']';
MAP_BEGIN:            '{';
MAP_END:              '}';
KV_SEPARATOR:         '=';
NODE_BEGIN:           '(';
EDGE_BEGIN:           '@(';
EDGE_NODE_END:        ')';

ARRAY_BIT_BEGIN:  '|' B -> pushMode(ARRAY_BIT);

ARRAY_I8_BEGIN:   '|' I '8' CHAR_WS -> pushMode(ARRAY_I);
ARRAY_I8B_BEGIN:  '|' I '8' B -> pushMode(ARRAY_I_B);
ARRAY_I8O_BEGIN:  '|' I '8' O -> pushMode(ARRAY_I_O);
ARRAY_I8X_BEGIN:  '|' I '8' X -> pushMode(ARRAY_I_X);
ARRAY_U8_BEGIN:   '|' U '8' CHAR_WS -> pushMode(ARRAY_U);
ARRAY_U8B_BEGIN:  '|' U '8' B -> pushMode(ARRAY_U_B);
ARRAY_U8O_BEGIN:  '|' U '8' O -> pushMode(ARRAY_U_O);
ARRAY_U8X_BEGIN:  '|' U '8' X -> pushMode(ARRAY_U_X);

ARRAY_I16_BEGIN:  '|' I '16' CHAR_WS -> pushMode(ARRAY_I);
ARRAY_I16B_BEGIN: '|' I '16' B -> pushMode(ARRAY_I_B);
ARRAY_I16O_BEGIN: '|' I '16' O -> pushMode(ARRAY_I_O);
ARRAY_I16X_BEGIN: '|' I '16' X -> pushMode(ARRAY_I_X);
ARRAY_U16_BEGIN:  '|' U '16' CHAR_WS -> pushMode(ARRAY_U);
ARRAY_U16B_BEGIN: '|' U '16' B -> pushMode(ARRAY_U_B);
ARRAY_U16O_BEGIN: '|' U '16' O -> pushMode(ARRAY_U_O);
ARRAY_U16X_BEGIN: '|' U '16' X -> pushMode(ARRAY_U_X);

ARRAY_I32_BEGIN:  '|' I '32' CHAR_WS -> pushMode(ARRAY_I);
ARRAY_I32B_BEGIN: '|' I '32' B -> pushMode(ARRAY_I_B);
ARRAY_I32O_BEGIN: '|' I '32' O -> pushMode(ARRAY_I_O);
ARRAY_I32X_BEGIN: '|' I '32' X -> pushMode(ARRAY_I_X);
ARRAY_U32_BEGIN:  '|' U '32' CHAR_WS -> pushMode(ARRAY_U);
ARRAY_U32B_BEGIN: '|' U '32' B -> pushMode(ARRAY_U_B);
ARRAY_U32O_BEGIN: '|' U '32' O -> pushMode(ARRAY_U_O);
ARRAY_U32X_BEGIN: '|' U '32' X -> pushMode(ARRAY_U_X);

ARRAY_I64_BEGIN:  '|' I '64' CHAR_WS -> pushMode(ARRAY_I);
ARRAY_I64B_BEGIN: '|' I '64' B -> pushMode(ARRAY_I_B);
ARRAY_I64O_BEGIN: '|' I '64' O -> pushMode(ARRAY_I_O);
ARRAY_I64X_BEGIN: '|' I '64' X -> pushMode(ARRAY_I_X);
ARRAY_U64_BEGIN:  '|' U '64' CHAR_WS -> pushMode(ARRAY_U);
ARRAY_U64B_BEGIN: '|' U '64' B -> pushMode(ARRAY_U_B);
ARRAY_U64O_BEGIN: '|' U '64' O -> pushMode(ARRAY_U_O);
ARRAY_U64X_BEGIN: '|' U '64' X -> pushMode(ARRAY_U_X);

ARRAY_F16_BEGIN:  '|' F '16' CHAR_WS -> pushMode(ARRAY_F);
ARRAY_F16X_BEGIN: '|' F '16' X -> pushMode(ARRAY_F_X);
ARRAY_F32_BEGIN:  '|' F '32' CHAR_WS -> pushMode(ARRAY_F);
ARRAY_F32X_BEGIN: '|' F '32' X -> pushMode(ARRAY_F_X);
ARRAY_F64_BEGIN:  '|' F '64' CHAR_WS -> pushMode(ARRAY_F);
ARRAY_F64X_BEGIN: '|' F '64' X -> pushMode(ARRAY_F_X);

CUSTOM_BINARY_BEGIN: '|' C B -> pushMode(CUSTOM_BINARY);
MEDA_BEGIN:          '|' CHAR_MEDIA_TYPE+ -> pushMode(MEDIA);

CUSTOM_TEXT: '|' C T (ESCAPE | CHAR_CUSTOM_TEXT)* '|';

NULL:  'null';
NAN:   'nan';
SNAN:  'snan';
INF:   NEG? 'inf';
TRUE:  'true';
FALSE: 'false';

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

DATE: DATE_PORTION ('/' TIME_PORTION)?; 
TIME: TIME_PORTION; 

UID
   : HEX HEX HEX HEX HEX HEX HEX HEX '-'
     HEX HEX HEX HEX '-'
     HEX HEX HEX HEX '-'
     HEX HEX HEX HEX '-'
     HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX HEX
   ;

STRING: '"' (ESCAPE | CHAR_QUOTED_STRING)* '"';
RID: '@"' (ESCAPE | CHAR_QUOTED_STRING)* '"';
REMOTE_REF: '$"' (ESCAPE | CHAR_QUOTED_STRING)* '"';

MARKER: '&' SEQ_MARKER_ID ':';
REFRENCE: '$' SEQ_MARKER_ID;

BLOCK_COMMENT: '/*' (BLOCK_COMMENT|.)*? '*/' -> channel(HIDDEN);
LINE_COMMENT:  '//' .*? '\n' -> channel(HIDDEN);


// ===========
mode ARRAY_BIT;
// ===========

ARRAY_BIT_BITS: BIT+;
ARRAY_BIT_END:  SEQ_ARRAY_END -> popMode;
ARRAY_BIT_WS:   WHITESPACE -> skip;


// =========
mode ARRAY_I;
// =========

ARRAY_I_ELEM_B: NEG? PREFIX_BIN DIGITS_BIN;
ARRAY_I_ELEM_O: NEG? PREFIX_OCT DIGITS_OCT;
ARRAY_I_ELEM_H: NEG? PREFIX_HEX DIGITS_HEX;
ARRAY_I_ELEM_D: NEG? DIGITS_DEC;
ARRAY_I_END:  SEQ_ARRAY_END -> popMode;
ARRAY_I_WS:   WHITESPACE -> skip;


// =========
mode ARRAY_U;
// =========

ARRAY_U_ELEM_B: PREFIX_BIN DIGITS_BIN;
ARRAY_U_ELEM_O: PREFIX_OCT DIGITS_OCT;
ARRAY_U_ELEM_H: PREFIX_HEX DIGITS_HEX;
ARRAY_U_ELEM_D: DIGITS_DEC;
ARRAY_U_END:  SEQ_ARRAY_END -> popMode;
ARRAY_U_WS:   WHITESPACE -> skip;


// =========
mode ARRAY_F;
// =========

ARRAY_F_ELEM_D: FLOAT_D;
ARRAY_F_ELEM_H: FLOAT_H_PREFIX;
ARRAY_F_END:  SEQ_ARRAY_END -> popMode;
ARRAY_F_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_F_X;
// ===========

ARRAY_F_X_ELEM: FLOAT_H_NOPREFIX;
ARRAY_F_X_END:  SEQ_ARRAY_END -> popMode;
ARRAY_F_X_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_I_B;
// ===========

ARRAY_I_B_ELEM: NEG? DIGITS_BIN;
ARRAY_I_B_END:  SEQ_ARRAY_END -> popMode;
ARRAY_I_B_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_I_O;
// ===========

ARRAY_I_O_ELEM: NEG? DIGITS_OCT;
ARRAY_I_O_END:  SEQ_ARRAY_END -> popMode;
ARRAY_I_O_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_I_X;
// ===========

ARRAY_I_X_ELEM: NEG? DIGITS_HEX;
ARRAY_I_X_END:  SEQ_ARRAY_END -> popMode;
ARRAY_I_X_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_U_B;
// ===========

ARRAY_U_B_ELEM: DIGITS_BIN;
ARRAY_U_B_END:  SEQ_ARRAY_END -> popMode;
ARRAY_U_B_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_U_O;
// ===========

ARRAY_U_O_ELEM: DIGITS_OCT;
ARRAY_U_O_END:  SEQ_ARRAY_END -> popMode;
ARRAY_U_O_WS:   WHITESPACE -> skip;


// ===========
mode ARRAY_U_X;
// ===========

ARRAY_U_X_ELEM: DIGITS_HEX;
ARRAY_U_X_END:  SEQ_ARRAY_END -> popMode;
ARRAY_U_X_WS:   WHITESPACE -> skip;


// ===============
mode CUSTOM_BINARY;
// ===============

CUSTOM_BINARY_ELEM: BYTE_HEX;
CUSTOM_BINARY_END:  SEQ_ARRAY_END -> popMode;
CUSTOM_BINARY_WS:   WHITESPACE -> skip;


// =======
mode MEDIA;
// =======

MEDIA_ELEM: BYTE_HEX;
MEDIA_END:  SEQ_ARRAY_END -> popMode;
MEDIA_WS:   WHITESPACE -> skip;


// =========
// Fragments
// =========

fragment WHITESPACE: CHAR_WS+;

fragment B: [bB];
fragment C: [cC];
fragment E: [eE];
fragment F: [fF];
fragment I: [iI];
fragment O: [oO];
fragment P: [pP];
fragment T: [tT];
fragment U: [uU];
fragment X: [xX];

fragment SEQ_ARRAY_END: '|';
fragment SEQ_MARKER_ID: CHAR_MARKER_ID+;

fragment PREFIX_BIN:    '0' B;
fragment PREFIX_OCT:    '0' O;
fragment PREFIX_HEX:    '0' X;
fragment FRACTION_DEC:  RADIX DIGITS_DEC;
fragment FRACTION_HEX:  RADIX DIGITS_HEX;
fragment RADIX:         [.];
fragment NEG:           '-';
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

fragment FLOAT_D
   : NEG? DIGITS_DEC (FRACTION_DEC
         | EXPONENT_DEC
         | FRACTION_DEC EXPONENT_DEC
      )
   ;
fragment FLOAT_H_PREFIX
   : NEG? PREFIX_HEX DIGITS_HEX (FRACTION_HEX
         | EXPONENT_HEX
         | FRACTION_HEX EXPONENT_HEX
      )
   ;
fragment FLOAT_H_NOPREFIX
   : NEG? DIGITS_HEX (FRACTION_HEX
         | EXPONENT_HEX
         | FRACTION_HEX EXPONENT_HEX
      )
   ;

fragment DATE_PORTION
   : NEG? DIGITS_DEC '-' DEC DEC '-' DEC DEC
   ; 

fragment TIME_PORTION
   : DEC DEC? ':' DEC DEC ':' DEC DEC (RADIX DEC+)? TIME_ZONE?
   ; 

fragment TIME_ZONE
   : TZ_AREALOC
   | TZ_LATLONG
   | TZ_OFFSET
   ;

fragment TZ_AREALOC: '/' [A-Z] CHAR_AREA_LOC*;

fragment TZ_LATLONG
   : '/' NEG? DEC DEC? (RADIX DEC DEC?)?
     '/' NEG? DEC DEC? DEC? (RADIX DEC DEC?)? 
   ;

fragment TZ_OFFSET: [+-] DEC DEC DEC DEC;

fragment CHAR_WS: [ \t\n\r];

fragment CHAR_MARKER_ID
   : [0-9a-zA-Z.\-_] // TODO
   ;

fragment CHAR_AREA_LOC
   : ~[\u0000-\u0020] // TODO
   ;

fragment CHAR_MEDIA_TYPE
   : ~[\u0000-\u0020] // TODO
   ;

fragment CHAR_QUOTED_STRING
   : ~[\u0000-\u001F"\\] // TODO
   ;

fragment CHAR_CUSTOM_TEXT
   : ~[\u0000-\u001F|\\] // TODO
   ;

fragment ESCAPE
   : '\\' ([nrt"*/\\|_-]
         | '\r' '\n'
         | '\n'
         | HEX+ '.'
         | '.' // TODO
      )
   ;
