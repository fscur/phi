#pragma once
#include <phi.h>

#include <core\node.h>
#include <rendering\renderPass.h>

namespace demon
{
    class layer
    {
    private:
        phi::vector<phi::renderPass*> _renderPasses;
        phi::node* _root;
        std::function<void(node*)> _onNodeAdded;
    public:
        layer(phi::vector<phi::renderPass*> renderPasses);
        ~layer();

        void add(phi::node* node);

        void update();
        void render();

        void setOnNodeAdded(std::function<void(node*)> onNodeAdded) { _onNodeAdded = onNodeAdded; }
    };
}