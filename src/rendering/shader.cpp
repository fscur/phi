#include <precompiled.h>
#include "shader.h"
#include "glError.h"

namespace phi
{
    shader::shader(string vertFile, string fragFile, vector<string> attributes) :
        _vertFile(vertFile),
        _fragFile(fragFile),
        _attributes(attributes),
        _initialized(false)
    {}

    bool shader::init()
    {
        if (_initialized)
            return true;

        bool result = true;

        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glError::check();

        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glError::check();

        string vertText = loadShaderFile(_vertFile.c_str());
        string fragText = loadShaderFile(_fragFile.c_str());

        const char *vertexText = vertText.c_str();
        const char *fragmentText = fragText.c_str();

        glShaderSource(_vertexShader, 1, &vertexText, 0);
        glError::check();

        glCompileShader(_vertexShader);
        glError::check();

#if _DEBUG
        result = validateShader(_vertexShader, _vertFile.c_str());
        if (!result)
            return false;
#endif
        glShaderSource(_fragmentShader, 1, &fragmentText, 0);
        glError::check();

        glCompileShader(_fragmentShader);
        glError::check();

#if _DEBUG
        result = validateShader(_fragmentShader, _fragFile.c_str());
        if (!result)
            return false;
#endif

        _id = glCreateProgram();
        glError::check();

        glAttachShader(_id, _vertexShader);
        glError::check();

        glAttachShader(_id, _fragmentShader);
        glError::check();

        initAttribs();

        glLinkProgram(_id);
        glError::check();

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
        glError::check();

        const unsigned int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
        glError::check();

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
        glError::check();

        if (length > 0)
            std::cout << "Program " << program << " link info:\n" << buffer << std::endl;

        GLint isLinked = 0;

        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        glError::check();

        return isLinked == GL_TRUE;
    }

    void shader::initAttribs()
    {
        for (uint i = 0; i < _attributes.size(); ++i)
        {
            glBindAttribLocation(_id, i, _attributes[i].c_str());
            glError::check();
        }
    }

    void shader::addUniform(uint location, string name)
    {
        if (!_initialized)
            return;

        _uniforms[location] = glGetUniformLocation(_id, name.c_str());
        glError::check();
    }

    void shader::setUniform(uint location, texture* value, GLuint index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glError::check();

        glBindTexture(value->type, value->id); 
        glError::check();

        glUniform1i(_uniforms[location], index);
        glError::check();
    }

    void shader::setUniform(uint location, GLuint value)
    { 
        glUniform1i(_uniforms[location], value); 
        glError::check();
    }

    void shader::setUniform(uint location, GLfloat value) 
    {
        glUniform1f(_uniforms[location], value);
        glError::check();
    }

    void shader::setUniform(uint location, color value) 
    { 
        glUniform4f(_uniforms[location], value.r, value.g, value.b, value.a);
        glError::check();
    }
    
    void shader::setUniform(uint location, sizef value) 
    {
        glUniform3f(_uniforms[location], value.w, value.h, value.d);
        glError::check();
    }
    
    void shader::setUniform(uint location, mat3 value)
    {
        glUniformMatrix3fv(_uniforms[location], 1, GL_FALSE, &value[0][0]);
        glError::check();
    }
    
    void shader::setUniform(uint location, mat4 value) 
    { 
        glUniformMatrix4fv(_uniforms[location], 1, GL_FALSE, &value[0][0]);
        glError::check();
    }
    
    void shader::setUniform(uint location, vec2 value)
    { 
        glUniform2f(_uniforms[location], value.x, value.y);
        glError::check();
    }
    
    void shader::setUniform(uint location, vec3 value) 
    { 
        glUniform3f(_uniforms[location], value.x, value.y, value.z);
        glError::check();
    }
    
    void shader::setUniform(uint location, vec4 value)
    { 
        glUniform4f(_uniforms[location], value.x, value.y, value.z, value.w);
        glError::check();
    }
    
    void shader::setUniform(uint location, bool value)
    { 
        glUniform1f(_uniforms[location], value ? 1.0f : 0.0f);
        glError::check();
    }
    
    void shader::setUniform(uint location, int value) 
    { 
        glUniform1i(_uniforms[location], value);
        glError::check();
    }
    
    void shader::setUniform(uint location, GLuint64 value)
    {
        glUniformHandleui64ARB(_uniforms[location], value);
        glError::check();
    }

    void shader::setUniform(uint location, vector<GLint> value)
    {
        glUniform1iv(_uniforms[location], static_cast<GLsizei>(value.size()), value.data());
        glError::check();
    }

    void shader::setUniform(uint location, vector<GLuint64> value)
    {
        glUniformHandleui64vARB(_uniforms[location], static_cast<GLsizei>(value.size()), value.data());
        glError::check();
    }

    void shader::bind()
    {
        if (!_initialized)
            return;

        _textureCount = 0;

        glUseProgram(_id);
        glError::check();
    }

    void shader::unbind()
    {
        if (!_initialized)
            return;

        glUseProgram(0);
        glError::check();
    }

    void shader::release()
    {
        if (!_initialized)
            return;

        glDetachShader(_id, _fragmentShader);
        glError::check();

        glDetachShader(_id, _vertexShader);
        glError::check();

        glDeleteShader(_fragmentShader);
        glError::check();

        glDeleteShader(_vertexShader);
        glError::check();

        glDeleteProgram(_id);
        glError::check();
    }
}