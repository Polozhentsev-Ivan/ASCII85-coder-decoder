#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "ascii85.h"

// Вспомогательная функция для тестирования
std::string encodeString(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;
    encode(iss, oss);
    return oss.str();
}

std::string decodeString(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;
    decode(iss, oss);
    return oss.str();
}

// Тесты для функции кодирования
TEST(ASCII85Test, BasicEncoding) {
    EXPECT_EQ(encodeString("Hello, World!"), "87cURD_*#4DfTZ)+T");
}

TEST(ASCII85Test, ZeroBytes) {
    std::string input(4, '\0');
    EXPECT_EQ(encodeString(input), "z");
}

TEST(ASCII85Test, IncompleteBlock) {
    EXPECT_EQ(encodeString("Test"), "<+U,m");
}

// Тесты для функции декодирования
TEST(ASCII85Test, BasicDecoding) {
    EXPECT_EQ(decodeString("87cURD_*#4DfTZ)+T"), "Hello, World!");
}

TEST(ASCII85Test, ZeroDecoding) {
    std::string expected(4, '\0');
    EXPECT_EQ(decodeString("z"), expected);
}

TEST(ASCII85Test, IncompleteBlockDecoding) {
    EXPECT_EQ(decodeString("<+U,m"), "Test");
}

// Тесты для полного цикла кодирования-декодирования
TEST(ASCII85Test, EncodeDecode) {
    std::string original = "Test ASCII85 encoding and decoding!";
    std::string encoded = encodeString(original);
    std::string decoded = decodeString(encoded);
    EXPECT_EQ(decoded, original);
}

TEST(ASCII85Test, EncodeDecodeEmpty) {
    std::string original = "";
    std::string encoded = encodeString(original);
    std::string decoded = decodeString(encoded);
    EXPECT_EQ(decoded, original);
}

TEST(ASCII85Test, EncodeDecodeBinary) {
    std::vector<char> binaryData = {(char)0x00, (char)0x01, (char)0x02, (char)0xFF, (char)0xFE, (char)0xAB, (char)0xCD};
    std::string original(binaryData.begin(), binaryData.end());
    std::string encoded = encodeString(original);
    std::string decoded = decodeString(encoded);
    EXPECT_EQ(decoded, original);
}

TEST(ASCII85Test, EncodeDecodeUnicode) {
    std::string original = "Привет, мир! 你好，世界！";
    std::string encoded = encodeString(original);
    std::string decoded = decodeString(encoded);
    EXPECT_EQ(decoded, original);
}

// Тесты для ошибок декодирования
TEST(ASCII85Test, InvalidCharacter) {
    std::istringstream iss("v{|}~");  // Символы вне допустимого диапазона ASCII85
    std::ostringstream oss;
    EXPECT_FALSE(decode(iss, oss));
}

TEST(ASCII85Test, SingleCharAtEnd) {
    std::istringstream iss("ABCDE!");  // Одиночный символ в конце
    std::ostringstream oss;
    EXPECT_FALSE(decode(iss, oss));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}