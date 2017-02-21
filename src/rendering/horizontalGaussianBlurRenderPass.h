#include <phi.h>
#include "renderingApi.h"

#include "renderPass.h"
#include "framebufferAllocator.h"

namespace phi
{
    class horizontalGaussianBlurRenderPass :
        public renderPass
    {
    public:
        horizontalGaussianBlurRenderPass(
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);

        ~horizontalGaussianBlurRenderPass();

        void onBeginRender() override;
        void onEndRender() override;

    private:
        renderTarget* _defaultRenderTarget;
        renderTarget* _finalImageRenderTarget;
        vertexArrayObject* _quadVao;
    };
}