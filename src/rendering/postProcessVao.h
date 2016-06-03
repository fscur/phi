#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "vao.h"

namespace phi
{
    class postProcessVao 
        : public vao
    {
    private:
        void create();
    public:
        RENDERING_API postProcessVao();
        RENDERING_API ~postProcessVao();
        RENDERING_API void render() override;
    };
}