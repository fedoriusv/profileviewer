#ifndef _PV_CURL_RESPONSE_H_
#define _PV_CURL_RESPONSE_H_

#include "curl/curl.h"
#include "common.h"

namespace pv
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct UserData;
    struct CurlData;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlResponse
    {
    public:

        CurlResponse();
        virtual         ~CurlResponse();

        int             getResponseCode()   const;
        void*           getCaller()         const;

        int             getSize()           const;
        void*           getData()           const;
        UserData*       getUserData()       const;

    private:

        friend class    CurlWrapper;

        int             _responseCode;
        int             _curlCode;
        void*           _caller;

        std::string     _error;

        CurlData*       _data;
        UserData*       _userData;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_RESPONSE_H_