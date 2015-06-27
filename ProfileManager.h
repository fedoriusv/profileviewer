#ifndef _PV_PROFILE_MANAGER_H_
#define _PV_PROFILE_MANAGER_H_

#include "common.h"

namespace pv
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class CurlWrapper;
    class CurlResponse;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef std::function<void(const CurlResponse*)> callback;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class ProfileManager
    {
    public:

        enum EProfileState
        {
            eErrorState,
            eWaitState,
            eGetConfigLinksState,
            eGetServiceLinksState,
            eGetAccessTokeState,
            eGetUserStorageState,
        };

        struct GameData
        {
            std::string     _pid;
            std::string     _ggi;
            std::string     _version;
            std::string     _platform;
            std::string     _download_source;

            std::string     _link;

            std::string     _user;
            std::string     _password;
            std::string     _profile;
        };

        ProfileManager();
        ~ProfileManager();

        bool                        parseConfig(const std::string& file);
        bool                        parseCredentialList(const std::string& file);

        const std::vector<std::string>& getCredentialList() const;

        bool                        getUserProfile();
        
        void                        update();

    private:

        std::string                 getClientID() const;
        const std::string&          getAccessToken() const;

        void                        setState(EProfileState state);

        void                        getConfigLinks(const std::string& clientId, callback callback, void* caller, int retry = 3);
        void                        getAccessToken(const std::string& user, const std::string& password, const std::string& clientId, callback callback, void* caller, int retry = 3);
        void                        getUserStorage(const std::string& token, callback callback, void* caller, int retry = 3);


        std::string                 getServiceLink(const std::string& service);


        bool                        parseConfigLinksResponse(const std::string& data);
        bool                        parseAccessTokenResponse(const std::string& data);


        CurlWrapper*                _curl;
        GameData                    _gameData;

        EProfileState               _state;

        std::vector<std::string>    _credentials;

        std::string                 _configLink;
        std::string                 _accessToken;


        void                        responseCallback(const CurlResponse* response);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_PROFILE_MANAGER_H_