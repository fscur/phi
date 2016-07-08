#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>
#include <core\geometry.h>

#include <rendering\buffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\mappedVertexBuffer.h>
#include <rendering\mappedBuffer.h>
#include <rendering\multiDrawMappedBuffer.h>
#include <rendering\drawElementsIndirectCmd.h>
#include <rendering\multiDrawElementsIndirectCommandBuffer.h>
#include <rendering\vertexArrayObject.h>

#include "meshInstance.h"

namespace phi
{
    class batch
    {
    private:
        const size_t MAX_VBO_SIZE = 1 * 1024 * 1024;
        size_t _freeSpace;
        GLint _vboOffset;
        GLint _eboOffset;
        GLint _indicesOffset;
        GLint _verticesOffset;
        GLint _instancesOffset;
        GLuint _drawCount;
        GLuint _objectsCount;
        bool _empty;

        //vector<drawElementsIndirectCmd> _drawCmdsBufferData;
        /*buffer* _drawCmdBuffer;*/
        multiDrawElementsIndirectCommandBuffer* _multiDrawCommandsBuffer;

        vector<geometry*> _geometries;
        map<geometry*, vector<const meshInstance*>> _instances;


        vertexArrayObject* _vao;
        mappedVertexBuffer<const geometry*, vertex>* _vbo;
        mappedBuffer<const geometry*, uint>* _ebo;
        //vertexBuffer* _vbo;
        //buffer* _ebo;

        multiDrawMappedBuffer<const meshInstance*, uint>* _materialsIdsBuffer;
        multiDrawMappedBuffer<const meshInstance*, mat4>* _modelMatricesBuffer;
        multiDrawMappedBuffer<const meshInstance*, vec4>* _selectionBuffer;

    private:
        void createVao(size_t vboSize);
        void createBuffers(size_t vboSize);
        void initializeVao();
        void initialize(const meshInstance* instance);
        void addGeometry(geometry* geometry);
        void addInstance(const meshInstance* instance);
        void updateMultiDrawCommands();

        bool doesGeometryFitInVao(const meshInstance * instance);

    public:
        SCENE_RENDERING_API batch();
        SCENE_RENDERING_API ~batch();
        SCENE_RENDERING_API bool canAdd(const meshInstance* instance);
        SCENE_RENDERING_API void add(const meshInstance* instance);
        SCENE_RENDERING_API void remove(const meshInstance* instance);
        SCENE_RENDERING_API void update(const meshInstance* instance);
        SCENE_RENDERING_API void updateSelectionBuffer(const meshInstance* instance, bool isSelected);
        SCENE_RENDERING_API void updateTransformBuffer(const meshInstance* instance);

        SCENE_RENDERING_API vertexArrayObject* getVao() { return _vao; }
    };
}