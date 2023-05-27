#include "main.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 5) { // Check if the program is called with correct number of command line arguments
        printHelpMessage();
        return 1; // Return 1 to indicate an error
    }

    std::string operation  = argv[1]; // Get the operation from the command line argument
    std::string inputFile  = argv[2]; // Get the input file name from the command line argument
    std::string outputFile = argv[3]; // Get the output file name from the command line argument
    std::string format     = argv[4]; // Get the format from the command line argument

    try {
        EncoderFactory encoderFactory; // Create an instance of EncoderFactory
        std::unique_ptr<BaseEncoder> encoder = encoderFactory.createEncoder(format); // Create an encoder based on the specified format

        std::vector<uint8_t> fileData = readFile(inputFile); // Read file data into vector of bytes

        if (operation == "encode") { // If the operation is "encode"
            fileData = encoder->encode(fileData); // Encode the file data using the selected encoder
            fileData = prepareTextForEncoding(fileData); // Prepare the text data for encoding
        }
        else if (operation == "decode") { // If the operation is "decode"
            fileData = encoder->decode(fileData); // Decode the file data using the selected encoder
            fileData = prepareTextAfterDecoding(fileData); // Prepare the text data after decoding
        }
        else {
            std::cerr << "Invalid operation: " << operation << std::endl; // Print error message for invalid operation
            return 1; // Return 1 to indicate an error
        }

        writeFile(outputFile, fileData); // Write the processed data to the output file
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl; // Print error message for exceptions
        return 1; // Return 1 to indicate an error
    }

    return 0; // Return 0 to indicate successful execution
}