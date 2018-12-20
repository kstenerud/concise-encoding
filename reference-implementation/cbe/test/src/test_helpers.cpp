#include "test_helpers.h"

#define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

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

namespace cbe_test
{

std::vector<uint8_t> encode_data(int buffer_size, std::shared_ptr<enc::encoding> encoding)
{
    std::vector<uint8_t> actual_memory;

    cbe_encoder encoder(buffer_size, [&](uint8_t* data_start, int64_t length)
        {
            // std::cout << "Data: " << length << std::endl;
            actual_memory.insert(actual_memory.end(), data_start, data_start + length);
            return true;
        });
    encoder.encode(encoding);
    return actual_memory;
}

std::shared_ptr<enc::encoding> decode_data(int buffer_size, std::vector<uint8_t> data)
{
    cbe_decoder decoder;
    for(unsigned offset = 0; offset < data.size(); offset += buffer_size)
    {
        std::vector<uint8_t>::iterator begin = data.begin() + offset;
        std::vector<uint8_t>::iterator end = data.end();
        if(offset + buffer_size < data.size())
        {
            end = data.begin() + offset + buffer_size;
        }
        std::vector<uint8_t> buffer(begin, end);
        decoder.feed(buffer);
    }
    return decoder.decoded();
}

void expect_encode(int buffer_size, std::shared_ptr<enc::encoding> encoding, const std::vector<uint8_t> expected_memory)
{
    std::vector<uint8_t> actual_memory = encode_data(buffer_size, encoding);
    EXPECT_EQ(expected_memory, actual_memory);
}

void expect_decode(int buffer_size, const std::vector<uint8_t> memory, std::shared_ptr<enc::encoding> expected_encoding)
{
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, memory);
    EXPECT_EQ(*expected_encoding, *actual_encoding);
}

void expect_decode_exact(int buffer_size, const std::vector<uint8_t> memory, std::shared_ptr<enc::encoding> expected_encoding)
{
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, memory);
    EXPECT_EX(*expected_encoding, *actual_encoding);
}

void expect_encode_decode_equality(int buffer_size, std::shared_ptr<enc::encoding> expected_encoding, const std::vector<uint8_t> expected_memory)
{
    KSLOG_DEBUG("Encode %s", expected_encoding->as_string().c_str());
    std::vector<uint8_t> actual_memory = encode_data(buffer_size, expected_encoding);
    KSLOG_DEBUG("Decode %s", as_string(actual_memory).c_str());
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, expected_memory);
    KSLOG_DEBUG("Done");
    EXPECT_EQ(expected_memory, actual_memory);
    EXPECT_EQ(*expected_encoding, *actual_encoding);
}

void expect_encode_decode_exact_equality(int buffer_size, std::shared_ptr<enc::encoding> expected_encoding, const std::vector<uint8_t> expected_memory)
{
    std::vector<uint8_t> actual_memory = encode_data(buffer_size, expected_encoding);
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size, expected_memory);
    EXPECT_EQ(expected_memory, actual_memory);
    EXPECT_EX(*expected_encoding, *actual_encoding);
}

}
