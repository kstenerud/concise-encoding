#include "as_string.h"

static char g_nybbles[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string as_string(std::vector<uint8_t>& value)
{
    std::string str;
    for(uint8_t ch: value)
    {
        str.append(1, g_nybbles[ch>>4]);
        str.append(1, g_nybbles[ch&15]);
        str.append(1, ' ');
    }
    if(str.size() == 0)
    {
        return str;
    }
    return str.substr(0, str.size()-1);
}
