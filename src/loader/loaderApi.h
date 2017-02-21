#pragma once

#ifdef _WIN32
#ifdef loader_EXPORTS
    #define LOADER_API __declspec(dllexport)
#else
    #define LOADER_API __declspec(dllimport)
#endif
#else
    #define LOADER_API
#endif
