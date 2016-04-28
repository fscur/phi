#pragma once
#include <phi.h>
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\texture.h>
#include <core\eventHandler.h>

namespace phi
{
    class controlRenderer :
        public component
    {
    private:
        geometry* _geometry;
        texture* _backgroundTexture;
        eventHandler<vec3>* _onSizeChanged;
        color _backgroundColor;
        vec3 _size;

    public:
        static componentType getComponentType() { return componentType::CONTROL_RENDERER; }

    public:
        controlRenderer(string name);
        ~controlRenderer();
        geometry* getGeometry() const { return _geometry; }
        texture* getBackgroundTexture() const { return _backgroundTexture; }
        color getBackgroundColor() const { return _backgroundColor; }
        vec3 getSize() const { return _size; }

        void setGeometry(geometry* value) { _geometry = value; }
        void setBackgroundTexture(texture* value) { _backgroundTexture = value; }
        void setBackgroundColor(color value) { _backgroundColor = value; }
        void setSize(vec3 value);

        eventHandler<vec3>* getOnSizeChanged() const { return _onSizeChanged; }
    };
}