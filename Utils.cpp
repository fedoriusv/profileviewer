#include "Utils.h"

using namespace pv;

static const char* s_base64EncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char Codec_Dec64_GetKeyFromChar(char nChar)
{
    ASSERT(Codec::IsInBase64Alphabet(nChar));

    if (nChar == '/')
    {
        return 63;
    }
    else if (nChar == '+')
    {
        return 62;
    }
    else if (nChar < 58)
    {//48-57 is '0'-'9', index is 52-61
        return (char)(nChar + 4);
    }
    else if (nChar < 91)
    {//65-91 is 'A'-'Z', index is 0-25 
        return (char)(nChar - 65);
    }
    else
    {//97-122 is 'a'-'z', index is 26-51
        return (char)(nChar - 71);
    }
}

size_t Codec_DecodeBase64Strict(const char* base64Data, size_t dataSize, void* outData)
{
    if (outData == 0)
    {
        return 0;
    }

    size_t length = dataSize;
    const char* data = base64Data;
    for (; data[length - 1] == '='; --length);

    size_t skipped = 0;
    // skip \r\n for PEM and MIME decoding
    for (size_t i = 0; i < length; ++i)
    {
        if (Codec::IsInBase64Alphabet(data[i]) == false)
        {
            ++skipped;
        }
    }

    const size_t stripped_length = length - skipped;
    const size_t tail_length = stripped_length % 4;

    const size_t baseLength = length - tail_length; //decode by group of 4 chars

    ASSERT((baseLength - skipped) % 4 == 0);

    unsigned char codedChar[4];
    unsigned char* decodedChar = static_cast<unsigned char*>(outData);

    size_t outIndex = 0;
    size_t i = 0;
    for (; i < baseLength; i += 4)
    {
        // skip \r\n for PEM and MIME decoding
        for (; i < baseLength && Codec::IsInBase64Alphabet(data[i]) == false; ++i);
        if (i >(baseLength - 3))
        {
            break;
        }

        codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);

        // skip \r\n for PEM and MIME decoding
        for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 1]) == false; ++i);
        if (i >(baseLength - 3))
        {
            break;
        }

        codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);

        for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 2]) == false; ++i);
        if (i >(baseLength - 3))
        {
            break;
        }

        codedChar[2] = Codec_Dec64_GetKeyFromChar(data[i + 2]);

        // skip \r\n for PEM and MIME decoding
        for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 3]) == false; ++i);
        if (i >(baseLength - 3))
        {
            break;
        }

        codedChar[3] = Codec_Dec64_GetKeyFromChar(data[i + 3]);

        decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
        decodedChar[outIndex++] = (codedChar[1] << 4) + ((codedChar[2] & 0x3c) >> 2);
        decodedChar[outIndex++] = (codedChar[2] << 6) + (codedChar[3] & 0x3f);

    }

    if (tail_length > 0)
    {
        if (tail_length == 1) //should never happen ...
        {
            // Do nothing 6bit is not enough to decode

        }
        else if (tail_length == 2) //encoded %3 + 1 (only 8 bits valid)
        {
            for (; i < baseLength && Codec::IsInBase64Alphabet(data[i]) == false; ++i);
            codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);

            for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 1]); ++i);
            codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);

            decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
        }
        else //encoded %3 + 2  (only 16 bits valid)
        {
            for (; i < baseLength && Codec::IsInBase64Alphabet(data[i]) == false; ++i);
            codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);

            for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 1]) == false; ++i);
            codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);

            for (; i < baseLength && Codec::IsInBase64Alphabet(data[i + 2]) == false; ++i);
            codedChar[2] = Codec_Dec64_GetKeyFromChar(data[i + 2]);

            decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
            decodedChar[outIndex++] = (codedChar[1] << 4) + ((codedChar[2] & 0x3c) >> 2);
        }
    }

    return outIndex;
}

size_t Codec_DecodeBase64Relaxed(const char* base64Data, size_t dataSize, void* outData)
{
    if (outData == 0)
    {
        return 0;
    }

    size_t length = dataSize;
    const char* data = base64Data;
    for (; data[length - 1] == '='; --length);

    const size_t tail_length = length % 4;

    const size_t baseLength = length - tail_length; //decode by group of 4 chars

    unsigned char codedChar[4];
    unsigned char* decodedChar = static_cast<unsigned char*>(outData);

    size_t outIndex = 0;
    size_t i = 0;
    for (; i < baseLength; i += 4)
    {
        codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);
        codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);
        codedChar[2] = Codec_Dec64_GetKeyFromChar(data[i + 2]);
        codedChar[3] = Codec_Dec64_GetKeyFromChar(data[i + 3]);

        decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
        decodedChar[outIndex++] = (codedChar[1] << 4) + ((codedChar[2] & 0x3c) >> 2);
        decodedChar[outIndex++] = (codedChar[2] << 6) + (codedChar[3] & 0x3f);

    }

    if (tail_length > 0)
    {
        if (tail_length == 1) //should never happen ...
        {
            // Do nothing 6bit is not enough to decode

        }
        else if (tail_length == 2) //encoded %3 + 1 (only 8 bits valid)
        {
            codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);
            codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);

            decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
        }
        else //encoded %3 + 2  (only 16 bits valid)
        {
            codedChar[0] = Codec_Dec64_GetKeyFromChar(data[i]);
            codedChar[1] = Codec_Dec64_GetKeyFromChar(data[i + 1]);
            codedChar[2] = Codec_Dec64_GetKeyFromChar(data[i + 2]);

            decodedChar[outIndex++] = (codedChar[0] << 2) + ((codedChar[1] & 0x3f) >> 4);
            decodedChar[outIndex++] = (codedChar[1] << 4) + ((codedChar[2] & 0x3c) >> 2);
        }
    }

    return outIndex;
}

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

