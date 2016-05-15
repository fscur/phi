#pragma once
#include <phi.h>
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <core\eventHandler.h>
#include <rendering\texture.h>

namespace phi
{
    class control :
        public component
    {
    private:
        geometry* _geometry;
        texture* _texture;
        eventHandler<vec3>* _onSizeChanged;
        color _color;
        vec3 _size;
        bool _isGlassy;

    public:
        static componentType getComponentType() { return componentType::CONTROL_RENDERER; }

    public:
        control(string name);
        ~control();
        geometry* getGeometry() const { return _geometry; }
        texture* getBackgroundTexture() const { return _texture; }
        color getBackgroundColor() const { return _color; }
        vec3 getSize() const { return _size; }
        bool getIsGlassy() const { return _isGlassy; }

        void setGeometry(geometry* value) { _geometry = value; }
        void setTexture(texture* value) { _texture = value; }
        void setColor(color value) { _color = value; }
        void setSize(vec3 value);
        void setIsGlassy(bool value) { _isGlassy = value; }

        eventHandler<vec3>* getOnSizeChanged() const { return _onSizeChanged; }
    };
}