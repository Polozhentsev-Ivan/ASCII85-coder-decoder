#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <fstream>

const uint32_t ASCII85_BASE = 85;
const char ASCII85_OFFSET = '!';
const char ASCII85_ZERO = 'z';

void encode(std::istream& input, std::ostream& output) {
    std::vector<unsigned char> buffer(4);
    size_t bytesRead;
    
    while (!input.eof()) {
        input.read(reinterpret_cast<char*>(buffer.data()), 4);
        bytesRead = input.gcount();
        
        if (bytesRead == 0) {
            break;
        }
        
        for (size_t i = bytesRead; i < 4; ++i) {
            buffer[i] = 0;
        }
        
        uint32_t value = ((uint32_t)buffer[0] << 24) | 
                        ((uint32_t)buffer[1] << 16) | 
                        ((uint32_t)buffer[2] << 8) | 
                        (uint32_t)buffer[3];
        
        if (value == 0 && bytesRead == 4) {
            output.put(ASCII85_ZERO);
            continue;
        }
        
        char encoded[5];
        for (int i = 4; i >= 0; --i) {
            encoded[i] = (value % ASCII85_BASE) + ASCII85_OFFSET;
            value /= ASCII85_BASE;
        }
        
        if (bytesRead < 4) {
            output.write(encoded, bytesRead + 1);
        } else {
            output.write(encoded, 5);
        }
    }
}

bool decode(std::istream& input, std::ostream& output) {
    std::vector<char> encodedBuffer(5);
    size_t encodedPos = 0;
    
    while (true) {
        char c;
        input.get(c);
        
        if (input.eof()) {
            break;
        }
        
        if (c <= ' ' || c > '~') {
            continue;
        }
        
        if (c == ASCII85_ZERO && encodedPos == 0) {
            char zeros[4] = {0, 0, 0, 0};
            output.write(zeros, 4);
            continue;
        }
        
        if (c < '!' || c > 'u') {
            std::cerr << "Ошибка декодирования: недопустимый символ ASCII85" << std::endl;
            return false;
        }
        
        encodedBuffer[encodedPos++] = c;
        
        if (encodedPos == 5) {
            uint32_t value = 0;
            
            for (size_t i = 0; i < 5; ++i) {
                value = value * ASCII85_BASE + (encodedBuffer[i] - ASCII85_OFFSET);
            }
            
            unsigned char decoded[4];
            decoded[0] = (value >> 24) & 0xFF;
            decoded[1] = (value >> 16) & 0xFF;
            decoded[2] = (value >> 8) & 0xFF;
            decoded[3] = value & 0xFF;
            
            output.write(reinterpret_cast<char*>(decoded), 4);
            encodedPos = 0;
        }
    }
    
    if (encodedPos > 1) {
        uint32_t value = 0;
        size_t originalBytes = encodedPos - 1;
        
        for (size_t i = 0; i < encodedPos; ++i) {
            value = value * ASCII85_BASE + (encodedBuffer[i] - ASCII85_OFFSET);
        }
        
        for (size_t i = encodedPos; i < 5; ++i) {
            value = value * ASCII85_BASE + 84; // 'u' - '!' = 84
        }
        
        unsigned char decoded[4];
        decoded[0] = (value >> 24) & 0xFF;
        decoded[1] = (value >> 16) & 0xFF;
        decoded[2] = (value >> 8) & 0xFF;
        decoded[3] = value & 0xFF;
        
        output.write(reinterpret_cast<char*>(decoded), originalBytes);
    } else if (encodedPos == 1) {
        std::cerr << "Ошибка декодирования: одиночный символ в конце" << std::endl;
        return false;
    }
    
    return true;
}

#ifndef ASCII85_LIB

void printUsage() {
    std::cerr << "Использование: ascii85 [ОПЦИЯ]" << std::endl;
    std::cerr << "Кодирует или декодирует ASCII85 из стандартного ввода в стандартный вывод." << std::endl;
    std::cerr << std::endl;
    std::cerr << "  -e, --encode    кодирует данные (это режим по умолчанию)" << std::endl;
    std::cerr << "  -d, --decode    декодирует данные" << std::endl;
    std::cerr << "  -h, --help      показывает эту справку и выходит" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Использование для работы с файлами:" << std::endl;
    std::cerr << "  ascii85 encode input_file output_file" << std::endl;
    std::cerr << "  ascii85 decode input_file output_file" << std::endl;
}

int main(int argc, char* argv[]) {
    bool encodeMode = true;
    std::string command;
    
    if (argc < 2) {
        printUsage();
        return 1;
    }

    command = argv[1];
    
    if (command == "-d" || command == "--decode") {
        encodeMode = false;
    } else if (command == "-e" || command == "--encode") {
        encodeMode = true;
    } else if (command == "-h" || command == "--help") {
        printUsage();
        return 0;
    } else if (command == "encode") {
        encodeMode = true;
        
        if (argc != 4) {
            std::cerr << "Использование для кодирования: ascii85 encode input_file output_file" << std::endl;
            return 1;
        }
        
        std::ifstream input(argv[2], std::ios::binary);
        if (!input) {
            std::cerr << "Ошибка открытия входного файла: " << argv[2] << std::endl;
            return 1;
        }
        
        std::ofstream output(argv[3], std::ios::binary);
        if (!output) {
            std::cerr << "Ошибка открытия выходного файла: " << argv[3] << std::endl;
            return 1;
        }
        
        encode(input, output);
        return 0;
    } else if (command == "decode") {
        encodeMode = false;
        
        if (argc != 4) {
            std::cerr << "Использование для декодирования: ascii85 decode input_file output_file" << std::endl;
            return 1;
        }
        
        std::ifstream input(argv[2], std::ios::binary);
        if (!input) {
            std::cerr << "Ошибка открытия входного файла: " << argv[2] << std::endl;
            return 1;
        }
        
        std::ofstream output(argv[3], std::ios::binary);
        if (!output) {
            std::cerr << "Ошибка открытия выходного файла: " << argv[3] << std::endl;
            return 1;
        }
        
        if (!decode(input, output)) {
            return 1;
        }
        return 0;
    } else {
        std::cerr << "Неизвестная команда: " << command << std::endl;
        printUsage();
        return 1;
    }
    
    // Обработка стандартного режима (ввод из stdin, вывод в stdout)
    std::ios_base::sync_with_stdio(false);
    std::cin.unsetf(std::ios::skipws);

    if (encodeMode) {
        encode(std::cin, std::cout);
    } else {
        if (!decode(std::cin, std::cout)) {
            return 1;
        }
    }
    
    return 0;
}

#endif