#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "shader.h"
#include "buffer.h"
#include "programCompilationResult.h"

namespace phi
{
    struct uniform
    {
        GLint location;
        string name;
    };

    class program
    {
    private:
        uint _id;
        vector<shader*> _shaders;

        map<uint, GLuint> _uniforms;
        map<uint, string> _uniformsNames;
        vector<string> _attributes;
        unordered_map<GLuint, buffer*> _buffers;

    private:
        bool validate();
        GLint getBufferLocation(const buffer* const buffer) const;

    public:
        RENDERING_API program();
        RENDERING_API ~program();

        RENDERING_API void addShader(shader* shader);
        RENDERING_API void addBuffer(buffer* buffer);
        RENDERING_API void addAttribute(const string& attribute);

        RENDERING_API void addUniform(uint location, string name);
        RENDERING_API void setUniform(uint location, GLuint value);
        RENDERING_API void setUniform(uint location, GLfloat value);
        RENDERING_API void setUniform(uint location, color value);
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

        RENDERING_API void link();
        RENDERING_API void bind();
        RENDERING_API void unbind();
        RENDERING_API programCompilationResult compile();

        GLuint getId() const { return _id; }
    };
}