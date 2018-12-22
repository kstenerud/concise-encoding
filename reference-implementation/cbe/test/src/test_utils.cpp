#include "test_utils.h"
#include <sstream>

std::string as_string(const std::vector<uint8_t>& value)
{
    static char nybbles[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string str;
    for(uint8_t ch: value)
    {
        str.append(1, nybbles[ch>>4]);
        str.append(1, nybbles[ch&15]);
        str.append(1, ' ');
    }
    if(str.size() == 0)
    {
        return str;
    }
    return str.substr(0, str.size()-1);
}

std::vector<uint8_t> generate_array_length_field(int64_t length)
{
    int64_t length_bytes = length << 2;
    uint8_t* bytes = (uint8_t*)&length_bytes;
    if(length > 0x3fffffff)
    {
        length_bytes += 3;
        return std::vector<uint8_t>(bytes, bytes + 8);
    }
    if(length > 0x3fff)
    {
        length_bytes += 2;
        return std::vector<uint8_t>(bytes, bytes + 4);
    }
    if(length > 0x3f)
    {
        length_bytes += 1;
        return std::vector<uint8_t>(bytes, bytes + 2);
    }
    return std::vector<uint8_t>(bytes, bytes + 1);
}

std::string make_string(int length)
{
    std::stringstream stream;
    static const char characters[] =
    {
        '0','1','2','3','4','5','6','7','8','9'
    };
    static const int character_count = sizeof(characters) / sizeof(*characters);
    for(int i = 0; i < length; i++)
    {
        stream << characters[i % character_count];
    }
    return stream.str();
}

std::vector<uint8_t> make_7f_bytes(int length)
{
    std::vector<uint8_t> vec;
    for(int i = 0; i < length; i++)
    {
        vec.push_back((uint8_t)(i & 0x7f));
    }
    return vec;
}

std::vector<uint8_t> make_incrementing_bytes(int length, int start_value)
{
    std::vector<uint8_t> vec;
    int value = start_value;
    for(int i = 0; i < length; i++, value++)
    {
        vec.push_back((uint8_t)(value & 0x7f));
    }
    return vec;
}
