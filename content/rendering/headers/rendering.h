#ifndef _PHI_RENDERING_H_
#define _PHI_RENDERING_H_

#ifdef WIN32
    #ifdef RENDERING_EXPORTS
        #define RENDERING_API __declspec(dllexport)
    #else
        #define RENDERING_API __declspec(dllimport)
    #endif
#else
    #define RENDERING_API
#endif

#define GLM_FORCE_RADIANS

#endif