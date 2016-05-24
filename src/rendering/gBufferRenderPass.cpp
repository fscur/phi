#include <precompiled.h>
#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(gl* gl) :
        _gl(gl),
        _shader(nullptr)
    {
        initShader();
    }

    gBufferRenderPass::~gBufferRenderPass()
    {
    }

    void gBufferRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");
        attribs.push_back("inMaterialId");
        attribs.push_back("inModelMatrix");

        _shader = _gl->shadersManager->load("geometryPass", attribs);
        _shader->addUniform(0, "textureArrays");
    }

    void gBufferRenderPass::update()
    {
        _shader->bind();

        if (_gl->currentState.useBindlessTextures)
            _shader->setUniform(0, _gl->texturesManager->handles);
        else
            _shader->setUniform(0, _gl->texturesManager->units);

        _shader->unbind();
    }

    void gBufferRenderPass::render(const vector<batch*>& batches)
    {
        _shader->bind();

        for (auto batch : batches)
            batch->render();

        _shader->unbind();
    }
}