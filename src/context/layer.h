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

        vector<std::function<void(void)>> _onUpdate;
        //vector<std::function<void(void)>> _onRender;
        vector<renderPass*> _renderPasses;
        vector<std::function<void(node*)>> _onNodeAdded;

        buffer* _frameUniformsBuffer;

    private:
        void initialize();
        void createFrameUniforms();
        void updateFrameUniforms();

    public:
        layer(camera* camera);
        ~layer();

        CONTEXT_API void addOnUpdate(std::function<void(void)> updateFunction);
        CONTEXT_API void addRenderPass(renderPass* renderPass);
        CONTEXT_API void addOnNodeAdded(std::function<void(node*)> onNodeAdded);

        CONTEXT_API void add(node* node);
        CONTEXT_API void update();
        CONTEXT_API void render();

        camera* getCamera() { return _camera; }
    };
}