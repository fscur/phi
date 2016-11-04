#pragma once
#include <phi.h>
#include "coreApi.h"
#include <core\component.h>
#include <core\color.h>
#include <core\clippingPlane.h>

namespace phi
{
    class translationPlaneGrid :
        public component
    {
    public:
        static componentType getComponentType() 
        { 
            return componentType::TRANSLATION_PLANE_GRID; 
        }

    public:
        CORE_API translationPlaneGrid();
        CORE_API translationPlaneGrid(const translationPlaneGrid& original);
        CORE_API ~translationPlaneGrid();

        color getColor() const { return _color; }
        float getLineThickness() const { return _lineThickness; }
        float getOpacity() const { return _opacity; }
        float getVisibilityFactor() const { return _visibilityFactor; }
        bool isVisible() const { return _isVisible; }
        vector<clippingPlane*> getClippingPlanes() const { return _clippingPlanes; }
        unordered_map<clippingPlane*, float> getClippingPlanesOpacities() const { return _clippingPlanesOpacities; }
        unordered_map<clippingPlane*, clippingDistance::clippingDistance> getClippingPlanesDistances() const { return _clippingPlanesDistances; }
        float getInnerPlaneDistance() const { return _innerPlaneDistance; }
        float getInnerGridSize() const { return _innerGridSize; }
        float getOuterPlaneDistance() const { return _outerPlaneDistance; }
        float getOuterGridSize() const { return _outerGridSize; }

        eventHandler<translationPlaneGrid*>* getColorChanged() { return &_colorChanged; }
        eventHandler<translationPlaneGrid*>* getLineThicknessChanged() { return &_lineThicknessChanged; }
        eventHandler<translationPlaneGrid*>* getOpacityChanged() { return &_opacityChanged; }
        eventHandler<translationPlaneGrid*>* getVisibleChanged() { return &_visibleChanged; }
        eventHandler<translationPlaneGrid*>* getClippingPlanesChanged() { return &_clippingPlanesChanged; }

        CORE_API void setColor(const color& value);
        CORE_API void setLineThickness(const float value);
        CORE_API void setOpacity(const float value); 
        CORE_API void setVisibilityFactor(const float value);
        CORE_API void setInnerPlaneDistance(const float value) { _innerPlaneDistance = value; }
        CORE_API void setInnerGridSize(const float value) { _innerGridSize = value; }
        CORE_API void setOuterPlaneDistance(const float value) { _outerPlaneDistance = value; }
        CORE_API void setOuterGridSize(const float value) { _outerGridSize = value; }

        CORE_API void addClippingPlane(clippingPlane* clippingPlane);
        CORE_API void removeClippingPlane(clippingPlane* clippingPlane);
        CORE_API void setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity);
        CORE_API void setClippingPlaneDistance(clippingPlane* clippingPlane, clippingDistance::clippingDistance distance);
        CORE_API void hide();
        CORE_API void show();

        CORE_API component* clone() const override;

    private:
        color _color;
        float _lineThickness;
        float _opacity;
        float _visibilityFactor;
        bool _isVisible;
        vector<clippingPlane*> _clippingPlanes;
        unordered_map<clippingPlane*, float> _clippingPlanesOpacities;
        unordered_map<clippingPlane*, clippingDistance::clippingDistance> _clippingPlanesDistances;
        float _innerPlaneDistance;
        float _innerGridSize;
        float _outerPlaneDistance;
        float _outerGridSize;

        eventHandler<translationPlaneGrid*> _colorChanged;
        eventHandler<translationPlaneGrid*> _lineThicknessChanged;
        eventHandler<translationPlaneGrid*> _opacityChanged;
        eventHandler<translationPlaneGrid*> _visibleChanged;
        eventHandler<translationPlaneGrid*> _clippingPlanesChanged;
    };
}