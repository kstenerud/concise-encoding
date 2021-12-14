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

parser grammar CTE;
options { tokenVocab=CTELexer; }

cte
   : version value
   ;

value
   : valueString
   | valueRid
   | valueUid
   | valueBool
   | valueNull
   | valueInt
   | valueFloat
   | valueTime
   | arrayBit
   | arrayI8
   | arrayI8b
   | arrayI8o
   | arrayI8x
   | arrayU8
   | arrayU8b
   | arrayU8o
   | arrayU8x
   | arrayI16
   | arrayI16b
   | arrayI16o
   | arrayI16x
   | arrayU16
   | arrayU16b
   | arrayU16o
   | arrayU16x
   | arrayI32
   | arrayI32b
   | arrayI32o
   | arrayI32x
   | arrayU32
   | arrayU32b
   | arrayU32o
   | arrayU32x
   | arrayI64
   | arrayI64b
   | arrayI64o
   | arrayI64x
   | arrayU64
   | arrayU64b
   | arrayU64o
   | arrayU64x
   | arrayF16
   | arrayF16x
   | arrayF32
   | arrayF32x
   | arrayF64
   | arrayF64x
   | customBinary
   | customText
   | media
   | containerMap
   | containerList
   | containerEdge
   | containerNode
   | containerMarkup
   | marker
   | reference
   | remoteRef
   ;

version: VERSION;

valueNull: NULL;

valueUid: UID;

valueBool: TRUE | FALSE;

valueInt
   : PINT_BIN
   | NINT_BIN
   | PINT_OCT
   | NINT_OCT
   | PINT_DEC
   | NINT_DEC
   | PINT_HEX
   | NINT_HEX
   ;

valueFloat
   : FLOAT_DEC
   | FLOAT_HEX
   | INF
   | NAN
   | SNAN
   ;

valueTime: DATE | TIME;

valueString: STRING;
valueRid: RID;
customText: CUSTOM_TEXT;

kvPair: value KV_SEPARATOR value;

markerID: MARKER;
marker: markerID value;
reference: REFRENCE;
remoteRef: REMOTE_REF;

markupName: MARKUP_NAME | MARKUP_SUB_NAME;
markupContents: MARKUP_CONTENTS | containerMarkup;

containerMap:    MAP_BEGIN  kvPair* MAP_END;
containerList:   LIST_BEGIN value* LIST_END;
containerEdge:   EDGE_BEGIN value value value EDGE_NODE_END;
containerNode:   NODE_BEGIN value+ EDGE_NODE_END;
containerMarkup
   : (MARKUP_BEGIN | MARKUP_SUB_BEGIN)
     markupName kvPair*
     (MARKUP_CONTENT_BEGIN markupContents*)?
     (MARKUP_END | MARKUP_CONTENTS_END);

arrayElemBits:   ARRAY_BIT_BITS;
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
arrayElemByteX:  CUSTOM_BINARY_ELEM | MEDIA_ELEM;

arrayBit:  ARRAY_BIT_BEGIN  arrayElemBits*   ARRAY_BIT_END;
arrayI8:   ARRAY_I8_BEGIN   arrayElemInt*    ARRAY_I_END;
arrayI8b:  ARRAY_I8B_BEGIN  arrayElemIntB*   ARRAY_I_B_END;
arrayI8o:  ARRAY_I8O_BEGIN  arrayElemIntO*   ARRAY_I_O_END;
arrayI8x:  ARRAY_I8X_BEGIN  arrayElemIntX*   ARRAY_I_X_END;
arrayU8:   ARRAY_U8_BEGIN   arrayElemUint*   ARRAY_U_END;
arrayU8b:  ARRAY_U8B_BEGIN  arrayElemUintB*  ARRAY_U_B_END;
arrayU8o:  ARRAY_U8O_BEGIN  arrayElemUintO*  ARRAY_U_O_END;
arrayU8x:  ARRAY_U8X_BEGIN  arrayElemUintX*  ARRAY_U_X_END;
arrayI16:  ARRAY_I16_BEGIN  arrayElemInt*    ARRAY_I_END;
arrayI16b: ARRAY_I16B_BEGIN arrayElemIntB*   ARRAY_I_B_END;
arrayI16o: ARRAY_I16O_BEGIN arrayElemIntO*   ARRAY_I_O_END;
arrayI16x: ARRAY_I16X_BEGIN arrayElemIntX*   ARRAY_I_X_END;
arrayU16:  ARRAY_U16_BEGIN  arrayElemUint*   ARRAY_U_END;
arrayU16b: ARRAY_U16B_BEGIN arrayElemUintB*  ARRAY_U_B_END;
arrayU16o: ARRAY_U16O_BEGIN arrayElemUintO*  ARRAY_U_O_END;
arrayU16x: ARRAY_U16X_BEGIN arrayElemUintX*  ARRAY_U_X_END;
arrayI32:  ARRAY_I32_BEGIN  arrayElemInt*    ARRAY_I_END;
arrayI32b: ARRAY_I32B_BEGIN arrayElemIntB*   ARRAY_I_B_END;
arrayI32o: ARRAY_I32O_BEGIN arrayElemIntO*   ARRAY_I_O_END;
arrayI32x: ARRAY_I32X_BEGIN arrayElemIntX*   ARRAY_I_X_END;
arrayU32:  ARRAY_U32_BEGIN  arrayElemUint*   ARRAY_U_END;
arrayU32b: ARRAY_U32B_BEGIN arrayElemUintB*  ARRAY_U_B_END;
arrayU32o: ARRAY_U32O_BEGIN arrayElemUintO*  ARRAY_U_O_END;
arrayU32x: ARRAY_U32X_BEGIN arrayElemUintX*  ARRAY_U_X_END;
arrayI64:  ARRAY_I64_BEGIN  arrayElemInt*    ARRAY_I_END;
arrayI64b: ARRAY_I64B_BEGIN arrayElemIntB*   ARRAY_I_B_END;
arrayI64o: ARRAY_I64O_BEGIN arrayElemIntO*   ARRAY_I_O_END;
arrayI64x: ARRAY_I64X_BEGIN arrayElemIntX*   ARRAY_I_X_END;
arrayU64:  ARRAY_U64_BEGIN  arrayElemUint*   ARRAY_U_END;
arrayU64b: ARRAY_U64B_BEGIN arrayElemUintB*  ARRAY_U_B_END;
arrayU64o: ARRAY_U64O_BEGIN arrayElemUintO*  ARRAY_U_O_END;
arrayU64x: ARRAY_U64X_BEGIN arrayElemUintX*  ARRAY_U_X_END;
arrayF16:  ARRAY_F16_BEGIN  arrayElemFloat*  ARRAY_F_END;
arrayF16x: ARRAY_F16X_BEGIN arrayElemFloatX* ARRAY_F_X_END;
arrayF32:  ARRAY_F32_BEGIN  arrayElemFloat*  ARRAY_F_END;
arrayF32x: ARRAY_F32X_BEGIN arrayElemFloatX* ARRAY_F_X_END;
arrayF64:  ARRAY_F64_BEGIN  arrayElemFloat*  ARRAY_F_END;
arrayF64x: ARRAY_F64X_BEGIN arrayElemFloatX* ARRAY_F_X_END;

customBinary: CUSTOM_BINARY_BEGIN arrayElemByteX* CUSTOM_BINARY_END;
media:        MEDA_BEGIN          arrayElemByteX* MEDIA_END;
