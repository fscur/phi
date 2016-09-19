#include <precompiled.h>
#include "planeGrid.h"

namespace phi
{
    planeGrid::planeGrid() :
        component(component::PLANE_GRID),
        _color(color::white),
        _lineThickness(10.0f),
        _opacity(0.0f),
        _visibilityFactor(0.0f),
        _isVisible(false),
        _clippingPlanes(vector<clippingPlane*>()),
        _clippingPlanesOpacities(unordered_map<clippingPlane*, float>()),
        _colorChanged(eventHandler<planeGrid*>()),
        _lineThicknessChanged(eventHandler<planeGrid*>()),
        _opacityChanged(eventHandler<planeGrid*>()),
        _visibleChanged(eventHandler<planeGrid*>())
    {
    }

    planeGrid::planeGrid(const planeGrid& original) :
        component(componentType::PLANE_GRID),
        _color(original._color),
        _lineThickness(original._lineThickness),
        _opacity(original._opacity),
        _isVisible(original._isVisible),
        _clippingPlanes(original._clippingPlanes),
        _colorChanged(eventHandler<planeGrid*>()),
        _lineThicknessChanged(eventHandler<planeGrid*>()),
        _opacityChanged(eventHandler<planeGrid*>()),
        _visibleChanged(eventHandler<planeGrid*>())
    {
    }

    planeGrid::~planeGrid()
    {
    }

    void planeGrid::setColor(const color& value)
    {
        if (_color == value)
            return;

        _color = value;
        _colorChanged.raise(this);
    }

    void planeGrid::setLineThickness(const float value)
    {
        if (_lineThickness == value)
            return;

        _lineThickness = value;
        _lineThicknessChanged.raise(this);
    }

    void planeGrid::setOpacity(const float value)
    {
        if (_opacity == value)
            return;

        _opacity = value;
        _opacityChanged.raise(this);
    }

    CORE_API void planeGrid::setVisibilityFactor(const float value)
    {
        if (_visibilityFactor == value)
            return;

        _visibilityFactor = value;
        _opacityChanged.raise(this);
    }

    void planeGrid::addClippingPlane(clippingPlane* plane)
    {
        _clippingPlanes.push_back(plane);
        _clippingPlanesOpacities[plane] = 0.0f;
        _clippingPlanesDistances[plane] = clippingDistance::negative;
        _clippingPlanesChanged.raise(this);
    }

    void planeGrid::removeClippingPlane(clippingPlane* plane)
    {
        phi::removeIfContains(_clippingPlanes, plane);
        _clippingPlanesOpacities.erase(plane);
        _clippingPlanesDistances.erase(plane);
        _clippingPlanesChanged.raise(this);
    }

    void planeGrid::setClippingPlaneOpacity(clippingPlane* clippingPlane, float opacity)
    {
        _clippingPlanesOpacities[clippingPlane] = opacity;
        _clippingPlanesChanged.raise(this);
    }

    void planeGrid::setClippingPlaneDistance(clippingPlane* clippingPlane, clippingDistance::clippingDistance distance)
    {
        _clippingPlanesDistances[clippingPlane] = distance;
        _clippingPlanesChanged.raise(this);
    }

    void planeGrid::show()
    {
        if (_isVisible)
            return;

        _isVisible = true;
        _visibleChanged.raise(this);
    }

    void planeGrid::hide()
    {
        if (!_isVisible)
            return;

        _isVisible = false;
        _visibleChanged.raise(this);
    }

    component* planeGrid::clone() const
    {
        auto p = static_cast<const planeGrid*>(this);
        return new planeGrid(*p);
    }
}