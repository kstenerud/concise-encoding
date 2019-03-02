#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_0, 1, list()->end(), {0x7b, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_1, 1, list()->i8(1)->end(), {0x7b, 0x01, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_2, 1, list()->str("1")->i8(1)->end(), {0x7b, 0x81, 0x31, 0x01, 0x7d})

TEST_ENCODE_DECODE_STATUS(List, unterminated, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list())
TEST_ENCODE_DECODE_STATUS(List, unterminated_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->f32(0.1))
TEST_ENCODE_DECODE_STATUS(List, unterminated_3, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->map())

// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(List, encode_extra_end, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->end()->end())
TEST_ENCODE_STATUS(List, encode_extra_end_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->map()->end()->end()->end())

TEST(List, too_deep)
{
    const int max_container_depth = 10;
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

    char cbe_encode_process_data[cbe_encode_process_size(max_container_depth)];
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)&cbe_encode_process_data ;
    cbe_encode_begin(encode_process, memory.data(), memory.size(), max_container_depth);
    cbe_encode_status status;
    for(int i = 0; i < max_container_depth + 1; i++)
    {
        status = cbe_encode_list_begin(encode_process);
        if(status == CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED)
        {
            break;
        }
    }
    EXPECT_EQ(CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED, status);
}
