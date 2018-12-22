#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


// ===========================================================================
// Google Test extension to test for exact equality in encoding objects

namespace testing {
namespace internal {

// The helper function for {ASSERT|EXPECT}_EQ.
template <typename T1, typename T2>
AssertionResult CmpHelperEX(const char* lhs_expression,
                            const char* rhs_expression,
                            const T1& lhs,
                            const T2& rhs) {
  if (lhs.is_equal_in_type_and_size(rhs)) {
    return AssertionSuccess();
  }

  return CmpHelperEQFailure(lhs_expression, rhs_expression, lhs, rhs);
}

template <bool lhs_is_null_literal>
class ExHelper {
 public:
  // This templatized version is for the general case.
  template <typename T1, typename T2>
  static AssertionResult Compare(const char* lhs_expression,
                                 const char* rhs_expression,
                                 const T1& lhs,
                                 const T2& rhs) {
    return CmpHelperEX(lhs_expression, rhs_expression, lhs, rhs);
  }

};

}}

#define EXPECT_EX(val1, val2) \
  EXPECT_PRED_FORMAT2(::testing::internal:: \
                      ExHelper<GTEST_IS_NULL_LITERAL_(val1)>::Compare, \
                      val1, val2)

// ===========================================================================

namespace cbe_test
{

// ===========================================================================
// Always true & always false decode callbacks

#define DEFINE_EMPTY_DECODE_CALLBACK_0_PARAMETER(RETURN_BOOL, NAME_FRAGMENT) \
static bool RETURN_BOOL ##_on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process) \
{ \
    (void)decode_process; \
    return RETURN_BOOL; \
}
#define DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, NAME_FRAGMENT, TYPE) \
static bool RETURN_BOOL ##_on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process, TYPE param) \
{ \
    (void)decode_process; \
    (void)param; \
    return RETURN_BOOL; \
}
#define DEFINE_EMPTY_DECODE_CALLBACK_2_PARAMETER(RETURN_BOOL, NAME_FRAGMENT, TYPE1, TYPE2) \
static bool RETURN_BOOL ##_on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process, TYPE1 param1, TYPE2 param2) \
{ \
    (void)decode_process; \
    (void)param1; \
    (void)param2; \
    return RETURN_BOOL; \
}
#define DEFINE_EMPTY_DECODE_CALLBACK_SUITE(RETURN_BOOL) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_boolean, bool) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_int_8, int8_t) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_int_16, int16_t) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_int_32, int32_t) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_int_64, int64_t) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_int_128, __int128) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_float_32, float) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_float_64, double) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_float_128, __float128) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_decimal_32, _Decimal32) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_decimal_64, _Decimal64) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_decimal_128, _Decimal128) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, add_time, smalltime) \
DEFINE_EMPTY_DECODE_CALLBACK_0_PARAMETER(RETURN_BOOL, add_empty) \
DEFINE_EMPTY_DECODE_CALLBACK_0_PARAMETER(RETURN_BOOL, end_container) \
DEFINE_EMPTY_DECODE_CALLBACK_0_PARAMETER(RETURN_BOOL, begin_list) \
DEFINE_EMPTY_DECODE_CALLBACK_0_PARAMETER(RETURN_BOOL, begin_map) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, begin_string, int64_t) \
DEFINE_EMPTY_DECODE_CALLBACK_1_PARAMETER(RETURN_BOOL, begin_binary, int64_t) \
DEFINE_EMPTY_DECODE_CALLBACK_2_PARAMETER(RETURN_BOOL, add_data, const uint8_t*, int64_t) \

DEFINE_EMPTY_DECODE_CALLBACK_SUITE(false)
DEFINE_EMPTY_DECODE_CALLBACK_SUITE(true)

