#include "main.h"

#include <iostream>

void printHelpMessage() {
    std::cout << "Usage: b64encoder <command> <input_file> <output_file> <encoding>\n\n"
        << "Commands:\n"
        << "    encode    Encode the input file using the specified encoding and save the result to the output file.\n"
        << "    decode    Decode the input file using the specified encoding and save the result to the output file.\n\n"
        << "Arguments:\n"
        << "    input_file    The path to the input file.\n"
        << "    output_file   The path to the output file.\n\n"
        << "    encoding   Specify the encoding to be used for the operation. Supported encodings: UTF8, UTF16LE, UTF16BE. If not provided, the program will use UTF8 encoding by default.\n"
        << "        Supported encodings:\n"
        << "            - cp1251\n"
        << "            - koi8r\n"
        << "            - cp866\n\n"
        << "Description:\n"
        << "    b64encoder is a command-line program that can encode or decode text files using a\n"
        << "    simple encoding algorithm. The encoding algorithm replaces all newline characters\n"
        << "    (both LF and CRLF) with the ASCII bell character (\a), and adds the \"\\a\" character\n"
        << "    to the beginning of the file to indicate that it has been encoded. The decoding\n"
        << "    algorithm does the opposite, replacing all occurrences of \"\\a\" with a newline and\n"
        << "    removing all other occurrences of the bell character.\n\n"
        << "Examples:\n"
        << "    b64encoder encode example.html encoded.txt cp866\n"
        << "    b64encoder decode encoded.txt decoded.txt cp866\n";
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printHelpMessage();
        return 1;
    }

    std::string operation = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];
    std::string format = argv[4];

    try {
        EncoderFactory encoderFactory;
        std::unique_ptr<BaseEncoder> encoder = encoderFactory.createEncoder(format);

        std::vector<uint8_t> fileData = readFile(inputFile);

        if (operation == "encode") {
            fileData = encoder->encode(fileData);
            fileData = prepareTextForEncoding(fileData);
        }
        else if (operation == "decode") {
            fileData = encoder->decode(fileData);
            fileData = prepareTextAfterDecoding(fileData);
        }
        else {
            std::cerr << "Invalid operation: " << operation << std::endl;
            return 1;
        }

        writeFile(outputFile, fileData);
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}