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

parser grammar CTEParser;
options { tokenVocab=CTELexer; }

cte: version WSL (recordTypes | separator)* value WSL? EOF;

version: VERSION;

recordTypes: containerRecordType (separator+ containerRecordType)*;

// TODO: Separate keyable, nonnull, etc.
value:   valueString
       | valueRid
       | valueUid
       | valueBool
       | valueNull
       | valueInt
       | valueFloat
       | valueInf
       | valueNinf
       | valueNan
       | valueSnan
       | valueDate
       | valueTime
       | arrayI8
       | arrayI16
       | arrayI32
       | arrayI64
       | arrayU8
       | arrayU16
       | arrayU32
       | arrayU64
       | arrayF16
       | arrayF32
       | arrayF64
       | arrayUid
       | arrayBit
       | customBinary
       | customText
       | mediaBinary
       | mediaText
       | containerMap
       | containerList
       | containerEdge
       | containerNode
       | containerRecord
       | marker
       | reference
       | valueRemoteRef
       ;

separator:    commentLine | commentBlock | WSL;
commentLine:  COMMENT_LINE;
commentBlock: COMMENT_BLOCK;

valueNull: NULL;

valueUid: VALUE_UID;

valueBool: TRUE | FALSE;

valueInt: PINT_BIN
        | NINT_BIN
        | PINT_OCT
        | NINT_OCT
        | PINT_DEC
        | NINT_DEC
        | PINT_HEX
        | NINT_HEX
        ;

valueFloat: FLOAT_DEC | FLOAT_HEX;
valueInf:   FLOAT_INF;
valueNinf:  FLOAT_NINF;
valueNan:   FLOAT_NAN;
valueSnan:  FLOAT_SNAN;

valueDate: DATE;
valueTime: TIME;

valueString:    STRING_BEGIN (stringContents | stringEscape)* STRING_END;
stringContents: STRING_CONTENTS;
stringEscape:   STRING_ESCAPE (verbatimSequence | codepointSequence | escapeChar | CONTINUATION);

verbatimSequence:  VERBATIM_INIT VERBATIM_SENTINEL VERBATIM_SEPARATOR ((verbatimContents VERBATIM_END) | VERBATIM_EMPTY);
verbatimContents:  VERBATIM_CONTENTS;
codepointSequence: CODEPOINT_INIT codepointContents;
codepointContents: CODEPOINT;
escapeChar:        ESCAPE_CHAR;

customText:        customTextBegin (stringContents | stringEscape)* STRING_END;
customBinary:      customBinaryBegin (arrayElemByteX (BYTES_WS arrayElemByteX)*)? BYTES_END;
customTextBegin:   ARRAY_TYPE_CUSTOM_TXT;
customBinaryBegin: ARRAY_TYPE_CUSTOM_BIN;

mediaText:         mediaTextBegin (stringContents | stringEscape)* STRING_END;
mediaBinary:       mediaBinaryBegin (arrayElemByteX (BYTES_WS arrayElemByteX)*)? BYTES_END;
mediaTextBegin:    ARRAY_TYPE_MEDIA_TXT;
mediaBinaryBegin:  ARRAY_TYPE_MEDIA_BIN;


valueRid:       RID_BEGIN (stringContents | stringEscape)* STRING_END;
valueRemoteRef: RREF_BEGIN (stringContents | stringEscape)* STRING_END;

markerID:  MARKER;
marker:    markerID value;
reference: REFERENCE;

containerMap:        MAP_BEGIN       separator* (kvPair (separator+ kvPair)*)? separator* MAP_OR_RECORD_END;
containerList:       LIST_BEGIN      separator* (value  (separator+ value)* )? separator* LIST_END;
containerRecordType: recordTypeBegin separator* (value  (separator+ value)* )? separator* RECORD_TYPE_END;
containerRecord:     recordBegin     separator* (value  (separator+ value)* )? separator* MAP_OR_RECORD_END;
containerNode:       NODE_BEGIN      separator*  value  (separator+ value)*    separator* EDGE_OR_NODE_END;
containerEdge:       EDGE_BEGIN separator* value separator+ value separator+ value separator* EDGE_OR_NODE_END;
kvPair:              value separator* KV_SEPARATOR separator* value;

recordTypeBegin: RECORD_TYPE_BEGIN;
recordBegin:     RECORD_BEGIN;

arrayElemInt:    ARRAY_I_ELEM_D | ARRAY_I_ELEM_B | ARRAY_I_ELEM_O | ARRAY_I_ELEM_H;
arrayElemIntB:   ARRAY_I_B_ELEM;
arrayElemIntO:   ARRAY_I_O_ELEM;
arrayElemIntX:   ARRAY_I_X_ELEM;
arrayElemUint:   ARRAY_U_ELEM_D | ARRAY_U_ELEM_B | ARRAY_U_ELEM_O | ARRAY_U_ELEM_H;
arrayElemUintB:  ARRAY_U_B_ELEM;
arrayElemUintO:  ARRAY_U_O_ELEM;
arrayElemUintX:  ARRAY_U_X_ELEM;
arrayElemFloat:  ARRAY_F_ELEM_D | ARRAY_F_ELEM_H;
arrayElemFloatX: ARRAY_F_X_ELEM;
arrayElemNan:    ARRAY_F_NAN | ARRAY_F_X_NAN;
arrayElemSnan:   ARRAY_F_SNAN | ARRAY_F_X_SNAN;
arrayElemInf:    ARRAY_F_INF | ARRAY_F_X_INF;
arrayElemNinf:   ARRAY_F_NINF | ARRAY_F_X_NINF;
arrayElemUid:    ARRAY_UID_ELEM;
arrayElemBits:   ARRAY_BIT_BITS;
arrayElemByteX:  BYTES_ELEM;

