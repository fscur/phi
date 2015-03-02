#ifndef _PHI_MODEL_H_
#define _PHI_MODEL_H_

#include "mesh.h"

namespace phi
{
	class model :
        public resource
	{
    private:
        std::vector<mesh*> _meshes;
    public:
        RENDERING_API model(std::string name);
        RENDERING_API model(std::string name, std::string path, std::vector<mesh*> meshes);
        RENDERING_API std::vector<mesh*> getMeshes() const { return _meshes; }
        RENDERING_API void addMesh(mesh* mesh);
    };
}
#endif