#pragma once

#ifdef _WIN32
    #ifdef layers_EXPORTS
        #define LAYERS_API __declspec(dllexport)
    #else
        #define LAYERS_API __declspec(dllimport)
    #endif
#else
    #define LAYERS_API
#endif
