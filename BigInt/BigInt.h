#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <tuple>
namespace  BigInteger
{
class BigInt
{

public:
    explicit BigInt();

    BigInt(BigInt const &) = default;

    BigInt& operator=(BigInt const &) = default;

    BigInt operator%(BigInt const &) const;

    void setByString(std::string const &); //values should be in hex format

    void setByArray(const uint8_t[], size_t);

    std::string toString() const; //values should be in hex format

    friend BigInt operator*(BigInt const &, BigInt const &);

    friend BigInt operator+(BigInt const &, BigInt const &);

    friend BigInt operator-(BigInt const &, BigInt const &);

    friend BigInt operator/(BigInt const &, BigInt const &);

    friend BigInt operator^(BigInt const &, BigInt const &);

    friend BigInt operator-(BigInt const &);

    BigInt& operator*=(BigInt const &);

    BigInt& operator+=(BigInt const &);

    BigInt& operator-=(BigInt const &);

    BigInt& operator/=(BigInt const &);

    BigInt& operator^=(BigInt const &);
    //prefix
    friend BigInt& operator++(BigInt&);

    friend BigInt& operator--(BigInt&);
    //postfix

    friend BigInt operator++(BigInt&, int);

    friend BigInt operator--(BigInt&, int);


    friend BigInt& operator<<=(BigInt&, uint64_t);

    friend BigInt& operator>>=(BigInt&, uint64_t);

    friend BigInt operator<<(BigInt const &, uint64_t);

    friend BigInt operator>>(BigInt const &, uint64_t);

    friend bool operator>(BigInt const &, BigInt const &);

    friend bool operator<(BigInt const &, BigInt const &);

    friend bool operator!=(BigInt const &, BigInt const &);

    friend bool operator==(BigInt const &, BigInt const &);

    friend bool operator>=(BigInt const &, BigInt const &);

    friend bool operator<=(BigInt const &, BigInt const &);

    size_t getBitSize() const { return bitSize;}
    std::vector<uint8_t> getVector() const { return number;}
protected:
    void calcBitSize();
    std::vector<uint8_t> number;
    bool negative = false;
    size_t bitSize;
};
          // d       x       y
std::tuple<BigInt, BigInt, BigInt> gcb(BigInt const &, BigInt const &);

BigInt operator""_BigInt(const char* str, size_t);

}