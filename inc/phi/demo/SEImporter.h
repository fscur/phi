#ifndef _SE_IMPORTER_
#define _SE_IMPORTER_

#include "phi/rendering/material.h"
#include "phi/rendering/model.h"
#include "phi/rendering/vertex.h"

#include "phi/scenes/sceneObject.h"

#include "phi/core/aabb.h"

#import <phi/demo/SEFacet.tlb>
#import <phi/demo/SEReader.tlb>

#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>
#include <windows.h>
#include <tchar.h>

const CLSID CLSID_FacetReaderClass =
{ 0x0B70E783, 0xC796, 0x11D4,{ 0xA3, 0xB1, 0x00, 0xC0, 0x4F, 0xA1, 0x54, 0xDE } };

const CLSID CLSID_IFacetReader =
{ 0x0B70E782, 0xC796, 0x11D4,{ 0xA3, 0xB1, 0x00, 0xC0, 0x4F, 0xA1, 0x54, 0xDE } };

const CLSID CLSID_StyleLibraryClass =
{ 0x27DC08BA, 0x3E4A, 0x11D5,{ 0xA4, 0x04, 0x00, 0xC0, 0x4F, 0xA1, 0x54, 0xDE } };

const CLSID CLSID_IStyleLibrary =
{ 0xC5890808, 0x43B5, 0x11D5,{ 0xA4, 0x0A, 0x00, 0xC0, 0x4F, 0xA1, 0x54, 0xDE } };

const CLSID CLSID_SEFacet =
{ 0x0F1764FC, 0xC6CD, 0x11D4,{ 0xA3, 0xB0, 0x00, 0xC0, 0x4F, 0xA1, 0x54, 0xDE } };

const CLSID CLSID_SEFileReader =
{ 0xC9165411, 0x2A39, 0x11D3,{ 0x92, 0x73, 0x00, 0xC0, 0x4F, 0x79, 0xBE, 0x98 } };

const CLSID CLSID_ISEFileReader =
{ 0xC9165410, 0x2A39, 0x11D3,{ 0x92, 0x73, 0x00, 0xC0, 0x4F, 0x79, 0xBE, 0x98 } };

typedef HRESULT(CALLBACK *DLLGETCLASSOBJECT) (REFCLSID rclsid, REFIID riid, LPVOID* ppv);
typedef HRESULT(CALLBACK *DLLCANUNLOADNOW)(void);

class SEImporter
{
private:
    class octree
    {
    private:

        struct positionData
        {
            unsigned int index;
            phi::vertex vertex;
        };

        phi::aabb _aabb;
        std::vector<glm::vec3> _positions;
        std::map<unsigned int, std::vector<positionData>> _items;

        unsigned int* _indexPtr;
        unsigned int _index;

        octree* _parent;
        octree* _children[8];
        bool _isSubdivided = false;
        unsigned int _maxItems;
        unsigned int _maxLevels;
        unsigned int _currentLevel;

        bool insertIntoChildren(phi::vertex vertex, unsigned int &index);
        void insertIntoChildren(glm::vec3 position, std::vector<positionData> posDataList);
        phi::aabb createAabb(phi::aabb parentAabb, unsigned int index);
        void subdivide();
    public:
        octree(phi::aabb aabb, unsigned int maxLevels, unsigned int totalItems);
        octree(octree* parent, unsigned int index, unsigned int maxItems);
        bool insert(phi::vertex vertex, unsigned int &index);
    };

    struct SEStyle
    {
    public:
        unsigned int ID;
        std::string Name;
        std::string Description;
        std::string BumpFileName;
        int BumpUnits;
        float BumpRotation;
        float BumpScaleX;
        float BumpScaleY;
        float BumpOffsetX;
        float BumpOffsetY;
        float BumpHeight;
        int BumpMirror;
        phi::color Diffuse;
        phi::color Specular;
        phi::color Ambient;
        phi::color Emission;
        float Shininess;
        float Alpha;
        float Reflectivity;
        float Refraction;
        int Shadows;
        std::string TextureFileName;
        int TextureUnits;
        float TextureRotation;
        float TextureScaleX;
        float TextureScaleY;
        float TextureOffsetX;
        float TextureOffsetY;
        float TextureWeight;
        int TextureMirror;
        bool TextureTransBG;
        int TextureTransColor;
    };

    struct SEVertex
    {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;

    public:
        SEVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord) :
            position(position), normal(normal), texCoord(texCoord) {}

