#pragma once
#include <phi.h>
#include "commonApi.h"

#include <core/component.h>
#include <input/mouseEventArgs.h>
#include <animation/iMouseClickAnimatable.h>
#include <animation/iMouseHoverAnimatable.h>

namespace phi
{
    class node;

    class mouseInteractionComponent :
        public component
    {
    private:
        vector<std::function<void(node*)>> _onMouseDown;
        vector<std::function<void(node*)>> _onMouseUp;
        vector<std::function<void(mouseEventArgs*, node*)>> _onMouseMove;
        vector<std::function<void(node*)>> _onMouseEnter;
        vector<std::function<void(node*)>> _onMouseLeave;
        uint32_t _pickId;
        bool _isMouseOver;

        iMouseClickAnimatable* _mouseClickAnimatable;
        iMouseHoverAnimatable* _mouseHoverAnimatable;

    public:
        COMMON_API mouseInteractionComponent();
        COMMON_API mouseInteractionComponent(const mouseInteractionComponent& original);
        COMMON_API ~mouseInteractionComponent();

        COMMON_API void onMouseDown() const;
        COMMON_API void onMouseUp() const;
        COMMON_API void onMouseMove(mouseEventArgs* e) const;
        COMMON_API void onMouseEnter();
        COMMON_API void onMouseLeave();
        COMMON_API void addOnMouseDown(std::function<void(node*)> onMouseDown);
        COMMON_API void addOnMouseUp(std::function<void(node*)> onMouseUp);
        COMMON_API void addOnMouseMove(std::function<void(mouseEventArgs*, node*)> onMouseMove);
        COMMON_API void addOnMouseEnter(std::function<void(node*)> onMouseEnter);
        COMMON_API void addOnMouseLeave(std::function<void(node*)> onMouseLeave);
        COMMON_API void setMouseClickAnimatable(iMouseClickAnimatable* animatable);
        COMMON_API void setMouseHoverAnimatable(iMouseHoverAnimatable* animatable);
        COMMON_API component * clone() const;
        COMMON_API vec3 getSelectionColor() const;
        COMMON_API bool isMouseOver() const;

        uint32_t getId() { return _pickId; }
        void setId(uint32_t id) { _pickId = id; }

        static componentType getComponentType() { return componentType::CLICK; }
    };
}