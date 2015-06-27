#include "ProfileManager.h"

#include "CurlWrapper.h"
#include "Logger.h"

#include "json/json.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace pv;

ProfileManager::ProfileManager()
: _state(eGetConfigLinks)
, _configLink("")
{
    _curl = new CurlWrapper();
}

ProfileManager::~ProfileManager()
{
    if (_curl)
    {
        delete _curl;
        _curl = nullptr;
    }

    _credentials.clear();
}

const std::vector<std::string>& ProfileManager::getCredentialList() const
{
    return _credentials;
}

void ProfileManager::setState(EProfileState state)
{
    _state = state;
}

void ProfileManager::update()
{
    _curl->update();

    switch (_state)
    {
        case eErrorState:
        {
            LOG_ERROR("Error State");
            break;
        }

        case eWaitState:
            break;

        case eGetConfigLinks:
        {
            getConfigLinks(getClientID(), std::bind(&ProfileManager::responseCallback, this, std::placeholders::_1), this);
            break;
        }


        case eGetAccessTokeState:
        {
            getAccessToken(_gameData._user, _gameData._password, getClientID(), std::bind(&ProfileManager::responseCallback, this, std::placeholders::_1), this);
            break;
        }

        default:
            break;

    }
}

bool ProfileManager::parseConfig(const std::string& file)
{
    std::ifstream sfile(file.c_str(), std::ifstream::binary);
    if (sfile)
    {
        sfile.seekg(0, sfile.end);
        int length = sfile.tellg();
        sfile.seekg(0, sfile.beg);

        std::string data;
        data.resize(length);

        sfile.read(&data[0], length);

        sfile.close();

        Json::Reader reader;
        Json::Value value;
        bool succes = reader.parse(data, value);

        if (!succes)
        {
            return false;
        }

        if (!value.isMember("game"))
        {
            return false;
        }
        _gameData._ggi = value["game"].get("ggi", "").asString();
        _gameData._pid = value["game"].get("pid", "").asString();
        _gameData._version = value["game"].get("version", "").asString();
        _gameData._platform = value["game"].get("platform", "").asString();
        _gameData._download_source = value["game"].get("download_source", "").asString();

        if (!value.isMember("link"))
        {
            return false;
        }
        _gameData._link = value.get("link", "").asString();

        if (!value.isMember("superuser"))
        {
            return false;
        }
        _gameData._user = value.get("superuser", "").asString();

        if (!value.isMember("password"))
        {
            return false;
        }
        _gameData._password = value.get("password", "").asString();

        if (!value.isMember("profile"))
        {
            return false;
        }
        _gameData._profile = value.get("profile", "").asString();

        return true;
    }

    return false;
}

bool ProfileManager::parseCredentialList(const std::string& file)
{
    std::ifstream sfile(file.c_str(), std::ifstream::binary);
    if (sfile)
    {
        sfile.seekg(0, sfile.end);
        int length = sfile.tellg();
        sfile.seekg(0, sfile.beg);

        sfile.seekg(0, sfile.beg);
        while (sfile.tellg() < length)
        {
            std::string str;
            std::getline(sfile, str);

            size_t pos = str.find("#");
            if (pos != 0 && !str.empty())
            {
                if (pos != std::string::npos)
                {
                    str.resize(pos);
                }

                str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
                _credentials.push_back(str);
            }
        }

        sfile.close();

        return true;
    }

    return false;
}

std::string ProfileManager::getClientID() const
{
    std::string client("");

    client.append(_gameData._pid);
    client.append(":");
    client.append(_gameData._ggi);
    client.append(":");
    client.append(_gameData._version);
    client.append(":");
    client.append(_gameData._platform);
    client.append(":");
    client.append(_gameData._download_source);

    return client;
}

void ProfileManager::getConfigLinks(const std::string& clientId, callback callback, void* caller)
{
    if (clientId.empty())
    {
        LOG_ERROR("ProfileManager::getServiceLinks: Empty client id");
        ProfileManager::setState(EProfileState::eErrorState);
    }

    if (_gameData._link.empty())
    {
        LOG_ERROR("ProfileManager::getServiceLinks: Empty config link");
        ProfileManager::setState(EProfileState::eErrorState);
    }

    std::string link = _gameData._link;
    link.append(clientId);

    pv::RequestSevice* req = new pv::RequestSevice(pv::RequestSevice::eGet);
    req->setUrl(link);
    req->setCallback(callback, caller);
    req->setService(ERequestSevice::eGetConfigLinks);

    LOG_GEBUG("ProfileManager::getServiceLinks: Set ServiceLinks request %s ", link.c_str());
    _curl->addAsyncRequest(req);

    ProfileManager::setState(EProfileState::eWaitState);
}

bool ProfileManager::parseConfigLinksResponse(const std::string& data)
{
    Json::Reader reader;
    Json::Value value;
    bool succes = reader.parse(data, value);

    if (!succes)
    {
        return false;
    }

    if (!value.isMember("pandora"))
    {
        return false;
    }

    _configLink = value.get("pandora", "").asString();
    LOG_INFO("ProfileManager::parseConfigLinksResponse: config link: %s", _configLink.c_str());

    return true;
}

