#include "UserProfile.h"
#include "Logger.h"
#include "Utils.h"

#include "lz4.h"

using namespace pv;

const char k_rawSize = sizeof(int);
const char k_compressSize = sizeof(int);

UserProfile::UserProfile()
: _compressSize(0)
, _rawSize(0)
, _data("")
{
}

UserProfile::~UserProfile()
{
}

bool UserProfile::read(const std::string& data)
{
    if (data.empty())
    {
        LOG_ERROR("UserProfile::read: Data empty");
        return false;
    }

    std::string decodeData;
    if (!UserProfile::decode(data, decodeData))
    {
        LOG_ERROR("UserProfile::read: Decompress error");
        return false;
    }

    if (decodeData.empty())
    {
        LOG_ERROR("UserProfile::read: Decode Data empty");
        return false;
    }

    std::string decompressedData;
    if (!UserProfile::decompress(decodeData, decompressedData))
    {
        LOG_ERROR("UserProfile::read: Decompress error");
        return false;
    }

    if (decompressedData.empty())
    {
        LOG_ERROR("UserProfile::read: Decompressed Data empty");
        return false;
    }

    _data = decompressedData;
    LOG_INFO("UserProfile::read: Read user data completed");
    return true;
}

bool UserProfile::write(std::string& data)
{
    if (_data.empty())
    {
        LOG_ERROR("UserProfile::write: User write data empty");
        return false;
    }

    std::string compressedData;
    if (!UserProfile::compress(_data, compressedData))
    {
        LOG_ERROR("UserProfile::write: Compress error");
        return false;
    }

    if (compressedData.empty())
    {
        LOG_ERROR("UserProfile::write: Compressed Data empty");
        return false;
    }

    if (!UserProfile::encode(compressedData, data))
    {
        LOG_ERROR("UserProfile::write: Encode error");
        return false;
    }

    LOG_INFO("UserProfile::write: Write user data completed");
    return false;
}

bool UserProfile::decompress(const std::string& inData, std::string& outData)
{
    if (inData.empty())
    {
        LOG_ERROR("UserProfile::decompress: Input Data Empty");
        return false;
    }

    memcpy(&_rawSize, inData.data(), k_rawSize);
    memcpy(&_compressSize, inData.data() + k_rawSize, k_compressSize);

    std::string compressStr;
    compressStr.reserve(_compressSize);
    memcpy(&compressStr[0], inData.data() + k_rawSize + k_compressSize, _compressSize);

    outData.clear();
    outData.resize(_rawSize);

    int readSize = LZ4_decompress_safe(compressStr.c_str(), &outData[0], _compressSize, _rawSize);
    if (readSize <= 0)
    {
        LOG_ERROR("UserProfile::decompress: LZ4 Decompress ERROR");
        return false;
    }

    return true;
}

bool UserProfile::compress(const std::string& inData, std::string& outData)
{
    if (inData.empty())
    {
        LOG_ERROR("UserProfile::compress: Input Data Empty");
        return false;
    }

    _compressSize = LZ4_compressBound(_rawSize);

    std::string compressStr;
    compressStr.resize(_compressSize);

    _compressSize = LZ4_compress(_data.c_str(), &compressStr[0], _rawSize);
    if (_compressSize <= 0)
    {
        LOG_ERROR("UserProfile::compress: LZ4 Compress ERROR");
        return false;
    }

    compressStr.resize(_compressSize);
    outData.resize(_compressSize + k_rawSize + k_compressSize);

    memcpy(&outData[0], &_rawSize, k_rawSize);                                            //raw size
    memcpy(&outData[0] + k_rawSize, &_compressSize, k_compressSize);                      //compress size
    memcpy(&outData[0] + k_rawSize + k_compressSize, compressStr.data(), _compressSize);  //compress string

    return true;
}

bool UserProfile::decode(const std::string& inData, std::string& outData)
{
    if (inData.empty())
    {
        LOG_ERROR("UserProfile::decode: Input Data Empty");
        return false;
    }
    int size = Codec::GetDecodedBase64DataSize(inData);
    outData.resize(size);

    int decodedSize = Codec::DecodeBase64(inData.c_str(), inData.size(), &outData[0]);
    ASSERT(size == decodedSize && "DecodeBase64 ERROR");

    return true;
}

bool UserProfile::encode(const std::string& inData, std::string& outData)
{
    if (inData.empty())
    {
        LOG_ERROR("UserProfile::encode: Compressed Data Empty");
        return false;
    }

    bool success = Codec::EncodeBase64(reinterpret_cast<const unsigned char*>(inData.data()), _compressSize + k_rawSize + k_compressSize, outData, 0);
    ASSERT(success && "EncodeBase64 ERROR");

    return true;
}