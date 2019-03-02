#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING(Map, size_0, 1, map()->end(), {0x7c, 0x7d})
TEST_ENCODE_DECODE_SHRINKING(Map, size_1, 1, map()->str("1")->i8(1)->end(), {0x7c, 0x81, 0x31, 0x01, 0x7d})
TEST_ENCODE_DECODE_SHRINKING(Map, size_2, 1, map()->str("1")->i8(1)->str("2")->i8(2)->end(), {0x7c, 0x81, 0x31, 0x01, 0x81, 0x32, 0x02, 0x7d})

TEST_ENCODE_DECODE_STATUS(Map, unterminated,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->f32(0.1)->i8(1))
TEST_ENCODE_DECODE_STATUS(Map, unterminated_3, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_4, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end())

// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(Map, encode_extra_end,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->end()->end())
TEST_ENCODE_STATUS(Map, encode_extra_end_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end()->end()->end())

TEST_ENCODE_STATUS(Map, encode_nil_key,  99, 9, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->nil()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_list_key, 99, 9, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->list()->end()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_map_key,  99, 9, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->map()->end()->str("")->end())

TEST_DECODE_STATUS(Map, decode_nil_key,  99, 9, true, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7e, 0x80, 0x7d})
TEST_DECODE_STATUS(Map, decode_list_key, 99, 9, true, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7b, 0x7d, 0x80, 0x7d})
TEST_DECODE_STATUS(Map, decode_map_key,  99, 9, true, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7c, 0x7d, 0x80, 0x7d})

TEST_ENCODE_STATUS(Map, encode_missing_value_i8,          99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i8(0)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i16,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i16(0xfff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i32,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i32(0xfffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i64,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i64(0xfffffffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_i128,        99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i128(0x0f, 0xffffffffffffffff)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f32,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f32(0.1)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f64,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f64(1.0123)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_f128,        99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f128(1.012345l)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d32,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d32(0.1)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d64,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d64(1000000.000001dd)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_d128,        99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d128(1000000000000.000000000001dl)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_time,        99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->smtime(0)->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_string,      99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("")->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_long_string, 99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("0000000000000000")->end())
TEST_ENCODE_STATUS(Map, encode_missing_value_map,         99, 9, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("")->map()->str("")->end()->end())

TEST_DECODE_STATUS(Map, decode_missing_value_i8,          99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x00, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_i16,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x6e, 0xff, 0x0f, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_i32,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x6f, 0xff, 0xff, 0xff, 0x0f, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_i64,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x70, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_i128,        99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_f32,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x72, 0xcd, 0xcc, 0xcc, 0x3d, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_f64,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x73, 0x51, 0xda, 0x1b, 0x7c, 0x61, 0x32, 0xf0, 0x3f, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_f128,        99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x0c, 0xe5, 0x44, 0xbb, 0x0a, 0x29, 0x03, 0xff, 0x3f, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_d32,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x75, 0x01, 0x00, 0x00, 0x32, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_d64,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x76, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_d128,        99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x77, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_time,        99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_string,      99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x80, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_long_string, 99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x90, 0x40, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x7d})
TEST_DECODE_STATUS(Map, decode_missing_value_map,         99, 9, true, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x80, 0x7c, 0x80, 0x7d, 0x7d})
