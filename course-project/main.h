#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iconv.h>
#include <libbase64.h>
#include <regex>

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
        << "            - cp1251 - Cyrillic Windows-1251\n"
        << "            - koi8r - Cyrillic\n"
        << "            - cp866 - Cyrillic\n\n"
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

std::string vector_to_string(const std::vector<uint8_t>& vec) {
    /*
    Converts vector of uint7_t to string.

    Arguments:
        vec -- vector of uint7_t.

    Return value:
        String value of vec.*/
    return std::string(vec.begin(), vec.end());
}

std::vector<uint8_t> string_to_vector(const std::string& str) {
    /*
    Converts a string to a vector of uint8_t.

    Arguments:
        str - string to be converted

    Return value:
        Vector of uint8_t created from the input string.
    */
    return std::vector<uint8_t>(str.begin(), str.end());
}

// Basic class of the encoder
class BaseEncoder {
public:
    /*
    Encodes data with a provided encoding.

    Arguments:
        data - vector of uint8 bytes to be encoded

    Return value:
        Vector of uint8 bytes with encoded data.
    */
    virtual std::vector<uint8_t> encode(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> decode(const std::vector<uint8_t>& data) = 0;
};

class Base64Encoder : public BaseEncoder {
public:
    std::vector<uint8_t> encode(const std::vector<uint8_t>& data) {
        /*
        Encodes binary data using base64 algorithm.

        Arguments:
            data - Input binary data as std::vector<uint8_t>.

        Return value:
            Encoded data as std::vector<uint8_t>.
        */
        std::string input(data.begin(), data.end());
        std::vector<uint8_t> output(input.length() * 4 / 3 + 4); // Allocate a buffer for the output
        size_t outlen;
        int flags = 0; // Use default operation

        base64_encode(input.c_str(), input.length(), reinterpret_cast<char*>(output.data()), &outlen, flags);

        output.resize(outlen); // Resize output to the actual length
        return output;
    }

    std::vector<uint8_t> decode(const std::vector<uint8_t>& data) {
        /*
        Decodes Base64 encoded data. 

        Arguments: 
            data - vector of uint8_t const reference 

        Return value: 
            Vector of uint8_t
        */
        std::string input(data.begin(), data.end());
        std::vector<uint8_t> output(input.length() * 3 / 4 + 4); // Allocate a buffer for the output
        size_t outlen;
        int flags = 0; // Use default operation

        if (!base64_decode(input.c_str(), input.length(), reinterpret_cast<char*>(output.data()), &outlen, flags)) {
            throw std::runtime_error("Error decoding Base64 string");
        }

        output.resize(outlen); // Resize output to the actual length
        return output;
    }
};

// Specific classes of coders
class TextEncoder : public BaseEncoder {
public:
    /*
    Encodes and decodes strings from and to a specified encoding.

    Arguments:
        fromEncoding - the string containing the character-encoding name of the data to be encoded or decoded 
        toEncoding - the string containing the character-encoding name of the data to be encoded or decoded 
        data - std::vector of uint8_t representing the data to be encoded or decoded

    Return value:
        A std::vector of uint8_t representing the encoded or decoded data.
    */
    TextEncoder(const std::string& fromEncoding, const std::string& toEncoding)
        : fromEncoding_(fromEncoding), toEncoding_(toEncoding) {}

    std::vector<uint8_t> encode(const std::vector<uint8_t>& data) override {
        Base64Encoder encoder;
        return encoder.encode(convert(data, fromEncoding_, toEncoding_));
    }