#define FILL_ALWAYS_RETURN_CALLBACKS(CALLBACKS_PTR, RETURN_BOOL) \
{ \
    (CALLBACKS_PTR)->on_add_empty = RETURN_BOOL ## _on_add_empty; \
    (CALLBACKS_PTR)->on_add_boolean = RETURN_BOOL ## _on_add_boolean; \
    (CALLBACKS_PTR)->on_add_int_8 = RETURN_BOOL ## _on_add_int_8; \
    (CALLBACKS_PTR)->on_add_int_16 = RETURN_BOOL ## _on_add_int_16; \
    (CALLBACKS_PTR)->on_add_int_32 = RETURN_BOOL ## _on_add_int_32; \
    (CALLBACKS_PTR)->on_add_int_64 = RETURN_BOOL ## _on_add_int_64; \
    (CALLBACKS_PTR)->on_add_int_128 = RETURN_BOOL ## _on_add_int_128; \
    (CALLBACKS_PTR)->on_add_float_32 = RETURN_BOOL ## _on_add_float_32; \
    (CALLBACKS_PTR)->on_add_float_64 = RETURN_BOOL ## _on_add_float_64; \
    (CALLBACKS_PTR)->on_add_float_128 = RETURN_BOOL ## _on_add_float_128; \
    (CALLBACKS_PTR)->on_add_decimal_32 = RETURN_BOOL ## _on_add_decimal_32; \
    (CALLBACKS_PTR)->on_add_decimal_64 = RETURN_BOOL ## _on_add_decimal_64; \
    (CALLBACKS_PTR)->on_add_decimal_128 = RETURN_BOOL ## _on_add_decimal_128; \
    (CALLBACKS_PTR)->on_add_time = RETURN_BOOL ## _on_add_time; \
    (CALLBACKS_PTR)->on_end_container = RETURN_BOOL ## _on_end_container; \
    (CALLBACKS_PTR)->on_begin_list = RETURN_BOOL ## _on_begin_list; \
    (CALLBACKS_PTR)->on_begin_map = RETURN_BOOL ## _on_begin_map; \
    (CALLBACKS_PTR)->on_begin_string = RETURN_BOOL ## _on_begin_string; \
    (CALLBACKS_PTR)->on_begin_binary = RETURN_BOOL ## _on_begin_binary; \
    (CALLBACKS_PTR)->on_add_data = RETURN_BOOL ## _on_add_data; \
}


static cbe_decode_callbacks g_always_false_callbacks;
static cbe_decode_callbacks g_always_true_callbacks;

static void init_callbacks()
{
    FILL_ALWAYS_RETURN_CALLBACKS(&g_always_false_callbacks, false);
    FILL_ALWAYS_RETURN_CALLBACKS(&g_always_true_callbacks, true);
}

const cbe_decode_callbacks* get_always_false_decode_callbacks()
{
    init_callbacks();
    return &g_always_false_callbacks;
}

const cbe_decode_callbacks* get_always_true_decode_callbacks()
{
    init_callbacks();
    return &g_always_true_callbacks;
}

// ===========================================================================


static std::vector<uint8_t> encode_data(int buffer_size, std::shared_ptr<enc::encoding> encoding, cbe_encode_status& status)
{
    std::vector<uint8_t> actual_memory;

    KSLOG_DEBUG("Encode %s with buffer size %d", encoding->as_string().c_str(), buffer_size);
    cbe_encoder encoder(buffer_size, [&](uint8_t* data_start, int64_t length)
        {
            KSLOG_DEBUG("Encoded data of length %d", length);
            actual_memory.insert(actual_memory.end(), data_start, data_start + length);
            return true;
        });
    status = encoder.encode(encoding);
    KSLOG_DEBUG("Status = %d", status);
    return actual_memory;
}

static std::shared_ptr<enc::encoding> decode_data(int buffer_size, std::vector<uint8_t> data, cbe_decode_status& status)
{
    cbe_decoder decoder;
    KSLOG_DEBUG("Decode %d bytes with buffer size %d", data.size(), buffer_size);
    for(unsigned offset = 0; offset < data.size(); offset += buffer_size)
    {
        std::vector<uint8_t>::iterator begin = data.begin() + offset;
        std::vector<uint8_t>::iterator end = data.end();
        if(offset + buffer_size < data.size())
        {
            end = data.begin() + offset + buffer_size;
        }
        KSLOG_DEBUG("Decoding data from %d for %d bytes", offset, buffer_size);
        std::vector<uint8_t> buffer(begin, end);
        status = decoder.feed(buffer);
        if(status != CBE_DECODE_STATUS_OK && status != CBE_DECODE_STATUS_NEED_MORE_DATA)
        {
            break;
        }
    }
    if(status == CBE_DECODE_STATUS_OK)
    {
        status = decoder.end();
    }
    KSLOG_DEBUG("Status = %d", status);
    return decoder.decoded();
}

