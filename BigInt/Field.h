#pragma  once
#include "IntegerMod.h"
#include "BigInt.h"

namespace Field
{

class BigInt : public BigInteger::BigInt
{
public:
    BigInt() = default;

    BigInt(BigInt const &) = default;
    // TODO : think about pow see small Fermat theorem
    explicit BigInt(BigInteger::BigInt const & a) : BigInteger::BigInt(BigInteger::BigInt(a) % IntegerMod::getIntegerMod()) {};

    friend BigInt operator*(BigInt const &, BigInt const &);

    friend BigInt operator+(BigInt const &, BigInt const &);

    friend BigInt operator-(BigInt const &, BigInt const &);

    friend BigInt operator/(BigInt const &, BigInt const &);

    friend BigInt operator^(BigInt const &, BigInt const &);

    friend bool operator>(BigInt const &, BigInt const &);

    friend bool operator<(BigInt const &, BigInt const &);

    friend bool operator!=(BigInt const &, BigInt const &);

    friend bool operator==(BigInt const &, BigInt const &);

    friend bool operator>=(BigInt const &, BigInt const &);

    friend bool operator<=(BigInt const &, BigInt const &);

    BigInt operator*=(BigInt const &);

    BigInt operator+=(BigInt const &);

    BigInt operator-=(BigInt const &);

    BigInt operator/=(BigInt const &);

    BigInt operator^=(BigInt const &);

    BigInt operator<<=(uint64_t);

    BigInt operator>>=(uint64_t);

    friend BigInt operator<<(BigInt const &, uint64_t);

    friend BigInt operator>>(BigInt const &, uint64_t);

    BigInt sqrt() const;
    friend BigInt sqrt(BigInt const &);
};

BigInt sqrt(BigInt const &);

BigInt operator""_BigIntMod(const char* str, size_t s);
}