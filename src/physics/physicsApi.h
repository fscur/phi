#pragma once

#ifdef _WIN32
#ifdef physics_EXPORTS
    #define PHYSICS_API __declspec(dllexport)
#else
    #define PHYSICS_API __declspec(dllimport)
#endif
#else
    #define PHYSICS_API
#endif