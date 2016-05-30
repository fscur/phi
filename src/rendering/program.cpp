#include <precompiled.h>
#include "program.h"
#include "glError.h"
#include <core\invalidInitializationException.h>

namespace phi
{
    string program::shadersPath = "";

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

    void program::add(shader * shader)
    {
        glAttachShader(_id, shader->getId());
        glError::check();

        //initializeAttributes();

        glLinkProgram(_id);
        glError::check();

#if _DEBUG
        validate(); //TODO: throw exception
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
            std::cout << "Program " << _id << " link info:\n" << buffer << std::endl;

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

    void program::initializeAttributes()
    {
        for (uint i = 0; i < _attributes.size(); ++i)
        {
            glBindAttribLocation(_id, i, _attributes[i].c_str());
            glError::check();
        }
    }

    void program::addUniform(uint location, string name)
    {
        _uniformsNames[location] = name;
        createUniform(location, name);
    }

    void program::setUniform(uint location, texture* value, GLuint index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glError::check();

        glBindTexture(value->type, value->id);
        glError::check();

        glUniform1i(_uniforms[location], index);
        glError::check();
    }

    void program::setUniform(uint location, GLuint value)
    {
        glUniform1i(_uniforms[location], value);
        glError::check();
    }

    void program::setUniform(uint location, GLfloat value)
    {
        glUniform1f(_uniforms[location], value);
        glError::check();
    }

    void program::setUniform(uint location, color value)
    {
        glUniform4f(_uniforms[location], value.r, value.g, value.b, value.a);
        glError::check();
    }

    void program::setUniform(uint location, mat3 value)
    {
        glUniformMatrix3fv(_uniforms[location], 1, GL_FALSE, &value[0][0]);
        glError::check();
    }

    void program::setUniform(uint location, mat4 value)
    {
        glUniformMatrix4fv(_uniforms[location], 1, GL_FALSE, &value[0][0]);
        glError::check();
    }

    void program::setUniform(uint location, vec2 value)
    {
        glUniform2f(_uniforms[location], value.x, value.y);
        glError::check();
    }

    void program::setUniform(uint location, vec3 value)
    {
        glUniform3f(_uniforms[location], value.x, value.y, value.z);
        glError::check();
    }

    void program::setUniform(uint location, vec4 value)
    {
        glUniform4f(_uniforms[location], value.x, value.y, value.z, value.w);
        glError::check();
    }

    void program::setUniform(uint location, bool value)
    {
        glUniform1f(_uniforms[location], value ? 1.0f : 0.0f);
        glError::check();
    }

    void program::setUniform(uint location, int value)
    {
        glUniform1i(_uniforms[location], value);
        glError::check();
    }

    void program::setUniform(uint location, GLuint64 value)
    {
        glUniformHandleui64ARB(_uniforms[location], value);
        glError::check();
    }

    void program::setUniform(uint location, vector<GLint> value)
    {
        glUniform1iv(_uniforms[location], static_cast<GLsizei>(value.size()), &value[0]);
        glError::check();
    }

    void program::setUniform(uint location, vector<GLuint64> value)
    {
        glUniformHandleui64vARB(_uniforms[location], static_cast<GLsizei>(value.size()), value.data());
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

        bool result = validate();

        if (result)
        {
            for (auto& pair : _uniformsNames)
            {
                createUniform(pair.first, pair.second);
            }
        }

        return result;
    }

    bool program::canCompile()
    {
        bool result = false;

        for (auto shader : _shaders)
            result &= shader->compile();

        return result;
    }
}