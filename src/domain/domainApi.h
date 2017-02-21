#pragma once

#ifdef _WIN32
#ifdef domain_EXPORTS
#define DOMAIN_API __declspec(dllexport)
#else
#define DOMAIN_API __declspec(dllimport)
#endif
#else
#define DOMAIN_API
#endif

