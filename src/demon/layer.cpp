#include <precompiled.h>
#include "layer.h"

namespace demon
{
    using namespace phi;

    layer::layer(vector<renderPass*>& renderPasses) :
        _renderPasses(renderPasses),
        _root(new node("root"))
    {
    }

    layer::layer(phi::vector<phi::renderPass*>&& renderPasses) :
        _renderPasses(renderPasses),
        _root(new node("root"))
    {
    }

    layer::~layer()
    {
        for (auto renderPass : _renderPasses)
            safeDelete(renderPass);
    }

    void layer::add(node* node)
    {
        _root->addChild(node);
        _onNodeAdded(node);
    }
    
    void layer::update()
    {
        for (auto renderPass : _renderPasses)
            renderPass->update();
    }
    
    void layer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }
}
