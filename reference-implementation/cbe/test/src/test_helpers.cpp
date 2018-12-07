#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

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

struct cbe_decode_process* new_always_false_decode_process()
{
    return cbe_decode_begin(get_always_false_decode_callbacks(), NULL);
}

struct cbe_decode_process* new_always_true_decode_process()
{
    return cbe_decode_begin(get_always_true_decode_callbacks(), NULL);
}


void expect_memory_after_operation(
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation,
                        std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();

    struct cbe_encode_process* encode_process = cbe_encode_begin(data, memory_size);
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, operation(encode_process));
    int actual_size = cbe_encode_get_buffer_offset(encode_process);
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process));
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_EQ(expected_size, actual_size);
    EXPECT_EQ(expected_memory, actual_memory);
}

void expect_incomplete_operation(
                        const int buffer_size,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation)
{
    uint8_t data[buffer_size];
    memset(data, 0xf7, buffer_size);
    struct cbe_encode_process* encode_process = cbe_encode_begin(data, buffer_size);
    EXPECT_NE(CBE_ENCODE_STATUS_OK, operation(encode_process));
    cbe_encode_end(encode_process);
    fflush(stdout);
}

void expect_incomplete_operation_decrementing(
                        const int buffer_size,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation)
{
    for(int size = buffer_size; size >= 0; size--)
    {
        expect_incomplete_operation(size, operation);
    }
}

void expect_piecemeal_operation(
                        const int buffer_cutoff,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation1,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation2,
                        std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();

    struct cbe_encode_process* encode_process = cbe_encode_begin(data, buffer_cutoff);
    EXPECT_NE(CBE_ENCODE_STATUS_OK, operation1(encode_process));
    fflush(stdout);

    int used_bytes = cbe_encode_get_buffer_offset(encode_process);
    data += used_bytes;
    cbe_encode_set_buffer(encode_process, data, memory_size - used_bytes);
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, operation2(encode_process));
    int actual_size = used_bytes + cbe_encode_get_buffer_offset(encode_process);
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process));
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(memory.data(), memory.data() + actual_size);
    EXPECT_EQ(expected_size, actual_size);
    EXPECT_EQ(expected_memory, actual_memory);
}

void expect_piecemeal_operation_decrementing(
                        const int buffer_cutoff_high,
                        const int buffer_cutoff_low,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation1,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation2,
                        std::vector<uint8_t> const& expected_memory)
{
    for(int size = buffer_cutoff_high; size >= buffer_cutoff_low; size--)
    {
        expect_piecemeal_operation(size, operation1, operation2, expected_memory);
    }
}

void expect_decode_encode(std::vector<uint8_t> const& expected_memory)
{
    const cbe_encode_status encode_status_decode_failed = (cbe_encode_status)10000;
    const uint8_t* document_start = expected_memory.data();
    int64_t document_size = expected_memory.size();

    expect_memory_after_operation([=](struct cbe_encode_process* encode_process)
    {
        cbe_decode_status status = perform_decode_encode(document_start, document_size, encode_process);
        if(status == CBE_DECODE_STATUS_OK)
        {
            return CBE_ENCODE_STATUS_OK;
        }
        KSLOG_WARN("perform_decode_encode returned decode status %d", status);
        return (cbe_encode_status)(encode_status_decode_failed + status);
    },
     expected_memory);
}

void buffered_decode_feed(cbe_decode_process* process, const std::vector<uint8_t>& src_document, const int buffer_size)
{
    uint8_t buffer[buffer_size];
    const uint8_t* const document = src_document.data();
    const int document_size = src_document.size();
    int document_offset = 0;
    int last_document_offset = 0;
    int bytes_used = buffer_size;

    for(;;)
    {
        int document_remaining_bytes = document_size - document_offset;
        KSLOG_DEBUG("Remaining document bytes: %d", document_remaining_bytes);
        int bytes_unused = buffer_size - bytes_used;
        KSLOG_DEBUG("Unused bytes: %d", bytes_unused);
        if(bytes_unused > 0)
        {
            memmove(buffer, buffer+bytes_used, bytes_unused);
        }
        int bytes_to_feed = document_remaining_bytes < buffer_size ? document_remaining_bytes : buffer_size;
        int bytes_to_fill = bytes_to_feed - bytes_unused;
        KSLOG_DEBUG("to feed: %d VS to fill: %d", bytes_to_feed, bytes_to_fill);
        KSLOG_DEBUG("memcpy(buffer+%d, document+%d, %d)", bytes_unused, document_offset, bytes_to_fill);
        memcpy(buffer+bytes_unused, document + document_offset, bytes_to_fill);

        KSLOG_DEBUG("Feeding bytes: %d", bytes_to_feed);
        cbe_decode_status status = cbe_decode_feed(process, buffer, bytes_to_feed);
        bytes_used = cbe_decode_get_buffer_offset(process);
        KSLOG_DEBUG("Bytes used: %d", bytes_used);

        document_offset += bytes_used;
        if(document_offset == last_document_offset)
        {
            ADD_FAILURE();
            return;
        }
        if(document_offset > document_size)
        {
            EXPECT_EQ(document_size, document_offset);
        }
        if(document_offset == document_size)
        {
            EXPECT_DECODE_OK(status);
            return;
        }
        EXPECT_EQ(CBE_DECODE_STATUS_NEED_MORE_DATA, status);
        last_document_offset = document_offset;
    }
}
