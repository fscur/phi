#include <precompiled.h>

#include "planeGridRenderPass.h"

namespace phi
{
    planeGridRenderPass::planeGridRenderPass(gl* gl) :
        _gl(gl),
        _quad(nullptr),
        _quadVao(0u),
        _quadVbo(nullptr),
        _quadEbo(nullptr),
        _shader(nullptr),
        _textureAddress(textureAddress()),
        _radiusFadeIn(0.0f),
        _radiusFadeOut(0.0f),
        _showing(false),
        _plane(finitePlane(vec3(), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f))),
        _doingAnimation(false),
        _animationTime(0.0),
        _animationInitialPosition(vec2()),
        _animationDelta(vec2())
    {
        createQuad();

        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _shader = _gl->shadersManager->load("planeGridPass", attribs);

        _shader->addUniform(0, "m");
        _shader->addUniform(1, "size");
        _shader->addUniform(2, "radiusFadeIn");
        _shader->addUniform(3, "radiusFadeOut");
        _shader->addUniform(4, "radiusWave");
        _shader->addUniform(5, "expansionPosition");
        _shader->addUniform(6, "textureArrayIndex");
        _shader->addUniform(7, "texturePageIndex");
        _shader->addUniform(8, "textureArrays");

        _transform.setLocalSize(vec3(PLANE_SIZE));
    }

    planeGridRenderPass::~planeGridRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
        safeDelete(_gridTexture);
    }

    void planeGridRenderPass::createQuad()
    {
        _quad = geometry::createQuad(1.0f);

        glCreateVertexArrays(1, &_quadVao);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _quadVbo = new vertexBuffer(attribs);
        _quadVbo->storage(_quad->vboSize, _quad->vboData, bufferStorageUsage::write);

        _quadEbo = new buffer(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }

    void planeGridRenderPass::renderQuad()
    {
        glBindVertexArray(_quadVao);
        glError::check();

        _shader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();
        _shader->unbind();

        glBindVertexArray(0);
        glError::check();
    }

    void planeGridRenderPass::setImage(image* image)
    {
        _gridTexture = new phi::texture(image);
        _gridTexture->type = GL_TEXTURE_2D;
        _gridTexture->internalFormat = GL_RGBA8;
        _gridTexture->wrapMode = GL_REPEAT;
        _gridTexture->minFilter = GL_LINEAR_MIPMAP_LINEAR;
        _gridTexture->magFilter = GL_LINEAR;
        _textureAddress = _gl->texturesManager->add(_gridTexture);
    }

    void planeGridRenderPass::setPositionAndOrientation(vec3 position, vec3 direction)
    {
        auto right = vec3(1.0f, 0.0f, 0.0f);
        auto dot = glm::dot(direction, vec3(0.0f, 1.0f, 0.0f));
        if (!mathUtils::isClose(glm::abs(dot), 1.0f))
            right = glm::cross(vec3(0.0f, 1.0f, 0.0f), direction);

        auto up = glm::cross(direction, right);

        _plane = finitePlane(position, position + right, position + up);

        auto rotation = glm::quat_cast(glm::mat3(right, up, direction));

        _transform.setLocalPosition(position);
        _transform.setLocalOrientation(rotation);
    }

    void planeGridRenderPass::setFocusPosition(vec2 focusPosition)
    {
        _focusPosition = focusPosition;
        _doingAnimation = false;
    }

    void planeGridRenderPass::animateFocusPosition(vec2 targetFocusPosition)
    {
        _animationInitialPosition = _focusPosition;
        _animationDelta = targetFocusPosition - _focusPosition;
        _doingAnimation = true;
        _animationTime = 0.0f;
    }

    void planeGridRenderPass::projectAndSetFocusPosition(vec3 toProjectFocusPosition)
    {
        setFocusPosition(projectPoint(toProjectFocusPosition));
    }

    void planeGridRenderPass::projectAndAnimateFocusPosition(vec3 toProjectTargetFocusPosition)
    {
        animateFocusPosition(projectPoint(toProjectTargetFocusPosition));
    }

    vec2 planeGridRenderPass::projectPoint(vec3 position)
    {
        return _plane.projectPoint(position);
    }

    void planeGridRenderPass::show()
    {
        _radiusFadeOut = 0.0f;
        _radiusFadeIn = 0.0f;
        _showing = true;
    }

    void planeGridRenderPass::hide()
    {
        _radiusFadeIn = 0.0f;
        _showing = false;
    }

    void planeGridRenderPass::updateShaderUniforms()
    {
        _shader->bind();

        _shader->setUniform(0, _transform.getModelMatrix());
        _shader->setUniform(1, PLANE_SIZE);
        _shader->setUniform(2, _radiusFadeIn);
        _shader->setUniform(3, glm::min(_radiusFadeOut, MAX_RADIUS));
        _shader->setUniform(4, _radiusFadeOut - RADIUS_WAVE_OFFSET);
        _shader->setUniform(5, _focusPosition + vec2(PLANE_SIZE * 0.5f));
        _shader->setUniform(6, _textureAddress.unit);
        _shader->setUniform(7, _textureAddress.page);

        if (_gl->currentState.useBindlessTextures)
            _shader->setUniform(8, _gl->texturesManager->handles);
        else
            _shader->setUniform(8, _gl->texturesManager->units);

        _shader->unbind();
    }

    void planeGridRenderPass::updateAnimation()
    {
        if (!_doingAnimation)
            return;

        auto deltaMilliseconds = phi::time::deltaSeconds * 1000.0;
        _animationTime += deltaMilliseconds;
        auto percent = -glm::exp(_animationTime / -100.0f) + 1.0f;
        auto delta = _animationDelta * static_cast<float>(percent);
        _focusPosition = _animationInitialPosition + delta;

        _doingAnimation = percent < 1.0f;
    }

    void planeGridRenderPass::update()
    {
        auto deltaRadius = static_cast<float>(phi::time::deltaSeconds * RADIUS_DELTA_PER_SECOND);
        _radiusFadeOut += deltaRadius;
        _radiusFadeIn = !_showing * (_radiusFadeIn + deltaRadius);

        updateShaderUniforms();
        updateAnimation();
    }

    void planeGridRenderPass::render()
    {
        glEnable(GL_DEPTH_TEST);
        glError::check();
        glEnable(GL_BLEND);
        glError::check();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glError::check();
        glDisable(GL_CULL_FACE);
        glError::check();
        glDepthMask(GL_FALSE);
        glError::check();

        renderQuad();

        glDepthMask(GL_TRUE);
        glError::check();
        glEnable(GL_CULL_FACE);
        glError::check();
        glDisable(GL_BLEND);
        glError::check();
        glDisable(GL_DEPTH_TEST);
        glError::check();
    }
}