#include <precompiled.h>
#include "translationPlaneGrid.h"

namespace phi
{
    translationPlaneGrid::translationPlaneGrid() :
        component(component::TRANSLATION_PLANE_GRID),
        _color(color::white),
        _lineThickness(10.0f),
        _opacity(0.0f),
        _visibilityFactor(0.0f),
        _isVisible(false),
        _clippingPlanes(vector<clippingPlane*>()),
        _clippingPlanesOpacities(unordered_map<clippingPlane*, float>()),
        _colorChanged(eventHandler<translationPlaneGrid*>()),
        _lineThicknessChanged(eventHandler<translationPlaneGrid*>()),
        _opacityChanged(eventHandler<translationPlaneGrid*>()),
        _visibleChanged(eventHandler<translationPlaneGrid*>()),
        _innerPlaneDistance(0.0f),
        _innerGridSize(1.0f),
        _outerPlaneDistance(0.0f),
        _outerGridSize(0.0f)
    {
    }

    translationPlaneGrid::translationPlaneGrid(const translationPlaneGrid& original) :
        component(componentType::TRANSLATION_PLANE_GRID),
        _color(original._color),
        _lineThickness(original._lineThickness),
        _opacity(original._opacity),
        _isVisible(original._isVisible),
        _clippingPlanes(original._clippingPlanes),
        _colorChanged(eventHandler<translationPlaneGrid*>()),
        _lineThicknessChanged(eventHandler<translationPlaneGrid*>()),
        _opacityChanged(eventHandler<translationPlaneGrid*>()),
        _visibleChanged(eventHandler<translationPlaneGrid*>())
    {
    }

    translationPlaneGrid::~translationPlaneGrid()
    {
    }

    void translationPlaneGrid::setColor(const color& value)
    {
        if (_color == value)
            return;

        _color = value;
        _colorChanged.raise(this);
    }

    void translationPlaneGrid::setLineThickness(const float value)
    {
        if (_lineThickness == value)
            return;

        _lineThickness = value;
        _lineThicknessChanged.raise(this);
    }

    void translationPlaneGrid::setOpacity(const float value)
    {
        if (_opacity == value)
            return;

        _opacity = value;
        _opacityChanged.raise(this);
    }

    CORE_API void translationPlaneGrid::setVisibilityFactor(const float value)
    {
        if (_visibilityFactor == value)
            return;

        _visibilityFactor = value;
        _opacityChanged.raise(this);
    }

    void translationPlaneGrid::addClippingPlane(clippingPlane* plane)
    {
        _clippingPlanes.push_back(plane);
        _clippingPlanesOpacities[plane] = 0.0f;
        _clippingPlanesDistances[plane] = clippingDistance::negative;
        _clippingPlanesChanged.raise(this);
    }

    void translationPlaneGrid::removeClippingPlane(clippingPlane* plane)
    {
        phi::removeIfContains(_clippingPlanes, plane);
        _clippingPlanesOpacities.erase(plane);
        _clippingPlanesDistances.erase(plane);
        _clippingPlanesChanged.raise(this);
    }

    void translationPlaneGrid::setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity)
    {
        _clippingPlanesOpacities[clippingPlane] = opacity;
        _clippingPlanesChanged.raise(this);
    }

    void translationPlaneGrid::setClippingPlaneDistance(clippingPlane* clippingPlane, clippingDistance::clippingDistance distance)
    {
        _clippingPlanesDistances[clippingPlane] = distance;
        _clippingPlanesChanged.raise(this);
    }

    void translationPlaneGrid::show()
    {
        if (_isVisible)
            return;

        _isVisible = true;
        _visibleChanged.raise(this);
    }

    void translationPlaneGrid::hide()
    {
        if (!_isVisible)
            return;

        _isVisible = false;
        _visibleChanged.raise(this);
    }

    component* translationPlaneGrid::clone() const
    {
        auto p = static_cast<const translationPlaneGrid*>(this);
        return new translationPlaneGrid(*p);
    }
}