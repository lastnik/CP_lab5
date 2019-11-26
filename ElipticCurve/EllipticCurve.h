#pragma once
#include "Field.h"
#include <tuple>

class BigInt;

using Point = std::tuple<Field::BigInt, Field::BigInt>;

class EllipticCurve
{
public:
    EllipticCurve() = default;
    EllipticCurve(EllipticCurve const&) = default;
    void set(Field::BigInt const& A, Field::BigInt const& B,  BigInteger::BigInt const& n);
    Field::BigInt getY(Field::BigInt x);
    Point add(Point const&, Point const&) const;
    Point mul(Point const&, BigInteger::BigInt const&) const;
private:
    Field::BigInt A, B;
    BigInteger::BigInt N;
};
