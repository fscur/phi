#ifndef _SCENES_H_
#define _SCENES_H_

#ifdef WIN32
    #ifdef SCENES_EXPORTS
        #define SCENES_API __declspec(dllexport)
    #else
        #define SCENES_API __declspec(dllimport)
    #endif
#else
    #define SCENES_API
#endif

#endif