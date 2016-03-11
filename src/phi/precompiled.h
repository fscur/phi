#pragma once
#ifdef MSVC
#pragma message ("Compiling precompiled headers")
#else
#pragma message "Compiling precompiled headers"
#endif

#ifdef MSVC
#pragma warning(disable : 4800)
#pragma warning(push, 3)
#endif

#include <algorithm>
#include <cmath>
////#include <codecvt>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <future>
#include <functional>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <exception>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <objbase.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#elif APPLE
#include <OpenGL\gl3.h>
#endif

#include <gl\glew.h>
#include <gl\wglew.h>
//
////#include <bullet\btBulletDynamicsCommon.h>
//
#define GLM_FORCE_RADIANS
#define GLM_FORCE_CXX11

#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_precision.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\norm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\vector_angle.hpp>

#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

#include <freeimage\FreeImage.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef MSVC
#pragma warning(pop)
#endif
