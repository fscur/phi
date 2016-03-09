#pragma once

#ifdef MSVC
    #ifdef SCENES_EXPORTS
        #define SCENES_API __declspec(dllexport)
    #else
        #define SCENES_API __declspec(dllimport)
    #endif
#else
    #define SCENES_API
#endif
