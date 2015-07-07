#ifndef _PHI_SCENE_MESH_
#define _PHI_SCENE_MESH_

#include "scenes.h"
#include "material.h"

namespace phi
{
    class sceneMesh
    {
    private:
        bool _isSelected;
        GLuint _meshID;
        std::string _materialName;
        material* _material;

    public:
        SCENES_API sceneMesh(GLuint meshID) { _meshID = meshID; _isSelected = false; }

        SCENES_API bool getIsSelected() const { return _isSelected; }
        SCENES_API GLuint getMeshID() const { return _meshID; }
        SCENES_API material* getMaterial() const { return _material; }

        SCENES_API void setIsSelected(bool value) { _isSelected = value; }
        SCENES_API void setMeshID(GLuint value) { _meshID = value; }
        SCENES_API void setMaterial(material* value);
    };
}

#endif