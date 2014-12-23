#ifndef _PHI_SHADER_H_
#define _PHI_SHADER_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "color.h"
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
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

		std::string _name;
		std::string _vertFile;
		std::string _fragFile;
		//std::string _geomFile;

		std::map<std::string, GLuint> _uniforms;
		
		std::vector<std::string> _attributes;
		GLuint _textureCount;

	private:
		static std::string loadShaderFile(const std::string fileName);
		static bool validateShader(GLuint shader, const std::string file = 0);
		static bool validateProgram(GLuint program);

	protected:
		shader(){}

	public:
		shader(std::string name, std::string vertFile, std::string fragFile, std::vector<std::string> attributes);
		~shader();

		GLuint getId() const { return _id; }
		std::string getName() const { return _name; }

		bool init();
		
		void addUniform(std::string name);
		void setUniform(std::string name, GLuint value) { glUniform1i(_uniforms[name], value); }
		void setUniform(std::string name, GLfloat value) { glUniform1f(_uniforms[name], value); }
		void setUniform(std::string name, color value) { glUniform4f(_uniforms[name], value.r, value.g, value.b, value.a); }
		void setUniform(std::string name, size<GLfloat> value) { glUniform3f(_uniforms[name], value.width, value.height, value.depth); }
		void setUniform(std::string name, glm::mat3 value) { glUniformMatrix3fv(_uniforms[name], 1, GL_FALSE, &value[0][0]); }
		void setUniform(std::string name, glm::mat4 value) { glUniformMatrix4fv(_uniforms[name], 1, GL_FALSE, &value[0][0]); }
		void setUniform(std::string name, glm::vec2 value) { glUniform2f(_uniforms[name], value.x, value.y); }
		void setUniform(std::string name, glm::vec3 value) { glUniform3f(_uniforms[name], value.x, value.y, value.z); }
		void setUniform(std::string name, glm::vec4 value) { glUniform4f(_uniforms[name], value.x, value.y, value.z, value.w); }
		void setUniform(std::string name, texture* value);
		
        RENDERING_API void initAttribs();

		void bind();
		void unbind();

		void release();
	};
}

#endif