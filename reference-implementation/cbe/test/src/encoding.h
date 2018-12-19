#pragma once

// https://github.com/libdfp/libdfp


// Literal Suffixes:
//  * decimal32: 1.1df
//  * decimal64: 1.1dd
//  * decimal128: 1.1dl
//  * binary32: 1.1f
//  * binary64: 1.1
//  * binary128: 1.1l

#include <iostream>
#include <memory>
#include <vector>
#include <quadmath.h> // Need -lquadmath at the end of the line
// stdlib.h:
// _Float128 strtof128 (const char *__restrict __nptr, char **__restrict __endptr)
// extern int strfromf128 (char *__dest, size_t __size, const char * __format, _Float128 __f)
#include <string.h>

// Use the same type names as C
#include <decimal/decimal>
typedef std::decimal::decimal32::__decfloat32   _Decimal32;
typedef std::decimal::decimal64::__decfloat64   _Decimal64;
typedef std::decimal::decimal128::__decfloat128 _Decimal128;

#include <smalltime/smalltime.h>



#define IDENTIFIER_MAX_LENGTH 20

namespace enc
{
    typedef enum
    {
        ENCODE_TYPE_BOOLEAN,
        ENCODE_TYPE_INTEGER,
        ENCODE_TYPE_FLOAT,
        ENCODE_TYPE_DECIMAL,
        ENCODE_TYPE_TIME,
        ENCODE_TYPE_STRING,
        ENCODE_TYPE_BINARY,
        ENCODE_TYPE_LIST,
        ENCODE_TYPE_MAP,
        ENCODE_TYPE_CONTAINER_END,
        ENCODE_TYPE_EMPTY,
        ENCODE_TYPE_PADDING,
    } major_type;

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

    static std::string to_string(_Decimal32 value)
    {
        (void)value;
        return "TODO";
    }
    static std::string to_string(_Decimal64 value)
    {
        (void)value;
        return "TODO";
    }
    static std::string to_string(_Decimal128 value)
    {
        (void)value;
        return "TODO";
    }


    static std::string to_id_string(bool value)        {return std::string("bl(")   + std::to_string(value) + ")";}
    static std::string to_id_string(int8_t value)      {return std::string("i8(")   + std::to_string(value) + ")";}
    static std::string to_id_string(int16_t value)     {return std::string("i16(")  + std::to_string(value) + ")";}
    static std::string to_id_string(int32_t value)     {return std::string("i32(")  + std::to_string(value) + ")";}
    static std::string to_id_string(int64_t value)     {return std::string("i64(")  + std::to_string(value) + ")";}
    static std::string to_id_string(__int128 value)    {return std::string("i128(") + to_string(value)      + ")";}
    static std::string to_id_string(float value)       {return std::string("f32(")  + std::to_string(value) + ")";}
    static std::string to_id_string(double value)      {return std::string("f64(")  + std::to_string(value) + ")";}
    static std::string to_id_string(__float128 value)  {return std::string("f128(") + to_string(value)      + ")";}
    static std::string to_id_string(_Decimal32 value)  {return std::string("d32(")  + to_string(value)      + ")";}
    static std::string to_id_string(_Decimal64 value)  {return std::string("d64(")  + to_string(value)      + ")";}
    static std::string to_id_string(_Decimal128 value) {return std::string("d128(") + to_string(value)      + ")";}
    static std::string to_id_string(const std::string& value) {return std::string("s(") + to_string(value, IDENTIFIER_MAX_LENGTH) + ")";}
    static std::string to_id_string(const std::vector<uint8_t>& value) {return std::string("b(") + to_string(value, IDENTIFIER_MAX_LENGTH) + ")";}
    static std::string to_id_string_time(smalltime value) {return std::string("t(") + to_time_string(value)  + ")";}

