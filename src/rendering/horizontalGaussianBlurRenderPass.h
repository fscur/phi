#include <phi.h>
#include "renderingApi.h"

#include "renderPass.h"
#include "framebufferAllocator.h"

namespace phi
{
    class horizontalGaussianBlurRenderPass
    {
    public:
        horizontalGaussianBlurRenderPass() = delete;
        ~horizontalGaussianBlurRenderPass() = delete;

        RENDERING_API static renderPass* configure(
            const resolution& resolution,
            const string& shadersPath,
            framebufferAllocator* framebufferAllocator);
    };
}