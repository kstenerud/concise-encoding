#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING(List, size_0, 1, list()->end(), {0x7b, 0x7d})
TEST_ENCODE_DECODE_SHRINKING(List, size_1, 1, list()->i8(1)->end(), {0x7b, 0x01, 0x7d})
TEST_ENCODE_DECODE_SHRINKING(List, size_2, 1, list()->str("1")->i8(1)->end(), {0x7b, 0x81, 0x31, 0x01, 0x7d})

TEST_ENCODE_DECODE_STATUS(List, unterminated,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list())
TEST_ENCODE_DECODE_STATUS(List, unterminated_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->f32(0.1))
TEST_ENCODE_DECODE_STATUS(List, unterminated_3, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->map())

// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(List, encode_extra_end,   99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->end()->end())
TEST_ENCODE_STATUS(List, encode_extra_end_2, 99, 9, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->map()->end()->end()->end())

TEST_ENCODE_STATUS(List, too_deep, 99, 3, CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED, list()->list()->list()->list()->end()->end()->end()->end())
