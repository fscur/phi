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
        texture* _thumbnail;
    public:
        RENDERING_API model(std::string name, std::string path);

        RENDERING_API std::vector<mesh*> getMeshes() const { return _meshes; }
        RENDERING_API texture* getThumbnail() const { return _thumbnail; }

        RENDERING_API void setThumbnail(texture* value) { _thumbnail = value; }

        RENDERING_API void addMesh(mesh* mesh);
        RENDERING_API static model* fromFile(std::string fileName);
    };
}
#endif