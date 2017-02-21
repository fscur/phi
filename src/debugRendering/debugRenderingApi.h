#pragma once

#ifdef _WIN32
    #ifdef debugRendering_EXPORTS
        #define DEBUG_RENDERING_API __declspec(dllexport)
    #else
        #define DEBUG_RENDERING_API __declspec(dllimport)
    #endif
#else
    #define DEBUG_RENDERING_API
#endif
