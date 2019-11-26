#pragma once
#include "BigInt.h"

namespace Field
{

class IntegerMod
{
    static BigInteger::BigInt prime;
    static BigInteger::BigInt nonDeduction;
    IntegerMod() = default;
public:
    static void setIntegerMod(BigInteger::BigInt const&);
    static BigInteger::BigInt const& getIntegerMod();
    static BigInteger::BigInt const& getNonDeduction();
    static bool check(BigInteger::BigInt const &);
};

}