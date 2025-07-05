#include "ngl_dx_draw.h"

#include "func_wrapper.h"

#include <ngl.h>
#include <ngl_mesh.h>
#include "fixedstring.h"
#include <trace.h>
#include <variables.h>

#include <cstdint>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "mod.h"


static Var<int> g_MinVertexIndex{0x009729B0};

static Var<IDirect3DVertexBuffer9 *> dword_972964{0x00972964};

HRESULT nglDrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,
                   IDirect3DIndexBuffer9 *a2,
                   UINT startIndex,
                   UINT NumIndices,
                   UINT NumVertices)
{
    TRACE("nglDrawIndexedPrimitive");

    UINT primCount;

    static Var<IDirect3DIndexBuffer9 *> dword_972968{0x00972968};

    if (a2 != dword_972968()) {
        g_Direct3DDevice()->lpVtbl->SetIndices(g_Direct3DDevice(), a2);
        dword_972968() = a2;
    }

    switch (PrimitiveType) {
    case D3DPT_POINTLIST:
        primCount = NumIndices;
        break;
    case D3DPT_LINELIST:
        primCount = NumIndices >> 1;
        break;
    case D3DPT_LINESTRIP:
        primCount = NumIndices - 1;
        break;
    case D3DPT_TRIANGLELIST:
        primCount = NumIndices / 3;
        break;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
        primCount = NumIndices - 2;
        break;
    default:
        primCount = 0;
        break;
    }
    return g_Direct3DDevice()->lpVtbl->DrawIndexedPrimitive(g_Direct3DDevice(),
                                                            PrimitiveType,
                                                            0,
                                                            g_MinVertexIndex(),
                                                            NumVertices,
                                                            startIndex,
                                                            primCount);
}

HRESULT nglDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, int a2, UINT a3)
{
    UINT v3;

    switch (PrimitiveType) {
    case D3DPT_POINTLIST:
        v3 = a3;
        break;
    case D3DPT_LINELIST:
        v3 = a3 >> 1;
        break;
    case D3DPT_LINESTRIP:
        v3 = a3 - 1;
        break;
    case D3DPT_TRIANGLELIST:
        v3 = a3 / 3;
        break;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
        v3 = a3 - 2;
        break;
    default:
        v3 = 0;
        break;
    }

    return g_Direct3DDevice()->lpVtbl->DrawPrimitive(g_Direct3DDevice(),
                                                     PrimitiveType,
                                                     a2 + g_MinVertexIndex(),
                                                     v3);
}

void nglSetStreamSourceAndDrawPrimitive(
        D3DPRIMITIVETYPE a1,
        IDirect3DVertexBuffer9 *a2,
        uint32_t numVertices,
        uint32_t baseVertexIndex,
        uint32_t stride,
        IDirect3DIndexBuffer9 *a6,
        uint32_t numIndices,
        uint32_t startIndex)
{
    if constexpr (0)
    {
        g_Direct3DDevice()->lpVtbl->SetStreamSource(g_Direct3DDevice(), 0, a2, 0, stride);
        dword_972964() = a2;
        if ( numIndices != 0 && a6 )
        {
            nglDrawIndexedPrimitive(a1, a6, startIndex, numIndices, numVertices);
        }
        else
        {
            nglDrawPrimitive(a1, baseVertexIndex, numVertices);
        }
    }
    else
    {
        CDECL_CALL(0x00771460,
                a1, a2,
                numVertices,
                baseVertexIndex,
                stride,
                a6,
                numIndices,
                startIndex);
    }
}

