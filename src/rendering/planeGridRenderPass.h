#pragma once

#include <phi.h>

#include "gl.h"
#include "vertexBuffer.h"

#include <core\geometry.h>
#include <core\transform.h>
#include <core\time.h>
#include <core\finitePlane.h>

namespace phi
{
    class planeGridRenderPass
    {
    private:
        const float PLANE_SIZE = 1000.0f;
        const float RADIUS_DELTA_PER_SECOND = 20.0f;
        const float MAX_RADIUS = 10.0f;
        const float RADIUS_WAVE_OFFSET = 5.0f;

        gl* _gl;
        geometry* _quad;
        uint _quadVao;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        shader* _shader;
        textureAddress _textureAddress;
        float _radiusFadeIn;
        float _radiusFadeOut;
        bool _showing;
        finitePlane _plane;
        bool _doingAnimation;
        double _animationTime;
        vec2 _animationInitialPosition;
        vec2 _animationDelta;
        texture* _gridTexture;
        transform _transform;
        vec2 _focusPosition;
        color _color;

    private:
        void createShader();
        void createQuad();
        void renderQuad();
        void updateShaderUniforms();
        void updateAnimation();

    public:
        RENDERING_API void show();
        RENDERING_API void hide();

    public:
        RENDERING_API planeGridRenderPass(gl* gl);
        RENDERING_API ~planeGridRenderPass();

        vec2 getFocusPosition() { return _focusPosition; }
        RENDERING_API void setImage(image* image);
        RENDERING_API void setPositionAndOrientation(vec3 position, vec3 direction);
        RENDERING_API void setFocusPosition(vec2 focusPosition);
        RENDERING_API void setColor(color color) { _color = color; }
        RENDERING_API void animateFocusPosition(vec2 targetFocusPosition);
        RENDERING_API void projectAndSetFocusPosition(vec3 toProjectFocusPosition);
        RENDERING_API void projectAndAnimateFocusPosition(vec3 toProjectTargetFocusPosition);
        RENDERING_API vec2 projectPoint(vec3 position);

        RENDERING_API void update();
        RENDERING_API void render();
    };
}