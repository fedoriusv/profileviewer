#include "App.h"

#include "ProfileManager.h"
#include "Logger.h"

using namespace pv;

App::App(int& argc, wchar_t** argv)
: _execute(true)
{
    _profile = new ProfileManager();
}

App::~App()
{
    if (_profile)
    {
        delete _profile;
        _profile = nullptr;
    }
}

bool App::isExecute() const
{
    return _execute;
}

void App::terminate()
{
    _execute = false;
}

int App::exec()
{
    App::init();

    while (App::isExecute())
    {
        App::run();
    }

    delete this;
    return 0;
}

void App::init()
{
    LOG_INFO("Parsing confing file: %s", k_config.c_str());
    if (!_profile->parseConfig(k_config))
    {
        LOG_ERROR("Can not parse config file: %s", k_config.c_str());
        std::system("pause");
        App::terminate();
    }

    LOG_INFO("Parsing credential list: %s", k_credentials.c_str());
    if (!_profile->parseCredentialList(k_credentials))
    {
        LOG_ERROR("Can not parse credential file: %s", k_credentials.c_str());
        std::system("pause");
        App::terminate();
    }
}

void App::run()
{
    _profile->update();
}