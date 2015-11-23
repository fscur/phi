#ifndef _PHI_RENDER_TARGET_H_
#define _PHI_RENDER_TARGET_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <string>
#include "phi/rendering/rendering.h"
#include "phi/rendering/texture.h"

namespace phi
{
	class renderTarget
	{
	protected:
		std::string _name;
		texture* _texture;
		GLenum _target;
		GLenum _attachment;
		GLenum _texTarget;
		GLuint _level;
	public:
		RENDERING_API renderTarget(std::string name, texture* texture);
		RENDERING_API ~renderTarget();

		RENDERING_API std::string getName() const { return _name; }
		RENDERING_API texture* getTexture() const { return _texture; }
		RENDERING_API GLenum getTarget() const { return _target; }
		RENDERING_API GLenum getAttachment() const { return _attachment; }
		RENDERING_API GLenum getTexTarget() const { return _texTarget; }
		RENDERING_API GLuint getLevel() const { return _level; }

		RENDERING_API void setTarget(GLenum value) { _target = value; }
		RENDERING_API void setAttachment(GLenum value) { _attachment = value; }
		RENDERING_API void setTexTarget(GLenum value) { _texTarget = value; }
		RENDERING_API void setLevel(GLuint value) { _level = value; }
	};
}

#endif