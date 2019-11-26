#include <iostream>
#include <cstdint>
#include "Logger.h"
#include "BigInt.h"
#include <chrono>
#include <thread>
#include "Field.h"
#include "IntegerMod.h"
#include "EllipticCurve.h"
#include "Rand.h"
#include <openssl/sha.h>
#include <tuple>
constexpr char const* file = "config.json";
struct config
{
std::string prime;
std::string A;
std::string B;
std::string X;
std::string Y;
std::string n;
std::string h;
std::string file;
std::string mode;
std::string LogLevel;
};
std::string find(std::string const& str, std::string const& key)
{
    auto k = str.find(key);
    if(k == std::string::npos)
    {
        throw error::ExeptionBase<error::ErrorList::InputError>("Lose mandatory field " + key);
    }
    k = str.find(':', k + 1);
    if(k == std::string::npos)
    {
        throw error::ExeptionBase<error::ErrorList::InputError>("Lose mandatory value of field " + key);
    }
    auto start = str.find("\"",k + 1);
    auto end   = str.find( "\"", start + 1);
    if(end == str.size())
        end  = str.find( "\n", start + 1);

    return str.substr(start + 1, end - start - 1);
};
config parse(std::string const& str)
{
    config cfg;
    cfg.prime = find(str,"prime");
    cfg.A = find(str,"A");
    cfg.B = find(str,"B");
    cfg.X = find(str,"X");
    cfg.Y = find(str,"Y");
    cfg.n = find(str,"n");
    cfg.h = find(str,"h");
    cfg.file = find(str,"file");
    cfg.mode = find(str,"mode");
    cfg.LogLevel = find(str,"LogLevel");
    return cfg;
}
struct configEcdsa
{
std::string R;
std::string S;
std::string QX;
std::string QY;
};
configEcdsa parseEcdsa(std::string const& str)
{
    configEcdsa cfg;
    cfg.R = find(str,"R");
    cfg.S = find(str,"S");
    cfg.QX = find(str,"QX");
    cfg.QY = find(str,"QY");
    return cfg;
}

