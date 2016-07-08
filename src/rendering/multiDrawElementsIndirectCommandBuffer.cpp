#include <precompiled.h>
#include "multiDrawElementsIndirectCommandBuffer.h"

#include "drawElementsIndirectCmd.h"

namespace phi
{
    multiDrawElementsIndirectCommandBuffer::multiDrawElementsIndirectCommandBuffer()
    {
    }

    multiDrawElementsIndirectCommandBuffer::~multiDrawElementsIndirectCommandBuffer()
    {
    }

    void multiDrawElementsIndirectCommandBuffer::add(const geometry * geometry)
    {
        auto geometryWhateverIterator = _geometriesWhatevers.find(geometry);
        auto containsGeometry = geometryWhateverIterator != _geometriesWhatevers.end();

        if (containsGeometry)
        {
            geometryWhateverIterator->second->instanceCount++;
        }
        else
        {
            auto geometryWhatever = new multiDrawElementsIndirectCommandBuffer::geometryWhatever(geometry);
            _geometries.push_back(geometryWhatever);
            _geometriesWhatevers[geometry] = geometryWhatever;
        }
    }

    void multiDrawElementsIndirectCommandBuffer::create()
    {
        _drawCmdBuffer = new buffer("drawIndirect", bufferTarget::drawIndirect);

        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        vector<drawElementsIndirectCmd> drawCmdsBufferData;

        for (auto& geometryWathever : _geometries)
        {
            auto geometry = geometryWathever->geometry;
            auto instanceCount = geometryWathever->instanceCount;
            auto indicesCount = geometry->indicesCount;

            auto drawCmd = drawElementsIndirectCmd(
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
        _drawCmdBuffer->data(sizeof(drawElementsIndirectCmd) * drawCmdsBufferData.size(), drawCmdData, bufferDataUsage::dynamicDraw);
    }

    void multiDrawElementsIndirectCommandBuffer::bind()
    {
        _drawCmdBuffer->bind();
    }

    void multiDrawElementsIndirectCommandBuffer::unbind()
    {
        _drawCmdBuffer->unbind();
    }
}