#pragma once

#ifdef _WIN32
    #ifdef scenes_EXPORTS
        #define SCENES_API __declspec(dllexport)
    #else
        #define SCENES_API __declspec(dllimport)
    #endif
#else
    #define SCENES_API
#endif