bool LoadOBJModelToBuffers(IDirect3DDevice9* dev, modGenericMesh& data, char * buf, size_t size) {
    if (!buf || size == 0) return false;
    
    std::istringstream iss(std::string(buf, size));
    std::vector<float> positions;
    std::vector<uint16_t> indices;
    std::string line;

    while (std::getline(iss, line)) {
        std::istringstream ls(line);
        std::string type;
        ls >> type;

        if (type == "v") {
            float x, y, z;
            ls >> x >> y >> z;
            positions.push_back(x); positions.push_back(y); positions.push_back(z);
            positions.push_back(0xFFFFFFFF);
        }
        else if (type == "f") {
            uint16_t a, b, c;
            ls >> a >> b >> c;
            indices.push_back(a - 1); indices.push_back(b - 1); indices.push_back(c - 1);
        }
    }

    UINT vertexSize = positions.size() * sizeof(float);
    UINT indexSize = indices.size() * sizeof(uint16_t);
    auto device = g_Direct3DDevice()->lpVtbl;
    if (FAILED(device->CreateVertexBuffer(g_Direct3DDevice(), vertexSize, 0, 0, D3DPOOL_DEFAULT, &data.vertexBuffer, nullptr)))
        return false;

    void* vbData;
    if (FAILED(data.vertexBuffer->lpVtbl->Lock(data.vertexBuffer, 0, vertexSize, &vbData, 0)))
        return false;

    memcpy(vbData, positions.data(), vertexSize);

    data.vertexBuffer->lpVtbl->Unlock(data.vertexBuffer);


    if (FAILED(device->CreateIndexBuffer(g_Direct3DDevice(), indexSize, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &data.indexBuffer, nullptr)))
        return false;

    void* ibData;
    if (FAILED(data.indexBuffer->lpVtbl->Lock(data.indexBuffer, 0, indexSize, &ibData, 0)))
        return false;

    memcpy(ibData, indices.data(), indexSize);

    data.indexBuffer->lpVtbl->Unlock(data.indexBuffer);



    // update
    data.vertices = std::move(positions);
    data.indices = std::move(indices);
    data.stride = 16;
    data.numVertices = static_cast<UINT>(data.vertices.size() / 4);
    data.numIndices = static_cast<UINT>(data.indices.size());
    return true;
}

void WriteOBJFromMeshSection(nglMeshSection* section) {
    if (!section || !section->field_3C.m_vertexBuffer) return;
    std::ofstream out("C:\\Temp\\first.obj");
    if (!out.is_open()) return;

    IDirect3DVertexBuffer9* vb = section->field_3C.m_vertexBuffer;
    IDirect3DIndexBuffer9* ib = section->m_indexBuffer;
    UINT stride = section->m_stride;
    UINT count = section->NVertices;
    UINT icount = section->NIndices;

    void* vbData = nullptr;
    if (FAILED(vb->lpVtbl->Lock(vb, 0, 0, &vbData, D3DLOCK_READONLY))) return;

    float* verts = reinterpret_cast<float*>(vbData);
    for (UINT i = 0; i < count; ++i) {
        float* v = verts + (i * (stride / sizeof(float)));
        out << "v " << v[0] << " " << v[1] << " " << v[2] << "\n";
    }
    vb->lpVtbl->Unlock(vb);

    void* ibData = nullptr;
    if (ib && SUCCEEDED(ib->lpVtbl->Lock(ib, 0, 0, &ibData, D3DLOCK_READONLY))) {
        uint16_t* idx = reinterpret_cast<uint16_t*>(ibData);
        for (UINT i = 0; i < icount; i += 3)
            out << "f " << idx[i] + 1 << " " << idx[i + 1] + 1 << " " << idx[i + 2] + 1 << "\n";
        ib->lpVtbl->Unlock(ib);
    }
}



Mod* dbgReplaceMesh = nullptr;

HRESULT nglSetStreamSourceAndDrawPrimitive(nglMeshSection *MeshSection)
{   
    auto mod = dbgReplaceMesh;
    uint8_t* data = nullptr;

    if (mod)
        data = &mod->Data.data()[0];

    if (mod && data) 
    {
        modGenericMesh modMesh;
        if (LoadOBJModelToBuffers(g_Direct3DDevice(), modMesh, (char*)data, mod->Data.size())) {
            MeshSection->field_3C.m_vertexBuffer = modMesh.vertexBuffer;
            MeshSection->m_indexBuffer = modMesh.indexBuffer;
            MeshSection->NVertices = modMesh.numVertices;
            MeshSection->NIndices = modMesh.numIndices;
            MeshSection->m_stride = modMesh.stride;
            MeshSection->m_primitiveType = D3DPT_TRIANGLELIST;
            dbgReplaceMesh = nullptr;
        }
    }


    uint32_t stride = MeshSection->m_stride;
    g_MinVertexIndex() = MeshSection->field_4C / stride;
    g_Direct3DDevice()->lpVtbl->SetStreamSource(g_Direct3DDevice(), 0, MeshSection->field_3C.m_vertexBuffer, 0, stride);
    dword_972964() = MeshSection->field_3C.m_vertexBuffer;

    if (MeshSection->NIndices != 0)
        return nglDrawIndexedPrimitive(MeshSection->m_primitiveType,
            MeshSection->m_indexBuffer,
            MeshSection->StartIndex,
            MeshSection->NIndices,
            MeshSection->NVertices);
    else
        return nglDrawPrimitive(MeshSection->m_primitiveType, 0, MeshSection->NVertices);
}

void SetRenderTarget(nglTexture *Tex, nglTexture *a2, int a3, int a4)
{
    CDECL_CALL(0x00771970, Tex, a2, a3, a4);
}

