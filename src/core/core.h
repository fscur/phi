#ifndef _PHI_CORE_H_
#define _PHI_CORE_H_

#ifdef WIN32
    #ifdef CORE_EXPORTS
        #define CORE_API __declspec(dllexport)
    #else
        #define CORE_API __declspec(dllimport)
    #endif
#else
    #define CORE_API
#endif

#define GLM_FORCE_RADIANS

#endif