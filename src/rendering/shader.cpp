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

        glShaderSource(_vertexShader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
        glCompileShader(_vertexShader); // Compile the vertex shader

#if _DEBUG
        result = validateShader(_vertexShader, _vertFile.c_str()); // Validate the vertex shader
        if (!result)
            return false;
#endif
        glShaderSource(_fragmentShader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
        glCompileShader(_fragmentShader); // Compile the fragment shader

#if _DEBUG
        result = validateShader(_fragmentShader, _fragFile.c_str()); // Validate the fragment shader
        if (!result)
            return false;
#endif

        _id = glCreateProgram(); // Create a GLSL program
        glAttachShader(_id, _vertexShader); // Attach a vertex shader to the program
        glAttachShader(_id, _fragmentShader); // Attach the fragment shader to the program

        initAttribs();

        glLinkProgram(_id); // Link the vertex and fragment shaders in the program

#if _DEBUG
        result = validateProgram(_id); // Validate the shader program
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

        glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader

        if (length > 0) // If we have any information to display
            std::cout << "shader " << shader << " (" << (file.c_str() ? file : "") << ") compile info:\n" << buffer << std::endl; // Output the information

        return success == GL_TRUE;
    }

    bool shader::validateProgram(GLuint program)
    {
        const unsigned int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        GLsizei length = 0;

        glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
        if (length > 0) // If we have any information to display
            std::cout << "Program " << program << " link info:\n" << buffer << std::endl; // Output the information

        //This should be called before drawing to see if the current OpenGL state is correct for this shader (also, should be only called when debugging):
        //glValidateProgram(program); // Get OpenGL to try validating the program
        //GLint status;
        //glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
        //if (status == GL_FALSE) // If there was a problem validating
        //{
        //    std::cout << "Error validating shader " << program << std::endl; // Output which program had the error
        //    return false;
        //}

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

        glDetachShader(_id, _fragmentShader); // Detach the fragment shader
        glDetachShader(_id, _vertexShader); // Detach the vertex shader

        glDeleteShader(_fragmentShader); // Delete the fragment shader
        glDeleteShader(_vertexShader); // Delete the vertex shader
        glDeleteProgram(_id); // Delete the shader program
    }
}