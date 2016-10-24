#pragma once

#ifdef _WIN32
#ifdef data_EXPORTS
    #define DATA_API __declspec(dllexport)
#else
    #define DATA_API __declspec(dllimport)
#endif
#else
    #define DATA_API
#endif