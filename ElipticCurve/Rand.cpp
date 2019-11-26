#include "Rand.h"
#include <cstdlib>
#include <ctime>
namespace Rand
{
BigInteger::BigInt rand(uint64_t bitSize)
{
    using namespace BigInteger;
    BigInteger::BigInt res;
    std::srand(unsigned(std::time(0)));
    if(bitSize == 0)
    {
        return  BigInteger::BigInt();
    }else
    {
        for(size_t i = 0; i < bitSize; i++)
        {
            res <<= 1;
            res += (std::rand() % 2) ? "1"_BigInt : "0"_BigInt;
        }
    }
    return res;
}
};