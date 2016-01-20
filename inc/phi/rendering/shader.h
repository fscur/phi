#ifndef _PHI_SHADER_H_
#define _PHI_SHADER_H_

#include <phi/core/color.h>

#include "texture.h"
#include "textureArray.h"

#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    class shader
    {
    private:
        GLuint _id;

        GLuint _vertexShader;
        GLuint _fragmentShader;

        bool _initialized;

        std::string _name;
        std::string _vertFile;
        std::string _fragFile;

        std::map<uint, GLuint> _uniforms;

        std::vector<std::string> _attributes;
        GLuint _textureCount;

    private:
        static std::string loadShaderFile(const std::string fileName);
        static bool validateShader(GLuint shader, const std::string file = 0);
        static bool validateProgram(GLuint program);

    protected:
        shader() {}

    public:
        RENDERING_API shader(std::string name, std::string vertFile, std::string fragFile, std::vector<std::string> attributes);
        RENDERING_API ~shader();

        RENDERING_API GLuint getId() const { return _id; }
        RENDERING_API std::string getName() const { return _name; }

        RENDERING_API bool init();

        RENDERING_API void addUniform(uint location, std::string name);
        RENDERING_API void setUniform(uint location, GLuint value) { glUniform1i(_uniforms[location], value); }
        RENDERING_API void setUniform(uint location, GLfloat value) { glUniform1f(_uniforms[location], value); }
        RENDERING_API void setUniform(uint location, color value) { glUniform4f(_uniforms[location], value.r, value.g, value.b, value.a); }
        RENDERING_API void setUniform(uint location, tsize<GLfloat> value) { glUniform3f(_uniforms[location], value.w, value.h, value.d); }
        RENDERING_API void setUniform(uint location, mat3 value) { glUniformMatrix3fv(_uniforms[location], 1, GL_FALSE, &value[0][0]); }
        RENDERING_API void setUniform(uint location, mat4 value) { glUniformMatrix4fv(_uniforms[location], 1, GL_FALSE, &value[0][0]); }
        RENDERING_API void setUniform(uint location, vec2 value) { glUniform2f(_uniforms[location], value.x, value.y); }
        RENDERING_API void setUniform(uint location, vec3 value) { glUniform3f(_uniforms[location], value.x, value.y, value.z); }
        RENDERING_API void setUniform(uint location, vec4 value) { glUniform4f(_uniforms[location], value.x, value.y, value.z, value.w); }
        RENDERING_API void setUniform(uint location, bool value) { glUniform1f(_uniforms[location], value ? 1.0f : 0.0f); }
        RENDERING_API void setUniform(uint location, int value) { glUniform1i(_uniforms[location], value); }
        RENDERING_API void setUniform(uint location, texture* value, GLuint index);
        RENDERING_API void setUniform(uint location, textureArray* textureArray);
        RENDERING_API void setUniform(uint location, GLuint64 value) { glUniform1ui64NV(_uniforms[location], value); }

        RENDERING_API void initAttribs();

        RENDERING_API void bind();
        RENDERING_API void unbind();

        RENDERING_API void release();
    };
}

#endif