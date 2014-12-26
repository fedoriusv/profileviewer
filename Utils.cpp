#include "Utils.h"

using namespace pv;

bool Codec::EncodeUrlRFC3986(const std::string& source, std::string& dest)
{
    const char* c = source.c_str();
    size_t srcSize = source.size();

    for (size_t i = 0; i< srcSize; i++)
    {
        if ((48 <= c[i] && c[i] <= 57) ||                              //0-9 //digits
            (65 <= c[i] && c[i] <= 90) ||                              //abc...xyz //lowalpha
            (97 <= c[i] && c[i] <= 122) ||                             //ABC...XYZ //highalpha
            (c[i] == '-' || c[i] == '.' || c[i] == '_' || c[i] == '~'))//other
        {
            dest.append(&c[i], 1);
        }
        else
        {
            dest.append("%");
            char hex1, hex2;
            EncUrlGetKeysFromChar(c[i], hex1, hex2);

            dest.append(&hex1, 1);  //converts char 255 to string "ff"
            dest.append(&hex2, 1);  //converts char 255 to string "ff"
        }
    }

    return true;
}

void Codec::EncUrlGetKeysFromChar(char nChar, char& hex1, char& hex2)
{
    hex1 = (nChar & 0xF0) >> 4;
    hex2 = (nChar & 0x0F);

    if (0 <= hex1 && hex1 <= 9)
    {
        hex1 += '0';    //0 => 48 inascii
    }

    if (10 <= hex1 && hex1 <= 15)
    {
        hex1 += 'A' - 10; //A => 65 inascii
    }

    if (0 <= hex2 && hex2 <= 9)
    {
        hex2 += '0';
    }

    if (10 <= hex2 && hex2 <= 15)
    {
        hex2 += 'A' - 10;
    }
}
