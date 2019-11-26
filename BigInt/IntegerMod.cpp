#include "IntegerMod.h"
#include "Field.h"
namespace Field
{
BigInteger::BigInt IntegerMod::nonDeduction;
BigInteger::BigInt IntegerMod::prime;

BigInteger::BigInt const& IntegerMod::getIntegerMod()
{
    return prime;
}
void IntegerMod::setIntegerMod(BigInteger::BigInt const& _prime)
{
    prime = _prime;
    using namespace BigInteger;
    nonDeduction = "3"_BigInt;
    while(check(nonDeduction))
    {
        nonDeduction++;
        if(nonDeduction > prime) nonDeduction = "1"_BigInt;
    }
}

bool IntegerMod::check(BigInteger::BigInt const & x) {
    using namespace BigInteger;
    Field::BigInt pow((Field::IntegerMod::getIntegerMod() - "1"_BigInt) / "2"_BigInt);
    return ((Field::BigInt(x) ^ pow) == "1"_BigIntMod);
}

BigInteger::BigInt const &IntegerMod::getNonDeduction() {
    return nonDeduction;
}

}