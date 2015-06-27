#ifndef _PV_CURL_WRAPPER_H_
#define _PV_CURL_WRAPPER_H_

#include "common.h"
#include "CurlRequest.h"
#include "CurlResponse.h"

namespace pv
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum EResopnceCode
    {
        eSuccess = 200,
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    class RequestSevice : public CurlRequest
    {
    public:

        RequestSevice(EMethod method);
        ~RequestSevice();

        void setService(ERequestSevice service);

    private:

        friend class    CurlWrapper;

        ERequestSevice  _service;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class ResponseService : public CurlResponse
    {
    public:

        ResponseService();
        ~ResponseService();

        ERequestSevice  getSevice() const;

    private:

        friend class    CurlWrapper;

        ERequestSevice  _service;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlWrapper
    {
    public:

        CurlWrapper();
        ~CurlWrapper();

        void                        update();

        void                        addAsyncRequest(CurlRequest* request);
        const CurlResponse*         addSyncRequest(CurlRequest* request);

        static  ERequestSevice      getServiceByName(const std::string& service);

    private:

        static int                  headerWrite(char* ptr, size_t size, size_t nmemb, void* userdata);
        static int                  dataWrite(char* ptr, size_t size, size_t nmemb, void* userdata);
        static int                  proccess(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow);

        bool                        runRequest(CurlRequest* request);
        void                        responseProcessed(CurlRequest* request);
        void                        setupHandler(CurlRequest* request);

        std::vector<CurlRequest*>   _requestList;
        FILE*                       _logFile;

        static std::recursive_mutex s_mutex;

        static  std::string         k_serviceName[eCountSevice];
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_WRAPPER_H_