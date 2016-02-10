#ifndef _PHI_LOADER_H_
#define _PHI_LOADER_H_

#ifdef WIN32
#ifdef LOADER_EXPORTS
#define LOADER_API __declspec(dllexport)
#else
#define LOADER_API __declspec(dllimport)
#endif
#else
#define LOADER_API
#endif

#endif