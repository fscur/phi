#ifndef _PHI_SCENE_MESH_
#define _PHI_SCENE_MESH_

namespace phi
{
    class sceneMesh
    {
    private:
        bool _isSelected;
        GLuint _meshID;

    public:
        SCENES_API sceneMesh(GLuint meshID) { _meshID = meshID; _isSelected = false; }

        SCENES_API bool getIsSelected() const { return _isSelected; }
        SCENES_API GLuint getMeshID() const { return _meshID; }

        SCENES_API void setIsSelected(bool value) { _isSelected = value; }
        SCENES_API void setMeshID(GLuint value) { _meshID = value; }
    };
}

#endif