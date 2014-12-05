#ifndef _PV_APP_H_
#define _PV_APP_H_

#include "common.h"

namespace pv
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class ProfileManager;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class App
    {
    public:

        App(int& argc, wchar_t** argv);
        ~App();

        int                 exec();
        bool                isExecute() const;

        void                terminate();

    private:

        void                init();
        void                run();

        bool                _execute;

        ProfileManager*     _profile;

        const std::string   k_config        = "config.json";
        const std::string   k_credentials   = "credentials_to_view.txt";

    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif //_PV_APP_H_