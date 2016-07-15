#pragma once
#ifdef _WIN32
#ifdef diagnostic_EXPORTS
    #define DIAGNOSTIC_API __declspec(dllexport)
#else
    #define DIAGNOSTIC_API __declspec(dllimport)
#endif
#else
    #define DIAGNOSTIC_API
#endif
