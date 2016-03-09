#include <precompiled.h>
#include "shader.h"

namespace phi
{
    bool shader::init()
    {
        if (_initialized)
            return true;

        bool result = true;

        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        string vertText = loadShaderFile(_vertFile.c_str());
        string fragText = loadShaderFile(_fragFile.c_str());

        const char *vertexText = vertText.c_str();
        const char *fragmentText = fragText.c_str();

        glShaderSource(_vertexShader, 1, &vertexText, 0);
        glCompileShader(_vertexShader);

#if _DEBUG
        result = validateShader(_vertexShader, _vertFile.c_str());
        if (!result)
            return false;
#endif
        glShaderSource(_fragmentShader, 1, &fragmentText, 0);
        glCompileShader(_fragmentShader);

#if _DEBUG
        result = validateShader(_fragmentShader, _fragFile.c_str());
        if (!result)
            return false;
#endif

        _id = glCreateProgram();
        glAttachShader(_id, _vertexShader);
        glAttachShader(_id, _fragmentShader);
        initAttribs();

        glLinkProgram(_id);

#if _DEBUG
        result = validateProgram(_id);
        if (!result)
            return false;
#endif

        _initialized = true;

        return true;
    }

    string shader::loadShaderFile(const string fileName)
    {
        string filePath = fileName;

        string fileString;
        string line;

        std::ifstream file(filePath);

        if (file.is_open()) 
        {
            while (!file.eof()) 
            {
                getline(file, line);
                fileString.append(line);
                fileString.append("\n");
            }

            file.close();
        }

        return fileString;
    }

    bool shader::validateShader(GLuint shader, const string file)
    {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        const unsigned int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

        if (length > 0)
            std::cout << "shader " << shader << " (" << (file.c_str() ? file : "") << ") compile info:\n" << buffer << std::endl;

        return success == GL_TRUE;
    }

    bool shader::validateProgram(GLuint program)
    {
        const unsigned int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
        if (length > 0)
            std::cout << "Program " << program << " link info:\n" << buffer << std::endl;

        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

        return isLinked == GL_TRUE;
    }

    void shader::initAttribs()
    {
        for (uint i = 0; i < _attributes.size(); ++i)
            glBindAttribLocation(_id, i, _attributes[i].c_str());
    }

    void shader::addUniform(uint location, string name)
    {
        if (!_initialized)
            return;

        _uniforms[location] = glGetUniformLocation(_id, name.c_str());
    }

    void shader::setUniform(uint location, texture* value, GLuint index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(value->type, value->id);
        glUniform1i(_uniforms[location], index);
    }

    void shader::bind()
    {
        if (!_initialized)
            return;

        _textureCount = 0;
        glUseProgram(_id);
    }

    void shader::unbind()
    {
        if (!_initialized)
            return;

        glUseProgram(0);
    }

    void shader::release()
    {
        if (!_initialized)
            return;

        glDetachShader(_id, _fragmentShader);
        glDetachShader(_id, _vertexShader);

        glDeleteShader(_fragmentShader);
        glDeleteShader(_vertexShader);
        glDeleteProgram(_id);
    }
}