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


HRESULT nglSetStreamSourceAndDrawPrimitive(nglMeshSection *MeshSection)
{   
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

