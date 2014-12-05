// profilemanager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "App.h"


int _tmain(int argc, _TCHAR* argv[])
{
    pv::App* app = new pv::App(argc, argv);

    return app->exec();
}