    static major_type get_major_type(bool value)                        { (void) value; return ENCODE_TYPE_BOOLEAN; }
    static major_type get_major_type(int8_t value)                      { (void) value; return ENCODE_TYPE_INTEGER; }
    static major_type get_major_type(int16_t value)                     { (void) value; return ENCODE_TYPE_INTEGER; }
    static major_type get_major_type(int32_t value)                     { (void) value; return ENCODE_TYPE_INTEGER; }
    static major_type get_major_type(int64_t value)                     { (void) value; return ENCODE_TYPE_INTEGER; }
    static major_type get_major_type(__int128 value)                    { (void) value; return ENCODE_TYPE_INTEGER; }
    static major_type get_major_type(float value)                       { (void) value; return ENCODE_TYPE_FLOAT; }
    static major_type get_major_type(double value)                      { (void) value; return ENCODE_TYPE_FLOAT; }
    static major_type get_major_type(__float128 value)                  { (void) value; return ENCODE_TYPE_FLOAT; }
    static major_type get_major_type(_Decimal32 value)                  { (void) value; return ENCODE_TYPE_DECIMAL; }
    static major_type get_major_type(_Decimal64 value)                  { (void) value; return ENCODE_TYPE_DECIMAL; }
    static major_type get_major_type(_Decimal128 value)                 { (void) value; return ENCODE_TYPE_DECIMAL; }
    static major_type get_major_type(const std::string& value)          { (void) value; return ENCODE_TYPE_STRING; }
    static major_type get_major_type(const std::vector<uint8_t>& value) { (void) value; return ENCODE_TYPE_BINARY; }


    namespace adl_helper
    {
        template<class T> std::string as_string(T&& object)
        {
            return std::to_string(std::forward<T>(object));
        }
    }
    template<class T> std::string to_string(T&& object)
    {
        return adl_helper::as_string(std::forward<T>(object));
    }

}

class encoder;

class encoding
{
public:
    friend std::string to_string(encoding const& self)
    {
        return self.as_string();
    } 
     
private:
    const enc::major_type _type;
    const size_t _size;
    const std::string _string_value;
    std::shared_ptr<encoding> _next;
    std::shared_ptr<encoding> _last;

public:
    encoding(const enc::major_type type, const size_t size, const std::string& string_value)
    : _type(type)
    , _size(size)
    , _string_value(string_value)
    {
        std::cout << "create type " << _type << " with value " << _string_value << std::endl;
    }

    std::shared_ptr<encoding> next()
    {
        return _next;
    }

