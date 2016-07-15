#pragma once

#ifdef _WIN32
    #ifdef context_EXPORTS
        #define CONTEXT_API __declspec(dllexport)
    #else
        #define CONTEXT_API __declspec(dllimport)
    #endif
#else
    #define CONTEXT_API
#endif
