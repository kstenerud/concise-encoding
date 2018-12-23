#include "encoding.h"

#include <quadmath.h> // Need -lquadmath at the end of the line
// stdlib.h:
// _Float128 strtof128 (const char *__restrict __nptr, char **__restrict __endptr)
// extern int strfromf128 (char *__dest, size_t __size, const char * __format, _Float128 __f)
#include <string.h>


// https://github.com/libdfp/libdfp

// Literal Suffixes:
//  * decimal32: 1.1df
//  * decimal64: 1.1dd
//  * decimal128: 1.1dl
//  * binary32: 1.1f
//  * binary64: 1.1
//  * binary128: 1.1l

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


#define IDENTIFIER_MAX_LENGTH 20


namespace enc
{

major_type get_major_type(bool value)
{
    (void) value; return ENCODE_TYPE_BOOLEAN;
}

major_type get_major_type(int8_t value)
{
    (void) value; return ENCODE_TYPE_INTEGER;
}

major_type get_major_type(int16_t value)
{
    (void) value; return ENCODE_TYPE_INTEGER;
}

major_type get_major_type(int32_t value)
{
    (void) value; return ENCODE_TYPE_INTEGER;
}

major_type get_major_type(int64_t value)
{
    (void) value; return ENCODE_TYPE_INTEGER;
}

major_type get_major_type(__int128 value)
{
    (void) value; return ENCODE_TYPE_INTEGER;
}

major_type get_major_type(float value)
{
    (void) value; return ENCODE_TYPE_FLOAT;
}

major_type get_major_type(double value)
{
    (void) value; return ENCODE_TYPE_FLOAT;
}

major_type get_major_type(__float128 value)
{
    (void) value; return ENCODE_TYPE_FLOAT;
}

major_type get_major_type(_Decimal32 value)
{
    (void) value; return ENCODE_TYPE_DECIMAL;
}

major_type get_major_type(_Decimal64 value)
{
    (void) value; return ENCODE_TYPE_DECIMAL;
}

major_type get_major_type(_Decimal128 value)
{
    (void) value; return ENCODE_TYPE_DECIMAL;
}

major_type get_major_type(const std::string& value)
{
    (void) value; return ENCODE_TYPE_STRING;
}

major_type get_major_type(const std::vector<uint8_t>& value)
{
    (void) value; return ENCODE_TYPE_BINARY;
}


static int g_days_to_the_month[] =
{
    0, // Nothing
    0, // January
    31, // February
    31 + 28, // March
    31 + 28 + 31, // April
    31 + 28 + 31 + 30, // May
    31 + 28 + 31 + 30 + 31, // June
    31 + 28 + 31 + 30 + 31 + 30, // July
    31 + 28 + 31 + 30 + 31 + 30 + 31, // August
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31, // September
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30, // October
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31, // November
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30, // December
};

static inline int is_leap_year(int year)
{
    return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
}

unsigned int to_doy(int year, unsigned int month, unsigned int day)
{
    // Extremely naive converter, only good for modern dates.
    unsigned int days = g_days_to_the_month[month] + day;
    if(is_leap_year(year))
    {
        days++;
    }
    return days;
}

static std::string to_string(__int128 value)
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

static std::string to_string(__float128 value)
{
    char buffer[50];
    int precision = 20;
    quadmath_snprintf(buffer, sizeof(buffer)-1, "%+-#46.*Qe", precision, value);
    buffer[sizeof(buffer)-1] = 0;
    return buffer;
}

static std::string to_string(const std::vector<uint8_t>& value, int max_length)
{
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

static std::string to_string(const std::string& value, int max_length)
{
    if(value.size() <= (unsigned)max_length)
    {
        return value;
    }
    return value.substr(0, max_length);
}

static std::string to_time_string(smalltime value)
{
    char buff[30];
    snprintf(buff, sizeof(buff),
        "%04d-%03d:%02d-%02d-%02d",
        smalltime_get_year(value),
        smalltime_get_day(value),
        smalltime_get_hour(value),
        smalltime_get_minute(value),
        smalltime_get_second(value)
        );
    int usec = smalltime_get_microsecond(value);
    int length = strlen(buff);
    if(usec != 0)
    {
        snprintf(buff + length, sizeof(buff) - length, ".%d", usec);
        length = strlen(buff);
    }

    return buff;
}

// TODO: These are only approximations
static std::string to_string(_Decimal32 value)
{
    return "~" + std::to_string((double)value);
}
static std::string to_string(_Decimal64 value)
{
    return "~" + std::to_string((double)value);
}
static std::string to_string(_Decimal128 value)
{
    return "~" + to_string((__float128)value);
}

std::string to_id_string(bool value)        {return std::string("bl(")   + std::to_string(value) + ")";}
std::string to_id_string(int8_t value)      {return std::string("i8(")   + std::to_string(value) + ")";}
std::string to_id_string(int16_t value)     {return std::string("i16(")  + std::to_string(value) + ")";}
std::string to_id_string(int32_t value)     {return std::string("i32(")  + std::to_string(value) + ")";}
std::string to_id_string(int64_t value)     {return std::string("i64(")  + std::to_string(value) + ")";}
std::string to_id_string(__int128 value)    {return std::string("i128(") + to_string(value)      + ")";}
std::string to_id_string(float value)       {return std::string("f32(")  + std::to_string(value) + ")";}
std::string to_id_string(double value)      {return std::string("f64(")  + std::to_string(value) + ")";}
std::string to_id_string(__float128 value)  {return std::string("f128(") + to_string(value)      + ")";}
std::string to_id_string(_Decimal32 value)  {return std::string("d32(")  + to_string(value)      + ")";}
std::string to_id_string(_Decimal64 value)  {return std::string("d64(")  + to_string(value)      + ")";}
std::string to_id_string(_Decimal128 value) {return std::string("d128(") + to_string(value)      + ")";}
std::string to_id_string(const std::string& value) {return std::string("s(") + to_string(value, IDENTIFIER_MAX_LENGTH) + ")";}
std::string to_id_string(const std::vector<uint8_t>& value) {return std::string("b(") + to_string(value, IDENTIFIER_MAX_LENGTH) + ")";}
std::string to_id_string_time(smalltime value) {return std::string("t(") + to_time_string(value)  + ")";}
std::string to_id_string(const std::string& name, int64_t value) {return name + "(" + std::to_string(value) + ")";}


encoding::encoding(const enc::major_type type, const size_t size, const std::string& string_value)
: _type(type)
, _size(size)
, _string_value(string_value)
{
    KSLOG_DEBUG("Create type %d: %s", _type, _string_value.c_str());
}

std::shared_ptr<encoding> encoding::next()
{
    return _next;
}

std::shared_ptr<encoding> encoding::set_next(std::shared_ptr<encoding> next)
{
    if(_last)
    {
        _last->_next = next;
    }
    else
    {
        _next = next;
    }
    _last = next;

    // FIXME:
    //
    // Ugly hack to keep things in shared_ptr land because there's no way
    // to get ahold of an owning smart pointer from within an object,
    // since it's not baked into the language.
    //
    // The fallout is a broken contract about this shared_ptr's lifetime
    // and validity.
    //
    // This is workable from within the confines of the testing framework
    // since the lifetime of an encoding object will never actually exceed
    // its creation context.
    //
    // Have I ever mentioned how much I hate C++?
    return std::shared_ptr<encoding>(this, [](encoding*){});
}

const std::string encoding::as_string() const
{
    std::string str = _string_value;
    if(_next)
    {
        str.append("->");
        str.append(_next->as_string());
    }

    return str;
}

enc::major_type encoding::get_type() const
{
    return _type;
}

bool encoding::is_equal_in_type(const encoding& rhs) const
{        
    return _type == rhs._type && is_equal(rhs);
}

bool encoding::is_equal_in_type_and_size(const encoding& rhs) const
{
    return _size == rhs._size && is_equal_in_type(rhs);
}

bool encoding::operator ==(const encoding& rhs) const
{
    return is_equal(rhs);
}

bool encoding::has_value(bool value) const
{
    (void) value; return false;
}

bool encoding::has_value(int8_t value) const
{
    (void) value; return false;
}

bool encoding::has_value(int16_t value) const
{
    (void) value; return false;
}

bool encoding::has_value(int32_t value) const
{
    (void) value; return false;
}

bool encoding::has_value(int64_t value) const
{
    (void) value; return false;
}

bool encoding::has_value(__int128 value) const
{
    (void) value; return false;
}

bool encoding::has_value(float value) const
{
    (void) value; return false;
}

bool encoding::has_value(double value) const
{
    (void) value; return false;
}

bool encoding::has_value(__float128 value) const
{
    (void) value; return false;
}

bool encoding::has_value(_Decimal32 value) const
{
    (void) value; return false;
}

bool encoding::has_value(_Decimal64 value) const
{
    (void) value; return false;
}

bool encoding::has_value(_Decimal128 value) const
{
    (void) value; return false;
}

bool encoding::has_value(const std::string& value) const
{
    (void) value; return false;
}

bool encoding::has_value(const std::vector<uint8_t>& value) const
{
    (void) value; return false;
}




std::ostream& operator << (std::ostream& os, const encoding& rhs)
{
    os << to_string(rhs);
    return os;
}

cbe_encode_status list_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status map_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status end_container_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status empty_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status padding_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status time_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status string_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status binary_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status string_header_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status binary_header_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status data_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status boolean_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<int8_t>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<int16_t>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<int32_t>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<int64_t>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

cbe_encode_status int128_encoding::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<float>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<double>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status number_encoding<__float128>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status dfp_encoding<_Decimal32>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status dfp_encoding<_Decimal64>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}

template <> cbe_encode_status dfp_encoding<_Decimal128>::encode(encoder& encoder)
{
    return encoder.encode(*this);
}



std::shared_ptr<encoding> encoding::list()                           {return this->set_next(enc::list());}
std::shared_ptr<encoding> encoding::map()                            {return this->set_next(enc::map());}
std::shared_ptr<encoding> encoding::end()                            {return this->set_next(enc::end());}
std::shared_ptr<encoding> encoding::empty()                          {return this->set_next(enc::empty());}
std::shared_ptr<encoding> encoding::pad(int count)                   {return this->set_next(enc::pad(count));}
std::shared_ptr<encoding> encoding::smtime(smalltime value)          {return this->set_next(enc::smtime(value));}
std::shared_ptr<encoding> encoding::str(const std::string& value)    {return this->set_next(enc::str(value));}
std::shared_ptr<encoding> encoding::bin(const std::vector<uint8_t>& value) {return this->set_next(enc::bin(value));}
std::shared_ptr<encoding> encoding::strh(int64_t byte_count)         {return this->set_next(enc::strh(byte_count));}
std::shared_ptr<encoding> encoding::binh(int64_t byte_count)         {return this->set_next(enc::binh(byte_count));}
std::shared_ptr<encoding> encoding::data(const std::vector<uint8_t>& value) {return this->set_next(enc::data(value));}
std::shared_ptr<encoding> encoding::bl(bool value)                   {return this->set_next(enc::bl(value));}
std::shared_ptr<encoding> encoding::i8(int8_t value)                 {return this->set_next(enc::i8(value));}
std::shared_ptr<encoding> encoding::i16(int16_t value)               {return this->set_next(enc::i16(value));}
std::shared_ptr<encoding> encoding::i32(int32_t value)               {return this->set_next(enc::i32(value));}
std::shared_ptr<encoding> encoding::i64(int64_t value)               {return this->set_next(enc::i64(value));}
std::shared_ptr<encoding> encoding::i128(__int128 value)             {return this->set_next(enc::i128(value));}
std::shared_ptr<encoding> encoding::i128(int64_t high, uint64_t low) {return this->set_next(enc::i128(high, low));}
std::shared_ptr<encoding> encoding::f32(float value)                 {return this->set_next(enc::f32(value));}
std::shared_ptr<encoding> encoding::f64(double value)                {return this->set_next(enc::f64(value));}
std::shared_ptr<encoding> encoding::f128(__float128 value)           {return this->set_next(enc::f128(value));}
std::shared_ptr<encoding> encoding::d32(_Decimal32 value)            {return this->set_next(enc::d32(value));}
std::shared_ptr<encoding> encoding::d64(_Decimal64 value)            {return this->set_next(enc::d64(value));}
std::shared_ptr<encoding> encoding::d128(_Decimal128 value)          {return this->set_next(enc::d128(value));}

std::shared_ptr<list_encoding>                 list()                           {return std::make_shared<list_encoding>();}
std::shared_ptr<map_encoding>                  map()                            {return std::make_shared<map_encoding>();}
std::shared_ptr<end_container_encoding>        end()                            {return std::make_shared<end_container_encoding>();}
std::shared_ptr<empty_encoding>                empty()                          {return std::make_shared<empty_encoding>();}
std::shared_ptr<time_encoding>                 smtime(smalltime value)          {return std::make_shared<time_encoding>(value);}
std::shared_ptr<time_encoding>                 smtime(int year, int month, int day, int hour, int minute, int second, int usec) {return smtime(smalltime_new(year, enc::to_doy(year, month, day), hour, minute, second, usec));}
std::shared_ptr<string_encoding>               str(const std::string& value)    {return std::make_shared<string_encoding>(value);}
std::shared_ptr<binary_encoding>               bin(const std::vector<uint8_t>& value) {return std::make_shared<binary_encoding>(value);}
std::shared_ptr<string_header_encoding>        strh(int64_t byte_count)         {return std::make_shared<string_header_encoding>(byte_count);}
std::shared_ptr<binary_header_encoding>        binh(int64_t byte_count)         {return std::make_shared<binary_header_encoding>(byte_count);}
std::shared_ptr<data_encoding>                 data(const std::vector<uint8_t>& value) {return std::make_shared<data_encoding>(value);}
std::shared_ptr<boolean_encoding>              bl(bool value)                   {return std::make_shared<boolean_encoding>(value);}
std::shared_ptr<number_encoding<int8_t>>       i8(int8_t value)                 {return std::make_shared<number_encoding<int8_t>>(value);}
std::shared_ptr<number_encoding<int16_t>>      i16(int16_t value)               {return std::make_shared<number_encoding<int16_t>>(value);}
std::shared_ptr<number_encoding<int32_t>>      i32(int32_t value)               {return std::make_shared<number_encoding<int32_t>>(value);}
std::shared_ptr<number_encoding<int64_t>>      i64(int64_t value)               {return std::make_shared<number_encoding<int64_t>>(value);}
std::shared_ptr<int128_encoding>               i128(__int128 value)             {return std::make_shared<int128_encoding>(value);}
std::shared_ptr<int128_encoding>               i128(int64_t high, uint64_t low) {return i128((((__int128)high) << 64) + low);}
std::shared_ptr<number_encoding<float>>        f32(float value)                 {return std::make_shared<number_encoding<float>>(value);}
std::shared_ptr<number_encoding<double>>       f64(double value)                {return std::make_shared<number_encoding<double>>(value);}
std::shared_ptr<number_encoding<__float128>>   f128(__float128 value)           {return std::make_shared<number_encoding<__float128>>(value);}
std::shared_ptr<dfp_encoding<_Decimal32>>      d32(_Decimal32 value)            {return std::make_shared<dfp_encoding<_Decimal32>>(value);}
std::shared_ptr<dfp_encoding<_Decimal64>>      d64(_Decimal64 value)            {return std::make_shared<dfp_encoding<_Decimal64>>(value);}
std::shared_ptr<dfp_encoding<_Decimal128>>     d128(_Decimal128 value)           {return std::make_shared<dfp_encoding<_Decimal128>>(value);}
std::shared_ptr<padding_encoding>              pad(int byte_count)              {return std::make_shared<padding_encoding>(byte_count);}

} // namespace enc