arrayI8:  ( (ARRAY_TYPE_I8   ARRAY_I_WSL?   (arrayElemInt  (ARRAY_I_WSL   arrayElemInt )* ARRAY_I_WSL?)?   ARRAY_I_END)
          | (ARRAY_TYPE_I8B  ARRAY_I_B_WSL? (arrayElemIntB (ARRAY_I_B_WSL arrayElemIntB)* ARRAY_I_B_WSL?)? ARRAY_I_B_END)
          | (ARRAY_TYPE_I8O  ARRAY_I_O_WSL? (arrayElemIntO (ARRAY_I_O_WSL arrayElemIntO)* ARRAY_I_O_WSL?)? ARRAY_I_O_END)
          | (ARRAY_TYPE_I8X  ARRAY_I_X_WSL? (arrayElemIntX (ARRAY_I_X_WSL arrayElemIntX)* ARRAY_I_X_WSL?)? ARRAY_I_X_END)
          )
          ;
arrayI16: ( (ARRAY_TYPE_I16   ARRAY_I_WSL?   (arrayElemInt  (ARRAY_I_WSL   arrayElemInt )* ARRAY_I_WSL?)?   ARRAY_I_END)
          | (ARRAY_TYPE_I16B  ARRAY_I_B_WSL? (arrayElemIntB (ARRAY_I_B_WSL arrayElemIntB)* ARRAY_I_B_WSL?)? ARRAY_I_B_END)
          | (ARRAY_TYPE_I16O  ARRAY_I_O_WSL? (arrayElemIntO (ARRAY_I_O_WSL arrayElemIntO)* ARRAY_I_O_WSL?)? ARRAY_I_O_END)
          | (ARRAY_TYPE_I16X  ARRAY_I_X_WSL? (arrayElemIntX (ARRAY_I_X_WSL arrayElemIntX)* ARRAY_I_X_WSL?)? ARRAY_I_X_END)
          )
          ;
arrayI32: ( (ARRAY_TYPE_I32   ARRAY_I_WSL?   (arrayElemInt  (ARRAY_I_WSL   arrayElemInt )* ARRAY_I_WSL?)?   ARRAY_I_END)
          | (ARRAY_TYPE_I32B  ARRAY_I_B_WSL? (arrayElemIntB (ARRAY_I_B_WSL arrayElemIntB)* ARRAY_I_B_WSL?)? ARRAY_I_B_END)
          | (ARRAY_TYPE_I32O  ARRAY_I_O_WSL? (arrayElemIntO (ARRAY_I_O_WSL arrayElemIntO)* ARRAY_I_O_WSL?)? ARRAY_I_O_END)
          | (ARRAY_TYPE_I32X  ARRAY_I_X_WSL? (arrayElemIntX (ARRAY_I_X_WSL arrayElemIntX)* ARRAY_I_X_WSL?)? ARRAY_I_X_END)
          )
          ;
arrayI64: ( (ARRAY_TYPE_I64   ARRAY_I_WSL?   (arrayElemInt  (ARRAY_I_WSL   arrayElemInt )* ARRAY_I_WSL?)?   ARRAY_I_END)
          | (ARRAY_TYPE_I64B  ARRAY_I_B_WSL? (arrayElemIntB (ARRAY_I_B_WSL arrayElemIntB)* ARRAY_I_B_WSL?)? ARRAY_I_B_END)
          | (ARRAY_TYPE_I64O  ARRAY_I_O_WSL? (arrayElemIntO (ARRAY_I_O_WSL arrayElemIntO)* ARRAY_I_O_WSL?)? ARRAY_I_O_END)
          | (ARRAY_TYPE_I64X  ARRAY_I_X_WSL? (arrayElemIntX (ARRAY_I_X_WSL arrayElemIntX)* ARRAY_I_X_WSL?)? ARRAY_I_X_END)
          )
          ;

arrayU8:  ( (ARRAY_TYPE_U8   ARRAY_U_WSL?   (arrayElemUint  (ARRAY_U_WSL   arrayElemUint )* ARRAY_U_WSL?  )? ARRAY_U_END)
          | (ARRAY_TYPE_U8B  ARRAY_U_B_WSL? (arrayElemUintB (ARRAY_U_B_WSL arrayElemUintB)* ARRAY_U_B_WSL?)? ARRAY_U_B_END)
          | (ARRAY_TYPE_U8O  ARRAY_U_O_WSL? (arrayElemUintO (ARRAY_U_O_WSL arrayElemUintO)* ARRAY_U_O_WSL?)? ARRAY_U_O_END)
          | (ARRAY_TYPE_U8X  ARRAY_U_X_WSL? (arrayElemUintX (ARRAY_U_X_WSL arrayElemUintX)* ARRAY_U_X_WSL?)? ARRAY_U_X_END)
          )
          ;
