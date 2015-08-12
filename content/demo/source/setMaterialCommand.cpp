#include "setMaterialCommand.h"

setMaterialCommand::setMaterialCommand(phi::sceneMesh* mesh, phi::material* material) :
    command()
{
    _mesh = mesh;
    _material = material;
    _previousMaterial = mesh->getMaterial();
}

void setMaterialCommand::execute()
{
    _mesh->setMaterial(_material);
}

void setMaterialCommand::executeUndo()
{
    _mesh->setMaterial(_previousMaterial);
}