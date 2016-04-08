#pragma once
#ifdef _WIN32
#ifdef diagnostics_EXPORTS
#define DIAGNOSTICS_API __declspec(dllexport)
#else
#define DIAGNOSTICS_API __declspec(dllimport)
#endif
#else
#define DIAGNOSTICS_API
#endif
