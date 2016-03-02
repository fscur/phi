#pragma once

#ifdef WIN32
    #ifdef SCENES_EXPORTS
        #define SCENES_API __declspec(dllexport)
    #else
        #define SCENES_API __declspec(dllimport)
    #endif
#else
    #define SCENES_API
#endif
