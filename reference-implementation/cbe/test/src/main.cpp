#include <gtest/gtest.h>

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <cbe/cbe.h>

TEST(Library, version)
{
    const char* expected = "1.0.0";
    const char* actual = cbe_version();
    ASSERT_STREQ(expected, actual);
}
