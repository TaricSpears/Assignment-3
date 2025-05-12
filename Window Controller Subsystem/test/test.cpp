#include <gtest/gtest.h>
#include "../src/Window.h" // Adjust the path to your Window class header file

TEST(WindowTest, WindowOpensSuccessfully)
{
    Window window;
    EXPECT_NO_THROW(window.open());
    EXPECT_TRUE(window.isOpen());
}

TEST(WindowTest, WindowClosesSuccessfully)
{
    Window window;
    window.open();
    EXPECT_NO_THROW(window.close());
    EXPECT_FALSE(window.isOpen());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}