        ~SEVertex() {}
    };

    class SEStrip
    {
    private:
        unsigned int _id;
        std::vector<SEVertex>* _vertices;
    public:
        SEStrip(unsigned int id) :
            _id(id)
        {
            _vertices = new std::vector<SEVertex>();
        }

        ~SEStrip() {}

        std::vector<SEVertex>* getVertices() const { return _vertices; }
    };

    class SEFace
    {
    private:
        unsigned int _id;
        SEStyle* _style;
        std::vector<SEStrip>* _strips;
    public:
        SEFace(unsigned int id) :
            _id(id)
        {
            _strips = new std::vector<SEStrip>();
        }

        ~SEFace() {}

        std::vector<SEStrip>* getStrips() const { return _strips; }

        void setStyle(SEStyle* style) { _style = style; }
        SEStyle* getStyle() const { return _style; }
    };

    class SEBody
    {
    private:
        unsigned int _id;
        SEStyle* _style;
        std::vector<SEFace>* _faces;

    public:
        SEBody(unsigned int id) :
            _id(id)
        {
            _faces = new std::vector<SEFace>();
        }

        ~SEBody() {}

        std::vector<SEFace>* getFaces() const { return _faces; }

        void setStyle(SEStyle* value) { _style = value; }
        SEStyle* getStyle() const { return _style; }
    };

    class SEPart
    {
    public:
        struct meshData
        {
            std::string name;
            octree* o;
            std::vector<phi::vertex> vs;
            std::vector<GLuint>* is;
            GLuint index = 0;

            meshData(phi::aabb aabb, unsigned int totalVertices)
            {
                o = new octree(aabb, 4, totalVertices);
                is = new std::vector<GLuint>();
            }

            ~meshData()
            {
                delete o;
            }
        };
    private:
        std::string _name;
        std::vector<SEBody>* _bodies;
        std::map<unsigned long, SEStyle*>* _styles;
        phi::aabb _aabb;
        unsigned int _totalVertices;

        std::map<unsigned long, meshData*>* _meshDataPerStyle;
        std::map<unsigned long, phi::material*>* _styleMaterials;

    public:
        SEPart(std::string name)
        {
            _name = name;
            _bodies = new std::vector<SEBody>();
            _styles = new std::map<unsigned long, SEStyle*>();
        }

        ~SEPart()
        {
            for each (auto pair in (*_styles))
                delete pair.second;

            delete _styles;
            delete _bodies;
        }

        std::string getName() const { return _name; }
        std::vector<SEBody>* getBodies() const { return _bodies; }
        std::map<unsigned long, SEStyle*>* getStyles() const { return _styles; }
        phi::aabb getAabb() const { return _aabb; }
        unsigned int getTotalVertices() const { return _totalVertices; }
        std::map<unsigned long, meshData*>* getMeshDataPerStyle() const { return _meshDataPerStyle; }
        std::map<unsigned long, phi::material*>* getStyleMaterials() const { return _styleMaterials; }

        void setAabb(phi::aabb value) { _aabb = value; }
        void setTotalVertices(unsigned int value) { _totalVertices = value; };
        void setMeshDataPerStyle(std::map<unsigned long, meshData*>* value) { _meshDataPerStyle = value; }
        void setStyleMaterials(std::map<unsigned long, phi::material*>* value) { _styleMaterials = value; }

    };

    class SEAssembly
    {
    private:
        std::string _name;
        std::vector<SEPart*>* _parts;
        std::vector<SEAssembly*>* _subAssemblies;

    public:
        SEAssembly(std::string name)
        {
            _name = name;
            _parts = new std::vector<SEPart*>();
            _subAssemblies = new std::vector<SEAssembly*>();
        }

        ~SEAssembly()
        {
            delete _parts;
            delete _subAssemblies;
        }

        std::string getName() const { return _name; }
        std::vector<SEPart*>* getParts() const { return _parts; }
        std::vector<SEAssembly*>* getSubAssemblies() { return _subAssemblies; }
    };

    class StyleLibrary
    {
    private:
        HINSTANCE _dll;
        SEFACETLib::IStyleLibrary* _instance;
    public:
        StyleLibrary();
        ~StyleLibrary();
        SEFACETLib::IStyleLibrary* getInstance() const { return _instance; }
    };

    class FacetReader
    {
    private:
        HINSTANCE _dll;
        SEFACETLib::IFacetReader* _instance;
    public:
        FacetReader();
        ~FacetReader();
        SEFACETLib::IFacetReader* getInstance() const { return _instance; }
    };

    class FileReader
    {
    private:
        HINSTANCE _dll;
        SEREADERLib::ISEFileReader* _instance;
    public:
        FileReader();
        ~FileReader();
        SEREADERLib::ISEFileReader* getInstance() const { return _instance; }
    };

private:
    unsigned char* _defaultTextureData;
    phi::texture* _defaultDiffuseTexture;
    phi::texture* _defaultSpecularTexture;
    phi::texture* _defaultNormalTexture;
    phi::texture* _defaultEmissiveTexture;
    SEStyle* _defaultStyle;

    std::map<std::string, SEAssembly*> _assemblies;
    std::map<std::string, SEPart*> _parts;
    std::map<SEPart*, std::vector<glm::mat4>> _partOccurrences;

private:
    SEStyle* loadStyle(SEFACETLib::IStyleLibrary* styleLib, unsigned long styleID);
    phi::material* createMaterialFromStyle(SEStyle* style);

    void buildMeshData(SEPart* part);

    glm::mat4 SEImporter::getModelMatrix(SEREADERLib::IAttachment* attachment);

    SEAssembly* loadAssembly(std::string fileName, glm::mat4 parentModelMatrix);
    SEPart* loadPart(std::string fileName, SEStyle* style);

public:
    SEImporter();
    ~SEImporter();
    phi::sceneObject* import(std::string fileName);
};

#endif