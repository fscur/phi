#ifndef _SET_MATERIAL_COMMAND_H_
#define _SET_MATERIAL_COMMAND_H_

#include "sceneObject.h"
#include "command.h"

class setMaterialCommand :
    public command
{
private:
    phi::sceneMesh* _mesh;
    phi::material* _previousMaterial;
    phi::material* _material;

protected:
    void execute() override;
    void executeUndo() override;

public:
    setMaterialCommand(phi::sceneMesh* mesh, phi::material* material);
};

#endif