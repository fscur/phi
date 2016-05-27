#pragma once
#include <phi.h>

#include <core\node.h>
#include <rendering\renderPass.h>

namespace demon
{
    class layer
    {
    private:
        phi::node* _root;
        phi::vector<phi::renderPass*> _renderPasses;
        std::function<void(phi::node*)> _onNodeAdded;

    public:
        layer(phi::vector<phi::renderPass*>& renderPasses);
        layer(phi::vector<phi::renderPass*>&& renderPasses);
        ~layer();

        void add(phi::node* node);

        void update();
        void render();

        void setOnNodeAdded(std::function<void(phi::node*)> onNodeAdded) { _onNodeAdded = onNodeAdded; }
    };
}