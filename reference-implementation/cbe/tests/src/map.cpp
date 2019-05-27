#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING(Map, size_0, 1, map()->end(), {0x95, 0x96})
TEST_ENCODE_DECODE_SHRINKING(Map, size_1, 1, map()->str("1")->si(1)->end(), {0x95, 0x81, 0x31, 0x01, 0x96})
TEST_ENCODE_DECODE_SHRINKING(Map, size_2, 1, map()->str("1")->si(1)->str("2")->si(2)->end(), {0x95, 0x81, 0x31, 0x01, 0x81, 0x32, 0x02, 0x96})

TEST_ENCODE_DECODE_STATUS(Map, unterminated,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->flt(0.1)->si(1))
TEST_ENCODE_DECODE_STATUS(Map, unterminated_3, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_4, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end())

// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(Map, encode_extra_end,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->end()->end())
TEST_ENCODE_STATUS(Map, encode_extra_end_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end()->end()->end())

TEST_ENCODE_STATUS(Map, encode_nil_key,  99, 9, CBE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE, map()->nil()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_list_key, 99, 9, CBE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE, map()->list()->end()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_map_key,  99, 9, CBE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE, map()->map()->end()->str("")->end())

TEST_DECODE_STATUS(Map, decode_nil_key,  99, 9, true, CBE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE, {0x95, 0x6f, 0x80, 0x96})
TEST_DECODE_STATUS(Map, decode_list_key, 99, 9, true, CBE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE, {0x95, 0x94, 0x7d, 0x80, 0x96})
TEST_DECODE_STATUS(Map, decode_map_key,  99, 9, true, CBE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE, {0x95, 0x95, 0x7d, 0x80, 0x96})

TEST_ENCODE_STATUS(Map, encode_missing_value_i8,          99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->ui(0)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i16,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->ui(0xfff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i32,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->ui(0xfffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i64,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->ui(0xfffffffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i128,        99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->ui(0x0f, 0xffffffffffffffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f32,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->flt(0.1)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f64,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->flt(1.0123)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f128,        99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->flt(1.012345l)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d32,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->dec(0.1)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d64,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->dec(1000000.000001dd)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d128,        99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->dec(1000000000000.000000000001dl)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_time,        99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->time((smalltime)0)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_string,      99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_long_string, 99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->str("0000000000000000")->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_map,         99, 9, CBE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, map()->str("")->map()->str("")->end()->end())

TEST_DECODE_STATUS(Map, decode_missing_value_i8,          99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x00, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_i16,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x6b, 0xff, 0x0f, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_i32,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x6c, 0xff, 0xff, 0xff, 0x0f, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_i64,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x6d, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_i128,        99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_f32,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x72, 0xcd, 0xcc, 0xcc, 0x3d, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_f64,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x73, 0x51, 0xda, 0x1b, 0x7c, 0x61, 0x32, 0xf0, 0x3f, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_f128,        99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x0c, 0xe5, 0x44, 0xbb, 0x0a, 0x29, 0x03, 0xff, 0x3f, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_d32,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x75, 0x01, 0x00, 0x00, 0x32, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_d64,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x76, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_d128,        99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x77, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_time,        99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_string,      99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x80, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_long_string, 99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x90, 0x40, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x96})
TEST_DECODE_STATUS(Map, decode_missing_value_map,         99, 9, true, CBE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY, {0x95, 0x80, 0x95, 0x80, 0x96, 0x96})
