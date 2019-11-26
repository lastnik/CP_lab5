#include "Field.h"
#include <sstream>

namespace Field
{

bool operator>(BigInt const& a, BigInt const& b)
{
    auto A = BigInteger::BigInt(a) % IntegerMod::getIntegerMod();
    auto B = BigInteger::BigInt(b) % IntegerMod::getIntegerMod();
    return A > B;
}
bool operator<(BigInt const& a, BigInt const& b)
{
    if(a == b) return false;
    return !(a > b);
}
bool operator!=(BigInt const& a, BigInt const& b)
{
    auto A = BigInteger::BigInt(a) % IntegerMod::getIntegerMod();
    auto B = BigInteger::BigInt(b) % IntegerMod::getIntegerMod();
    return A != B;
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

BigInt operator*(BigInt const& a, BigInt const& b)
{
    auto p = IntegerMod::getIntegerMod();
    BigInt _a = BigInt(BigInteger::BigInt(a) % p);
    BigInt _b = BigInt(BigInteger::BigInt(b) % p);

    if(_b == "0"_BigIntMod) return "0"_BigIntMod;
    if(_b == "1"_BigIntMod) return BigInt(_a);

    BigInt A(_a), B(_b);

    for(size_t i = 1; i < B.bitSize; i++)
    {
        A <<= 1;
        uint8_t mask = (1 << ((B.bitSize - 1 - i) & 0b111));
        if((b.number[(B.bitSize - 1 - i) >> 3] & mask) != 0)
        {
            A += _a;
        }
    }
    return A;

}

BigInt operator+(BigInt const& a, BigInt const& b)
{
    auto p = IntegerMod::getIntegerMod();
    auto _a = BigInteger::BigInt(a) % p;
    auto _b = BigInteger::BigInt(b) % p;
    return BigInt((_a + _b) % p);
}

BigInt operator-(BigInt const& a, BigInt const& b)
{
    auto p = IntegerMod::getIntegerMod();
    auto _a = BigInteger::BigInt(a) % p;
    auto _b = BigInteger::BigInt(b) % p;
    if(_a < _b) _a += p;
    return BigInt((_a - _b) % p);
}
// TODO operator/() in FIELD.CPP
BigInt operator/(BigInt const& a, BigInt const& b)
{
    auto p = IntegerMod::getIntegerMod();
    auto _a = BigInteger::BigInt(a) % p;
    auto _b = BigInteger::BigInt(b) % p;
    BigInt P(p);
    return BigInt(_a) * (BigInt(_b) ^ (P - "2"_BigIntMod));
}

BigInt operator^(BigInt const& _a, BigInt const& b)
{
    if(b.bitSize == 0)
    {
        return "1"_BigIntMod;
    }
    if(b.bitSize == 1 && b.number[0] == 1) return _a;

    BigInt a = _a;
    for(size_t i = 1; i < b.bitSize; i++)
    {
        a *= a;
        uint8_t mask = (1 << ((b.bitSize - 1 - i) & 0b111));
        if((b.number[(b.bitSize - 1 - i) >> 3] & mask) != 0)
        {
            a *= _a;
        }
    }
    return a;
}

BigInt BigInt::operator*=(BigInt const & a) {
    *this = *this * a;
    return *this;
}

BigInt BigInt::operator+=(BigInt const & a) {
    *this = *this + a;
    return *this;
}

BigInt BigInt::operator-=(BigInt const & a) {
    *this = *this - a;
    return *this;
}

BigInt BigInt::operator/=(BigInt const & a) {
    *this = *this / a;
    return *this;
}

BigInt BigInt::operator^=(BigInt const & a) {
    *this = *this ^ a;
    return *this;
}

BigInt BigInt::operator<<=(uint64_t n) {
    *this = *this << n;
    return *this;
}

BigInt BigInt::operator>>=(uint64_t n) {
    *this = *this >> n;
    return *this;
}

BigInt operator<<(BigInt const & a, uint64_t n)
{
    auto p = IntegerMod::getIntegerMod();
    BigInteger::BigInt _a = a;
    auto A = ((_a % p) << n) % p;
    return BigInt(A);
}

BigInt operator>>(BigInt const & a, uint64_t n)
{
    auto p = IntegerMod::getIntegerMod();
    BigInteger::BigInt _a = a;
    auto A = ((_a % p) << n) % p;
    return BigInt(A);
}

BigInt BigInt::sqrt() const
{
    using namespace BigInteger;
    if(Field::IntegerMod::getIntegerMod() % "4"_BigInt == "3"_BigInt)
    {
        return *this ^ BigInt((Field::IntegerMod::getIntegerMod() + "1"_BigInt) / "4"_BigInt);
    }else
    {
        BigInt pow = "0"_BigIntMod;
        uint64_t alpha = 0;
        BigInteger::BigInt s = Field::IntegerMod::getIntegerMod() - "1"_BigInt;
        while((s % "2"_BigInt) == "0"_BigInt)
        {
            alpha++;
            pow++;
            s /= "2"_BigInt;
        }
        BigInt b = BigInt(Field::IntegerMod::getNonDeduction()) ^ BigInt(s);
        BigInt r = *this ^ BigInt((s + "1"_BigInt) / "2"_BigInt);
        BigInt _a = ("1"_BigIntMod / *this);
        BigInt r2_a = r * r * _a;
        BigInteger::BigInt J = "0"_BigIntMod;
        BigInteger::BigInt powB = "1"_BigIntMod;
        for(size_t i = 0; i <= alpha - 2; i++)
        {
            std::ostringstream  stream;
            stream << std::hex << i;
            BigInteger::BigInt local; local.setByString(stream.str());
            local = ("2"_BigInt ^ (pow - local - "2"_BigInt)) % (IntegerMod::getIntegerMod() - "1"_BigInt);
            if(i == 0)
            {
                if((r2_a ^ BigInt(local)) == ("0"_BigIntMod - "1"_BigIntMod))
                    J += powB;
            }else
            {
                if((((BigInt(b) ^ "2"_BigIntMod) * r2_a) ^ BigInt(local)) == ("0"_BigIntMod - "1"_BigIntMod))
                    J += powB;
            }
            powB *= "2"_BigInt;
            if(J > IntegerMod::getIntegerMod() - "1"_BigInt)
                J = J % (IntegerMod::getIntegerMod() - "1"_BigInt);
        }
        return (BigInt(b) ^ BigInt(J)) * r;
    }
}

BigInt sqrt(BigInt const & a) {
    return a.sqrt();
}

BigInt operator""_BigIntMod(const char* str, size_t s)
{
    BigInt i; i.setByString(str);
    return i;
}

}
