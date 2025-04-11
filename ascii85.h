#ifndef ASCII85_H
#define ASCII85_H

#include <iostream>
#include <cstdint>

// Функция для кодирования данных в формат ASCII85
void encode(std::istream& input, std::ostream& output);

// Функция для декодирования данных из формата ASCII85
bool decode(std::istream& input, std::ostream& output);

#endif // ASCII85_H 