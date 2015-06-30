#ifndef _PV_USER_PROFILE_H_
#define _PV_USER_PROFILE_H_

#include "common.h"

namespace pv
{
    class UserProfile
    {
    public:

        UserProfile();
        ~UserProfile();

        bool            read(const std::string& data);
        bool            write(std::string& data);

    private:

        bool            decompress(const std::string& inData, std::string& outData);
        bool            compress(const std::string& inData, std::string& outData);

        bool            decode(const std::string& inData, std::string& outData);
        bool            encode(const std::string& inData, std::string& outData);

        int             _compressSize;
        int             _rawSize;
        std::string     _data;

    };
}

#endif //_PV_USER_PROFILE_H_