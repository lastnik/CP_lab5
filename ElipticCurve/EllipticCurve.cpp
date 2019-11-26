#include "Exeptions.h"
#include "EllipticCurve.h"
#include "Logger.h"
#include "IntegerMod.h"
#include "Field.h"
void EllipticCurve::set(Field::BigInt const& a, Field::BigInt const& b, BigInteger::BigInt const& n)
{
    using namespace Field;
    A = a; B = b; N = n;
    auto l = "1B"_BigIntMod * (b ^ "2"_BigIntMod); auto r = "4"_BigIntMod * (a ^ "3"_BigIntMod);
    if((r + l) != "0"_BigIntMod)
    {
        Logger::print<Log::Level::info>("Create elliptic curve with A = %s and B = %s" , A.toString().c_str(), B.toString().c_str());
    }else
    {
        A = "0"_BigIntMod; B = "0"_BigIntMod; N = "0"_BigIntMod;
        throw error::ExeptionBase<error::ErrorList::InputError>("Can't create elliptic curve with A = " + A.toString() + " and B = " + B.toString());
    }
}

Field::BigInt EllipticCurve::getY(Field::BigInt x)
{
    using namespace Field;
    Field::BigInt Y = (x ^ "3"_BigIntMod) + A * x + B;
    Logger::print<Log::Level::info>("X = %s",x.toString().c_str());
    Logger::print<Log::Level::info>("Y^2 = %s",Y.toString().c_str());

    if(Field::IntegerMod::check(Y))
    {
        return sqrt(Y);
    }else
    {
        throw error::ExeptionBase<error::ErrorList::ArithmeticError>("X = " + x.toString() + " and Y = " + Y.toString() + " is quadratic non-deduction in Field by prime = " + Field::IntegerMod::getIntegerMod().toString());
    }
}

Point EllipticCurve::add(Point const & a, Point const & b) const
{
    const auto& [xA, yA] = a; const auto& [xB, yB] = b;
    using namespace Field;
    if( xA == "0"_BigIntMod && yA == "0"_BigIntMod)
        return b;
    if( xB == "0"_BigIntMod && yB == "0"_BigIntMod)
        return a;
    if( xA == xB && yA == (IntegerMod::getIntegerMod() - yB))
        return {"0"_BigIntMod , "0"_BigIntMod};
    Field::BigInt m;
    if( xA == xB && yA == yB)
    {
        m = "3"_BigIntMod * ((xA ^ "2"_BigIntMod) + A) * (("2"_BigIntMod * yA) ^ Field::BigInt("0"_BigIntMod - "2"_BigIntMod));
    }else
    {
        m = (yA - yB) * ((xA - xB) ^ Field::BigInt("0"_BigIntMod - "2"_BigIntMod));
    }
    auto x = (m ^ "2"_BigIntMod) - xA - xB;
    auto y = yA + m * (x - xA);
    return {x, "0"_BigIntMod - y};
}

Point EllipticCurve::mul(Point const& p, BigInteger::BigInt const& a) const
{
    using namespace Field;
    using namespace BigInteger;
    auto it = (a % N);
    Point res = p;
    auto vecIT = it.getVector();
    for(size_t index = 1; index < it.getBitSize(); index++)
    {
        //   Logger::print<Log::Level::debug>(("ResX: " + std::get<0>(res).toString()).c_str());
        //   Logger::print<Log::Level::debug>(("ResY: " + std::get<1>(res).toString()).c_str());
        res = add(res, res);
        uint8_t mask = (1 << ((it.getBitSize() - 1 - index) & 0b111));
        if(vecIT[(it.getBitSize() - 1 - index) >> 3] & mask)
        {
            res = add(res, p);
        }
    }
    return res;
}


