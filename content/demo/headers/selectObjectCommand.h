#ifndef _SELECT_OBJECT_COMMAND_H_
#define _SELECT_OBJECT_COMMAND_H_

#include <vector>
#include <map>
#include "command.h"
#include "sceneObject.h"

class selectObjectCommand :
    public command
{
private:
    phi::sceneObject* _object;
    unsigned int* _meshID;
    std::map<phi::sceneObject*, std::vector<phi::sceneMesh*>> _selectedObjects;
    bool _initialized;

private:
    void init();
    void unselectAllSceneObjects();

protected:
    void execute() override;
    void executeUndo() override;

public:
    selectObjectCommand(phi::sceneObject* object, unsigned int* meshID);
};

#endif