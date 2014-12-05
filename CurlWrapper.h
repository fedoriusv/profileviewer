#ifndef _PV_CURL_WRAPPER_H_
#define _PV_CURL_WRAPPER_H_

#include "curl/curl.h"

#include "common.h"

namespace pv
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum EResopnceCode
    {
        eSuccess = 200,
    };


    enum ERequestSevice
    {
        eUnknownService,
        eGetConfigLinks,
        eGetSevriceLink,
        eGetAccessToken,
        eGetUserProfile,
        ePutUserProfile,

        eCountSevice
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum ERequestStatus
    {
        eError = -1,
        eNotStarted = 0,
        eWaitStart,
        eStarted,
        eCompleted
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct CurlData
    {
        CurlData()
        : _size(0)
        , _data(nullptr)
        {}

        ~CurlData()
        {
            _size = 0;
            if (_data)
            {
                free(_data);
            }
        }

        int     _size;
        void*   _data;

        void    swap(CurlData* data)
        {
            std::swap(_size, data->_size);
            std::swap(_data, data->_data);
        }
    };

    struct UserData
    {
        UserData()
        : _value(0)
        {}

        int _value;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlResponse
    {
    public:

        CurlResponse();
        ~CurlResponse();

        int             getResponseCode()   const;
        void*           getCaller()         const;
        ERequestSevice  getSevice()         const;

        int             getSize()           const;
        void*           getData()           const;
        UserData*       getUserData()       const;

    private:

        friend class    CurlWrapper;

        ERequestSevice  _service;
        int             _responseCode;
        int             _curlCode;
        void*           _caller;

        std::string     _error;

        CurlData*       _data;
        UserData*       _userData;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef std::function<void(const CurlResponse*)> callback;

    typedef std::unordered_map<std::string, std::string> ParamList;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlRequest
    {
    public:

        CurlRequest();
        ~CurlRequest();

        void            setCallback(callback callback, void* caller = nullptr);
        void            setUrl(const std::string& url);
        void            setService(ERequestSevice service);
        void            setUserData(UserData* userdata);

        void            addParam(const std::string& field, const std::string& value);

    private:

        friend class    CurlWrapper;

        CURL*           _curl;

        std::string     _url;

        callback        _callback;
        void*           _caller;
        ERequestSevice  _service;

        CurlData*       _data;
        UserData*       _userData;

        void            setStatus(ERequestStatus status);

        static int      dataWrite(char* ptr, size_t size, size_t nmemb, void* userdata);
        static int      headerWrite(char* ptr, size_t size, size_t nmemb, void* userdata);

        ERequestStatus  _status;
        int             _maxConnections;
        int             _timeout;

        ParamList       _paramList;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlWrapper
    {
    public:

        CurlWrapper();
        ~CurlWrapper();

        void                        update();

        void                        addAsynRequest(CurlRequest* request);
        const CurlResponse*         addSynRequest(CurlRequest* request);

        static  ERequestSevice      getServiceByName(const std::string& service);

    private:

        bool                        runRequest(CurlRequest* request);
        void                        responseProcessed(CurlRequest* request);

        std::vector<CurlRequest*>   _requestList;

        const size_t                _errBufSize = 1024;
        std::string                 _errBuf;
        
        static std::recursive_mutex s_mutex;

        static  std::string         k_serviceName[eCountSevice];
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_WRAPPER_H_