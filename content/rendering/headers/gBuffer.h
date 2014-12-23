#ifndef _PHI_GBUFFER_H_
#define _PHI_GBUFFER_H_

#include "renderTarget.h"
#include "texture.h"

namespace phi
{
    class gBuffer :
        public renderTarget
    {
    private:
        texture* _positionTexture;
		texture* _normalTexture;
		texture* _ambientTexture;
		texture* _diffuseTexture;
		texture* _specularTexture;
		texture* _shininessTexture;
		texture* _finalTexture;
		texture* _depthTexture;
	private:
		void createTextures();
    public:
        RENDERING_API gBuffer(size<GLuint> size);
        RENDERING_API ~gBuffer();

		RENDERING_API texture* getPositionTexture() const { return _positionTexture; }
		RENDERING_API texture* getNormalTexture() const { return _normalTexture; }
		RENDERING_API texture* getAmbientTexture() const { return _ambientTexture; }
		RENDERING_API texture* getDiffuseTexture() const { return _diffuseTexture; }
		RENDERING_API texture* getSpecularTexture() const { return _specularTexture; }
		RENDERING_API texture* getShininessTexture() const { return _shininessTexture; }
		RENDERING_API texture* getFinalTexture() const { return _finalTexture; }
		RENDERING_API texture* getDepthTexture() const { return _depthTexture; }

        RENDERING_API bool init() override;
        RENDERING_API void bind() override;
        RENDERING_API void clear() override;
        RENDERING_API void blit(GLuint buffer, GLuint x, GLuint y, GLsizei width, GLsizei height);
		
		RENDERING_API void beginFrame();
		RENDERING_API void bindForGeomPass();
		RENDERING_API void bindForStencilPass();
		RENDERING_API void bindForLightPass();
    };
}

#endif