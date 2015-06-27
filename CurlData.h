#ifndef _PV_CURL_DATA_H_
#define _PV_CURL_DATA_H_

#include "common.h"

namespace pv
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct CurlData
    {
        CurlData();
        ~CurlData();

        void    addHeader(const std::string& header);
        void    reset();
        void    swap(CurlData* data);
        void    copy(size_t size, void* data);

        std::vector<std::string>    _headerLines;

        int                         _size;
        void*                       _data;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct UserData
    {
        UserData()
        : _value(0)
        {}

        int _value;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_CURL_DATA_H_