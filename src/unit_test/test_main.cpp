#include <gtest/gtest.h>

//er apparently need this to run test with gmock test package. I am not sure if we should just use the gtest_main.cc... let me know your thoughts
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}