#pragma once
#include <phi.h>
#include "coreApi.h"
#include <core\component.h>
#include <core\color.h>
#include <core\clippingPlane.h>

namespace phi
{
    class planeGrid :
        public component
    {
    private:
        color _color;
        float _lineThickness;
        float _opacity;
        bool _isVisible;
        vector<clippingPlane*> _clippingPlanes;
        unordered_map<clippingPlane*, float> _clippingPlanesOpacities;
        unordered_map<clippingPlane*, clippingDistance::clippingDistance> _clippingPlanesDistances;

        eventHandler<planeGrid*> _colorChanged;
        eventHandler<planeGrid*> _lineThicknessChanged;
        eventHandler<planeGrid*> _opacityChanged;
        eventHandler<planeGrid*> _visibleChanged;
        eventHandler<planeGrid*> _clippingPlanesChanged;

    public:
        static componentType getComponentType() 
        { 
            return componentType::PLANE_GRID; 
        }

    public:
        CORE_API planeGrid();
        CORE_API planeGrid(const planeGrid& original);
        CORE_API ~planeGrid();

        color getColor() const { return _color; }
        float getLineThickness() const { return _lineThickness; }
        float getOpacity() const { return _opacity; }
        bool isVisible() const { return _isVisible; }
        vector<clippingPlane*> getClippingPlanes() const { return _clippingPlanes; }
        unordered_map<clippingPlane*, float> getClippingPlanesOpacities() const { return _clippingPlanesOpacities; }
        unordered_map < clippingPlane*, clippingDistance::clippingDistance> getClippingPlanesDistances() const { return _clippingPlanesDistances; }

        eventHandler<planeGrid*>* getColorChanged() { return &_colorChanged; }
        eventHandler<planeGrid*>* getLineThicknessChanged() { return &_lineThicknessChanged; }
        eventHandler<planeGrid*>* getOpacityChanged() { return &_opacityChanged; }
        eventHandler<planeGrid*>* getVisibleChanged() { return &_visibleChanged; }
        eventHandler<planeGrid*>* getClippingPlanesChanged() { return &_clippingPlanesChanged; }

        CORE_API void setColor(const color& value);
        CORE_API void setLineThickness(const float value);
        CORE_API void setOpacity(const float value);

        CORE_API void addClippingPlane(clippingPlane* clippingPlane);
        CORE_API void removeClippingPlane(clippingPlane* clippingPlane);
        CORE_API void setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity);
        CORE_API void setClippingPlaneDistance(clippingPlane* clippingPlane, clippingDistance::clippingDistance distance);
        CORE_API void hide();
        CORE_API void show();
        
        CORE_API component* clone() const override;
    };
}