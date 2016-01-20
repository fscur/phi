#include <phi/core/globals.h>
#include <phi/rendering/shader.h>

#include <fstream>
#include <iostream>

namespace phi
{
    shader::shader(std::string name, std::string vertFile, std::string fragFile, std::vector<std::string> attributes) 
    {
        _name = name;
        _vertFile = vertFile;
        _fragFile = fragFile;
        _initialized = false;
        _attributes = attributes;
    }

    shader::~shader()
    {}

    bool shader::init()
    {
        if (_initialized)
            return true;

        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vertText = loadShaderFile(_vertFile.c_str());
        std::string fragText = loadShaderFile(_fragFile.c_str());

        const char *vertexText = vertText.c_str();
        const char *fragmentText = fragText.c_str();

        glShaderSource(_vertexShader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
        glCompileShader(_vertexShader); // Compile the vertex shader
        validateShader(_vertexShader, _vertFile.c_str()); // Validate the vertex shader

        glShaderSource(_fragmentShader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
        glCompileShader(_fragmentShader); // Compile the fragment shader
        bool result = validateShader(_fragmentShader, _fragFile.c_str()); // Validate the fragment shader

        if(!result)
        {
            log(_name);
            return false;
        }

        _id = glCreateProgram(); // Create a GLSL program
        glAttachShader(_id, _vertexShader); // Attach a vertex shader to the program
        glAttachShader(_id, _fragmentShader); // Attach the fragment shader to the program

        initAttribs();

        glLinkProgram(_id); // Link the vertex and fragment shaders in the program
        //result = validateProgram(_id); // Validate the shader program

        //if(!result)
        //{
        //    log(_name);
        //    return false;
        //}

        _initialized = true;
        return true;
    }

    std::string shader::loadShaderFile(const std::string fileName)
    {
        std::string filePath = fileName;

        std::string fileString;
        std::string line;

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

    bool shader::validateShader(GLuint shader, const std::string file) 
    {
        const GLuint BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader

        if (length > 0) // If we have any information to display
        {
            std::cout << "Shader " << shader << " (" << (file.c_str() ? file : "") << ") compile error: " << buffer << std::endl;
            return false;
        }

        return true;
    }

    bool shader::validateProgram(GLuint program) 
    {
        const GLuint BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
        if (length > 0) // If we have any information to display
        {
            std::cout << "Program " << program << " link error: " << buffer << std::endl; // Output the information
            return false;
        }

        glValidateProgram(program); // Get OpenGL to try validating the program
        GLint status;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
        if (status == GL_FALSE) // If there was a problem validating
        {
            glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
            std::cout << "Error validating shader " << program << buffer << std::endl; // Output which program had the error
            return false;
        }

        return true;
    }

    void shader::initAttribs()
    {
        for (unsigned int i = 0; i < _attributes.size(); i++)
            glBindAttribLocation(_id, i, _attributes[i].c_str());
    }

    void shader::addUniform(uint location, std::string name)
    {
        if (!_initialized)
        {
            log("Shader not initialized:" + _name);
            return;
        }

        _uniforms[location] = glGetUniformLocation(_id, name.c_str());
    }

    void shader::setUniform(uint location, texture* value, GLuint index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(value->getTextureType(), value->getId());
        glUniform1i(_uniforms[location], index);
    }

    void shader::setUniform(uint location, GLuint id, GLuint index)
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glUniform1i(_uniforms[location], index);
    }

    void shader::bind()
    {
        if (!_initialized)
        {
            log("Shader not initialized:" + _name);
            return;
        }

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

        glDetachShader(_id, _fragmentShader); // Detach the fragment shader
        glDetachShader(_id, _vertexShader); // Detach the vertex shader

        glDeleteShader(_fragmentShader); // Delete the fragment shader
        glDeleteShader(_vertexShader); // Delete the vertex shader
        glDeleteProgram(_id); // Delete the shader program
    }
}