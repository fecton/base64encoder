// Include necessary header files
#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <string>

#include "main.h" // Include the header file containing function declarations


// Define a Google Test case named TextEncodingTest, which tests the prepareTextForEncoding function
TEST(TextEncodingTest, PrepareTextForEncodingTest) {
	// Define an input vector containing the text "Hello\nWorld"
	std::vector<uint8_t> input_text{ 'H', 'e', 'l', 'l', 'o', '\n', 'W', 'o', 'r', 'l', 'd' };
	// Define an expected output vector containing the text "Hello\aWorld"
	std::vector<uint8_t> expected_output{ 'H', 'e', 'l', 'l', 'o', '\a', 'W', 'o', 'r', 'l', 'd' };

	// Call the prepareTextForEncoding function with the input vector
	auto output_text = prepareTextForEncoding(input_text);

	// Check if the output vector matches the expected output vector
	EXPECT_EQ(output_text, expected_output);
}

// Define another Google Test case named TextEncodingTest, which tests the prepareTextAfterDecoding function
TEST(TextEncodingTest, PrepareTextAfterDecodingTest) {
	// Define an input vector containing the text "Hello\aWorld"
	std::vector<uint8_t> input_text{ 'H', 'e', 'l', 'l', 'o', '\a', 'W', 'o', 'r', 'l', 'd' };
	// Define an expected output vector containing the text "Hello#World"
	std::vector<uint8_t> expected_output{ 'H', 'e', 'l', 'l', 'o', '#', 'W', 'o', 'r', 'l', 'd' };

	// Call the prepareTextAfterDecoding function with the input vector
	auto output_text = prepareTextAfterDecoding(input_text);

	// Check if the output vector matches the expected output vector
	EXPECT_EQ(output_text, expected_output);
}