    std::shared_ptr<encoding> set_next(std::shared_ptr<encoding> next)
    {
        _next = next;
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

    std::shared_ptr<encoding> list();
    std::shared_ptr<encoding> map();
    std::shared_ptr<encoding> end();
    std::shared_ptr<encoding> empty();
    std::shared_ptr<encoding> pad(int count);
    std::shared_ptr<encoding> time(smalltime value);
    std::shared_ptr<encoding> str(std::string& value);
    std::shared_ptr<encoding> bin(std::vector<uint8_t>& value);
    std::shared_ptr<encoding> bl(bool value);
    std::shared_ptr<encoding> i8(int8_t value);
    std::shared_ptr<encoding> i16(int16_t value);
    std::shared_ptr<encoding> i32(int32_t value);
    std::shared_ptr<encoding> i64(int64_t value);
    std::shared_ptr<encoding> i128(__int128 value);
    std::shared_ptr<encoding> f32(float value);
    std::shared_ptr<encoding> f64(double value);
    std::shared_ptr<encoding> f128(__float128 value);
    std::shared_ptr<encoding> d32(_Decimal32 value);
    std::shared_ptr<encoding> d64(_Decimal64 value);
    std::shared_ptr<encoding> d128(_Decimal128 value);

    enc::major_type get_type() const {return _type;}

    virtual bool is_equal(const encoding& rhs) const = 0;

    bool is_equal_in_type(const encoding& rhs) const
    {        
        return _type == rhs._type && is_equal(rhs);
    }

    bool is_equal_in_type_and_size(const encoding& rhs) const
    {
        return _size == rhs._size && is_equal_in_type(rhs);
    }

    bool operator ==(const encoding& rhs) const
    {
        return is_equal(rhs);
    }

    const std::string& as_string() const
    {
        return _string_value;
    }

    virtual bool has_value(bool value) const                        { (void) value; return false; }
    virtual bool has_value(int8_t value) const                      { (void) value; return false; }
    virtual bool has_value(int16_t value) const                     { (void) value; return false; }
    virtual bool has_value(int32_t value) const                     { (void) value; return false; }
    virtual bool has_value(int64_t value) const                     { (void) value; return false; }
    virtual bool has_value(__int128 value) const                    { (void) value; return false; }
    virtual bool has_value(float value) const                       { (void) value; return false; }
    virtual bool has_value(double value) const                      { (void) value; return false; }
    virtual bool has_value(__float128 value) const                  { (void) value; return false; }
    virtual bool has_value(_Decimal32 value) const                  { (void) value; return false; }
    virtual bool has_value(_Decimal64 value) const                  { (void) value; return false; }
    virtual bool has_value(_Decimal128 value) const                 { (void) value; return false; }
    virtual bool has_value(const std::string& value) const          { (void) value; return false; }
    virtual bool has_value(const std::vector<uint8_t>& value) const { (void) value; return false; }

    virtual cbe_encode_status encode(encoder& encoder) = 0;
};

inline std::ostream& operator << (std::ostream& os, const encoding& rhs)
{
    os << to_string(rhs);
    return os;
}


class no_value_encoding: public encoding
{
public:
    no_value_encoding(enc::major_type type, const std::string& type_string): encoding(type, 0, type_string) {}
    bool is_equal(const encoding& rhs) const { (void) rhs; return true; }
};

class list_encoding: public no_value_encoding
{
public:
    list_encoding(): no_value_encoding(enc::ENCODE_TYPE_LIST, "List") {}
    cbe_encode_status encode(encoder& encoder);
};

class map_encoding: public no_value_encoding
{
public:
    map_encoding(): no_value_encoding(enc::ENCODE_TYPE_MAP, "Map") {}
    cbe_encode_status encode(encoder& encoder);
};

class end_container_encoding: public no_value_encoding
{
public:
    end_container_encoding(): no_value_encoding(enc::ENCODE_TYPE_CONTAINER_END, "End") {}
    cbe_encode_status encode(encoder& encoder);
};

class empty_encoding: public no_value_encoding
{
public:
    empty_encoding(): no_value_encoding(enc::ENCODE_TYPE_EMPTY, "Empty") {}
    cbe_encode_status encode(encoder& encoder);
};

class padding_encoding: public encoding
{
private:
    const int _byte_count;
public:
    padding_encoding(int byte_count): encoding(enc::ENCODE_TYPE_PADDING, byte_count, "Padding"), _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_byte_count); }
    int byte_count() {return _byte_count;}
    bool has_value(int value) const { return _byte_count == value; }
};

