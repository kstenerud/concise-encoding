#include "encoding.h"

#include <quadmath.h> // Need -lquadmath at the end of the line
// stdlib.h:
// _Float128 strtof128 (const char *__restrict __nptr, char **__restrict __endptr)
// extern int strfromf128 (char *__dest, size_t __size, const char * __format, _Float128 __f)


// https://github.com/libdfp/libdfp

// Literal Suffixes:
//  * decimal32: 1.1df
//  * decimal64: 1.1dd
//  * decimal128: 1.1dl
//  * binary32: 1.1f
//  * binary64: 1.1
//  * binary128: 1.1l

namespace enc
{

#define IDENTIFIER_MAX_LENGTH 20000

std::string to_string(const int128_ct value)
{
    char buffer[50];
    char* ptr = buffer + sizeof(buffer) - 1;
    *ptr = 0;
    if(value == 0)
    {
        *--ptr = '0';
        return ptr;
    }

    // TODO: Does not handle -max
    __uint128_t uvalue = value > 0 ? value : -value;

    while(uvalue > 0)
    {
        *--ptr = (uvalue % 10) + '0';
        uvalue /= 10;
    }
    if(value < 0)
    {
        *--ptr = '-';
    }
    return ptr;
}

std::string to_string(uint128_ct value)
{
    char buffer[50];
    char* ptr = buffer + sizeof(buffer) - 1;
    *ptr = 0;
    if(value == 0)
    {
        *--ptr = '0';
        return ptr;
    }

    while(value > 0)
    {
        *--ptr = (value % 10) + '0';
        value /= 10;
    }
    return ptr;
}

std::string to_string(const float128_ct value)
{
    char buffer[50];
    int precision = 20;
    quadmath_snprintf(buffer, sizeof(buffer)-1, "%+-#46.*Qe", precision, value);
    buffer[sizeof(buffer)-1] = 0;
    return buffer;
}

std::string to_string(const std::vector<uint8_t>& value)
{
    const int max_length = IDENTIFIER_MAX_LENGTH;
    static char hex_table[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };

    std::string str;
    int length = value.size();
    if(max_length < length)
    {
        length = max_length;
    }

    for(int i = 0; i < length; i++)
    {
        char buffer[3] = {' ', ' ', ' '};
        uint8_t byte = value[i];
        buffer[0] = hex_table[byte >> 4];
        buffer[1] = hex_table[byte & 15];
        str.append(buffer, 3);
    }
    if(str.size() > 0)
    {
        if(value.size() > (unsigned)max_length)
        {
            str.append("...");
        }
        else
        {
            str.erase(str.size() - 1, 1);
        }
    }
    return str;
}

std::string to_string(const std::string& value)
{
    const int max_length = IDENTIFIER_MAX_LENGTH;
    if(value.size() <= (unsigned)max_length)
    {
        return value;
    }
    return value.substr(0, max_length);
}

std::string to_string(const smalltime value)
{
    char buff[40];
    snprintf(buff, sizeof(buff),
        "%04d--%02d-%02d:%02d-%02d-%02d",
        smalltime_get_year(value),
        smalltime_get_day(value),
        smalltime_get_month(value),
        smalltime_get_hour(value),
        smalltime_get_minute(value),
        smalltime_get_second(value)
        );
    int usec = smalltime_get_microsecond(value);
    int length = strlen(buff);
    if(usec != 0)
    {
        snprintf(buff + length, sizeof(buff) - length, ".%06d", usec);
        length = strlen(buff);
    }

    return buff;
}

std::string to_string(const nanotime value)
{
    char buff[40];
    snprintf(buff, sizeof(buff),
        "%04d--%02d-%02d:%02d-%02d-%02d",
        nanotime_get_year(value),
        nanotime_get_day(value),
        nanotime_get_month(value),
        nanotime_get_hour(value),
        nanotime_get_minute(value),
        nanotime_get_second(value)
        );
    int nsec = nanotime_get_nanosecond(value);
    int length = strlen(buff);
    if(nsec != 0)
    {
        snprintf(buff + length, sizeof(buff) - length, ".%09d", nsec);
        length = strlen(buff);
    }

    return buff;
}

// TODO: These are only approximations
std::string to_string(const dec128_ct value)
{
    return "~" + to_string((const float128_ct)value);
}


bool encoding::operator ==(const encoding& rhs) const
{
    // std::cout << as_string() << " vs " << rhs.as_string() << std::endl;
    return as_string() == rhs.as_string();
}

cbe_encode_status list_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status map_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status container_end_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status nil_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status padding_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status time_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status boolean_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status int_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status uint_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status float_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status decimal_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status string_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status bytes_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status uri_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status comment_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status string_header_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status bytes_header_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status uri_header_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status comment_header_encoding::encode(encoder& encoder) {return encoder.encode(*this);}
cbe_encode_status data_encoding::encode(encoder& encoder) {return encoder.encode(*this);}

std::shared_ptr<encoding> encoding::list()                          {return this->set_next(enc::list());}
std::shared_ptr<encoding> encoding::map()                           {return this->set_next(enc::map());}
std::shared_ptr<encoding> encoding::end()                           {return this->set_next(enc::end());}
std::shared_ptr<encoding> encoding::nil()                           {return this->set_next(enc::nil());}
std::shared_ptr<encoding> encoding::pad(int count)                  {return this->set_next(enc::pad(count));}
std::shared_ptr<encoding> encoding::time(smalltime value)           {return this->set_next(enc::time(value));}
std::shared_ptr<encoding> encoding::time(nanotime value)            {return this->set_next(enc::time(value));}
std::shared_ptr<encoding> encoding::str(const std::string& value)   {return this->set_next(enc::str(value));}
std::shared_ptr<encoding> encoding::bin(const std::vector<uint8_t>& value) {return this->set_next(enc::bin(value));}
std::shared_ptr<encoding> encoding::uri(const std::string& value)   {return this->set_next(enc::uri(value));}
std::shared_ptr<encoding> encoding::cmt(const std::string& value)   {return this->set_next(enc::cmt(value));}
std::shared_ptr<encoding> encoding::strh(int64_t byte_count)        {return this->set_next(enc::strh(byte_count));}
std::shared_ptr<encoding> encoding::binh(int64_t byte_count)        {return this->set_next(enc::binh(byte_count));}
std::shared_ptr<encoding> encoding::urih(int64_t byte_count)        {return this->set_next(enc::urih(byte_count));}
std::shared_ptr<encoding> encoding::cmth(int64_t byte_count)        {return this->set_next(enc::cmth(byte_count));}
std::shared_ptr<encoding> encoding::data(const std::vector<uint8_t>& value) {return this->set_next(enc::data(value));}
std::shared_ptr<encoding> encoding::bl(bool value)                  {return this->set_next(enc::bl(value));}
std::shared_ptr<encoding> encoding::si(int128_ct value)             {return this->set_next(enc::si(value));}
std::shared_ptr<encoding> encoding::si(int64_t high, uint64_t low)  {return this->set_next(enc::si(high, low));}
std::shared_ptr<encoding> encoding::ui(uint128_ct value)            {return this->set_next(enc::ui(value));}
std::shared_ptr<encoding> encoding::ui(uint64_t high, uint64_t low) {return this->set_next(enc::ui(high, low));}
std::shared_ptr<encoding> encoding::flt(float128_ct value)          {return this->set_next(enc::flt(value));}
std::shared_ptr<encoding> encoding::dec(dec128_ct value)            {return this->set_next(enc::dec(value));}

} // namespace enc
