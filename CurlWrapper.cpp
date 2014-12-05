#include "CurlWrapper.h"

#include <future>

using namespace pv;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

CurlResponse::CurlResponse()
: _responseCode(0)
, _curlCode(CURLE_OK)
, _caller(nullptr)
, _service(eUnknownService)
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

ERequestSevice CurlResponse::getSevice() const
{
    return _service;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CurlRequest::CurlRequest()
: _curl(nullptr)
, _url("")
, _callback(nullptr)
, _status(eNotStarted)
, _caller(nullptr)
, _maxConnections(1)
, _timeout(120)
, _data(nullptr)
, _service(eUnknownService)
, _userData(nullptr)
{
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

void CurlRequest::setService(ERequestSevice service)
{
    _service = service;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::recursive_mutex CurlWrapper::s_mutex;

std::string CurlWrapper::k_serviceName[eCountSevice] =
{
    "unknown",
    "eve",
    "pandora",
    "auth",
    "storage",
    "storage",
};

ERequestSevice CurlWrapper::getServiceByName(const std::string& service)
{
    for (int i = 0; i < eCountSevice; ++i)
    {
        if (k_serviceName[i] == service)
        {
            return (ERequestSevice)i;
        }
    }

    return eUnknownService;
}

CurlWrapper::CurlWrapper()
{
    _errBuf.resize(_errBufSize);
}

CurlWrapper::~CurlWrapper()
{
    _requestList.clear();
}

void CurlWrapper::update()
{
    while (_requestList.size() > 0)
    {
        for (std::vector<CurlRequest*>::iterator iter = _requestList.begin(); iter < _requestList.end(); ++iter)
        {
            if ((*iter)->_status == eWaitStart)
            {
                (*iter)->setStatus(eStarted);
                std::async(std::launch::async, &CurlWrapper::runRequest, this, (*iter));

                continue;
            }

            if ( (*iter)->_status == eCompleted)
            {
                delete (*iter);
                (*iter) = nullptr;

                _requestList.erase(iter);

                break;
            }
        }
    }
}

const CurlResponse* CurlWrapper::addSynRequest(CurlRequest* request)
{
    CurlResponse* response = nullptr;

    if (request->_status == eNotStarted)
    {
        for (auto& param : request->_paramList)
        {
            if (param.first.empty())
            {
                continue;
            }

            request->_url.append("&");
            request->_url.append(param.first);
            request->_url.append("=");
            request->_url.append(param.second);
        }

        request->_curl = curl_easy_init();

        curl_easy_setopt(request->_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(request->_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(request->_curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(request->_curl, CURLOPT_URL, request->_url.c_str());

        curl_easy_setopt(request->_curl, CURLOPT_MAXCONNECTS, request->_maxConnections);
        curl_easy_setopt(request->_curl, CURLOPT_CONNECTTIMEOUT, request->_timeout);

        curl_easy_setopt(request->_curl, CURLOPT_WRITEFUNCTION, request->dataWrite);
        curl_easy_setopt(request->_curl, CURLOPT_WRITEDATA, request->_data);

        //curl_easy_setopt(request._curl, CURLOPT_HEADERFUNCTION, request.headerWrite);
        //curl_easy_setopt(request._curl, CURLOPT_WRITEHEADER, &request._data);

        curl_easy_setopt(request->_curl, CURLOPT_SSL_VERIFYPEER, 0L);

        _errBuf.clear();
        _errBuf.resize(_errBufSize);
        curl_easy_setopt(request->_curl, CURLOPT_ERRORBUFFER, &_errBuf[0]);

        CURLcode code = curl_easy_perform(request->_curl);

        response = new CurlResponse();
        if (code == CURLE_OK)
        {
            response->_curlCode = CURLE_OK;
            response->_service = request->_service;
            response->_caller = request->_caller;
            request->_data->swap(response->_data);
            if (request->_userData)
            {
                response->_userData = request->_userData;
            }

            curl_easy_getinfo(request->_curl, CURLINFO_RESPONSE_CODE, &response->_responseCode);
        }
        else
        {
            response->_responseCode = -1;
            response->_curlCode = code;

            request->setStatus(eError);
        }

        curl_easy_cleanup(request->_curl);
    }

    request->setStatus(eCompleted);

    return response;
}

void CurlWrapper::addAsynRequest(CurlRequest* request)
{
    if (request->_status == eNotStarted)
    {
        for (auto& param : request->_paramList)
        {
            if (param.first.empty())
            {
                continue;
            }

            request->_url.append("&");
            request->_url.append(param.first);
            request->_url.append("=");
            request->_url.append(param.second);
        }

        request->_curl = curl_easy_init();

        curl_easy_setopt(request->_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(request->_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(request->_curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(request->_curl, CURLOPT_URL, request->_url.c_str());

        curl_easy_setopt(request->_curl, CURLOPT_MAXCONNECTS, request->_maxConnections);
        curl_easy_setopt(request->_curl, CURLOPT_CONNECTTIMEOUT, request->_timeout);

        curl_easy_setopt(request->_curl, CURLOPT_WRITEFUNCTION, request->dataWrite);
        curl_easy_setopt(request->_curl, CURLOPT_WRITEDATA, request->_data);

        curl_easy_setopt(request->_curl, CURLOPT_SSL_VERIFYPEER, 0L);

       //curl_easy_setopt(request._curl, CURLOPT_HEADERFUNCTION, request.headerWrite);
        //curl_easy_setopt(request._curl, CURLOPT_WRITEHEADER, &request._data);

        _errBuf.clear();
        _errBuf.resize(_errBufSize);
        curl_easy_setopt(request->_curl, CURLOPT_ERRORBUFFER, &_errBuf[0]);

        request->setStatus(eWaitStart);

        _requestList.push_back(request);
    }
}

bool CurlWrapper::runRequest(CurlRequest* request)
{
    s_mutex.lock();

    if (request->_status == eStarted)
    {
        CURLcode code = curl_easy_perform(request->_curl);

        if (code == CURLE_OK)
        {
            responseProcessed(request);
        }
        else
        {
            CurlResponse* response = new CurlResponse();
            response->_responseCode = -1;
            response->_curlCode = code;
            response->_service = request->_service;
            response->_caller = request->_caller;
            response->_error = _errBuf;

            request->setStatus(eError);
            request->_callback(response);
        }

        curl_easy_cleanup(request->_curl);
    }

    request->setStatus(eCompleted);

    s_mutex.unlock();

    return true;
}

void CurlWrapper::responseProcessed(CurlRequest* request)
{
    s_mutex.lock();

    CurlResponse* response = new CurlResponse();

    response->_curlCode = CURLE_OK;
    response->_service = request->_service;
    response->_caller = request->_caller;

    request->_data->swap(response->_data);

    if (request->_userData)
    {
        response->_userData = request->_userData;
    }

    curl_easy_getinfo(request->_curl, CURLINFO_RESPONSE_CODE, &response->_responseCode);

    request->_callback(response);

    s_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////