std::string ProfileManager::getServiceLink(const std::string& service)
{
    if (_configLink.empty())
    {
        LOG_ERROR("ProfileManager::getServiceLink: ServiceLink is Empty");
        ProfileManager::setState(EProfileState::eErrorState);

        return "";
    }

    std::string link = _configLink;
    link.append("/locate?service=");
    link.append(service);

    pv::RequestSevice* req = new pv::RequestSevice(pv::RequestSevice::eGet);
    req->setUrl(link);
    req->setService(ERequestSevice::eGetSevriceLink);

    LOG_GEBUG("ProfileManager::getServiceLink: Set ServiceLinks request %s ", link.c_str());

    const CurlResponse* res = _curl->addSyncRequest(req);
    if (!res)
    {
        LOG_ERROR("ProfileManager::getServiceLink: Invalid Response");
        ProfileManager::setState(EProfileState::eErrorState);

        return "";
    }

    int code = res->getResponseCode();
    if (code == EResopnceCode::eSuccess && res->getSize() > 0)
    {
        std::string data(reinterpret_cast<const char*>(res->getData()), res->getSize());

        return data;
    }
    else
    {
        LOG_ERROR("ProfileManager::getServiceLink: Invalid Response code %d", code);
        ProfileManager::setState(EProfileState::eErrorState);

        return "";
    }

}

void ProfileManager::getAccessToken(const std::string& user, const std::string& password, const std::string& clientId, callback callback, void* caller)
{
    if (user.empty() || password.empty() || clientId.empty())
    {
        LOG_ERROR("ProfileManager::getAccessToken: Empty input data user: %s, pass: %d, clientid: %s", user.c_str(), password.c_str(), clientId.c_str());
        ProfileManager::setState(EProfileState::eErrorState);
    }


    std::string address = ProfileManager::getServiceLink("auth");
    LOG_INFO("ProfileManager::getAccessToken: Get auth link: %s", address.c_str());

    if (address.empty())
    {
        LOG_ERROR("ProfileManager::getAccessToken: Empty link address");
        ProfileManager::setState(EProfileState::eErrorState);
    }

    std::string link = "https://";
    link.append(address);
    link.append("/authorize");

    pv::RequestSevice* req = new pv::RequestSevice(pv::RequestSevice::ePost);
    req->setUrl(link);

    req->setCallback(callback, caller);
    req->setService(ERequestSevice::eGetAccessToken);

    req->addParam("client_id", clientId);
    req->addParam("username", user);
    req->addParam("password", password);
    req->addParam("scope", "config storage");

    //req->addParam("scope", "storage storage_restricted storage_admin");
    //req->addParam("access_token_only", "true");

    //req->addHeaders("Content-Type", "application/x-www-form-urlencoded");

    LOG_GEBUG("ProfileManager::getServiceLinks: Set ServiceLinks request %s ", link.c_str());
    //_curl->addAsyncRequest(req);

    const CurlResponse* res = _curl->addSyncRequest(req);
    int code = res->getResponseCode();
    std::string data(reinterpret_cast<const char*>(res->getData()), res->getSize());
    int a = 0;
}

void ProfileManager::responseCallback(const CurlResponse* response)
{
    if (!response && this != static_cast<ProfileManager*>(response->getCaller()))
    {
        LOG_ERROR("ProfileManager::responseCallback: Invalid response");
        ProfileManager::setState(EProfileState::eErrorState);

        return;
    }

    int code = response->getResponseCode();
    ERequestSevice service = static_cast<const ResponseService*>(response)->getSevice();

    switch (service)
    {
        case ERequestSevice::eGetConfigLinks:
        {
            LOG_GEBUG("ProfileManager::responseCallback: Received ConfigLinks data. Code %d", code);

            if (code == EResopnceCode::eSuccess)
            {
                std::string data(reinterpret_cast<const char*>(response->getData()), response->getSize());
                
                if (ProfileManager::parseConfigLinksResponse(data))
                {
                    LOG_GEBUG("ProfileManager::responseCallback: Parse ConfigLinks data completed");
                    ProfileManager::setState(EProfileState::eGetAccessTokeState);
                }
                else
                {
                    LOG_ERROR("ProfileManager::responseCallback:  Error Parse ConfigLinks data");
                    ProfileManager::setState(EProfileState::eErrorState);
                }
            }
            else
            {
                LOG_ERROR("ProfileManager::responseCallback:  Received ConfigLinks data. Code %d", code);
                ProfileManager::setState(EProfileState::eErrorState);
            }
            break;
        }

        case ERequestSevice::eGetAccessToken:
        {
            LOG_GEBUG("ProfileManager::responseCallback: Received AccessToken data. Code %d", code);

            //if (code == EResopnceCode::eSuccess)
            {
                std::string data(reinterpret_cast<const char*>(response->getData()), response->getSize());

                int a = 0;
            }
            //else
            {
                LOG_ERROR("ProfileManager::responseCallback:  Received AccessToken data. Code %d", code);
                ProfileManager::setState(EProfileState::eErrorState);
            }
            break;
        }

        default:
            break;
    }

    delete response;
    response = nullptr;


}