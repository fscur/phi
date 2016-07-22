#pragma once

#include <phi.h>

#include <core\transform.h>

namespace phi
{
    class transformTranslator
    {
    private:
        transform* _transform;

    public:
        transformTranslator() {}
        ~transformTranslator() {}

        virtual void setTransform(transform* transform)
        {
            _transform = transform;
        }

        virtual void translateTransform(vec3 offset)
        {
            _transform->translate(offset);
        }
    };
}