bool expect_encode(int buffer_size,
    std::shared_ptr<enc::encoding> encoding,
    const std::vector<uint8_t> expected_memory,
    cbe_encode_status expected_status)
{
    KSLOG_DEBUG("Encode with buffer size %d: %s", buffer_size, encoding->as_string().c_str());
    cbe_encode_status actual_status = CBE_ENCODE_STATUS_OK;
    std::vector<uint8_t> actual_memory = encode_data(buffer_size, encoding, actual_status);
    EXPECT_EQ(expected_status, actual_status);
    if(actual_status == CBE_ENCODE_STATUS_OK)
    {
        EXPECT_EQ(expected_memory, actual_memory);
    }
    return actual_status == CBE_ENCODE_STATUS_OK;
}

void expect_decode(int buffer_size,
    const std::vector<uint8_t> memory,
    std::shared_ptr<enc::encoding> expected_encoding,
    cbe_decode_status expected_status)
{
    KSLOG_DEBUG("Decode %s", as_string(memory).c_str());
    cbe_decode_status actual_status = CBE_DECODE_STATUS_OK;
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, memory, actual_status);
    ASSERT_EQ(expected_status, actual_status);
    ASSERT_TRUE(actual_encoding);
    ASSERT_EQ(*expected_encoding, *actual_encoding);
}

void expect_decode_exact(int buffer_size,
    const std::vector<uint8_t> memory,
    std::shared_ptr<enc::encoding> expected_encoding,
    cbe_decode_status expected_status)
{
    KSLOG_DEBUG("Decode %s", as_string(memory).c_str());
    cbe_decode_status actual_status = CBE_DECODE_STATUS_OK;
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, memory, actual_status);
    ASSERT_EQ(expected_status, actual_status);
    ASSERT_TRUE(actual_encoding);
    EXPECT_EX(*expected_encoding, *actual_encoding);
}

void expect_encode_decode(int buffer_size,
    std::shared_ptr<enc::encoding> expected_encoding,
    const std::vector<uint8_t> expected_memory,
    cbe_encode_status expected_encode_status,
    cbe_decode_status expected_decode_status)
{
    if(expect_encode(buffer_size, expected_encoding, expected_memory, expected_encode_status))
    {
        expect_decode(buffer_size, expected_memory, expected_encoding, expected_decode_status);
    }
}

void expect_encode_decode_equality(int min_buffer_size,
    std::shared_ptr<enc::encoding> expected_encoding,
    const std::vector<uint8_t> expected_memory)
{
    int expected_buffer_size = expected_memory.size();
    if(min_buffer_size == 0)
    {
        min_buffer_size = expected_buffer_size;
    }
    for(int buffer_size = expected_buffer_size+16; buffer_size >= min_buffer_size; buffer_size--)
    {
        expect_encode_decode(buffer_size, expected_encoding, expected_memory, CBE_ENCODE_STATUS_OK, CBE_DECODE_STATUS_OK);
    }
    if(min_buffer_size > 1)
    {
        expect_encode(min_buffer_size - 1, expected_encoding, expected_memory, CBE_ENCODE_STATUS_NEED_MORE_ROOM);
    }
}

void expect_encode_decode_exact_equality(
    std::shared_ptr<enc::encoding> expected_encoding,
    const std::vector<uint8_t> expected_memory)
{
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    cbe_decode_status decode_status = CBE_DECODE_STATUS_OK;
    int buffer_size = expected_memory.size() + 100;
    std::vector<uint8_t> actual_memory = encode_data(buffer_size, expected_encoding, encode_status);
    ASSERT_EQ(encode_status, CBE_ENCODE_STATUS_OK);
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, expected_memory, decode_status);
    ASSERT_EQ(decode_status, CBE_DECODE_STATUS_OK);
    ASSERT_EQ(expected_memory, actual_memory);
    EXPECT_EX(*expected_encoding, *actual_encoding);
}

void expect_encode_decode_status(
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status expected_encode_status,
    cbe_decode_status expected_decode_status)
{
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    cbe_decode_status decode_status = CBE_DECODE_STATUS_OK;
    int buffer_size = 100;
    std::vector<uint8_t> memory = encode_data(buffer_size, encoding, encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, memory, decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

void expect_encode_status(
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status expected_encode_status)
{
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    int buffer_size = 100;
    std::vector<uint8_t> memory = encode_data(buffer_size, encoding, encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
}

void expect_decode_status(
    std::vector<uint8_t> document,
    cbe_decode_status expected_decode_status)
{
    cbe_decode_status decode_status = CBE_DECODE_STATUS_OK;
    int buffer_size = 100;
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, document, decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

} // namespace cbe_test
