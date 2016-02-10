#ifndef _PHI_GL_H_
#define _PHI_GL_H_

#include <core\globals.h>

#include <vector>
#include <map>
#include <string>

#include <GL\glew.h>

namespace phi
{
    namespace gl
    {
        static std::map<std::string, bool> extensions;

        static void initExtensions()
        {
            const GLubyte* extension = nullptr;
            auto i = 0;

            extension = glGetStringi(GL_EXTENSIONS, i++);
            std::vector<std::string> glExtensions;

            while (extension != NULL)
            {
                glExtensions.push_back(std::string((char*)extension));
                extension = glGetStringi(GL_EXTENSIONS, i++);
            }

            std::vector<std::string> phiExtensions;
            phiExtensions.push_back("GL_ARB_bindless_texture");

            for (auto &extension : phiExtensions)
                extensions[extension] = phi::contains(glExtensions, extension);
        }
    }
}

#endif
