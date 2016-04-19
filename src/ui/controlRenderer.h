#pragma once
#include <phi.h>
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\texture.h>

namespace phi
{
    class controlRenderer :
        public component
    {
    private:
        geometry* _geometry;
        texture* _backgroundTexture;
        color _backgroundColor;

    public:
        static componentType getComponentType() { return componentType::CONTROL_RENDERER; }

    public:
        controlRenderer(string name);
        ~controlRenderer();
        geometry* getGeometry() const { _geometry; }
        texture* getBackgroundTexture() const { return _backgroundTexture; }
        color getBackgroundColor() const { return _backgroundColor; }

        void setGeometry(geometry* value) { _geometry = value; }
        void setBackgroundTexture(texture* value) { _backgroundTexture = value; }
        void setBackgroundColor(color value) { _backgroundColor = value; }
    };
}