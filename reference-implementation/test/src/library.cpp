#include <gtest/gtest.h>
#include <cbe/cbe.h>

TEST(Library, version)
{
    const char* expected = "1.0.0";
    const char* actual = cbe_version();
    ASSERT_STREQ(expected, actual);
}
