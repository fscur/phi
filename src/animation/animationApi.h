#pragma once

#ifdef _WIN32
#ifdef animation_EXPORTS
#define ANIMATION_API __declspec(dllexport)
#else
#define ANIMATION_API __declspec(dllimport)
#endif
#else
#define ANIMATION_API
#endif

