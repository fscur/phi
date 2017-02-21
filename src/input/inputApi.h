#pragma once
#ifdef _WIN32
#ifdef input_EXPORTS
    #define INPUT_API __declspec(dllexport)
#else
    #define INPUT_API __declspec(dllimport)
#endif
#else
    #define INPUT_API
#endif
