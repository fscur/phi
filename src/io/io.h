#pragma once

#ifdef MSVC
#ifdef IO_EXPORTS
#define IO_API __declspec(dllexport)
#else
#define IO_API __declspec(dllimport)
#endif
#else
#define IO_API
#endif
