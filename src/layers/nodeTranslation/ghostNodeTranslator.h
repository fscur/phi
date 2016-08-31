#pragma once
#include <phi.h>
#include <context/layer.h>

namespace phi
{
    class ghostNodeTranslator
    {
    private:
        layer* _layer;
        bool _showingGhosts;
        unordered_map<node*, node*> _ghostNodes;

    private:
        node* cloneNodeAsGhost(node * node);

    public:
        ghostNodeTranslator(layer* layer);

        ~ghostNodeTranslator();

        void enable();
        void disable();
        void translate(vec3 offset);
        void addRange(const vector<node*>& nodes);
        void clear();
    };
}
