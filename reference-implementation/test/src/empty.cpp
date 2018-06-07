#include "test_helpers.h"

TEST(EmptyTest, empty)
{
    expect_memory_after_operation([](cbe_buffer* buffer) {
        return cbe_add_empty(buffer);
    }, {TYPE_EMPTY});
}
