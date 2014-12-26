#ifndef _PV_CURL_REQUEST_H_
#define _PV_CURL_REQUEST_H_

#include "curl/curl.h"
#include "common.h"

namespace pv
{
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

    class CurlResponse;
    struct UserData;
    struct CurlData;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef std::function<void(const CurlResponse*)> callback;
    typedef std::unordered_map<std::string, std::string> ParamList;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlRequest
    {
    public:

        enum EMethod
        {
            eInvalid,
            eGet,
            ePost,
            eHead,
            eDelete
        };

        CurlRequest(EMethod method);
        virtual         ~CurlRequest();

        void            setCallback(callback callback, void* caller = nullptr);
        void            setUrl(const std::string& url);
        void            setUserData(UserData* userdata);

        void            addParam(const std::string& field, const std::string& value);
        void            addHeaders(const std::string& field, const  std::string& value);

    private:

        friend class    CurlWrapper;

        CURL*           _curl;

        std::string     _url;

        callback        _callback;
        void*           _caller;

        CurlData*       _data;
        UserData*       _userData;

        void            setStatus(ERequestStatus status);

        static int      dataWrite(char* ptr, size_t size, size_t nmemb, void* userdata);
        static int      headerWrite(char* ptr, size_t size, size_t nmemb, void* userdata);

        void            appendData(std::string& data);

        ERequestStatus  _status;
        int             _maxConnections;
        int             _timeout;

        ParamList       _paramList;
        EMethod         _method;

        curl_slist*     _headerlist;

        const size_t   _errBufSize = 1024;
        std::string    _errBuf;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_REQUEST_H_