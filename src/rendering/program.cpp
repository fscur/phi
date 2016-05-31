#include <precompiled.h>
#include <core\invalidInitializationException.h>

#include "program.h"
#include "glError.h"
#include "glslCompiler.h"

namespace phi
{
    program::program()
    {
        _id = glCreateProgram();
        glError::check();
    }

    program::~program()
    {
        unbind();

        for (auto shader : _shaders)
        {
            glDetachShader(_id, shader->getId());
            glError::check();

            safeDelete(shader);
        }

        glDeleteProgram(_id);
        glError::check();
    }

#ifdef _DEBUG
    map<shader*, eventToken> _dirtyShadersTokens;
#endif

    void program::addShader(shader * shader)
    {
        _shaders.push_back(shader);

#ifdef _DEBUG
        auto token = shader->getOnIsDirtyChanged()->assign([&](phi::shader* shader)
        {
            reload();
        });

        _dirtyShadersTokens[shader] = token;
#endif

        glAttachShader(_id, shader->getId());
        glError::check();
    }

    void program::addAttribute(const string& attribute)
    {
        _attributes.push_back(attribute);
    }

    void program::link()
    {
        glLinkProgram(_id);
        glError::check();

#if _DEBUG
        if (!validate())
            throw invalidInitializationException("[program::validate]: program not linked; ");
#endif
    }

    bool program::validate()
    {
        const unsigned int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetProgramInfoLog(_id, BUFFER_SIZE, &length, buffer);
        glError::check();

        if (length > 0)
            phi::debug("Program " + std::to_string(_id) + " link info:\n" + buffer);

        GLint isLinked = 0;

        glGetProgramiv(_id, GL_LINK_STATUS, &isLinked);
        glError::check();

        return isLinked == GL_TRUE;
    }

    void program::createUniform(uint location, const string & name)
    {
        _uniforms[location] = glGetUniformLocation(_id, name.c_str());
        glError::check();
    }

    void program::addUniform(uint location, string name)
    {
        _uniformsNames[location] = name;
    }

    void program::setUniform(uint location, texture* value, GLuint index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glError::check();

        glBindTexture(value->type, value->id);
        glError::check();

        glUniform1i(location, index);
        glError::check();
    }

    void program::setUniform(uint location, GLuint value)
    {
        glUniform1i(location, value);
        glError::check();
    }

    void program::setUniform(uint location, GLfloat value)
    {
        glUniform1f(location, value);
        glError::check();
    }

    void program::setUniform(uint location, color value)
    {
        glUniform4f(location, value.r, value.g, value.b, value.a);
        glError::check();
    }

    void program::setUniform(uint location, mat3 value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
        glError::check();
    }

    void program::setUniform(uint location, mat4 value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        glError::check();
    }

    void program::setUniform(uint location, vec2 value)
    {
        glUniform2f(location, value.x, value.y);
        glError::check();
    }

    void program::setUniform(uint location, vec3 value)
    {
        glUniform3f(location, value.x, value.y, value.z);
        glError::check();
    }

    void program::setUniform(uint location, vec4 value)
    {
        glUniform4f(location, value.x, value.y, value.z, value.w);
        glError::check();
    }

    void program::setUniform(uint location, bool value)
    {
        glUniform1f(location, value ? 1.0f : 0.0f);
        glError::check();
    }

    void program::setUniform(uint location, int value)
    {
        glUniform1i(location, value);
        glError::check();
    }

    void program::setUniform(uint location, GLuint64 value)
    {
        glUniformHandleui64ARB(location, value);
        glError::check();
    }

    void program::setUniform(uint location, vector<GLint> value)
    {
        glUniform1iv(location, static_cast<GLsizei>(value.size()), &value[0]);
        glError::check();
    }

    void program::setUniform(uint location, vector<GLuint64> value)
    {
        glUniformHandleui64vARB(location, static_cast<GLsizei>(value.size()), value.data());
        glError::check();
    }

    inline void program::bind()
    {
        glUseProgram(_id);
        glError::check();
    }

    inline void program::unbind()
    {
        glUseProgram(0);
        glError::check();
    }

    bool program::reload()
    {
        unbind();
        glLinkProgram(_id);
        glError::check();

        if (!validate())
            return false;

        for (auto& pair : _uniformsNames)
            createUniform(pair.first, pair.second);

        return true;
    }

    bool program::canCompile()
    {
        bool result = false;

        for (auto shader : _shaders)
            result &= shader->compile();

        return result;
    }
}