arrayU16: ( (ARRAY_TYPE_U16  ARRAY_U_WSL?   (arrayElemUint  (ARRAY_U_WSL   arrayElemUint )* ARRAY_U_WSL?  )? ARRAY_U_END)
          | (ARRAY_TYPE_U16B ARRAY_U_B_WSL? (arrayElemUintB (ARRAY_U_B_WSL arrayElemUintB)* ARRAY_U_B_WSL?)? ARRAY_U_B_END)
          | (ARRAY_TYPE_U16O ARRAY_U_O_WSL? (arrayElemUintO (ARRAY_U_O_WSL arrayElemUintO)* ARRAY_U_O_WSL?)? ARRAY_U_O_END)
          | (ARRAY_TYPE_U16X ARRAY_U_X_WSL? (arrayElemUintX (ARRAY_U_X_WSL arrayElemUintX)* ARRAY_U_X_WSL?)? ARRAY_U_X_END)
          )
          ;
arrayU32: ( (ARRAY_TYPE_U32  ARRAY_U_WSL?   (arrayElemUint  (ARRAY_U_WSL   arrayElemUint )* ARRAY_U_WSL?  )? ARRAY_U_END)
          | (ARRAY_TYPE_U32B ARRAY_U_B_WSL? (arrayElemUintB (ARRAY_U_B_WSL arrayElemUintB)* ARRAY_U_B_WSL?)? ARRAY_U_B_END)
          | (ARRAY_TYPE_U32O ARRAY_U_O_WSL? (arrayElemUintO (ARRAY_U_O_WSL arrayElemUintO)* ARRAY_U_O_WSL?)? ARRAY_U_O_END)
          | (ARRAY_TYPE_U32X ARRAY_U_X_WSL? (arrayElemUintX (ARRAY_U_X_WSL arrayElemUintX)* ARRAY_U_X_WSL?)? ARRAY_U_X_END)
          )
          ;
arrayU64: ( (ARRAY_TYPE_U64  ARRAY_U_WSL?   (arrayElemUint  (ARRAY_U_WSL   arrayElemUint )* ARRAY_U_WSL?  )? ARRAY_U_END)
          | (ARRAY_TYPE_U64B ARRAY_U_B_WSL? (arrayElemUintB (ARRAY_U_B_WSL arrayElemUintB)* ARRAY_U_B_WSL?)? ARRAY_U_B_END)
          | (ARRAY_TYPE_U64O ARRAY_U_O_WSL? (arrayElemUintO (ARRAY_U_O_WSL arrayElemUintO)* ARRAY_U_O_WSL?)? ARRAY_U_O_END)
          | (ARRAY_TYPE_U64X ARRAY_U_X_WSL? (arrayElemUintX (ARRAY_U_X_WSL arrayElemUintX)* ARRAY_U_X_WSL?)? ARRAY_U_X_END)
          )
          ;

arrayF16: ( (
              ARRAY_TYPE_F16  ARRAY_F_WSL?
              (
                (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_WSL (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_WSL?
              )?
              ARRAY_F_END
            )
          | (
              ARRAY_TYPE_F16X  ARRAY_F_X_WSL?
              (
                (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_X_WSL (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_X_WSL?
              )?
              ARRAY_F_X_END
            )
          )
          ;
arrayF32: ( (
              ARRAY_TYPE_F32  ARRAY_F_WSL?
              (
                (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_WSL (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_WSL?
              )?
              ARRAY_F_END
            )
          | (
              ARRAY_TYPE_F32X  ARRAY_F_X_WSL?
              (
                (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_X_WSL (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_X_WSL?
              )?
              ARRAY_F_X_END
            )
          )
          ;
arrayF64: ( (
              ARRAY_TYPE_F64  ARRAY_F_WSL?
              (
                (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_WSL (arrayElemFloat  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_WSL?
              )?
              ARRAY_F_END
            )
          | (
              ARRAY_TYPE_F64X  ARRAY_F_X_WSL?
              (
                (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf)
                (ARRAY_F_X_WSL (arrayElemFloatX  | arrayElemNan | arrayElemSnan | arrayElemInf | arrayElemNinf))*
                ARRAY_F_X_WSL?
              )?
              ARRAY_F_X_END
            )
          )
          ;

arrayUid: ARRAY_TYPE_UID ARRAY_UID_WSL? (arrayElemUid (ARRAY_UID_WSL arrayElemUid)* ARRAY_UID_WSL?)? ARRAY_UID_END
          ;

arrayBit: ARRAY_TYPE_BIT (ARRAY_BIT_WSL | arrayElemBits)* ARRAY_BIT_END
          ;
