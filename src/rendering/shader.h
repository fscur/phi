#pragma once
#include <phi.h>
#include "rendering.h"
#include <core\color.h>
#include <core\size.h>
#include "texture.h"

namespace phi
{
    class shader
    {
    private:
        GLuint _id;

        GLuint _vertexShader;
        GLuint _fragmentShader;

        bool _initialized;

        string _vertFile;
        string _fragFile;

        map<uint, GLuint> _uniforms;

        vector<string> _attributes;
        GLuint _textureCount;

    private:
        string loadShaderFile(const string fileName);
        bool validateShader(GLuint shader, const string file = 0);
        bool validateProgram(GLuint program);

    public:
        RENDERING_API shader(string vertFile, string fragFile, vector<string> attributes);

        RENDERING_API bool init();
        RENDERING_API void addUniform(uint location, string name);
        RENDERING_API void setUniform(uint location, texture* value, GLuint index);
        RENDERING_API void setUniform(uint location, GLuint value);
        RENDERING_API void setUniform(uint location, GLfloat value);
        RENDERING_API void setUniform(uint location, color value);
        RENDERING_API void setUniform(uint location, sizef value);
        RENDERING_API void setUniform(uint location, mat3 value);
        RENDERING_API void setUniform(uint location, mat4 value);
        RENDERING_API void setUniform(uint location, vec2 value);
        RENDERING_API void setUniform(uint location, vec3 value);
        RENDERING_API void setUniform(uint location, vec4 value);
        RENDERING_API void setUniform(uint location, bool value);
        RENDERING_API void setUniform(uint location, int value);
        RENDERING_API void setUniform(uint location, GLuint64 value);
        RENDERING_API void setUniform(uint location, vector<GLint> value);
        RENDERING_API void setUniform(uint location, vector<GLuint64> value);

        RENDERING_API void initAttribs();
        RENDERING_API void bind();
        RENDERING_API void unbind();
        RENDERING_API void release();

        GLuint getId() const { return _id; }
    };
}