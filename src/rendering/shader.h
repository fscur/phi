#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\color.h>
#include <core\size.h>
#include "texture.h"

namespace phi
{
    class shader
    {
    private:
        GLuint _programId;
        GLuint _vertexShaderId;
        GLuint _fragmentShaderId;

        string _vertexFile;
        string _fragmentFile;

        bool _initialized;

        map<uint, GLuint> _uniforms;
        vector<string> _attributes;

    private:
        string loadShaderFile(string fileName);
        bool validateShader(GLuint shader, const string& file = 0);
        bool validateProgram(GLuint program);

    public:
        RENDERING_API shader(
            string vertFile, 
            string fragFile, 
            const vector<string>& attributes);

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

        GLuint getId() const { return _programId; }
    };
}