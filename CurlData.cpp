#include "CurlData.h"

using namespace pv;

CurlData::CurlData()
    : _size(0)
    , _data(nullptr)
{
}

CurlData::~CurlData()
{
    CurlData::reset();
}

void CurlData::addHeader(const std::string& header)
{
    _headerLines.push_back(header);
}

void CurlData::reset()
{
    _headerLines.clear();

    _size = 0;
    if (_data)
    {
        free(_data);
        _data = nullptr;
    }
}

void CurlData::swap(CurlData* data)
{
    std::swap(_size, data->_size);
    std::swap(_data, data->_data);
    std::swap(_headerLines, data->_headerLines);
};

void CurlData::copy(size_t size, void* data)
{
    if (_data)
    {
        free(_data);
        _data = nullptr;
    }

    _size = size;
    _data = malloc(size);
    memcpy(_data, data, size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

UserData::UserData()
    : _value(0)
{
}

UserData::~UserData()
{
}