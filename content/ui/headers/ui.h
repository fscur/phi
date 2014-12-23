#ifdef WIN32
    #ifdef UI_EXPORTS
        #define UI_API __declspec(dllexport)
    #else
        #define UI_API __declspec(dllimport)
    #endif
#else
    #define UI_API
#endif

#define GLM_FORCE_RADIANS