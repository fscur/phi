#include <precompiled.h>
#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(gl* gl) :
        _gl(gl),
        _program(nullptr)
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

        _program = _gl->shadersManager->load("geometryPass", attribs);
        _program->addUniform(0, "textureArrays");
    }

    void gBufferRenderPass::update()
    {
        _program->bind();

        if (_gl->currentState.useBindlessTextures)
            _program->setUniform(0, _gl->texturesManager->handles);
        else
            _program->setUniform(0, _gl->texturesManager->units);

        _program->unbind();
    }

    void gBufferRenderPass::render(const vector<batch*>& batches)
    {
        _program->bind();

        for (auto batch : batches)
            batch->render();

        _program->unbind();
    }
}