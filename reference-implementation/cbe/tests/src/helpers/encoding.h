#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <quadmath.h> // Need -lquadmath at the end of the line
#include <string.h>

#include <smalltime/smalltime.h>
#include <smalltime/nanotime.h>
#include <cbe/cbe.h>

namespace enc
{

std::string to_string(const int128_ct value);
std::string to_string(uint128_ct value);
std::string to_string(const float128_ct value);
std::string to_string(const std::vector<uint8_t>& value);
std::string to_string(const std::string& value);
std::string to_string(const smalltime value);
std::string to_string(const nanotime value);
std::string to_string(const dec128_ct value);

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

class encoder;

class encoding
{
private:
    std::shared_ptr<encoding> _next;
    std::shared_ptr<encoding> _last;

public:
    encoding() {}
    virtual ~encoding() {}

    std::shared_ptr<encoding> next() const {return _next;}
    std::shared_ptr<encoding> set_next(std::shared_ptr<encoding> next)
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
        return std::shared_ptr<encoding>(this, [](encoding*){});
    }

    std::shared_ptr<encoding> list();
    std::shared_ptr<encoding> map();
    std::shared_ptr<encoding> end();
    std::shared_ptr<encoding> nil();
    std::shared_ptr<encoding> pad(int count);
    std::shared_ptr<encoding> time(smalltime value);
    std::shared_ptr<encoding> time(nanotime value);
    std::shared_ptr<encoding> str(const std::string& value);
    std::shared_ptr<encoding> bin(const std::vector<uint8_t>& value);
    std::shared_ptr<encoding> uri(const std::string& value);
    std::shared_ptr<encoding> cmt(const std::string& value);
    std::shared_ptr<encoding> strh(int64_t length);
    std::shared_ptr<encoding> binh(int64_t length);
    std::shared_ptr<encoding> urih(int64_t length);
    std::shared_ptr<encoding> cmth(int64_t length);
    std::shared_ptr<encoding> data(const std::vector<uint8_t>& value);
    std::shared_ptr<encoding> bl(bool value);
    std::shared_ptr<encoding> si(int128_ct value);
    std::shared_ptr<encoding> si(int64_t high, uint64_t low);
    std::shared_ptr<encoding> ui(uint128_ct value);
    std::shared_ptr<encoding> ui(uint64_t high, uint64_t low);
    std::shared_ptr<encoding> flt(float128_ct value);
    std::shared_ptr<encoding> dec(dec128_ct value);

    bool operator ==(const encoding& rhs) const;

    virtual const std::string as_string() const = 0;
    friend std::string to_string(encoding const& self) {return self.as_string();}

    virtual cbe_encode_status encode(encoder& encoder) = 0;
};


class list_encoding: public encoding
{
public:
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return "list()";}
};

class map_encoding: public encoding
{
public:
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return "map()";}
};

class container_end_encoding: public encoding
{
public:
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return "end()";}
};

class nil_encoding: public encoding
{
public:
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return "nil()";}
};

class padding_encoding: public encoding
{
private:
    const int _byte_count;
public:
    padding_encoding(int byte_count): _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("pad(") + to_string(_byte_count) + std::string(")");}
    int byte_count() const {return _byte_count;}
};

class time_encoding: public encoding
{
private:
    const smalltime _smalltime_value;
    const nanotime _nanotime_value;
public:
    time_encoding(smalltime value): _smalltime_value(value), _nanotime_value(0) {}
    time_encoding(nanotime value): _smalltime_value(to_smalltime(value)), _nanotime_value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const
    {
        if(is_smalltime(_nanotime_value))
        {
            return std::string("time(") + to_string(_smalltime_value) + std::string(")");
        }
        else
        {
            return std::string("time(") + to_string(_nanotime_value) + std::string(")");
        }
    }
    bool is_smalltime() const {return is_smalltime(_nanotime_value);}
    smalltime smalltime_value() const {return _smalltime_value;}
    nanotime nanotime_value() const {return _nanotime_value;}

