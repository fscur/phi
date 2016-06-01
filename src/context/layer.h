#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\node.h>
#include <rendering\renderPass.h>
#include <rendering\camera.h>
#include <rendering\buffer.h>
#include <rendering\frameUniformBlock.h>

namespace phi
{
    class layer
    {
    private:
        camera* _camera;
        node* _root;
        vector<renderPass*> _renderPasses;
        std::function<void(node*)> _onNodeAdded;

        buffer<frameUniformBlock>* _frameUniformsBuffer;

    private:
        void initialize();
        void createFrameUniforms();
        void updateFrameUniforms();

    public:
        layer(camera* camera, vector<renderPass*>& renderPasses);
        layer(camera* camera, vector<renderPass*>&& renderPasses);
        ~layer();

        CONTEXT_API void add(node* node);

        void update();
        void render();

        void setOnNodeAdded(std::function<void(node*)> onNodeAdded) { _onNodeAdded = onNodeAdded; }

        camera* getCamera() { return _camera; }
    };
}