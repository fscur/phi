#pragma once

#ifdef _WIN32
    #ifdef application_EXPORTS
        #define APPLICATION_API __declspec(dllexport)
    #else
        #define APPLICATION_API __declspec(dllimport)
    #endif
#else
    #define APPLICATION_API
#endif