    static bool is_smalltime(nanotime value)
    {
        return (value % 1000) == 0;
    }

    static smalltime to_smalltime(nanotime value)
    {
        if(is_smalltime(value))
        {
            return (smalltime)(value >> 10);
        }
        return (smalltime)0;
    }

};

class boolean_encoding: public encoding
{
private:
    const bool _value;
public:
    boolean_encoding(bool value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("bl(") + to_string(_value) + std::string(")");}
    bool value() const {return _value;}
};

class int_encoding: public encoding
{
private:
    const int128_ct _value;
public:
    int_encoding(int128_ct value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("si(") + to_string(_value) + std::string(")");}
    int128_ct value() const {return _value;}
};

class uint_encoding: public encoding
{
private:
    const uint128_ct _value;
public:
    uint_encoding(uint128_ct value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("ui(") + to_string(_value) + std::string(")");}
    uint128_ct value() const {return _value;}
};

class float_encoding: public encoding
{
private:
    const float128_ct _value;
public:
    float_encoding(float128_ct value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("flt(") + to_string(_value) + std::string(")");}
    float128_ct value() const {return _value;}
};

class decimal_encoding: public encoding
{
private:
    const dec128_ct _value;
public:
    decimal_encoding(dec128_ct value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("dec(") + to_string(_value) + std::string(")");}
    dec128_ct value() const {return _value;}
};

class string_encoding: public encoding
{
private:
    const std::string _value;
public:
    string_encoding(const std::string& value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("str(") + to_string(_value) + std::string(")");}
    const std::string& value() const {return _value;}
};

class bytes_encoding: public encoding
{
private:
    const std::vector<uint8_t> _value;
public:
    bytes_encoding(const std::vector<uint8_t>& value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("bin(") + to_string(_value) + std::string(")");}
    const std::vector<uint8_t> value() const {return _value;}
};

class uri_encoding: public encoding
{
private:
    const std::string _value;
public:
    uri_encoding(const std::string& value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("uri(") + to_string(_value) + std::string(")");}
    const std::string value() const {return _value;}
};

class comment_encoding: public encoding
{
private:
    const std::string _value;
public:
    comment_encoding(const std::string& value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("com(") + to_string(_value) + std::string(")");}
    const std::string value() const {return _value;}
};

class string_header_encoding: public encoding
{
private:
    const int64_t _byte_count;
public:
    string_header_encoding(int64_t byte_count): _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("strh(") + to_string(_byte_count) + std::string(")");}
    int64_t byte_count() const {return _byte_count;}
};

class bytes_header_encoding: public encoding
{
private:
    const int64_t _byte_count;
public:
    bytes_header_encoding(int64_t byte_count): _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("binh(") + to_string(_byte_count) + std::string(")");}
    int64_t byte_count() const {return _byte_count;}
};

class uri_header_encoding: public encoding
{
private:
    const int64_t _byte_count;
public:
    uri_header_encoding(int64_t byte_count): _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("urih(") + to_string(_byte_count) + std::string(")");}
    int64_t byte_count() const {return _byte_count;}
};

class comment_header_encoding: public encoding
{
private:
    const int64_t _byte_count;
public:
    comment_header_encoding(int64_t byte_count): _byte_count(byte_count) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("comh(") + to_string(_byte_count) + std::string(")");}
    int64_t byte_count() const {return _byte_count;}
};

class data_encoding: public encoding
{
private:
    const std::vector<uint8_t> _value;
public:
    data_encoding(const std::vector<uint8_t>& value): _value(value) {}
    cbe_encode_status encode(encoder& encoder);
    const std::string as_string() const {return std::string("data(") + to_string(_value) + std::string(")");}
    std::vector<uint8_t> value() const {return _value;}
};


