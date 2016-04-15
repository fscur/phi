#include <precompiled.h>
#include "shader.h"
#include "glError.h"

namespace phi
{
    shader::shader(
        string vertexFile, 
        string fragmentFile, 
        const vector<string>& attributes) :
        _programId(0u),
        _vertexShaderId(0u),
        _fragmentShaderId(0u),
        _vertexFile(vertexFile),
        _fragmentFile(fragmentFile),
        _initialized(false),
        _uniforms(map<uint, GLuint>()),
        _attributes(attributes)
    {
    }

    shader::~shader()
    {
        if (!_initialized)
            return;

        unbind();

        glDetachShader(_programId, _vertexShaderId);
        glError::check();
        glDetachShader(_programId, _fragmentShaderId);
        glError::check();

        glDeleteShader(_vertexShaderId);
        glError::check();
        glDeleteShader(_fragmentShaderId);
        glError::check();

        glDeleteProgram(_programId);
        glError::check();
    }

    bool shader::init()
    {
        if (_initialized)
            return true;

        bool result = true;

        _vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glError::check();

        _fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glError::check();

        result = compileShader(_vertexShaderId, _vertexFile);
        result = compileShader(_fragmentShaderId, _fragmentFile);

        _programId = glCreateProgram();
        glError::check();

        glAttachShader(_programId, _vertexShaderId);
        glError::check();

        glAttachShader(_programId, _fragmentShaderId);
        glError::check();

        initializeAttributes();

        glLinkProgram(_programId);
        glError::check();

#if _DEBUG
        result = validateProgram(_programId);
        if (!result)
            return false;
#endif
        _initialized = true;

        return result;
    }

    bool shader::compileShader(GLuint shaderId, string& file)
    {
        auto result = true;
        auto content = loadShaderFile(file.c_str());
        auto contentText = content.c_str();
        glShaderSource(shaderId, 1, &contentText, 0);
        glError::check();

        glCompileShader(shaderId);
        glError::check();

#if _DEBUG
        result = validateShader(shaderId, _fragmentFile.c_str());
#endif

        return result;
    }

    string shader::loadShaderFile(string fileName)
    {
        string fileString;
        string line;

        std::ifstream file(fileName);

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

    bool shader::validateShader(GLuint shader, const string& file)
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

    void shader::createUniform(uint location, const string & name)
    {
        _uniforms[location] = glGetUniformLocation(_programId, name.c_str());
        glError::check();
    }

    void shader::initializeAttributes()
    {
        for (uint i = 0; i < _attributes.size(); ++i)
        {
            glBindAttribLocation(_programId, i, _attributes[i].c_str());
            glError::check();
        }
    }

    void shader::addUniform(uint location, string name)
    {
        if (!_initialized)
            return;

        _uniformsNames[location] = name;
        createUniform(location, name);
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

        glUseProgram(_programId);
        glError::check();
    }

    void shader::unbind()
    {
        if (!_initialized)
            return;

        glUseProgram(0);
        glError::check();
    }

    bool shader::reload()
    {
        unbind();

        bool result = true;

        result = compileShader(_fragmentShaderId, _fragmentFile);
        result = compileShader(_vertexShaderId, _vertexFile);

        glLinkProgram(_programId);
        glError::check();
        result = validateProgram(_programId);

        if (result)
        {
            for (auto& pair : _uniformsNames)
            {
                createUniform(pair.first, pair.second);
            }
        }

        return result;
    }
}