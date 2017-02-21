#pragma once

#ifdef _WIN32
#ifdef io_EXPORTS
    #define IO_API __declspec(dllexport)
#else
    #define IO_API __declspec(dllimport)
#endif
#else
    #define IO_API
#endif
