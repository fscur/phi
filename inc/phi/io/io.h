#ifndef _PHI_IO_H_
#define _PHI_IO_H_

#ifdef WIN32
#ifdef IO_EXPORTS
#define IO_API __declspec(dllexport)
#else
#define IO_API __declspec(dllimport)
#endif
#else
#define IO_API
#endif

#define GLM_FORCE_RADIANS

#endif