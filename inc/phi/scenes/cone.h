#ifndef _PHI_CONE_H_
#define _PHI_CONE_H_

#include "phi/core/globals.h"

#include "phi/rendering/model.h"

#include "phi/scenes/scenes.h"

namespace phi
{
    class cone
    {
    private:
        model* _model;

    private:
        cone(){};
        mesh* create(unsigned int sectors);

    public:
        SCENES_API cone(unsigned int sectors, material* material);
        SCENES_API ~cone(void);

        SCENES_API model* getModel() const { return _model; }
    };
}

#endif