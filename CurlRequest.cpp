#include "CurlRequest.h"
#include "CurlData.h"
#include "Utils.h"

using namespace pv;

CurlRequest::CurlRequest(EMethod method)
: _curl(nullptr)
, _url("")
, _callback(nullptr)
, _status(eNotStarted)
, _caller(nullptr)
, _maxConnections(1)
, _timeout(120)
, _data(nullptr)
, _userData(nullptr)
, _method(method)
, _headerlist(nullptr)
{
    _errBuf.resize(_errBufSize);

    _data = new CurlData();
}

CurlRequest::~CurlRequest()
{
    if (_data)
    {
        delete _data;
        _data = nullptr;
    }

    _paramList.clear();

    if (_headerlist)
    {
        curl_slist_free_all(_headerlist);
        _headerlist = nullptr;
    }
}

void CurlRequest::setCallback(callback callback, void* caller)
{
    _callback = callback;
    _caller = caller;
}

void CurlRequest::setUrl(const std::string& url)
{
    _url = url;
}

void CurlRequest::setStatus(ERequestStatus status)
{
    _status = status;
}

void CurlRequest::setUserData(UserData* userdata)
{
    _userData = userdata;
}

void CurlRequest::addParam(const std::string& field, const std::string& value)
{
    _paramList[field] = value;
}

int CurlRequest::dataWrite(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    if (userdata)
    {
        CurlData* data = static_cast<CurlData*>(userdata);

        data->_size = size * nmemb;

        if (data->_size > 0)
        {
            data->_data = malloc(data->_size);
            memcpy(data->_data, ptr, data->_size);

            return data->_size;
        }
    }

    return 0;
}

int CurlRequest::headerWrite(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    if (userdata)
    {
        CurlData* data = static_cast<CurlData*>(userdata);

        data->_size = size * nmemb;

        if (data->_size > 0)
        {
            data->_data = malloc(data->_size);
            memcpy(data->_data, ptr, data->_size);

            return data->_size;
        }
    }

    return 0;
}

void CurlRequest::appendData(std::string& data)
{
    data.clear();
    for (auto& param : _paramList)
    {
        if (param.first.empty())
        {
            continue;
        }

        data.append(param.first);
        data.append("=");

        std::string coded("");
        Codec::EncodeUrlRFC3986(param.second, coded);
        data.append(coded);
        data.append("&");
    }

    if (data.size())
    {
        data.resize(data.size() - 1);
    }
}

void CurlRequest::addHeaders(const std::string& field, const  std::string& value)
{
    if (_status != eStarted)
    {
        std::string headerfield = field;
        headerfield.append(": ");
        headerfield.append(value);

        _headerlist = curl_slist_append(_headerlist, headerfield.c_str());
    }
}
