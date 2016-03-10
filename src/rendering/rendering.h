#pragma once

#ifdef MSVC
    #ifdef RENDERING_EXPORTS
        #define RENDERING_API __declspec(dllexport)
    #else
        #define RENDERING_API __declspec(dllimport)
    #endif
#else
    #define RENDERING_API
#endif