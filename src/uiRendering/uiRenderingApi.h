#pragma once

#ifdef _WIN32
    #ifdef uiRendering_EXPORTS
        #define UI_RENDERING_API __declspec(dllexport)
    #else
        #define UI_RENDERING_API __declspec(dllimport)
    #endif
#else
    #define UI_RENDERING_API
#endif
