#pragma once
#include <phi.h>
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <core\image.h>
#include <core\eventHandler.h>

namespace phi
{
    class controlRenderer :
        public component
    {
    private:
        geometry* _geometry;
        image* _image;
        eventHandler<vec3>* _onSizeChanged;
        color _color;
        vec3 _size;
		bool _isGlassy;

    public:
        static componentType getComponentType() { return componentType::CONTROL_RENDERER; }

    public:
        controlRenderer(string name);
        ~controlRenderer();
        geometry* getGeometry() const { return _geometry; }
        image* getBackgroundImage() const { return _image; }
        color getBackgroundColor() const { return _color; }
        vec3 getSize() const { return _size; }
		bool getIsGlassy() const { return _isGlassy; }

        void setGeometry(geometry* value) { _geometry = value; }
        void setImage(image* value) { _image = value; }
        void setColor(color value) { _color = value; }
        void setSize(vec3 value);
		void setIsGlassy(bool value) { _isGlassy = value; }

        eventHandler<vec3>* getOnSizeChanged() const { return _onSizeChanged; }
    };
}