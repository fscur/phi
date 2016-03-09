#pragma once
#ifdef MSVC
#ifdef DIAGNOSTICS_EXPORTS
#define DIAGNOSTICS_API __declspec(dllexport)
#else
#define DIAGNOSTICS_API __declspec(dllimport)
#endif
#else
#define DIAGNOSTICS_API
#endif