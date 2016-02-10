#ifndef _PHI_DIAGNOSTICS_H_
#define _PHI_DIAGNOSTICS_H_

#ifdef WIN32
    #ifdef DIAGNOSTICS_EXPORTS
        #define DIAGNOSTICS_API __declspec(dllexport)
    #else
        #define DIAGNOSTICS_API __declspec(dllimport)
    #endif
#else
    #define DIAGNOSTICS_API
#endif

#endif