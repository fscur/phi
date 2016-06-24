#pragma once
#include <phi.h>
#include "coreApi.h"

#include "component.h"

namespace phi
{
    class node;

    class clickComponent :
        public component
    {
    private:
        vector<std::function<void(node*)>> _onClick;
        uint32_t _pickId;

    public:
        CORE_API clickComponent(const string& name);
        CORE_API clickComponent(const clickComponent& original);
        CORE_API ~clickComponent();

        CORE_API void onClick() const;
        CORE_API void addOnClick(std::function<void(node*)> onClick);
        CORE_API component * clone() const;
        CORE_API vec3 getSelectionColor() const;

        uint32_t getId() { return _pickId; }
        void setId(uint32_t id) { _pickId = id; }

        static componentType getComponentType() { return componentType::CLICK; }
    };
}