class encoder
{
public:
    virtual ~encoder() {}
    virtual cbe_encode_status encode(const list_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const map_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const container_end_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const nil_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const padding_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const time_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const string_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const bytes_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const uri_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const comment_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const string_header_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const bytes_header_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const uri_header_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const comment_header_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const data_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const boolean_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const int_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const uint_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const float_encoding& encoding) = 0;
    virtual cbe_encode_status encode(const decimal_encoding& encoding) = 0;
};


static inline std::shared_ptr<list_encoding>           list()                          {return std::make_shared<list_encoding>();}
static inline std::shared_ptr<map_encoding>            map()                           {return std::make_shared<map_encoding>();}
static inline std::shared_ptr<container_end_encoding>  end()                           {return std::make_shared<container_end_encoding>();}
static inline std::shared_ptr<nil_encoding>            nil()                           {return std::make_shared<nil_encoding>();}
static inline std::shared_ptr<time_encoding>           time(smalltime value)           {return std::make_shared<time_encoding>(value);}
static inline std::shared_ptr<time_encoding>           time(nanotime value)            {return std::make_shared<time_encoding>(value);}
static inline std::shared_ptr<time_encoding>           stime(int year, int month, int day, int hour, int minute, int second, int usec) {return time(smalltime_new(year, month, day, hour, minute, second, usec));}
static inline std::shared_ptr<time_encoding>           ntime(int year, int month, int day, int hour, int minute, int second, int nsec) {return time(nanotime_new(year, month, day, hour, minute, second, nsec));}
static inline std::shared_ptr<string_encoding>         str(const std::string& value)   {return std::make_shared<string_encoding>(value);}
static inline std::shared_ptr<bytes_encoding>          bin(const std::vector<uint8_t>& value) {return std::make_shared<bytes_encoding>(value);}
static inline std::shared_ptr<uri_encoding>            uri(const std::string& value)   {return std::make_shared<uri_encoding>(value);}
static inline std::shared_ptr<comment_encoding>        cmt(const std::string& value)   {return std::make_shared<comment_encoding>(value);}
static inline std::shared_ptr<string_header_encoding>  strh(int64_t byte_count)        {return std::make_shared<string_header_encoding>(byte_count);}
static inline std::shared_ptr<bytes_header_encoding>   binh(int64_t byte_count)        {return std::make_shared<bytes_header_encoding>(byte_count);}
static inline std::shared_ptr<uri_header_encoding>     urih(int64_t byte_count)        {return std::make_shared<uri_header_encoding>(byte_count);}
static inline std::shared_ptr<comment_header_encoding> cmth(int64_t byte_count)        {return std::make_shared<comment_header_encoding>(byte_count);}
static inline std::shared_ptr<data_encoding>           data(const std::vector<uint8_t>& value) {return std::make_shared<data_encoding>(value);}
static inline std::shared_ptr<boolean_encoding>        bl(bool value)                  {return std::make_shared<boolean_encoding>(value);}
static inline std::shared_ptr<int_encoding>            si(int128_ct value)             {return std::make_shared<int_encoding>(value);}
static inline std::shared_ptr<int_encoding>            si(int64_t high, uint64_t low)  {return si((((int128_ct)high) << 64) + low);}
static inline std::shared_ptr<uint_encoding>           ui(uint128_ct value)            {return std::make_shared<uint_encoding>(value);}
static inline std::shared_ptr<uint_encoding>           ui(uint64_t high, uint64_t low) {return ui((((uint128_ct)high) << 64) + low);}
static inline std::shared_ptr<float_encoding>          flt(float128_ct value)          {return std::make_shared<float_encoding>(value);}
static inline std::shared_ptr<decimal_encoding>        dec(dec128_ct value)            {return std::make_shared<decimal_encoding>(value);}
static inline std::shared_ptr<padding_encoding>        pad(int byte_count)             {return std::make_shared<padding_encoding>(byte_count);}

} // namespace enc

static inline std::ostream& operator << (std::ostream& os, const enc::encoding& rhs)
{
    os << to_string(rhs);
    return os;
}
