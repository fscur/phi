#include "phi/demo/SEImporter.h"

#include "phi/core/globals.h"
#include "phi/io/path.h"

#include <algorithm>
#include <iostream>
#include <tchar.h>
#include <chrono>
#include <limits>
#include <objbase.h>

BSTR ConvertMBSToBSTR(const std::string& str)
{
    int wslen = ::MultiByteToWideChar(CP_ACP, 0,
        str.data(), (int)str.length(),
        NULL, 0);

    BSTR wsdata = ::SysAllocStringLen(NULL, wslen);
    ::MultiByteToWideChar(CP_ACP, 0,
        str.data(), (int)str.length(),
        wsdata, wslen);
    return wsdata;
}

std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
    int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

    std::string dblstr(len, '\0');
    len = ::WideCharToMultiByte(CP_ACP, 0,
        pstr, wslen,
        &dblstr[0], len,
        NULL, NULL);

    return dblstr;
}

std::string ConvertBSTRToMBS(BSTR bstr)
{
    int wslen = ::SysStringLen(bstr);
    return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

SEImporter::octree::octree(phi::aabb aabb, unsigned int maxLevels, unsigned int totalItems)
{
    _index = 0;
    _indexPtr = &_index;

    _currentLevel = 0;
    _parent = nullptr;
    _aabb = aabb;
    _maxLevels = maxLevels;

    _maxItems = (unsigned int)(totalItems / (unsigned int)pow<float>(8.0f, (float)_maxLevels));

    if (_maxItems < 10)
        _maxItems = 10;
}

SEImporter::octree::octree(octree* parent, unsigned int index, unsigned int maxItems)
{
    _indexPtr = parent->_indexPtr;
    _currentLevel = parent->_currentLevel + 1;
    _parent = parent;
    _aabb = createAabb(parent->_aabb, index);
    _maxItems = maxItems;
}

phi::aabb SEImporter::octree::createAabb(phi::aabb parentAabb, unsigned int index)
{
    auto width = parentAabb.getHalfWidth();
    auto height = parentAabb.getHalfHeight();
    auto depth = parentAabb.getHalfDepth();

    auto minX = 0.0f;
    auto minY = 0.0f;
    auto minZ = 0.0f;
    auto maxX = 0.0f;
    auto maxY = 0.0f;
    auto maxZ = 0.0f;

    auto parentAabbMin = parentAabb.getMin();
    auto parentAabbMax = parentAabb.getMax();

    auto xIndex = index & 4;
    auto yIndex = index & 2;
    auto zIndex = index & 1;

    xIndex = xIndex >> 2;
    yIndex = yIndex >> 1;

    if (xIndex == 0)
    {
        minX = parentAabbMin.x;
        maxX = parentAabbMin.x + width;
    }
    else
    {
        minX = parentAabbMin.x + width;
        maxX = parentAabbMax.x;
    }

    if (yIndex == 0)
    {
        minY = parentAabbMin.y;
        maxY = parentAabbMin.y + height;
    }
    else
    {
        minY = parentAabbMin.y + height;
        maxY = parentAabbMax.y;
    }

    if (zIndex == 0)
    {
        minZ = parentAabbMin.z;
        maxZ = parentAabbMin.z + depth;
    }
    else
    {
        minZ = parentAabbMin.z + depth;
        maxZ = parentAabbMax.z;
    }

    auto min = phi::vec3(minX, minY, minZ);
    auto max = phi::vec3(maxX, maxY, maxZ);

    return phi::aabb(min, max);
}

void SEImporter::octree::insertIntoChildren(phi::vec3 position, std::vector<positionData> posDataList)
{
    auto pos = position;

    pos -= _aabb.getMin();

    auto x = (int)(pos.x / _aabb.getHalfWidth());
    auto y = (int)(pos.y / _aabb.getHalfHeight());
    auto z = (int)(pos.z / _aabb.getHalfDepth());

    auto childOctreeIndex = 0;

    childOctreeIndex |= x;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= y;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= z;

    auto childOctree = _children[childOctreeIndex];

    /*for (unsigned int i = 0; i < 8; i++)
    {
        auto childOctree = _children[i];

        if (childOctree->_aabb.contains(position))
        {
            childOctree->_positions.push_back(position);
            childOctree->_items[childOctree->_positions.size() - 1] = posDataList;
            break;
        }
    }*/
}

bool SEImporter::octree::insertIntoChildren(phi::vertex vertex, unsigned int &index)
{
    auto pos = vertex.getPosition();

    pos -= _aabb.getMin();

    auto x = (int)(pos.x / _aabb.getHalfWidth());
    auto y = (int)(pos.y / _aabb.getHalfHeight());
    auto z = (int)(pos.z / _aabb.getHalfDepth());

    auto childOctreeIndex = 0;

    childOctreeIndex |= x;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= y;
    childOctreeIndex = childOctreeIndex << 1;
    childOctreeIndex |= z;

    auto childOctree = _children[childOctreeIndex];

    return childOctree->insert(vertex, index);

    /*for (unsigned int i = 0; i < 8; i++)
    {
        auto childOctree = _children[i];

        if (childOctree->_aabb.contains(vertex.getPosition()))
        {
            return childOctree->insert(vertex, index);
            break;
        }
    }

    return false;*/
}

void SEImporter::octree::subdivide()
{
    for (unsigned int i = 0; i < 8; i++)
        _children[i] = new octree(this, i, _maxItems);

    auto s = _positions.size();
    for (auto i = 0; i < s; i++)
    {
        insertIntoChildren(_positions[i], _items[i]);
    }

    _items.clear();
    _positions.clear();
    _isSubdivided = true;
}

bool SEImporter::octree::insert(phi::vertex vertex, unsigned int &index)
{
    auto pos = vertex.getPosition();

    if (!_isSubdivided)
    {
        auto s = _positions.size();

        if (s >= _maxItems)
        {
            subdivide();
            return insertIntoChildren(vertex, index);
        }

        bool found = false;
        unsigned int posIndex = 0;

        for (unsigned int i = 0; i < s; i++)
        {
            if (_positions[i] == pos)
            {
                found = true;
                posIndex = i;
                break;
            }
        }

        if (!found)
        {
            auto posData = positionData();

            posData.index = (*_indexPtr)++;
            posData.vertex = vertex;

            std::vector<positionData> posDataList;
            posDataList.push_back(posData);

            _positions.push_back(pos);
            auto pSize = (unsigned int)_positions.size();
            _items[pSize - 1] = posDataList;

            index = posData.index;

            return true;
        }
        else if (found)
        {
            auto posDataList = &(_items[posIndex]);
            auto s = posDataList->size();

            for (unsigned int i = 0; i < s; i++)
            {
                auto pData = (*posDataList)[i];
                if (pData.vertex == vertex)
                {
                    index = pData.index;
                    return false;
                }
            }

            auto posData = positionData();

            posData.index = (*_indexPtr)++;
            posData.vertex = vertex;

            posDataList->push_back(posData);

            index = posData.index;
            return true;
        }

        return false;
    }
    else
    {
        return insertIntoChildren(vertex, index);
    }

    return false;
}

SEImporter::StyleLibrary::StyleLibrary()
{
    _dll = LoadLibrary(_T("SEFacet.dll"));
    DLLGETCLASSOBJECT dllGetClassObject;
    IUnknown* pIUnknown = NULL;
    HRESULT hr;

    if (_dll == NULL)
        std::cout << "Unable to load SEFacet.dll" << std::endl;
    else
    {
        dllGetClassObject = (DLLGETCLASSOBJECT)GetProcAddress(_dll, "DllGetClassObject");

        if (dllGetClassObject == NULL)
            std::cout << "Unable to load DllGetClassObject" << std::endl;

        hr = E_FAIL;
        IClassFactory* pClassFactory;

        hr = dllGetClassObject(CLSID_StyleLibraryClass, IID_IClassFactory, (void**)&pClassFactory);

        if (SUCCEEDED(hr))
        {
            hr = pClassFactory->CreateInstance(NULL, IID_IUnknown, (void**)&pIUnknown);
            pClassFactory->Release();
        }

        pClassFactory = NULL;
        phi::safeDelete(pClassFactory);

        pIUnknown->QueryInterface(CLSID_IStyleLibrary, (void**)&_instance);
    }
}

SEImporter::StyleLibrary::~StyleLibrary()
{
    CoFreeLibrary(_dll);
}

SEImporter::FacetReader::FacetReader()
{
    _dll = LoadLibrary(_T("SEFacet.dll"));
    DLLGETCLASSOBJECT dllGetClassObject;
    IUnknown* pIUnknown = NULL;
    HRESULT hr;

    if (_dll == NULL)
        std::cout << "Unable to load SEFacet.dll" << std::endl;
    else
    {
        dllGetClassObject = (DLLGETCLASSOBJECT)GetProcAddress(_dll, "DllGetClassObject");

        if (dllGetClassObject == NULL)
            std::cout << "Unable to load DllGetClassObject" << std::endl;

        hr = E_FAIL;
        IClassFactory* pClassFactory;

        hr = dllGetClassObject(CLSID_FacetReaderClass, IID_IClassFactory, (void**)&pClassFactory);

        if (SUCCEEDED(hr))
        {
            hr = pClassFactory->CreateInstance(NULL, IID_IUnknown, (void**)&pIUnknown);
            pClassFactory->Release();
        }

        pClassFactory = NULL;
        phi::safeDelete(pClassFactory);

        pIUnknown->QueryInterface(CLSID_IFacetReader, (void**)&_instance);
    }
}

SEImporter::FacetReader::~FacetReader()
{
    CoFreeLibrary(_dll);
}

SEImporter::FileReader::FileReader()
{
    _dll = LoadLibrary(_T("SEReader.dll"));
    DLLGETCLASSOBJECT dllGetClassObject;
    IUnknown* pIUnknown = NULL;
    HRESULT hr;

    if (_dll == NULL)
        std::cout << "Unable to load SEReader.dll" << std::endl;
    else
    {
        dllGetClassObject = (DLLGETCLASSOBJECT)GetProcAddress(_dll, "DllGetClassObject");

        if (dllGetClassObject == NULL)
            std::cout << "Unable to load DllGetClassObject" << std::endl;

        hr = E_FAIL;
        IClassFactory* pClassFactory;

        hr = dllGetClassObject(CLSID_SEFileReader, IID_IClassFactory, (void**)&pClassFactory);

        if (SUCCEEDED(hr))
        {
            hr = pClassFactory->CreateInstance(NULL, IID_IUnknown, (void**)&pIUnknown);
            pClassFactory->Release();
        }

        pClassFactory = NULL;
        phi::safeDelete(pClassFactory);

        pIUnknown->QueryInterface(CLSID_ISEFileReader, (void**)&_instance);
    }
}

SEImporter::FileReader::~FileReader()
{
    CoFreeLibrary(_dll);
}

SEImporter::SEImporter()
{
    byte* data = new unsigned char[4];
    data[0] = 255;
    data[1] = 255;
    data[2] = 255;
    data[3] = 255;

    _defaultDiffuseTexture =
        new phi::texture((uint)1, (uint)1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);

    _defaultSpecularTexture =
        new phi::texture((uint)1, (uint)1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);

    _defaultEmissiveTexture =
        new phi::texture((uint)1, (uint)1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);

    data[0] = 255;
    data[1] = 128;
    data[2] = 128;
    data[3] = 255;

    _defaultNormalTexture =
        new phi::texture((uint)1, (uint)1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, data);

    _defaultStyle = new SEStyle();
    _defaultStyle->ID = 0;
    _defaultStyle->Name = "default";
    _defaultStyle->Description = "default style";
    _defaultStyle->Alpha = 1.0f;
    _defaultStyle->Ambient = phi::color::white;
    _defaultStyle->BumpFileName = "";
    _defaultStyle->BumpHeight = 0.0f;
    _defaultStyle->BumpMirror = 0;
    _defaultStyle->BumpOffsetX = 0.0f;
    _defaultStyle->BumpOffsetY = 0.0f;
    _defaultStyle->BumpRotation = 0.0f;
    _defaultStyle->BumpScaleX = 0.0f;
    _defaultStyle->BumpScaleY = 0.0f;
    _defaultStyle->BumpUnits = 0;
    _defaultStyle->Diffuse = phi::color::white;
    _defaultStyle->Emission = phi::color::white;
    _defaultStyle->Reflectivity = 0.0f;
    _defaultStyle->Refraction = 0.0f;
    _defaultStyle->Shadows = 0;
    _defaultStyle->Shininess = 0.0f;
    _defaultStyle->Specular = phi::color::white;
    _defaultStyle->TextureFileName = "";
    _defaultStyle->TextureMirror = 0;
    _defaultStyle->TextureOffsetX = 0.0f;
    _defaultStyle->TextureOffsetY = 0.0f;
    _defaultStyle->TextureRotation = 0.0f;
    _defaultStyle->TextureScaleX = 0.0f;
    _defaultStyle->TextureScaleY = 0.0f;
    _defaultStyle->TextureTransBG = false;
    _defaultStyle->TextureTransColor = 0;
    _defaultStyle->TextureUnits = 0;
    _defaultStyle->TextureWeight = 0.0f;
}

SEImporter::~SEImporter()
{
    delete[] _defaultTextureData;
}

SEImporter::SEStyle* SEImporter::loadStyle(SEFACETLib::IStyleLibrary* styleLib, unsigned long styleID)
{
    HRESULT hr = E_FAIL;

    VARIANT_BOOL pVal = styleLib->GetStyleExists(styleID);

    bool exists = !(pVal == VARIANT_FALSE);

    if (!exists)
        return nullptr;

    BSTR name = styleLib->GetStyleName(styleID);
    BSTR description = styleLib->GetStyleDescription(styleID);
    BSTR bumpFileName;
    int bumpUnits;
    float bumpRotation;
    float bumpScaleX;
    float bumpScaleY;
    float bumpOffsetX;
    float bumpOffsetY;
    float bumpHeight;
    int bumpMirror;
    long diffuse;
    long specular;
    long ambient;
    long emission;
    float shininess;
    float alpha;
    float reflectivity;
    float refraction;
    int shadows;
    BSTR textureFileName;
    int textureUnits;
    float textureRotation;
    float textureScaleX;
    float textureScaleY;
    float textureOffsetX;
    float textureOffsetY;
    float textureWeight;
    int textureMirror;
    VARIANT_BOOL textureTransBG;
    long textureTransColor;

    styleLib->StyleBumpmap(
        styleID,
        &bumpFileName,
        &bumpUnits,
        &bumpRotation,
        &bumpScaleX,
        &bumpScaleY,
        &bumpOffsetX,
        &bumpOffsetY,
        &bumpHeight,
        &bumpMirror);

    styleLib->StyleSurface(
        styleID,
        &diffuse,
        &specular,
        &ambient,
        &emission,
        &shininess,
        &alpha);

    styleLib->StyleSurfaceEx(
        styleID,
        &reflectivity,
        &refraction,
        &shadows);

    styleLib->StyleTexture(
        styleID,
        &textureFileName,
        &textureUnits,
        &textureRotation,
        &textureScaleX,
        &textureScaleY,
        &textureOffsetX,
        &textureOffsetY,
        &textureWeight,
        &textureMirror,
        &textureTransBG,
        &textureTransColor);

    auto style = new SEStyle();

    style->ID = styleID;
    style->Name = ConvertBSTRToMBS(name);
    style->Description = ConvertBSTRToMBS(description);
    style->Alpha = alpha;
    style->Ambient = phi::color::fromRGBA(ambient);
    style->BumpFileName = ConvertBSTRToMBS(bumpFileName);
    style->BumpHeight = bumpHeight;
    style->BumpMirror = bumpMirror;
    style->BumpOffsetX = bumpOffsetX;
    style->BumpOffsetY = bumpOffsetY;
    style->BumpRotation = bumpRotation;
    style->BumpScaleX = bumpScaleX;
    style->BumpScaleY = bumpScaleY;
    style->BumpUnits = bumpUnits;
    style->Diffuse = phi::color::fromRGBA(diffuse);
    style->Emission = phi::color::fromRGBA(emission);
    style->Reflectivity = reflectivity;
    style->Refraction = refraction;
    style->Shadows = shadows;
    style->Shininess = shininess;
    style->Specular = phi::color::fromRGBA(specular);
    style->TextureFileName = ConvertBSTRToMBS(textureFileName);
    style->TextureMirror = textureMirror;
    style->TextureOffsetX = textureOffsetX;
    style->TextureOffsetY = textureOffsetY;
    style->TextureRotation = textureRotation;
    style->TextureScaleX = textureScaleX;
    style->TextureScaleY = textureScaleY;
    style->TextureTransBG = !(textureTransBG == VARIANT_FALSE);
    style->TextureTransColor = textureTransColor;
    style->TextureUnits = textureUnits;
    style->TextureWeight = textureWeight;

    return style;
}

phi::material* SEImporter::createMaterialFromStyle(SEStyle* style)
{
    auto diffuseTexture = _defaultDiffuseTexture;

    // TODO: if you are seeing these lines, and you are not Fernando, and you are not using Fernando PC's, Fernando did cagadinha
    //if (!style->TextureFileName.empty() && phi::path::exists(style->TextureFileName))
    //    diffuseTexture = phi::texture::fromFile(style->TextureFileName);

    auto normalTexture = _defaultNormalTexture;

    //if (!style->BumpFileName.empty() && phi::path::exists(style->BumpFileName))
    //    normalTexture = phi::texture::fromFile(style->BumpFileName);

    GUID guid;
    CoCreateGuid(&guid);

    auto material = new phi::material(
        diffuseTexture,
        normalTexture,
        _defaultSpecularTexture,
        _defaultEmissiveTexture,
        style->Ambient,
        style->Diffuse,
        style->Specular,
        style->Emission,
        0.2f,
        0.8f,
        0.0f,
        0.0f,
        0.0f,
        false
        );

    return material;
}

void SEImporter::buildMeshData(SEPart* part)
{
    /*auto styleMaterials = new std::map<unsigned long, phi::material*>();
    auto meshDataPerStyle = new std::map<unsigned long, SEPart::meshData*>();
    auto styles = part->getStyles();

    for each (auto pair in (*styles))
        (*styleMaterials)[pair.first] = createMaterialFromStyle(pair.second);

    struct vertexData
    {
        phi::vertex vertex;
        unsigned int index;
    };

    auto bodyCount = part->getBodies()->size();
    auto bodies = part->getBodies();

    auto partAabb = part->getAabb();
    auto partTotalVertices = part->getTotalVertices();

    for (unsigned int i = 0; i < bodyCount; i++)
    {
        auto faces = ((*bodies)[i]).getFaces();
        auto facesCount = faces->size();

        for (unsigned int j = 0; j < facesCount; j++)
        {
            auto style = ((*faces)[j]).getStyle();

            if (style == nullptr)
                style = _defaultStyle;

            auto faceStyleID = style->ID;
            auto it = meshDataPerStyle->find(faceStyleID);

            SEPart::meshData* data;

            auto addVertex = [&](vertexData &vData) -> void
            {
                if (data->o->insert(vData.vertex, vData.index))
                    data->vs.push_back(vData.vertex);

                data->is->push_back(vData.index);
            };

            if (it != meshDataPerStyle->end())
                data = (*meshDataPerStyle)[faceStyleID];
            else
            {
                (*meshDataPerStyle)[faceStyleID] = data = new SEPart::meshData(partAabb, partTotalVertices);
                data->name = part->getName() + "_" + style->Name;
            }

            auto strips = ((*faces)[j]).getStrips();
            auto stripsCount = strips->size();

            for (unsigned int k = 0; k < stripsCount; k++)
            {
                auto strip = (*strips)[k];
                auto vertices = strip.getVertices();
                auto verticesCount = vertices->size();

                auto vs = *vertices;

                auto v0 = vs[0];
                auto v1 = vs[1];
                auto v2 = vs[2];

                if (dot(cross((v1.position - v0.position), (v2.position - v0.position)), v0.normal) < 0)
                {
                    v2 = v1;
                    v1 = vs[2];
                }

                auto a = vertexData();
                a.vertex = phi::vertex(v0.position, v0.texCoord, v0.normal);
                addVertex(a);

                auto b = vertexData();
                b.vertex = phi::vertex(v1.position, v1.texCoord, v1.normal);
                addVertex(b);

                auto c = vertexData();
                c.vertex = phi::vertex(v2.position, v2.texCoord, v2.normal);
                addVertex(c);

                for (unsigned int l = 3; l < verticesCount; l++)
                {
                    a = b;
                    b = c;

                    auto v = vs[l];
                    c = vertexData();
                    c.vertex = phi::vertex(v.position, v.texCoord, v.normal);

                    if (data->o->insert(c.vertex, c.index))
                        data->vs.push_back(c.vertex);

                    auto v0 = a.vertex.getPosition();
                    auto v1 = b.vertex.getPosition();
                    auto v2 = c.vertex.getPosition();
                    auto n0 = a.vertex.getNormal();

                    if (dot(cross((v1 - v0), (v2 - v0)), n0) < 0)
                    {
                        data->is->push_back(a.index);
                        data->is->push_back(c.index);
                        data->is->push_back(b.index);
                    }
                    else
                    {
                        data->is->push_back(a.index);
                        data->is->push_back(b.index);
                        data->is->push_back(c.index);
                    }
                }
            }
        }
    }

    for each (auto pair in (*meshDataPerStyle))
        phi::geometry::calcTangents(pair.second->vs, *pair.second->is);

    part->setStyleMaterials(styleMaterials);
    part->setMeshDataPerStyle(meshDataPerStyle);*/
}

phi::mat4 SEImporter::getModelMatrix(SEREADERLib::IAttachment* attachment)
{
    void *pVoid = 0;
    SAFEARRAY* pData = attachment->GetMatrix();
    HRESULT hr = ::SafeArrayAccessData(pData, &pVoid);
    const double *pDoubles = reinterpret_cast<double*>(pVoid);

    hr = ::SafeArrayUnaccessData(pData);

    return phi::mat4(
        pDoubles[0], pDoubles[1], pDoubles[2], pDoubles[3],
        pDoubles[4], pDoubles[5], pDoubles[6], pDoubles[7],
        pDoubles[8], pDoubles[9], pDoubles[10], pDoubles[11],
        pDoubles[12], pDoubles[13], pDoubles[14], pDoubles[15]);
}

SEImporter::SEAssembly* SEImporter::loadAssembly(std::string fileName, phi::mat4 parentModelMatrix)
{
    auto fileReader = new FileReader();
    auto assemblyReader = fileReader->getInstance();

    if (assemblyReader->FamilyOfAssemblies)
        return nullptr;

    assemblyReader->OpenEx(ConvertMBSToBSTR(fileName), SEREADERLib::SEFileReaderOpenProperties_Shared);

    auto assemblyName = phi::path::getFileNameWithoutExtension(fileName);
    auto assembly = new SEAssembly(assemblyName);
    auto attachments = assemblyReader->GetAttachments();
    auto nrAttachments = attachments->GetCount();

    for (int i = 0; i < nrAttachments; i++)
    {
        auto attachment = attachments->Item(i + 1);
        auto attachmentFileName = ConvertBSTRToMBS(attachment->GetFilename());
        auto attachmentExtension = toLower(phi::path::getExtension(attachmentFileName));
        auto occurrenceName = ConvertBSTRToMBS(attachment->GetOccurrenceName());
        auto attachmentName = phi::path::getFileName(attachmentFileName);
        auto attachmentModelMatrix = getModelMatrix(attachment);
        auto modelMatrix = parentModelMatrix * attachmentModelMatrix;

        if (attachmentExtension.compare(".asm") == 0)
        {
            auto subAssembly = loadAssembly(attachmentFileName, modelMatrix);
            assembly->getSubAssemblies()->push_back(subAssembly);

            //_assemblies[fileNameWithoutExtension] = subAssembly;
            //assemblyOccurrence.assembly = subAssembly;

            /*auto it = _assemblies.find(fileNameWithoutExtension);

            SEAssembly* subAssembly = nullptr;
            assemblyOccurrence assemblyOccurrence;

            if (it != _assemblies.end())
            {
                subAssembly = it->second;
                assemblyOccurrence.assembly = subAssembly;
            }
            else
            {
                subAssembly = loadAssembly(attachmentFileName, modelMatrix);
                assembly->getSubAssemblies()->push_back(subAssembly);

                _assemblies[fileNameWithoutExtension] = subAssembly;
                assemblyOccurrence.assembly = subAssembly;
            }

            assemblyOccurrence.modelMatrix = modelMatrix;
            _assemblyOccurrences[subAssembly].push_back(assemblyOccurrence);*/
        }
        else if (attachmentExtension.compare(".par") == 0 || attachmentExtension.compare(".psm") == 0)
        {
            auto it = _parts.find(attachmentName);

            SEPart* part = nullptr;

            if (it != _parts.end())
            {
                part = it->second;
            }
            else
            {
                auto styleLibrary = new StyleLibrary();
                auto styleLib = styleLibrary->getInstance();
                styleLib->Open(ConvertMBSToBSTR(fileName));
                auto style = loadStyle(styleLib, attachment->GetStyle());

                part = loadPart(attachmentFileName, style);

                assembly->getParts()->push_back(part);

                _parts[attachmentName] = part;
            }

            _partOccurrences[part].push_back(modelMatrix);
        }
    }

    return assembly;
}

SEImporter::SEPart* SEImporter::loadPart(std::string fileName, SEStyle* style)
{
    auto facetReader = new FacetReader();
    auto partReader = facetReader->getInstance();

    BSTR f = ConvertMBSToBSTR(fileName);
    HRESULT hr = partReader->Open(f);

    if (hr != S_OK)
    {
        phi::safeDelete(facetReader);
        return nullptr;
    }

    auto styleLibrary = new StyleLibrary();
    auto styleLib = styleLibrary->getInstance();
    styleLib->Open(f);

    long bodyCount = partReader->GetBodyCount();

    auto partName = phi::path::getFileNameWithoutExtension(fileName);
    auto part = new SEPart(partName);
    auto bodies = part->getBodies();

    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto minZ = std::numeric_limits<double>::max();

    auto maxX = std::numeric_limits<double>::lowest();
    auto maxY = std::numeric_limits<double>::lowest();
    auto maxZ = std::numeric_limits<double>::lowest();

    unsigned int totalVertices = 0;

    for (int i = 0; i < bodyCount; i++)
    {
        long faceCount = partReader->GetBodyFaceCount(i + 1);

        if (faceCount == 0)
            continue;

        unsigned long bodyStyleId = partReader->GetBodyStyle(i + 1);

        SEStyle* bodyStyle = nullptr;

        if (hr == S_OK)
            bodyStyle = loadStyle(styleLib, bodyStyleId);

        auto styles = part->getStyles();
        auto it = styles->find(bodyStyleId);

        if (bodyStyle != nullptr && it == styles->end())
            (*styles)[bodyStyleId] = bodyStyle;

        auto body = SEBody(i);
        body.setStyle(bodyStyle);

        auto faces = body.getFaces();

        for (int j = 0; j < faceCount; j++)
        {
            long faceStripCount = partReader->GetBodyFaceStripCount(i + 1, j + 1);

            unsigned long bodyFaceStyleId = partReader->GetBodyFaceStyle(i + 1, j + 1);

            SEStyle* bodyFaceStyle = loadStyle(styleLib, bodyFaceStyleId);

            if (bodyFaceStyle == nullptr && bodyStyle != nullptr)
            {
                bodyFaceStyle = bodyStyle;
                bodyFaceStyleId = bodyFaceStyle->ID;
            }
            else if (bodyFaceStyle == nullptr && bodyStyle == nullptr)
            {
                if (style != nullptr)
                {
                    bodyFaceStyle = style;
                    bodyFaceStyleId = style->ID;
                }
                else
                {
                    bodyFaceStyle = _defaultStyle;
                    bodyFaceStyleId = bodyFaceStyle->ID;
                }
            }

            auto styles = part->getStyles();
            auto it = styles->find(bodyFaceStyleId);

            if (bodyFaceStyle != nullptr && it == styles->end())
                (*styles)[bodyFaceStyleId] = bodyFaceStyle;

            auto face = SEFace(j);
            face.setStyle(bodyFaceStyle);

            auto strips = face.getStrips();

            for (int k = 0; k < faceStripCount; k++)
            {
                long faceStripVertCount = partReader->GetBodyFaceStripVertCount(i + 1, j + 1, k + 1);

                auto strip = SEStrip(k);
                auto vertices = strip.getVertices();

                for (int l = 0; l < faceStripVertCount; l++)
                {
                    double posX;
                    double posY;
                    double posZ;
                    double normX;
                    double normY;
                    double normZ;
                    double texU;
                    double texV;

                    partReader->GetBodyFaceStripVertex(
                        i + 1,
                        j + 1,
                        k + 1,
                        l + 1,
                        &posX,
                        &posY,
                        &posZ,
                        &normX,
                        &normY,
                        &normZ,
                        &texU,
                        &texV);

                    if (posX < minX)
                        minX = posX;

                    if (posX > maxX)
                        maxX = posX;

                    if (posY < minY)
                        minY = posY;

                    if (posY > maxY)
                        maxY = posY;

                    if (posZ < minZ)
                        minZ = posZ;

                    if (posZ > maxZ)
                        maxZ = posZ;

                    totalVertices++;

                    auto position = (phi::vec4(posX, posY, posZ, 1.0));
                    auto normal = phi::vec3(normX, normY, normZ);
                    auto texCoord = phi::vec2(texU, texV);

                    auto vert = SEVertex(phi::vec3(position.x, position.y, position.z), normal, texCoord);

                    vertices->push_back(vert);
                }

                strips->push_back(strip);
            }

            faces->push_back(face);
        }

        bodies->push_back(body);
    }

    auto cX = (minX + maxX) * 0.5;
    auto cY = (minY + maxY) * 0.5;
    auto cZ = (minZ + maxZ) * 0.5;

    auto c = phi::vec3(cX, cY, cZ);
    auto min = phi::vec3(minX, minY, minZ);
    auto max = phi::vec3(maxX, maxY, maxZ);

    min -= c;
    max -= c;

    min *= 1.05;
    max *= 1.05;

    min += c;
    max += c;

    part->setAabb(phi::aabb(min, max));
    part->setTotalVertices(totalVertices);

    partReader->Close();

    phi::safeDelete(facetReader);
    phi::safeDelete(styleLibrary);

    buildMeshData(part);

    return part;
}

phi::object3D * SEImporter::import(std::string fileName)
{
    /*auto initial = std::chrono::high_resolution_clock::now();
    auto ext = toLower(phi::path::getExtension(fileName));

    std::vector<phi::geometry*>* meshes = new std::vector<phi::geometry*>();

    if (ext.compare(".asm") == 0)
    {
        loadAssembly(fileName, phi::mat4());
    }
    else if (ext.compare(".par") == 0 || ext.compare(".psm") == 0)
    {
        auto part = loadPart(fileName, nullptr);
        _partOccurrences[part].push_back(phi::mat4());
    }

    auto meshId = 0;

    for (auto pair : _partOccurrences)
    {
        auto part = pair.first;
        auto styleMaterials = part->getStyleMaterials();
        auto meshDataPerStyle = part->getMeshDataPerStyle();

        for (auto modelMatrix : pair.second)
        {
            for each (auto pair in (*meshDataPerStyle))
            {
                auto meshData = pair.second;
                std::vector<phi::vertex> vs;

                for (auto vertex : meshData->vs)
                {
                    auto p3 = vertex.getPosition();
                    auto p4 = modelMatrix * phi::vec4(p3.x, p3.y, p3.z, 1.0);

                    auto n3 = vertex.getNormal();
                    auto n4 = modelMatrix * phi::vec4(n3.x, n3.y, n3.z, 0.0);

                    vertex.setPosition(phi::vec3(p4.x, p4.y, p4.z));
                    vertex.setNormal(phi::vec3(n4.x, n4.y, n4.z));

                    vs.push_back(vertex);
                }

                auto name = meshData->name;
                auto mesh = phi::geometry::create(name + "_" + std::to_string(meshId), vs, meshData->is);
                mesh->setId(meshId++);
                auto mat = (*styleMaterials)[pair.first];
                mesh->setMaterial(mat);
                meshes->push_back(mesh);
            }
        }
    }

    auto name = phi::path::getFileNameWithoutExtension(fileName);
    auto model = new phi::model(name);

    for each (auto mesh in *meshes)
        model->addMesh(mesh);

    auto sceneObject = phi::object3D::create(model);

    sceneObject->pitch(-phi::PI_OVER_2);
    sceneObject->yaw(-phi::PI_OVER_2);

    auto final = std::chrono::high_resolution_clock::now();
    auto diff = final - initial;
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(diff).count();
    auto msg = "Importing '" + fileName + "': " + std::to_string(elapsedSeconds);
    phi::log(msg);*/
    //return sceneObject;

    return nullptr;
}