    std::vector<uint8_t> decode(const std::vector<uint8_t>& data) override {
        Base64Encoder encoder;
        return encoder.decode(convert(data, toEncoding_, fromEncoding_));
    }

protected:
    std::vector<uint8_t> convert(const std::vector<uint8_t>& data, const std::string& fromEncoding, const std::string& toEncoding) {
        iconv_t conv = iconv_open(toEncoding.c_str(), fromEncoding.c_str());
        if (conv == (iconv_t)-1) {
            throw std::runtime_error("Failed to open iconv conversion descriptor");
        }

        std::string input = vector_to_string(data);

        std::string output(input.size() * 2, '\0');
        char* inputPtr = &input[0];
        char* outputPtr = &output[0];
        size_t inputLeft = input.size();
        size_t outputLeft = output.size();

        while (inputLeft > 0) {
            size_t res = iconv(conv, &inputPtr, &inputLeft, &outputPtr, &outputLeft);
            if (res == (size_t)-1) {
                iconv_close(conv);
                throw std::runtime_error("Error in iconv conversion");
            }
        }

        iconv_close(conv);

        output.resize(output.size() - outputLeft);

        return string_to_vector(output);
    }

private:
    std::string fromEncoding_;
    std::string toEncoding_;
};

// Modified classes to inherit from TextEncoder
class CP1251Encoder : public TextEncoder {
public:
    /*
    Constructor for CP1251Encoder class.

    Arguments
        None

    Return value
        None
    */
    CP1251Encoder() : TextEncoder("CP1251", "UTF-8") {}
};

class KOI8REncoder : public TextEncoder {
public:
    /*
    Constructs a TextEncoder for encoding text from UTF-8 to KOI8-R.

    Arguments
        None

    Return
        None
    */
    KOI8REncoder() : TextEncoder("KOI8-R", "UTF-8") {}
};

class CP866Encoder : public TextEncoder {
public:
    /*
    Constructs text encoder for converting from UTF-8 to CP866 encodings.

    Arguments
        None

    Return
        None
    */
    CP866Encoder() : TextEncoder("CP866", "UTF-8") {}
};

// Class of the encoder factory
class EncoderFactory {
public:
    /*
    Creates and returns an encoder based on the provided encoding type.

    Arguments: 
        encodingType - a string specifying the type of the encoder to be created.

    Return value:
        A unique pointer to a BaseEncoder subclass depending on the \encodingType\ argument.
    */
    std::unique_ptr<BaseEncoder> createEncoder(const std::string& encodingType) {
        if (encodingType == "cp1251") {
            return std::make_unique<CP1251Encoder>();
        }
        else if (encodingType == "koi8r") {
            return std::make_unique<KOI8REncoder>();
        }
        else if (encodingType == "cp866") {
            return std::make_unique<CP866Encoder>();
        }
        else {
            throw std::invalid_argument("Unknown encoding type: " + encodingType);
        }
    }
};

// Function to read a file
std::vector<uint8_t> readFile(const std::string& filename) {
    /*
    Read a file and return its content as a vector of uint8_t.

    Arguments: 
        filename - std::string, destination file

    Return value: 
        vector of uint8_t representing the content of the file
    */
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unknown error during file openning: " + filename);
    }

    std::vector<uint8_t> content(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));

    file.close();
    return content;
}

// Function to write a file
void writeFile(const std::string& filename, const std::vector<uint8_t>& content) {
    /*
    Writes content of the vector to the file.

    Arguments: 
        filename - std::string, destination file
        content - std::vector<uint8_t>, content

    Return value: 
        None
    */
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unknown error during file writing: " + filename);
    }

    file.write(reinterpret_cast<const char*>(content.data()), content.size());
    file.close();
}

int defineOperationSystem(void) {
    /*
    Determines the operating system.
    Return value:
        An integer representing the detected operating system:
        - 0: Windows
        - 1: Unix/Linux
        - 2: macOS / Other
    */
    int os = -1;
    #ifdef _WIN32
        os = 0;
    #elif __unix__
        os = 1;
    #elif __APPLE__
        os = 2;
    #else
        os = 2;
    #endif
    return os;
}

std::vector<uint8_t> prepareTextForEncoding(const std::vector<uint8_t>& text) {
    /*
    Prepares the text before encoding.
    Arguments:
        text - vector of 8-bit unsigned integers.
    Return value:
        Vector of 8-bit unsigned integers representing the prepared text.
    */
    std::vector<uint8_t> preparedText;

    int os = defineOperationSystem();

    for (const auto& ch : text) {
        if (ch == '\t') {
            preparedText.push_back(' ');
            preparedText.push_back(' ');
            preparedText.push_back(' ');
            preparedText.push_back(' ');
            break;
        }

        if (os == 0) {
            if (ch == '\n' or ch == '\r') {
                preparedText.push_back('\a');
                break;
            }
        }
        else if (os == 1) {
            if (ch == '\n') {
                preparedText.push_back('\a');
                break;
            }
        }
        preparedText.push_back(ch);
    }
    return preparedText;
}

std::vector<uint8_t> prepareTextAfterDecoding(const std::vector<uint8_t>& text) {
    /*
    Prepares the text after decoding.
    Arguments:
        text - vector of 8-bit unsigned integers.
    Return value:
        Vector of 8-bit unsigned integers representing the prepared text.
    */
    std::vector<uint8_t> preparedText;
    for (const auto& ch : text) {
        if (ch == '\a') {
            preparedText.push_back('\n');
        }
        else {
            preparedText.push_back(ch);
        }
    }
    return preparedText;
}