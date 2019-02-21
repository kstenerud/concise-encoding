#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


namespace cbe_test
{

static std::vector<uint8_t> encode_data(
    int buffer_size,
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status& status)
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

static std::shared_ptr<enc::encoding> decode_data(
    int buffer_size,
    std::vector<uint8_t> data,
    cbe_decode_status& status)
{
    cbe_decoder decoder;
    status = decoder.begin();
    if(status != CBE_DECODE_STATUS_OK)
    {
        return decoder.decoded();
    }

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
    // Also checking NEED_MORE_DATA to allow testing for premature end of data.
    if(status == CBE_DECODE_STATUS_OK || status == CBE_DECODE_STATUS_NEED_MORE_DATA)
    {
        status = decoder.end();
    }
    KSLOG_DEBUG("Status = %d", status);
    return decoder.decoded();
}

static std::shared_ptr<enc::encoding> decode(
    std::vector<uint8_t> data,
    cbe_decode_status& status)
{
    cbe_decoder decoder;
    status = decoder.decode(data);
    KSLOG_DEBUG("Status = %d", status);
    return decoder.decoded();
}

bool expect_encode_produces_data_and_status(
    int buffer_size,
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

void expect_decode_produces_data_and_status(
    int buffer_size,
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

void expect_encode_decode_produces_data_and_status(
    int buffer_size,
    std::shared_ptr<enc::encoding> expected_encoding,
    const std::vector<uint8_t> expected_memory,
    cbe_encode_status expected_encode_status,
    cbe_decode_status expected_decode_status)
{
    if(expect_encode_produces_data_and_status(
        buffer_size,
        expected_encoding,
        expected_memory,
        expected_encode_status))
    {
        expect_decode_produces_data_and_status(
            buffer_size,
            expected_memory,
            expected_encoding,
            expected_decode_status);
    }
}

void expect_encode_decode_with_shrinking_buffer_size(
    int min_buffer_size,
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
        expect_encode_decode_produces_data_and_status(
            buffer_size,
            expected_encoding,
            expected_memory,
            CBE_ENCODE_STATUS_OK,
            CBE_DECODE_STATUS_OK);
    }
    if(min_buffer_size > 1)
    {
        expect_encode_produces_data_and_status(
            min_buffer_size - 1,
            expected_encoding,
            expected_memory,
            CBE_ENCODE_STATUS_NEED_MORE_ROOM);
    }
}

void expect_encode_decode_produces_status(
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status expected_encode_status,
    cbe_decode_status expected_decode_status)
{
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    cbe_decode_status decode_status = CBE_DECODE_STATUS_OK;
    int buffer_size = 100;
    std::vector<uint8_t> memory = encode_data(buffer_size, encoding, encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
    std::shared_ptr<enc::encoding> actual_encoding = decode(memory, decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

void expect_encode_produces_status(
    std::shared_ptr<enc::encoding> encoding,
    cbe_encode_status expected_encode_status)
{
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    int buffer_size = 100;
    std::vector<uint8_t> memory = encode_data(buffer_size, encoding, encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
}

void expect_decode_produces_status(
    std::vector<uint8_t> document,
    cbe_decode_status expected_decode_status)
{
    cbe_decode_status decode_status = CBE_DECODE_STATUS_OK;
    std::shared_ptr<enc::encoding> actual_encoding = decode(document, decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

void expect_decode_stop_in_callback(
    std::shared_ptr<enc::encoding> encoding)
{
    cbe_encode_status expected_encode_status = CBE_ENCODE_STATUS_OK;
    cbe_encode_status encode_status = CBE_ENCODE_STATUS_OK;
    int buffer_size = 100;
    std::vector<uint8_t> data = encode_data(buffer_size, encoding, encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);

    cbe_decode_status expected_decode_status = CBE_DECODE_STATUS_STOPPED_IN_CALLBACK;
    cbe_decoder decoder(false);
    cbe_decode_status decode_status = decoder.decode(data);
    ASSERT_EQ(decode_status, expected_decode_status);
}

} // namespace cbe_test
