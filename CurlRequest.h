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

        void            cancel();

        ERequestStatus  getStatus() const;

    private:

        friend class    CurlWrapper;

        std::string     _url;

        callback        _callback;
        void*           _caller;

        CurlData*       _data;
        UserData*       _userData;

        void            setStatus(ERequestStatus status);

        int             dataWrite(char* ptr, size_t size);
        int             headerWrite(char* ptr, size_t size);
        int             proccess(double dltotal, double dlnow, double ultotal, double ulnow);

        void            appendData(std::string& data);

        ERequestStatus  _status;
        int             _maxConnections;
        int             _timeout;
        int             _progress;
        bool            _canceled;

        ParamList       _paramList;
        EMethod         _method;

        CURL*           _curl;
        curl_slist*     _headerlist;

        const size_t    _errBufSize = 1024;
        std::string     _errBuf;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_REQUEST_H_