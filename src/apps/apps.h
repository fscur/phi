#pragma once
#ifdef MSVC
    #ifdef APPS_EXPORTS
        #define APPS_API __declspec(dllexport)
    #else
        #define APPS_API __declspec(dllimport)
    #endif
#else
    #define APPS_API
#endif