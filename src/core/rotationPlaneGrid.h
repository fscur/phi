#pragma once
#include <phi.h>
#include "coreApi.h"
#include <core\component.h>
#include <core\color.h>
#include <core\clippingPlane.h>

namespace phi
{
    class rotationPlaneGrid :
        public component
    {
    private:
        color _color;
        float _lineThickness;
        float _opacity;
        float _visibilityFactor;
        bool _isVisible;
        vector<clippingPlane*> _clippingPlanes;
        unordered_map<clippingPlane*, float> _clippingPlanesOpacities;
        unordered_map<clippingPlane*, clippingDistance::clippingDistance> _clippingPlanesDistances;

        eventHandler<rotationPlaneGrid*> _colorChanged;
        eventHandler<rotationPlaneGrid*> _lineThicknessChanged;
        eventHandler<rotationPlaneGrid*> _opacityChanged;
        eventHandler<rotationPlaneGrid*> _visibleChanged;
        eventHandler<rotationPlaneGrid*> _clippingPlanesChanged;

    public:
        static componentType getComponentType() 
        { 
            return componentType::ROTATION_PLANE_GRID; 
        }

    public:
        CORE_API rotationPlaneGrid();
        CORE_API rotationPlaneGrid(const rotationPlaneGrid& original);
        CORE_API ~rotationPlaneGrid();

        color getColor() const { return _color; }
        float getLineThickness() const { return _lineThickness; }
        float getOpacity() const { return _opacity; }
        float getVisibilityFactor() const { return _visibilityFactor; }
        bool isVisible() const { return _isVisible; }
        vector<clippingPlane*> getClippingPlanes() const { return _clippingPlanes; }
        unordered_map<clippingPlane*, float> getClippingPlanesOpacities() const { return _clippingPlanesOpacities; }
        unordered_map<clippingPlane*, clippingDistance::clippingDistance> getClippingPlanesDistances() const { return _clippingPlanesDistances; }

        eventHandler<rotationPlaneGrid*>* getColorChanged() { return &_colorChanged; }
        eventHandler<rotationPlaneGrid*>* getLineThicknessChanged() { return &_lineThicknessChanged; }
        eventHandler<rotationPlaneGrid*>* getOpacityChanged() { return &_opacityChanged; }
        eventHandler<rotationPlaneGrid*>* getVisibleChanged() { return &_visibleChanged; }
        eventHandler<rotationPlaneGrid*>* getClippingPlanesChanged() { return &_clippingPlanesChanged; }

        CORE_API void setColor(const color& value);
        CORE_API void setLineThickness(const float value);
        CORE_API void setOpacity(const float value); 
        CORE_API void setVisibilityFactor(const float value);

        CORE_API void addClippingPlane(clippingPlane* clippingPlane);
        CORE_API void removeClippingPlane(clippingPlane* clippingPlane);
        CORE_API void setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity);
        CORE_API void setClippingPlaneDistance(clippingPlane* clippingPlane, clippingDistance::clippingDistance distance);
        CORE_API void hide();
        CORE_API void show();
        
        CORE_API component* clone() const override;
    };
}