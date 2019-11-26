#include "BigInt.h"
#include "Exeptions.h"
#include "Logger.h"
#include <cstdint>

namespace BigInteger
{
namespace details
{

inline uint16_t convert(char const& a)
{
    if(a >= '0' && a <= '9')
    {
        return a - '0';
    }else if(a >= 'A' && a <= 'F')
    {
        return a - 'A' + 10;
    } else if(a >= 'a' && a <= 'f')
    {
        return a - 'a' + 10;
    } else
    {
        throw error::ExeptionBase<error::ErrorList::InputError>(std::string("Can't convert value: ") + a);
    }
}
enum  BitDepth : size_t
{
    low = 0,
    high = 4
};
template<BitDepth bit>
inline char convert(uint8_t const& a)
{
    uint8_t r = (a >> bit) & 0b1111;
    if(r < 10)
    {
        return '0' + r;
    }else
    {
        return 'A' + (r - 10);
    }
}

}




BigInt::BigInt():number(0), bitSize(0)
{}

void BigInt::setByString(std::string const& str)
{
    number.clear(); negative = false; bitSize = 0;
    bitSize = (str.size() & 1) ? 4 : 0;
    size_t newSize = (str.size() + 1) >> 1;
    number.resize(newSize, 0);
    auto iterator = newSize - 1;
    for(auto i : str)
    {
        try
        {
            number[iterator - (bitSize >> 3)] += ((bitSize % 8 == 0 ? (details::convert(i) << 4) : details::convert(i)));
        }
        catch(error::Exeption& exp)
        {
            if(negative)
            {
                number.clear();
                Logger::print<Log::Level::fatal>(exp.what().c_str());
                throw error::ExeptionBase<error::ErrorList::FatalTrace>("Fail function BigInt::setByString()");
            }else
            {
                if(i == '-')
                    negative = true;
            }
        }
        bitSize += 4;
    }
    calcBitSize();
}

void BigInt::setByArray(const uint8_t array[], size_t size)
{
    number.resize(size);
    for(size_t i = 0; i < size; i++)
    {
        number[i] = array[i];
    }
    calcBitSize();
}

bool operator>(BigInt const& a, BigInt const& b)
{
    if(a == b) return false;
    bool res = false;
    if (a.bitSize > b.bitSize)
    {
        res = true;
    }else if(a.bitSize < b.bitSize)
    {
        res = false;
    }
    else
    {
        for (int i = a.number.size() - 1; i >= 0; i--) {
            if (a.number[i] > b.number[i]) {
                res = true;
                break;
            } else if (a.number[i] < b.number[i]) {
                res = false;
                break;
            }
        }
    }
    if(a.negative && b.negative)
    {
        res = !res;
    }else if(!a.negative && !b.negative)
    {
        
    }
    else if(a.negative)
    {
        res = false;
    }else if(b.negative)
    {
        res = true;
    }
    return res;
}
bool operator<(BigInt const& a, BigInt const& b)
{
    if(a == b) return false;
    return !(a > b);
}
bool operator!=(BigInt const& a, BigInt const& b)
{
    if(a.bitSize != b.bitSize || a.negative != b.negative) return true;
    uint8_t x = 0;
    for(size_t i = 0; i < a.number.size(); i++)
    {
        x = a.number[i] ^ b.number[i];
        if(x == 0)
            continue;
        else
            return true;
    }
    return false;
}
bool operator==(BigInt const& a, BigInt const& b) {
    return !(a != b);
}
bool operator>=(BigInt const& a, BigInt const& b)
{
    return !(a < b);
}
bool operator<=(BigInt const& a, BigInt const& b)
{
    return !(a > b);
}

BigInt BigInt::operator%(BigInt const & p) const
{
    return *this - p * (*this / p);
}

BigInt operator*(BigInt const& _a, BigInt const& b) {
    if (b.bitSize == 0) return BigInt();
    if (b.bitSize == 1 && b.number[0] == 1)
    {
        if(!b.negative)
            return _a;
        else
            return -_a;
    }
    BigInt a = _a;
    a.negative = false;
    for(size_t i = 1; i < b.bitSize; i++)
    {
        a <<= 1;
        uint8_t mask = (1 << ((b.bitSize - 1 - i) & 0b111));
        if((b.number[(b.bitSize - 1 - i) >> 3] & mask) != 0)
        {
            a += _a;
        }
    }
    if(_a.negative ^ b.negative)
        a.negative = true;
    return a;
}
// TODO: refactoring without recursive call operator+()
BigInt operator+(BigInt const& _a, BigInt const& _b)
{
    if(_a.negative != _b.negative)
    {
        if(_a.negative)
        {
            return _b - (-_a);
        }else
        {
            return _a - (-_b);
        }
    }
    auto a = _a; auto b = _b; a.negative = b.negative = false;
    bool p = (a > b);
    BigInt max = p ? a : b;
    BigInt min = p ? b : a;
    for(size_t i = min.number.size(); i < max.number.size(); i++)
        min.number.push_back(0);
    BigInt xorRes, andRes;
    xorRes.number.resize(max.number.size(),0);
    andRes.number.resize(max.number.size(),0);
    for(size_t i = 0; i < max.number.size(); i++)
    {
        xorRes.number[i] = max.number[i] ^ min.number[i];
        andRes.number[i] = max.number[i] & min.number[i];
    }
    BigInt zero;
    zero.number.resize(xorRes.number.size(),0);
    zero.bitSize = andRes.bitSize;
    xorRes.calcBitSize();
    if(andRes == zero)
    {

        return xorRes;
    }
    else
    {
        return xorRes + (andRes << 1);
    }

}
// TODO: refactoring without recursive call operator-()
BigInt operator-(BigInt const& _a, BigInt const& _b)
{
    if(_b > _a)
    {
        return -(_b - _a);
    }else if(_a == _b)
    {
        return BigInt();
    }else if(_b.negative)
    {
        return _a + (-_b);
    }
    BigInt b(_b);
    BigInt a(_a);
    for(size_t i = b.number.size(); i < a.number.size(); i++)
        b.number.push_back(0);
    for(size_t i = 0; i < a.number.size(); i++)
    {
        auto k = a.number[i];
        a.number[i] = b.number[i] ^ k;
        b.number[i] = (~k) & b.number[i];
    }
    a.calcBitSize();
    b.calcBitSize();
    if(b.bitSize != 0)
        return a - (b << 1);
    else
        return a;
}

BigInt operator/(BigInt const& _a, BigInt const& _b)
{
    if(_b.negative)
    {
        return -(_a / -_b);
    }
    if(_a.negative)
    {
        return -(((-_a) - "1"_BigInt) / _b + "1"_BigInt);
    }
    if(_a < _b) {
        return BigInt();
    }
    size_t k = _a.bitSize - _b.bitSize;
    BigInt a = _a;
    BigInt b = _b;
    BigInt res;

    if(_a == _b)
        return "1"_BigInt;
    while(a > b)
    {
        if(a < (b << k))
        {
            k--;
        }else
        {
            a = a - (b << k);
            res += "1"_BigInt << k;
            k = a.bitSize - b.bitSize;
        }
    }
    if(a == b)
            res++;
    return res;
}

BigInt operator^(BigInt const& _a, BigInt const& b)
{
    if(b.negative)
    {
        throw error::ExeptionBase<error::ErrorList::ArithmeticError>("a(" + _a.toString() + ") ^ b(" +  b.toString() + ") can't calculated b < 0");
    }
    if(b.bitSize == 0)
    {
        return "1"_BigInt;
    }
    if(b.bitSize == 1 && b.number[0] == 1) return _a;

    BigInt a = _a;
    a.negative = false;
    for(size_t i = 1; i < b.bitSize; i++)
    {
        a *= a;
        uint8_t mask = (1 << ((b.bitSize - 1 - i) & 0b111));
        if((b.number[(b.bitSize - 1 - i) >> 3] & mask) != 0)
        {
            a *= _a;
        }
    }
    if(_a.negative && (b.number[0] & 0x1))
        a.negative = true;
    return a;
}

BigInt operator<<(BigInt const & a,  uint64_t n)
{
    if (n == 0) return a;
    uint64_t k = 0;
    while( n > 0x7)
    {
        n = n - 8;
        k++;
    }
    BigInt res;
    res.number.resize(k + a.number.size(),0);
    uint8_t t = 0;
    uint8_t const mask = (uint16_t(255) << 8) >> n;
    for(size_t i = 0; i < a.number.size(); i++)
    {
        res.number[i + k] = (a.number[i] << n) | t;
        t = (a.number[i] & mask) >> (8 - n);
    }
    if(t != 0) res.number.push_back(t);
    res.calcBitSize();
    res.negative = a.negative;
    return res;
}


BigInt operator>>(BigInt const & a, uint64_t n)
{
    if (n == 0) return a;
    if(n > a.bitSize)
    {
        BigInt i;
        i.number.resize(0,0);
        i.bitSize = 0;
        return i;
    }
    BigInt res;
    res.bitSize = a.bitSize - n;
    res.number.resize((res.bitSize + 7) >> 3, 0);
    uint8_t left = n & 0x7;
    uint8_t mask = 1;
    uint8_t maskT = (1 << left);
    uint8_t local = 0;
    uint8_t low  = 255 << left;
    uint8_t high = 255 ^ (255 << left);
    for(size_t i = 0; i < res.bitSize; i++)
    {
        auto k = (i + n) >> 3;

        local |= (a.number[k] & maskT);
        mask <<= 1;
        maskT <<= 1;
        if(mask == 0)
        {
            mask = 1;
            res.number[i >> 3] = ((local & low) >> left) | ((local & high) << (8 - left));
            local = 0;
        }
        if(maskT == 0)
            maskT = 1;
    }
    if(local != 0)
    {
        res.number[(res.bitSize - 1) >> 3] = ((local & low) >> left) | ((local & high) << (8 - left));
    }
    res.negative = a.negative;
    return res;
}

void BigInt::calcBitSize()
{
    if(!number.size())
    {
        bitSize = 0;
        return;
    }
    bitSize = number.size() * 8;
    if(number[number.size() - 1] == 0)
    {
        number.pop_back();
        calcBitSize();
        return;
    }
    uint8_t mask = 128;
    while(mask != 0)
    {
        if(mask & number[number.size() - 1]) return;
        mask >>= 1;
        bitSize--;
    }
}

std::string BigInt::toString() const
{
    if(number.size() == 0)
    {
        return "0";
    }
    std::string str;
    if((number[number.size() - 1 ] > 0b1111))
        str.resize(number.size() * 2);
    else
        str.resize(number.size() * 2 - 1);
    bool first = true;
    int add = 0;
    for(size_t i = 0; i < number.size(); i++)
    {
        if(!first || (number[number.size() - 1 - i] > 0b1111))
        {
            str[2 * i + add] = details::convert<details::BitDepth::high>(number[number.size() - 1 - i]);
            str[2 * i + 1 + add] = details::convert<details::BitDepth::low>(number[number.size() - 1 - i]);
        }else
        {
            str[0] = details::convert<details::BitDepth::low>(number[number.size() - 1 - i]);
            add = -1;
        }
        first = false;
    }
    if(negative)
        str = "-" + str;
    return str;
}

BigInt& BigInt::operator*=(BigInt const & a) {
    *this = *this * a;
    return *this;
}

BigInt& BigInt::operator+=(BigInt const & a) {
    *this = *this + a;
    return *this;
}

BigInt& BigInt::operator-=(BigInt const & a) {
    *this = *this - a;
    return *this;
}

BigInt& BigInt::operator/=(BigInt const & a) {
    *this = *this / a;
    return *this;
}

BigInt& BigInt::operator^=(BigInt const & a) {
    *this = *this ^ a;
    return *this;
}

BigInt& operator<<=(BigInt& a, uint64_t n) {
    a = a << n;
    return a;
}

BigInt& operator>>=(BigInt& a, uint64_t n){
    a = a >> n;
    return a;
}

BigInt& operator++(BigInt& a)
{
    return a += "1"_BigInt;
}

BigInt& operator--(BigInt& a)
{
    return a -= "1"_BigInt;
}

BigInt operator++(BigInt& a, int)
{
    auto b(a);
    a += "1"_BigInt;
    return b;
}

BigInt operator--(BigInt &a, int)
{
    auto b(a);
    a -= "1"_BigInt;
    return b;
}

BigInt operator-(BigInt const & a) {
    BigInt _a = a;
    _a.negative = !_a.negative;
    return _a;
}



BigInt operator""_BigInt(const char* str, size_t)
{
    BigInt i; i.setByString(str);
    return i;
}

std::tuple<BigInt, BigInt, BigInt> gcb(BigInt const& a, BigInt const& b)
{
    if(a > b)
        return gcb(b, a);
    BigInt x, y;
    if(a == "0"_BigInt)
    {
        x = "0"_BigInt;
        y = "1"_BigInt;
        return {b, x, y};
    }
    auto [d1, x1, y1] = gcb(b % a, a);
    auto o = (b / a) * x1;
    x = y1 - o;
    y = x1;
    return {d1, x, y};
}

}
