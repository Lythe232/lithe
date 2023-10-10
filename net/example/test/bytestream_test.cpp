#include "byteStream.h"
#include <iostream>

using namespace lithe;
using namespace std;
int main()
{
    ByteStream bs;
    uint32_t uuv32 = 0xFFFFFFFF;
    uint64_t uuv64 = 0xFFFFFFFFFFFFFFFF;

    uint8_t fuv8  = 65;
    uint16_t fuv16 = 0x1FFF;
    uint32_t fuv32 = 0x1FFFFFFF;
    uint64_t fuv64 = 0x1FFFFFFFFFFFFFFF;

    int8_t  fsv8  = 64;
    int16_t fsv16 = 0x7FFF;
    int32_t fsv32 = 0x7FFFFFFF;
    int64_t fsv64 = 0x7FFFFFFFFFFFFFFF;

    float   fpv   = 123456.789;
    double  dpv   = 987654321.123456;

    std::string str16 = "adcdefghijklmnopqrstuvwxyz";
    std::string str32 = "adcdefghijklmnopqrstuvwxyz";
    std::string str64 = "adcdefghijklmnopqrstuvwxyz";
    std::string strV  = "adcdefghijklmnopqrstuvwxyz";

    std::cout << "uuv32 = " << uuv32 << endl;
    std::cout << "uuv64 = " << uuv64 << endl;
    std::cout << "fuv8 = " << fuv8 << endl;
    std::cout << "fuv16 = " << fuv16 << endl;
    std::cout << "fuv32 = " << fuv32 << endl;
    std::cout << "fuv64 = " << fuv64 << endl;
    std::cout << "fsv8 = " << fsv8 << endl;
    std::cout << "fsv16 = " << fsv16 << endl;
    std::cout << "fsv32 = " << fsv32 << endl;
    std::cout << "fsv64 = " << fsv64 << endl;
    std::cout << "fpv = " << fpv << endl;
    std::cout << "dpv = " << dpv << endl;
    std::cout << "str16 = " << str16 << endl;
    std::cout << "str32 = " << str32 << endl;
    std::cout << "str64 = " << str64 << endl;
    std::cout << "strV = " << strV << endl;
    std::cout << "______________________________________________\n";
    bs.writeUuint32(uuv32);
    bs.writeUuint64(uuv64);

    bs.writeFuint8(fuv8);
    bs.writeFuint16(fuv16);
    bs.writeFuint32(fuv32);
    bs.writeFuint64(fuv64);

    bs.writeFint8(fsv8);
    bs.writeFint16(fsv16);
    bs.writeFint32(fsv32);
    bs.writeFint64(fsv64);

    bs.writeFloat(fpv);
    bs.writeDouble(dpv);
    
    bs.writeStringF16(str16);
    bs.writeStringF32(str32);
    bs.writeStringF64(str64);
    bs.writeStringVint(strV);

    std::cout << "uuv32 = " << bs.readUuint32() << endl;
    std::cout << "uuv64 = " << bs.readUuint64() << endl;

    std::cout << "fuv8 = " << bs.readFuint8() << endl;
    std::cout << "fuv16 = " << bs.readFuint16() << endl;
    std::cout << "fuv32 = " << bs.readFuint32() << endl;
    std::cout << "fuv64 = " << bs.readFuint64() << endl;

    std::cout << "fsv8 = " << bs.readFint8() << endl;
    std::cout << "fsv16 = " << bs.readFint16() << endl;
    std::cout << "fsv32 = " << bs.readFint32() << endl;
    std::cout << "fsv64 = " << bs.readFint64() << endl;
    
    std::cout << "fpv = " << bs.readFloat() << endl;
    std::cout << "dpv = " << bs.readDouble() << endl;
    std::cout << "str16 = " << bs.readStringF16() << endl;
    std::cout << "str32 = " << bs.readStringF32() << endl;

    std::cout << "str64 = " << bs.readStringF64() << endl;
    std::cout << "strV = " << bs.readStringVint() << endl;
}
