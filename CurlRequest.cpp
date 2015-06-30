#include "CurlRequest.h"
#include "CurlData.h"
#include "Utils.h"

using namespace pv;

CurlRequest::CurlRequest(EMethod method)
: _url("")
, _params("")

, _callback(nullptr)
, _caller(nullptr)

, _data(nullptr)
, _userData(nullptr)

, _status(eNotStarted)
, _maxConnections(1)
, _timeout(120)
, _progress(0)
, _canceled(false)

, _curl(nullptr)
, _headerlist(nullptr)

, _method(method)
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

    if (_userData)
    {
        delete _userData;
        _userData = nullptr;
    }

    _errBuf.clear();
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
    if (userdata)
    {
        _userData = userdata;
    }
}

void CurlRequest::addParam(const std::string& field, const std::string& value)
{
    _paramList[field] = value;
}

void CurlRequest::cancel()
{
    _canceled = true;
}

ERequestStatus CurlRequest::getStatus() const
{
    return _status;
}

int CurlRequest::dataWrite(char* ptr, size_t size)
{
    if (_data && size > 0)
    {
        _data->copy(size, ptr);
        return static_cast<int>(size);
     }

    return 0;
}

int CurlRequest::headerWrite(char* ptr, size_t size)
{
    if (!ptr || size <= 0)
    {
        return 0;
    }

    std::string header(ptr, size);
    if (_data)
    {
        _data->addHeader(header);
        return  static_cast<int>(size);
    }

    return 0;

}

int CurlRequest::proccess(double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (_timeout > 0)
    {
        ++_progress;
    }

    if (_canceled || _progress > _timeout)
    {
        return -1;
    }

    return 0;
}

const std::string& CurlRequest::appendData()
{
    _params.clear();
    for (auto& param : _paramList)
    {
        if (param.first.empty())
        {
            continue;
        }

        _params.append(param.first);
        _params.append("=");

        std::string coded("");
        Codec::EncodeUrlRFC3986(param.second, coded);
        _params.append(coded);
        _params.append("&");
    }

    if (_params.size())
    {
        _params.resize(_params.size() - 1);
    }

    return _params;
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
