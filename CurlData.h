#ifndef _PV_CURL_DATA_H_
#define _PV_CURL_DATA_H_

#include "common.h"

namespace pv
{
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