size_t Codec::GetDecodedBase64DataSize(const std::string& base64Data, bool strict)
{
    if (base64Data.empty())
    {
        return 0;
    }

    size_t nbChar = base64Data.size();
    const char* str64 = base64Data.c_str();
    for (; nbChar > 0 && str64[nbChar - 1] == '='; --nbChar); //do not count '=' padding

    // skip \r\n for PEM and MIME decoding
    if (strict)
    {
        for (size_t i = (nbChar - 1); i < nbChar; --i)
        {
            if (IsInBase64Alphabet(str64[i]) == false)
            {
                --nbChar;
            }
        }
    }

    size_t dataSize = (nbChar / 4) * 3;

    switch (nbChar % 4)
    {
        case 2: //12bit
        {
            dataSize += 1;
            break;
        }
        case 3: //18bit
        {
            dataSize += 2;
            break;
        }
        default:
        {
            //do nothing, if 1, only 6bit is available
            break;
        }
    }

    return dataSize;
}

bool Codec::IsInBase64Alphabet(char data)
{
    return  ((data == '+') || (data == '/') || (data >= '0' && data <= '9') || (data >= 'A' && data <= 'Z') || (data >= 'a' && data <= 'z'));
}

bool Codec::EncodeBase64(const unsigned char* src, size_t dataSize, std::string& dest, size_t maxLineLength)
{
    //We skip the line CR/LF after 76 bytes tp avoid a conversion problem in NSURLRequest
    if (src == 0 || dataSize == 0)
    {
        return false;
    }

    size_t baseLength = dataSize - (dataSize % 3); //encode by group of 3 chars

    char codedChar[4];
    for (size_t i = 0; i < baseLength; i += 3)
    {
        codedChar[0] = s_base64EncodeTable[src[i] >> 2];
        codedChar[1] = s_base64EncodeTable[((src[i] & 0x03) << 4) + (src[i + 1] >> 4)];
        codedChar[2] = s_base64EncodeTable[((src[i + 1] & 0x0f) << 2) + ((src[i + 2] & 0xC0) >> 6)];
        codedChar[3] = s_base64EncodeTable[src[i + 2] & 0x3f];
        dest.append(codedChar, 4);
    }

    if (baseLength != dataSize)
    {
        memset(codedChar, '=', 4); //Pad with =
        if (dataSize - baseLength == 1)
        {
            codedChar[0] = s_base64EncodeTable[src[baseLength] >> 2];
            codedChar[1] = s_base64EncodeTable[((src[baseLength] & 0x03) << 4)];
        }
        else //2
        {
            codedChar[0] = s_base64EncodeTable[src[baseLength] >> 2];
            codedChar[1] = s_base64EncodeTable[((src[baseLength] & 0x03) << 4) + (src[baseLength + 1] >> 4)];
            codedChar[2] = s_base64EncodeTable[((src[baseLength + 1] & 0x0f) << 2)];
        }
        dest.append(codedChar, 4);
    }

    // insert line feed every maxLineLength
    if (maxLineLength > 0)
    {
        // Do not insert the last line feed.
        const size_t line_feeds = ((dest.size() % maxLineLength) == 0) ? (dest.size() / maxLineLength) - 1 : (dest.size() / maxLineLength);
        for (size_t i = line_feeds; i > 0; --i)
        {
            dest.insert(i*maxLineLength, "\r\n");
        }
    }

    return true;
}

size_t Codec::DecodeBase64(const std::string& base64Data, void* outData, bool strict)
{
    return DecodeBase64(base64Data.c_str(), base64Data.size(), outData, strict);
}

size_t Codec::DecodeBase64(const char* base64Data, size_t dataSize, void* outData, bool strict)
{
    if (base64Data == 0 || dataSize == 0)
        return 0;

    return strict ? Codec_DecodeBase64Strict(base64Data, dataSize, outData) : Codec_DecodeBase64Relaxed(base64Data, dataSize, outData);
}