int main() {
    BigInteger::BigInt a, b, prime, n;
    Field::BigInt A, B;
    Field::BigInt X, Y, N;
    Logger::setLevel(Log::Level::debug);
    bool crash = false;

    try {
        Logger::start();
    }
    catch (error::Exeption &exp) {
        std::cout << exp.what() << std::endl;
        return -1;
    }
    /*
    A.setByString("0");
    B.setByString("7");
    prime.setByString("25");
    n.setByString("27");
    X.setByString("3");
    Y.setByString("10");
    BigInteger::BigInt lop;
    lop.setByString("26");
    Field::IntegerMod::setIntegerMod(prime);
    EllipticCurve curve;
    curve.set(A,B,n);
    auto [RX, RY] = curve.mul({X,Y}, lop);
    Logger::print<Log::Level::fatal>(("Rx: " + RX.toString()).c_str());
    Logger::print<Log::Level::fatal>(("Ry: " + RY.toString()).c_str());
    */

    std::fstream in(file, std::ios_base::in);

    if (!in.is_open()) {
        Logger::print<Log::Level::fatal>((std::string("Can't open file: ") + file).c_str());
        crash = true;
    }

    std::stringstream buf;
    buf << in.rdbuf();
    std::string json = buf.str();
    EllipticCurve curve;
    std::string fileHash, mode;
    in.close();
    try {
        auto cfg = parse(json);
        Logger::setLevel(cfg.LogLevel);
        a.setByString(cfg.A);
        b.setByString(cfg.B);
        n.setByString(cfg.n);
        prime.setByString(cfg.prime);
        Field::IntegerMod::setIntegerMod(prime);
        A = Field::BigInt(a);
        B = Field::BigInt(b);
        curve.set(A, B, n);
        X.setByString(cfg.X);
        Y.setByString(cfg.Y);
        fileHash = cfg.file;
        mode = cfg.mode;
        /*
        if (Y != curve.getY(X)) {
            Logger::print<Log::Level::info>("(%s,%s) - point of curve Y=%s", X.toString().c_str(),
                                            curve.getY(X).toString().c_str(), Y.toString().c_str());
            crash = true;
        }
        */

    }
    catch (error::Exeption &exp) {
        Logger::print<Log::Level::fatal>(exp.what().c_str());
        std::cout << exp.what() << std::endl;
        crash = true;
    }


    std::fstream inHash(fileHash, std::ios_base::in);
    if (!inHash.is_open()) {
        Logger::print<Log::Level::fatal>((std::string("Can't open file: ") + fileHash).c_str());
        crash = true;
    }
    std::stringstream bufHash;
    bufHash << inHash.rdbuf();
    std::string all = bufHash.str();
    inHash.close();
    if (mode == "create") {
        uint8_t sha[20] = {};
        SHA1(reinterpret_cast<const unsigned char *>(all.c_str()), all.length(), sha);
        using namespace Field;
        BigInteger::BigInt h;
        h.setByArray(sha, 20);
        BigInteger::BigInt x = "0"_BigIntMod, k = "0"_BigIntMod;
        Field::BigInt QX = "0"_BigIntMod, QY = "0"_BigIntMod;
        while (QX == "0"_BigIntMod || QY == "0"_BigIntMod)
        {
            x = "0"_BigIntMod;
            while (x == "0"_BigIntMod)
                x = Rand::rand(n.getBitSize() - 1);
            auto [qx, qy] = curve.mul({X, Y}, x);
            QX = qx; QY = qy;
        }
        Logger::print<Log::Level::info>(("X: " + x.toString()).c_str());
        Logger::print<Log::Level::info>(("QX: " + QX.toString()).c_str());
        Logger::print<Log::Level::info>(("QY: " + QY.toString()).c_str());
        BigInteger::BigInt s = "0"_BigIntMod;
        Field::BigInt pointX = "0"_BigIntMod, pointY = "0"_BigIntMod;
        while (s == "0"_BigIntMod)
        {
            pointX = "0"_BigIntMod; pointY = "0"_BigIntMod;

            while (pointX == "0"_BigIntMod || pointY == "0"_BigIntMod)
            {
                k = "0"_BigIntMod;
                while (k == "0"_BigIntMod || k == x)
                    k = Rand::rand(n.getBitSize() - 1);
                auto[PX, PY] = curve.mul({X, Y}, k);
                pointX = PX; pointY = PY;
            }
            auto[d1, k_1, wq] = BigInteger::gcb(k, n);
            Logger::print<Log::Level::info>("d1=%s, k_1=%s, wq=%s", d1.toString().c_str(), k_1.toString().c_str(), wq.toString().c_str());
            IntegerMod::setIntegerMod(n);
            Logger::print<Log::Level::info>(("k_1: " + (Field::BigInt(k_1) + "0"_BigIntMod).toString()).c_str());
            s = Field::BigInt((Field::BigInt(h) + Field::BigInt(x) * Field::BigInt(pointX)) * Field::BigInt(k_1));
            Logger::print<Log::Level::info>(("s: " + s.toString()).c_str());
            if(d1 != "1"_BigIntMod)
            {
                s = "0"_BigIntMod;
                Logger::print<Log::Level::info>("k_1: not exist");
            }
            IntegerMod::setIntegerMod(prime);
        }
        Logger::print<Log::Level::info>(("K: " + k.toString()).c_str());
        Logger::print<Log::Level::info>(("pointX: " + pointX.toString()).c_str());
        Logger::print<Log::Level::info>(("pointY: " + pointY.toString()).c_str());
        std::fstream out(fileHash + ".ecdsa", std::ios::out);
        out << "{\n  \"R\" : \"" << pointX.toString() << "\",\n";
        out << "  \"S\" : \"" << s.toString() << "\"\n";
        out << "  \"QX\" : \"" << QX.toString() << "\"\n";
        out << "  \"QY\" : \"" << QY.toString() << "\"\n}";
        out.close();
    } else if (mode == "confirm") {
        uint8_t sha[20]{};
        SHA1(reinterpret_cast<const unsigned char *>(all.c_str()), all.length(), sha);
        using namespace Field;
        BigInteger::BigInt h;
        h.setByArray(sha, 20);

        std::fstream inEcdsa(fileHash + ".ecdsa", std::ios_base::in);
        if (!inEcdsa.is_open())
        {
            Logger::print<Log::Level::fatal>((std::string("Can't open file: ") + fileHash).c_str());
            crash = true;
        }

        std::stringstream bufEcdsa;
        bufEcdsa << inEcdsa.rdbuf();
        std::string json2 = bufEcdsa.str();
        inEcdsa.close();
        auto dsa = parseEcdsa(json2);
        Field::BigInt R, S, QX, QY;
        R.setByString(dsa.R);
        QX.setByString(dsa.QX);
        QY.setByString(dsa.QY);

        Field::IntegerMod::setIntegerMod(n);
        S.setByString(dsa.S);
        Field::IntegerMod::setIntegerMod(prime);

        auto [d1, s_1, eoqp] = BigInteger::gcb(S, n);
        Logger::print<Log::Level::info>("d1=%s, s_1=%s, wq=%s", d1.toString().c_str(), s_1.toString().c_str(), eoqp.toString().c_str());
        Field::IntegerMod::setIntegerMod(n);
        auto u1 = Field::BigInt(h) * Field::BigInt(s_1); auto u2 = R * Field::BigInt(s_1);
        Logger::print<Log::Level::info>("u1=%s, u2=%s", u1.toString().c_str(), u2.toString().c_str());
        Field::IntegerMod::setIntegerMod(prime);
        auto[CX, CY] = curve.add(curve.mul({X, Y}, u1), curve.mul({QX, QY}, u2));
        if(CX == R)
        {
            std::cout << "Okey!\n";
        }else
            std::cout << "Error!\n";
    } else
        crash = true;
    if (crash) {
        Logger::setLevel(Log::Level::debug);
        Logger::stop();
        return -1;
    }

    Logger::print<Log::Level::debug>((std::string("Json config input: \n") + json).c_str());
    Logger::setLevel(Log::Level::debug);
    Logger::stop();
    return 0;

}
