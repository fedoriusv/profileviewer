#include "CurlWrapper.h"
#include "CurlData.h"
#include "Utils.h"

#include <future>

using namespace pv;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

RequestSevice::RequestSevice(EMethod method)
: CurlRequest(method)
, _service(eUnknownService)
{
}

RequestSevice::~RequestSevice()
{
}

void RequestSevice::setService(ERequestSevice service)
{
    _service = service;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResponseService::ResponseService()
: CurlResponse()
{
}

ResponseService::~ResponseService()
{
}

ERequestSevice ResponseService::getSevice() const
{
    return _service;
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
    CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
    char* version = curl_version();

    fopen_s(&_logFile, "dump.log", "wb");
}

CurlWrapper::~CurlWrapper()
{
    fclose(_logFile);
    _requestList.clear();


    curl_global_cleanup();
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

const CurlResponse* CurlWrapper::addSyncRequest(CurlRequest* request)
{
    s_mutex.lock();

   ResponseService* response = nullptr;

    if (request->_status == eNotStarted)
    {
        CurlWrapper::setupHandler(request);

        CURLcode code = curl_easy_perform(request->_curl);

        response = new ResponseService();
        if (code == CURLE_OK)
        {
            response->_curlCode = CURLE_OK;
            response->_service = static_cast<RequestSevice*>(request)->_service;
            response->_caller = request->_caller;
            request->_data->swap(response->_data);
            if (request->_userData)
            {
                std::swap(response->_userData, request->_userData);
            }

            curl_easy_getinfo(request->_curl, CURLINFO_RESPONSE_CODE, &response->_responseCode);
        }
        else
        {
            response->_responseCode = -1;
            response->_curlCode = code;
#if _DEBUG
            printf("Request ended with error : [%d] %s", code, curl_easy_strerror(code));
#endif //_DEBUG

            request->setStatus(eError);
        }

        curl_easy_cleanup(request->_curl);
    }

    request->setStatus(eCompleted);

    s_mutex.unlock();

    return response;
}

void CurlWrapper::addAsyncRequest(CurlRequest* request)
{
    s_mutex.lock();

    if (request->_status == eNotStarted)
    {
        CurlWrapper::setupHandler(request);

        request->setStatus(eWaitStart);
        _requestList.push_back(request);
    }

    s_mutex.unlock();
}

void CurlWrapper::setupHandler(CurlRequest* request)
{
    s_mutex.lock();

    const std::string& data = request->appendData();
    if (request->_method == CurlRequest::eGet && !data.empty())
    {
        request->_url.append("?");
        request->_url += data;
    }

    request->_curl = curl_easy_init();
    curl_easy_setopt(request->_curl, CURLOPT_CUSTOMREQUEST, 0);

    switch (request->_method)
    {
    case CurlRequest::eGet:

        curl_easy_setopt(request->_curl, CURLOPT_HTTPGET, 1L);
        break;

    case CurlRequest::ePost:

        curl_easy_setopt(request->_curl, CURLOPT_POST, 1L);
        curl_easy_setopt(request->_curl, CURLOPT_POSTFIELDSIZE, data.size());
        curl_easy_setopt(request->_curl, CURLOPT_POSTFIELDS, data.c_str());
        break;

    case CurlRequest::eHead:

        curl_easy_setopt(request->_curl, CURLOPT_NOBODY, 1L);
        break;

    case CurlRequest::eDelete:

        curl_easy_setopt(request->_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;

    default:
        break;
    }

    curl_easy_setopt(request->_curl, CURLOPT_URL, request->_url.c_str());
    curl_easy_setopt(request->_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(request->_curl, CURLOPT_MAXCONNECTS, request->_maxConnections);
    curl_easy_setopt(request->_curl, CURLOPT_CONNECTTIMEOUT, request->_timeout);

    curl_easy_setopt(request->_curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(request->_curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(request->_curl, CURLOPT_PROGRESSFUNCTION, CurlWrapper::proccess);
    curl_easy_setopt(request->_curl, CURLOPT_PROGRESSDATA, request);
    curl_easy_setopt(request->_curl, CURLOPT_HEADERFUNCTION, CurlWrapper::headerWrite);
    curl_easy_setopt(request->_curl, CURLOPT_WRITEHEADER, request);
    curl_easy_setopt(request->_curl, CURLOPT_WRITEFUNCTION, CurlWrapper::dataWrite);
    curl_easy_setopt(request->_curl, CURLOPT_WRITEDATA, request);

    curl_easy_setopt(request->_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    //curl_easy_setopt(request->_curl, CURLOPT_ACCEPT_ENCODING, "gzip;q=1.0, deflate;q=1.0, identity;q=0.5, *;q=0");

#ifdef _DEBUG
    request->_errBuf.clear();
    request->_errBuf.resize(request->_errBufSize);
    curl_easy_setopt(request->_curl, CURLOPT_ERRORBUFFER, &request->_errBuf[0]);

    curl_easy_setopt(request->_curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(request->_curl, CURLOPT_STDERR, _logFile);
#endif //_DEBUG

    if (request->_headerlist)
    {
        curl_easy_setopt(request->_curl, CURLOPT_HTTPHEADER, request->_headerlist);
    }

    s_mutex.unlock();
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
            ResponseService* response = new ResponseService();
            response->_responseCode = -1;
            response->_curlCode = code;
            response->_service = static_cast<RequestSevice*>(request)->_service;
            response->_caller = request->_caller;
            response->_error = request->_errBuf;
#if _DEBUG
            printf("Request ended with error : [%d] %s", code, curl_easy_strerror(code));
#endif //_DEBUG

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

    ResponseService* response = new ResponseService();

    response->_curlCode = CURLE_OK;
    response->_service = static_cast<RequestSevice*>(request)->_service;
    response->_caller = request->_caller;

    request->_data->swap(response->_data);

    if (request->_userData)
    {
        std::swap(response->_userData, request->_userData);
    }

    curl_easy_getinfo(request->_curl, CURLINFO_RESPONSE_CODE, &response->_responseCode);

    request->_callback(response);

    s_mutex.unlock();
}

int CurlWrapper::proccess(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (userdata)
    {
        CurlRequest* request = static_cast<CurlRequest*>(userdata);

        return request->proccess(dltotal, dlnow, ultotal, ulnow);
    }

    return -1;
}

int CurlWrapper::headerWrite(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    if (userdata)
    {
        CurlRequest* request = static_cast<CurlRequest*>(userdata);
        size_t sizeStr = size * nmemb;

        return request->headerWrite(ptr, sizeStr);
    }

    return 0;
}

int CurlWrapper::dataWrite(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    if (userdata)
    {
        CurlRequest* request = static_cast<CurlRequest*>(userdata);
        size_t sizeStr = size * nmemb;

        if (!request->_canceled)
        {
            return request->dataWrite(ptr, sizeStr);
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////