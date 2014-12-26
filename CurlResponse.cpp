#include "CurlResponse.h"
#include "CurlData.h"

using namespace pv;

CurlResponse::CurlResponse()
: _responseCode(0)
, _curlCode(CURLE_OK)
, _caller(nullptr)
, _userData(nullptr)
{
    _data = new CurlData();
}

CurlResponse::~CurlResponse()
{
    if (_data)
    {
        delete _data;
        _data = nullptr;
    }

    if (_userData)
    {
        delete _userData;
        _userData = nullptr;
    }
}

int CurlResponse::getResponseCode() const
{
    return _responseCode;
}

void* CurlResponse::getCaller() const
{
    return _caller;
}

int CurlResponse::getSize() const
{
    if (_data)
    {
        return _data->_size;
    }

    return 0;
}

void* CurlResponse::getData() const
{
    if (_data && _data->_data)
    {
        return _data->_data;
    }

    return nullptr;
}

UserData* CurlResponse::getUserData() const
{
    if (_userData)
    {
        return _userData;
    }

    return nullptr;
}