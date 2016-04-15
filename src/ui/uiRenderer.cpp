#include <precompiled.h>
#include "uiRenderer.h"
#include "label.h"

namespace phi
{
    uiRenderer::uiRenderer(const gl* const gl, float w, float h) :
        _gl(gl),
        _controlsRenderPass(new controlsRenderPass(gl, w, h)),
        _textRenderPass(new textRenderPass(gl, w, h)),
        _w(w),
        _h(h)
    {   
    }

    uiRenderer::~uiRenderer()
    {
        safeDelete(_controlsRenderPass);
        safeDelete(_textRenderPass);
    }

    void uiRenderer::update(const vector<control*>& controls)
    {
        _controlsRenderPass->update(controls);

        vector<textRenderData> texts;

        for (auto control : controls)
        {
            switch (control->type)
            {
                case controlType::label:
                {
                    textRenderData data;
                    auto label = static_cast<phi::label*>(control);
                    data.font = label->getFont();
                    data.position = label->getPosition();
                    data.text = label->getText();
                    texts.push_back(data);
                }
                break;
            default:
                break;
            }
        }

        _textRenderPass->update(texts);
    }
    
    void uiRenderer::update(const control* const control)
    {
    }

    void uiRenderer::render() const
    {   
        _controlsRenderPass->render();
        _textRenderPass->render();
    }
}