#pragma once
#include <phi.h>
#include "uiApi.h"
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
        UI_API planeGrid();
        UI_API planeGrid(const planeGrid& original);
        UI_API ~planeGrid();

        color getColor() const { return _color; }
        float getLineThickness() const { return _lineThickness; }
        float getOpacity() const { return _opacity; }
        bool isVisible() const { return _isVisible; }
        vector<clippingPlane*> getClippingPlanes() const { return _clippingPlanes; }
        unordered_map<clippingPlane*, float> getClippingPlanesOpacities() const { return _clippingPlanesOpacities; }

        eventHandler<planeGrid*>* getColorChanged() { return &_colorChanged; }
        eventHandler<planeGrid*>* getLineThicknessChanged() { return &_lineThicknessChanged; }
        eventHandler<planeGrid*>* getOpacityChanged() { return &_opacityChanged; }
        eventHandler<planeGrid*>* getVisibleChanged() { return &_visibleChanged; }
        eventHandler<planeGrid*>* getClippingPlanesChanged() { return &_clippingPlanesChanged; }

        UI_API void setColor(const color& value);
        UI_API void setLineThickness(const float value);
        UI_API void setOpacity(const float value);

        UI_API void addClippingPlane(clippingPlane* clippingPlane);
        UI_API void removeClippingPlane(clippingPlane* clippingPlane);
        UI_API void setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity);
        UI_API void hide();
        UI_API void show();

        UI_API component* clone() const override;
    };
}