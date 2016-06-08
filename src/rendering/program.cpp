﻿#include <precompiled.h>
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
            shader->getOnIsDirtyChanged()->unassign(_dirtyShadersTokens[shader]);
        }

        glDeleteProgram(_id);
        glError::check();
    }

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

    void program::addBuffer(buffer* buffer)
    {
        auto location = getBufferLocation(buffer);

        if (location == -1)
            throw argumentException("<program::addBuffer> Buffer (" + buffer->getName() + ") not found.");

        _buffers[location] = buffer;
    }

    void program::addAttribute(const string& attribute)
    {
        _attributes.push_back(attribute);
    }

    void program::addUniform(uint location, string name)
    {
        _uniformsNames[location] = name;
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

    void program::link()
    {
        glLinkProgram(_id);
        glError::check();

#if _DEBUG
        if (!validate())
            throw invalidInitializationException("<program::link>: program not linked; ");
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

    GLint program::getBufferLocation(const buffer * const buffer) const
    {
        GLenum programInterface = 0;
        switch (buffer->getTarget())
        {
        case bufferTarget::shader:
            programInterface = GL_SHADER_STORAGE_BLOCK;
            break;
        case bufferTarget::uniform:
            programInterface = GL_UNIFORM_BLOCK;
            break;
        default:
            break;
        }

        GLint numUniforms = 0;
        glGetProgramInterfaceiv(_id, programInterface, GL_ACTIVE_RESOURCES, &numUniforms);
        auto index = glGetProgramResourceIndex(_id, programInterface, buffer->getName().c_str());

        const GLenum props[1] = { GL_BUFFER_BINDING };
        GLint values[1] = {-1};
        glGetProgramResourceiv(_id, programInterface, index, 1, props, 1, NULL, values);

        return values[0];
    }

    inline void program::bind()
    {
        glUseProgram(_id);

        for (auto& pair : _buffers)
            pair.second->bindBufferBase(pair.first);

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

        if (canCompile())
        {
            glLinkProgram(_id);
            glError::check();

            return validate();
        }

        return false;
    }

    bool program::canCompile()
    {
        bool result = true;

        for (auto shader : _shaders)
            result &= shader->compile();

        return result;
    }
}