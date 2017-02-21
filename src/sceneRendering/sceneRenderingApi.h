#pragma once

#ifdef _WIN32
    #ifdef sceneRendering_EXPORTS
        #define SCENE_RENDERING_API __declspec(dllexport)
    #else
        #define SCENE_RENDERING_API __declspec(dllimport)
    #endif
#else
    #define SCENE_RENDERING_API
#endif
