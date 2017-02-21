#pragma once
#ifdef _WIN32
#ifdef common_EXPORTS
#define COMMON_API __declspec(dllexport)
#else
#define COMMON_API __declspec(dllimport)
#endif
#else
#define CORE_API
#endif
