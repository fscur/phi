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
        RENDERING_API void initAttribs();
        RENDERING_API void bind();
        RENDERING_API void unbind();
        RENDERING_API void release();

        GLuint getId() const { return _id; }

        void setUniform(uint location, GLuint value) { glUniform1i(_uniforms[location], value); }
        void setUniform(uint location, GLfloat value) { glUniform1f(_uniforms[location], value); }
        void setUniform(uint location, color value) { glUniform4f(_uniforms[location], value.r, value.g, value.b, value.a); }
        void setUniform(uint location, sizef value) { glUniform3f(_uniforms[location], value.w, value.h, value.d); }
        void setUniform(uint location, mat3 value) { glUniformMatrix3fv(_uniforms[location], 1, GL_FALSE, &value[0][0]); }
        void setUniform(uint location, mat4 value) { glUniformMatrix4fv(_uniforms[location], 1, GL_FALSE, &value[0][0]); }
        void setUniform(uint location, vec2 value) { glUniform2f(_uniforms[location], value.x, value.y); }
        void setUniform(uint location, vec3 value) { glUniform3f(_uniforms[location], value.x, value.y, value.z); }
        void setUniform(uint location, vec4 value) { glUniform4f(_uniforms[location], value.x, value.y, value.z, value.w); }
        void setUniform(uint location, bool value) { glUniform1f(_uniforms[location], value ? 1.0f : 0.0f); }
        void setUniform(uint location, int value) { glUniform1i(_uniforms[location], value); }
        void setUniform(uint location, GLuint64 value) { glUniformHandleui64ARB(_uniforms[location], value); }

        void setUniform(uint location, vector<GLint> value)
        {
            glUniform1iv(_uniforms[location], static_cast<GLsizei>(value.size()), value.data());
        }

        void setUniform(uint location, vector<GLuint64> value)
        {
            glUniformHandleui64vARB(_uniforms[location], static_cast<GLsizei>(value.size()), value.data());
        }
    };
}