class time_encoding: public encoding
{
private:
    const smalltime _value;
public:
    time_encoding(smalltime value): encoding(enc::ENCODE_TYPE_TIME, sizeof(value), enc::to_id_string_time(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(__int128 value) const { return _value == value; }
    smalltime value() {return _value;}
};

class boolean_encoding: public encoding
{
private:
    const bool _value;
public:
    boolean_encoding(bool value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(bool value) const           { return _value == value; }
    bool value() {return _value;}
};

// FIXME: number_encoding, int128_encoding, and dfp_encoding are split because
// conversions between decimal floating point and 128-bit integers cause the
// compiler to reference __bid_floattisd and __bid_fixddti from libdfp, which
// is only present on PPC and s390 in Ubuntu :/

template<typename T> class number_encoding: public encoding
{
private:
    const T _value;
public:
    number_encoding(T value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    bool has_value(__int128 value) const           { return (__int128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    bool has_value(_Decimal32 value) const         { return (_Decimal32)_value == value; }
    bool has_value(_Decimal64 value) const         { return (_Decimal64)_value == value; }
    bool has_value(_Decimal128 value) const        { return (_Decimal128)_value == value; }
    T value() {return _value;}
};

class int128_encoding: public encoding
{
private:
    const __int128 _value;
public:
    int128_encoding(__int128 value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    bool has_value(__int128 value) const           { return (__int128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    // FIXME: Casting to float instead of decimal to avoid linker error due to
    //        missing libdfp: undefined reference to `__bid_floattisd'
    bool has_value(_Decimal32 value) const         { return (float)_value == value; }
    bool has_value(_Decimal64 value) const         { return (double)_value == value; }
    bool has_value(_Decimal128 value) const        { return (__float128)_value == value; }
    __int128 value() {return _value;}
};

template<typename T> class dfp_encoding: public encoding
{
private:
    const T _value;
public:
    dfp_encoding(T value): encoding(enc::get_major_type(value), sizeof(value), enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const       { return rhs.has_value(_value); }
    bool has_value(int8_t value) const             { return (int8_t)_value == value; }
    bool has_value(int16_t value) const            { return (int16_t)_value == value; }
    bool has_value(int32_t value) const            { return (int32_t)_value == value; }
    bool has_value(int64_t value) const            { return (int64_t)_value == value; }
    // FIXME: Casting to __float128 instead of __int128 to avoid linker error due to
    //        missing libdfp: undefined reference to `__bid_fixddti'
    bool has_value(__int128 value) const           { return (__float128)_value == value; }
    bool has_value(float value) const              { return (float)_value == value; }
    bool has_value(double value) const             { return (double)_value == value; }
    bool has_value(__float128 value) const         { return (__float128)_value == value; }
    bool has_value(_Decimal32 value) const         { return (_Decimal32)_value == value; }
    bool has_value(_Decimal64 value) const         { return (_Decimal64)_value == value; }
    bool has_value(_Decimal128 value) const        { return (_Decimal128)_value == value; }
    T value() {return _value;}
};

class string_encoding: public encoding
{
private:
    const std::string _value;
public:
    string_encoding(const std::string& value): encoding(enc::get_major_type(value), 1, enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(const std::string& value) const       { return _value == value; }
    std::string value() {return _value;}
};

class binary_encoding: public encoding
{
private:
    const std::vector<uint8_t> _value;
public:
    binary_encoding(const std::vector<uint8_t>& value): encoding(enc::get_major_type(value), 1, enc::to_id_string(value)), _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    bool is_equal(const encoding& rhs) const    { return get_type() == rhs.get_type() && rhs.has_value(_value); }
    bool has_value(const std::vector<uint8_t>& value) const { return _value == value; }
    std::vector<uint8_t> value() {return _value;}
};

class encoder
{
public:
    virtual cbe_encode_status encode(list_encoding& encoding) = 0;
    virtual cbe_encode_status encode(map_encoding& encoding) = 0;
    virtual cbe_encode_status encode(end_container_encoding& encoding) = 0;
    virtual cbe_encode_status encode(empty_encoding& encoding) = 0;
    virtual cbe_encode_status encode(padding_encoding& encoding) = 0;
    virtual cbe_encode_status encode(time_encoding& encoding) = 0;
    virtual cbe_encode_status encode(string_encoding& encoding) = 0;
    virtual cbe_encode_status encode(binary_encoding& encoding) = 0;
    virtual cbe_encode_status encode(boolean_encoding& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int8_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int16_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int32_t>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<int64_t>& encoding) = 0;
    virtual cbe_encode_status encode(int128_encoding& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<float>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<double>& encoding) = 0;
    virtual cbe_encode_status encode(number_encoding<__float128>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal32>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal64>& encoding) = 0;
    virtual cbe_encode_status encode(dfp_encoding<_Decimal128>& encoding) = 0;
};

inline cbe_encode_status list_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status map_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status end_container_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status empty_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status padding_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status time_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status string_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status binary_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status boolean_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<int8_t>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<int16_t>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<int32_t>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<int64_t>::encode(encoder& encoder) { return encoder.encode(*this); }
inline cbe_encode_status int128_encoding::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<float>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<double>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status number_encoding<__float128>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status dfp_encoding<_Decimal32>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status dfp_encoding<_Decimal64>::encode(encoder& encoder) { return encoder.encode(*this); }
template <> inline cbe_encode_status dfp_encoding<_Decimal128>::encode(encoder& encoder) { return encoder.encode(*this); }

static inline std::shared_ptr<list_encoding>                 list()                           {return std::make_shared<list_encoding>();}
static inline std::shared_ptr<map_encoding>                  map()                            {return std::make_shared<map_encoding>();}
static inline std::shared_ptr<end_container_encoding>        end()                            {return std::make_shared<end_container_encoding>();}
static inline std::shared_ptr<empty_encoding>                empty()                          {return std::make_shared<empty_encoding>();}
static inline std::shared_ptr<time_encoding>                 time(smalltime value)            {return std::make_shared<time_encoding>(value);}
static inline std::shared_ptr<string_encoding>               str(std::string& value)          {return std::make_shared<string_encoding>(value);}
static inline std::shared_ptr<binary_encoding>               bin(std::vector<uint8_t>& value) {return std::make_shared<binary_encoding>(value);}
static inline std::shared_ptr<boolean_encoding>              bl(bool value)                   {return std::make_shared<boolean_encoding>(value);}
static inline std::shared_ptr<number_encoding<int8_t>>       i8(int8_t value)                 {return std::make_shared<number_encoding<int8_t>>(value);}
static inline std::shared_ptr<number_encoding<int16_t>>      i16(int16_t value)               {return std::make_shared<number_encoding<int16_t>>(value);}
static inline std::shared_ptr<number_encoding<int32_t>>      i32(int32_t value)               {return std::make_shared<number_encoding<int32_t>>(value);}
static inline std::shared_ptr<number_encoding<int64_t>>      i64(int64_t value)               {return std::make_shared<number_encoding<int64_t>>(value);}
static inline std::shared_ptr<int128_encoding>               i128(__int128 value)             {return std::make_shared<int128_encoding>(value);}
static inline std::shared_ptr<number_encoding<float>>        f32(float value)                 {return std::make_shared<number_encoding<float>>(value);}
static inline std::shared_ptr<number_encoding<double>>       f64(float value)                 {return std::make_shared<number_encoding<double>>(value);}
static inline std::shared_ptr<number_encoding<__float128>>   f128(float value)                {return std::make_shared<number_encoding<__float128>>(value);}
static inline std::shared_ptr<dfp_encoding<_Decimal32>>      d32(_Decimal32 value)            {return std::make_shared<dfp_encoding<_Decimal32>>(value);}
static inline std::shared_ptr<dfp_encoding<_Decimal64>>      d64(_Decimal64 value)            {return std::make_shared<dfp_encoding<_Decimal64>>(value);}
static inline std::shared_ptr<dfp_encoding<_Decimal128>>     d128(_Decimal32 value)           {return std::make_shared<dfp_encoding<_Decimal128>>(value);}
static inline std::shared_ptr<padding_encoding>              pad(int byte_count)              {return std::make_shared<padding_encoding>(byte_count);}

inline std::shared_ptr<encoding> encoding::list()                           {return this->set_next(::list());}
inline std::shared_ptr<encoding> encoding::map()                            {return this->set_next(::map());}
inline std::shared_ptr<encoding> encoding::end()                            {return this->set_next(::end());}
inline std::shared_ptr<encoding> encoding::empty()                          {return this->set_next(::empty());}
inline std::shared_ptr<encoding> encoding::pad(int count)                   {return this->set_next(::pad(count));}
inline std::shared_ptr<encoding> encoding::time(smalltime value)            {return this->set_next(::time(value));}
inline std::shared_ptr<encoding> encoding::str(std::string& value)          {return this->set_next(::str(value));}
inline std::shared_ptr<encoding> encoding::bin(std::vector<uint8_t>& value) {return this->set_next(::bin(value));}
inline std::shared_ptr<encoding> encoding::bl(bool value)                   {return this->set_next(::bl(value));}
inline std::shared_ptr<encoding> encoding::i8(int8_t value)                 {return this->set_next(::i8(value));}
inline std::shared_ptr<encoding> encoding::i16(int16_t value)               {return this->set_next(::i16(value));}
inline std::shared_ptr<encoding> encoding::i32(int32_t value)               {return this->set_next(::i32(value));}
inline std::shared_ptr<encoding> encoding::i64(int64_t value)               {return this->set_next(::i64(value));}
inline std::shared_ptr<encoding> encoding::i128(__int128 value)             {return this->set_next(::i128(value));}
inline std::shared_ptr<encoding> encoding::f32(float value)                 {return this->set_next(::f32(value));}
inline std::shared_ptr<encoding> encoding::f64(double value)                {return this->set_next(::f64(value));}
inline std::shared_ptr<encoding> encoding::f128(__float128 value)           {return this->set_next(::f128(value));}
inline std::shared_ptr<encoding> encoding::d32(_Decimal32 value)            {return this->set_next(::d32(value));}
inline std::shared_ptr<encoding> encoding::d64(_Decimal64 value)            {return this->set_next(::d64(value));}
inline std::shared_ptr<encoding> encoding::d128(_Decimal128 value)          {return this->set_next(::d128(value));}
