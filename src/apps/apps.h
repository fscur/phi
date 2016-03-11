#pragma once
#ifdef _WIN32
    #ifdef apps_EXPORTS
        #define APPS_API __declspec(dllexport)
    #else
        #define APPS_API __declspec(dllimport)
    #endif
#else
    #define APPS_API
#endif