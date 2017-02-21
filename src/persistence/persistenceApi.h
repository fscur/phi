#pragma once

#ifdef _WIN32
#ifdef persistence_EXPORTS
#define PERSISTENCE_API __declspec(dllexport)
#else
#define PERSISTENCE_API __declspec(dllimport)
#endif
#else
#define PERSISTENCE_API
#endif

