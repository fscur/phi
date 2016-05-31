#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\node.h>
#include <rendering\renderPass.h>
#include <rendering\camera.h>
#include <rendering\buffer.h>
#include <rendering\frameUniformBlock.h>

namespace demon
{
    class layer
    {
    private:
        phi::camera* _camera;
        phi::node* _root;
        phi::vector<phi::renderPass*> _renderPasses;
        std::function<void(phi::node*)> _onNodeAdded;

        phi::buffer<phi::frameUniformBlock>* _frameUniformsBuffer;

    private:
        void initialize();
        void createFrameUniforms();
        void updateFrameUniforms();

    public:
        layer(phi::camera* camera, phi::vector<phi::renderPass*>& renderPasses);
        layer(phi::camera* camera, phi::vector<phi::renderPass*>&& renderPasses);
        ~layer();

        CONTEXT_API void add(phi::node* node);

        void update();
        void render();

        void setOnNodeAdded(std::function<void(phi::node*)> onNodeAdded) { _onNodeAdded = onNodeAdded; }

        phi::camera* getCamera() { return _camera; }
    };
}