#include <precompiled.h>
#include "multiDrawElementsIndirectCommandBuffer.h"

#include "drawElementsIndirectCommand.h"

namespace phi
{
    multiDrawElementsIndirectCommandBuffer::multiDrawElementsIndirectCommandBuffer() :
        _drawCmdBuffer(new buffer("drawIndirect", bufferTarget::drawIndirect))
    {
    }

    multiDrawElementsIndirectCommandBuffer::~multiDrawElementsIndirectCommandBuffer()
    {
        clear();
        safeDelete(_drawCmdBuffer);
    }

    void multiDrawElementsIndirectCommandBuffer::add(const geometry * geometry)
    {
        auto geometryInstanceIterator = _geometriesInstances.find(geometry);
        auto containsGeometry = geometryInstanceIterator != _geometriesInstances.end();

        if (containsGeometry)
        {
            geometryInstanceIterator->second->instanceCount++;
        }
        else
        {
            auto geometryInstance = new multiDrawElementsIndirectCommandBuffer::geometryInstance(geometry);
            _geometries.push_back(geometryInstance);
            _geometriesInstances[geometry] = geometryInstance;
        }
    }

    void multiDrawElementsIndirectCommandBuffer::create()
    {
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        vector<drawElementsIndirectCommand> drawCmdsBufferData;

        for (auto& geometryWathever : _geometries)
        {
            auto geometry = geometryWathever->geometry;
            auto instanceCount = geometryWathever->instanceCount;
            auto indicesCount = geometry->indicesCount;

            auto drawCmd = drawElementsIndirectCommand(
                indicesCount,
                instanceCount,
                indicesOffset,
                verticesOffset,
                instancesOffset);

            indicesOffset += indicesCount;
            verticesOffset += geometry->verticesCount;
            instancesOffset += instanceCount;

            drawCmdsBufferData.push_back(drawCmd);
        }

        auto drawCmdData = drawCmdsBufferData.size() > 0 ? &drawCmdsBufferData[0] : nullptr;
        _drawCmdBuffer->data(sizeof(drawElementsIndirectCommand) * drawCmdsBufferData.size(), drawCmdData, bufferDataUsage::dynamicDraw);
    }

    void multiDrawElementsIndirectCommandBuffer::bind()
    {
        _drawCmdBuffer->bind();
    }

    void multiDrawElementsIndirectCommandBuffer::unbind()
    {
        _drawCmdBuffer->unbind();
    }

    void multiDrawElementsIndirectCommandBuffer::clear()
    {
        for (auto& instance : _geometries)
            safeDelete(instance);

        _geometries.clear();
        _geometriesInstances.clear();
    }
}