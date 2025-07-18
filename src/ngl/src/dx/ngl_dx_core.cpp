#include "ngl_dx_core.h"

#include "ngl.h"
#include "ngl_font.h"
#include "ngl_lighting.h"
#include "ngl_scene.h"
#include "ngl_dx_state.h"
#include "timer.h"
#include "trace.h"
#include "variable.h"
#include "variables.h"

#include <func_wrapper.h>

#include <psapi.h>
#include <windows.h>

static Var<const float> PCFreq {0x0093A294};

static Var<nglFrameLockType> nglFrameLock = {0x0093AED0};

static Var<int> nglFrameLockImmediate{0x00972668};

static Var<int> nglLastFlipCycle{0x00972678};

static Var<int> nglVBlankCount{0x00972908};

static Var<int> nglLastFlipVBlank{0x00972670};

Var<int> nglFrameVBlankCount = (0x0097290C);

static Var<int> nglFlipCycle{0x00972674};

static Var<BOOL> nglFlipQueued = {0x00972668};

static Var<char *> nglListWork {0x00971F08};

static Var<int> dword_93AED4 {0x0093AED4};

void nglVif1RenderScene()
{
    TRACE("nglVif1RenderScene");

    if constexpr (0)
    {}
    else
    {
        CDECL_CALL(0x0077D060);
    }
}

uint32_t sub_413A50(float a1, float a2, float a3, float a4)
{
    auto v5 = a1 * 255.0;
    auto v6 = a2 * 255.0;
    auto v7 = a3 * 255.0;
    auto v8 = a4 * 255.0;
    return (((uint64_t)v6 | (((uint64_t)v5 | ((uint32_t)(uint64_t)v8 << 8)) << 8)) << 8) | (uint64_t)v7;
}

void nglVif1SetupScene(nglScene *a1)
{
    TRACE("nglVif1SetupScene");

    if constexpr (0)
    {
        if ( nglCurScene()->AnimTime == 0.0f )
        {
            float v1 = ( nglIsFBPAL() ? 20.0 : 16.666666);

            float v2 = nglFrameVBlankCount();
            if ( nglFrameVBlankCount() < 0 ) {
                v2 += 4.2949673e9;
            }

            nglCurScene()->field_3FC = v2 * v1 * 0.001f;
        }
        else
        {
            nglCurScene()->field_3FC = nglCurScene()->AnimTime;
        }

        if ( a1->ZWriteEnable || a1->ZTestEnable )
        {
            g_renderState().setDepthBuffer(D3DZB_TRUE);

            if ( a1->ZTestEnable )
            {
                g_renderState().setDepthBufferFunction(D3DCMP_LESSEQUAL);
            }
            else
            {
                auto stencilCheckEnabled = g_renderState().m_stencilCheckEnabled;

                if ( stencilCheckEnabled )
                {
                    g_renderState().setStencilCheckEnabled(false);
                }
                else
                {
                    g_renderState().setStencilCheckEnabled(true);

                    g_renderState().setStencilBufferTestFunction(D3DCMP_ALWAYS);

                    g_renderState().setStencilBufferCompareMask(255u);

                    g_renderState().setStencilPassOperation(D3DSTENCILOP_ZERO);
                }

                g_renderState().setDepthBufferFunction(D3DCMP_ALWAYS);
            }
        }
        else
        {
            g_renderState().setDepthBuffer(D3DZB_FALSE);
        }

        auto v4 = nglCurScene()->field_334;
        SetRenderTarget(v4, nglCurScene()->field_338, 0, nglCurScene()->field_8);

        float ScreenWidth = v4->m_width;
        if ( v4->m_width < 0 ) {
            ScreenWidth += 4.2949673e9;
        }

        float ScreenHeight = v4->m_height;
        if ( v4->m_height < 0 ) {
            ScreenHeight += 4.2949673e9;
        }

        uint32_t v7 = ((nglCurScene()->sx1 + 1.0f ) * 0.5f * ScreenWidth + 0.5f);
        uint32_t v8 = ((nglCurScene()->sy1 + 1.0f ) * 0.5f * ScreenHeight + 0.5f);
        uint32_t v20 = ((nglCurScene()->sx2 + 1.0f) * 0.5f * ScreenWidth + 0.5f);
        uint32_t v9 = ((nglCurScene()->sy2 + 1.0f) * 0.5f * ScreenHeight + 0.5f);

        D3DVIEWPORT9 v23;
        v23.X = 0;
        v23.Y = 0;
        v23.Width = ScreenWidth;
        v23.Height = ScreenHeight;
        v23.MinZ = 0.0;
        v23.MaxZ = 1.0;
        g_Direct3DDevice()->lpVtbl->SetViewport(g_Direct3DDevice(), &v23);
        auto *v10 = nglCurScene();
        if ( nglCurScene()->sx1 != -1.0f 
            || nglCurScene()->sy1 != -1.0f
            || nglCurScene()->sx2 != 1.0f 
            || nglCurScene()->sy2 != 1.0f )
        {
            RECT v22;
            v22.right = v20;
            v22.left = v7;
            v22.top = v8;
            v22.bottom = v9;

            g_renderState().setScissorTestEnabled(true);

            g_Direct3DDevice()->lpVtbl->SetScissorRect(g_Direct3DDevice(), &v22);
        }

        if ( v10->ClearFlags )
        {
            g_renderState().setColourBufferWriteEnabled(15u);

            auto v18 = v10->ClearStencil;
            auto v17 = v10->ClearZ;
            auto v13 = sub_413A50(v10->ClearColor.r,
                                v10->ClearColor.g,
                                v10->ClearColor.b,
                                v10->ClearColor.a);

            g_Direct3DDevice()->lpVtbl->Clear(g_Direct3DDevice(), 0, 0, v10->ClearFlags, v13, v17, v18);
        }

        g_renderState().setColourBufferWriteEnabled(v10->FBWriteMask);

        auto v21 = nglIFLSpeed() * v10->field_3FC;
        nglCurScene()->IFLFrame = std::round(v21);
        if ( !EnableShader() )
        {
            g_Direct3DDevice()->lpVtbl->SetTransform(
                g_Direct3DDevice(),
                D3DTS_PROJECTION,
                bit_cast<D3DMATRIX *>(&a1->ViewToScreen));

            g_Direct3DDevice()->lpVtbl->SetTransform(g_Direct3DDevice(),
                    D3DTS_VIEW,
                    bit_cast<D3DMATRIX *>(&a1->WorldToView));
        }
    }
    else
    {
        CDECL_CALL(0x0077CBB0, a1);
    }
}

void sub_781A30()
{
    ;
}

LARGE_INTEGER query_perf_counter()
{
    LARGE_INTEGER PerformanceCount;

    QueryPerformanceCounter(&PerformanceCount);
    return PerformanceCount;
}

void sub_76DE60()
{
    nglPerfInfo().field_28 = query_perf_counter();
}

nglLightContext *nglCreateLightContext()
{
    return (nglLightContext *) CDECL_CALL(0x00775EC0);
}

void nglListInit()
{
    TRACE("nglListInit");

    if constexpr (1)
    {
        nglFrameVBlankCount() = nglVBlankCount();
        nglPerfInfo().field_38 = query_perf_counter();
        nglListWorkPos() = CAST(nglListWorkPos(), nglListWork());
        nglDefaultLightContext() = nglCreateLightContext();
        if ( nglSyncDebug().DumpFrameLog ) {
            nglDebug().DumpFrameLog = 0;
        }

        if ( nglSyncDebug().DumpSceneFile ) {
            nglDebug().DumpSceneFile = 0;
        }

        if ( nglSyncDebug().DumpTextures ) {
            nglDebug().DumpTextures = 0;
        }

        nglSyncDebug() = nglDebug();
        nglCurScene() = nullptr;
        nglListBeginScene(static_cast<nglSceneParamType>(0));
        nglSceneDumpStart();
        auto *v3 = nglScratchBuffer().field_0[0].m_vertexData;
        auto v0 = nglScratchBuffer().field_44;
        nglScratchBuffer().field_4C = nglScratchBuffer().field_0[v0];

        nglScratchBuffer().field_48 = (IDirect3DIndexBuffer9 *)nglScratchBuffer().field_18[v0];
        if ( nglScratchBuffer().field_4C.m_vertexBuffer != nullptr ) {
            nglScratchBuffer().field_4C.m_vertexBuffer->lpVtbl->Lock(nglScratchBuffer().field_4C.m_vertexBuffer, 0, 0, (void **)&v3, D3DLOCK_DISCARD);
            nglScratchBuffer().field_4C.m_vertexData = v3;
        }

        auto *v2 = nglScratchBuffer().field_48;
        if ( v2 != nullptr ) {
            int16_t *v3 = nullptr;
            v2->lpVtbl->Lock(v2, 0, 0, (void **)&v3, 0);
            nglScratchBuffer().field_20 = v3;
        }
    } else {
        CDECL_CALL(0x0076E050);
    }
}


void nglSetFrameLock(nglFrameLockType a2)
{
    TRACE("nglSetFrameLock");

    if constexpr (1) {
        int v1 = 1;
        if (a2) {
            if (a2 == 1) {
                nglFrameLock() = static_cast<nglFrameLockType>(1);
                nglFrameLockImmediate() = 0;
                v1 = 1;

            } else if (a2 == 2) {
                nglFrameLock() = static_cast<nglFrameLockType>(2);
                nglFrameLockImmediate() = 0;
                v1 = 2;
            }

        } else {
            nglFrameLock() = static_cast<nglFrameLockType>(0);
            nglFrameLockImmediate() = 0;
            v1 = 0x80000000;
            assert(0);
        }

        if (v1 != dword_93AED4()) {
            auto *v2 = g_timer();
            if (g_timer() != nullptr) {
                operator delete(v2);
            }

            float v4 = nglFrameLock();
            if (nglFrameLock() < 0) {
                v4 += 4.2949673e9;
            }

            v4 = 60.0 / v4;

            g_timer() = new Timer{v4, v4};

            g_timer()->sub_582180();
            dword_93AED4() = v1;
        }
    } else {
        CDECL_CALL(0x0076E750, a2);
    }
}

static Var<float> g_renderTime {0x00972664};

void sub_76DE80()
{
    nglPerfInfo().field_30 = query_perf_counter();
    g_renderTime() = (nglPerfInfo().field_30.QuadPart - nglPerfInfo().field_28.QuadPart) / PCFreq();
    if ( !nglFrameLock()
        || nglFrameLockImmediate() && nglVBlankCount() - nglLastFlipVBlank() >= (unsigned int)nglFrameLock() )
    {
        nglLastFlipCycle() = nglFlipCycle();
        nglFlipCycle() = query_perf_counter().LowPart;
        nglLastFlipVBlank() = nglVBlankCount();
        nglFlipQueued() = false;
    }
    else
    {
        nglFlipQueued() = true;
    }
}

size_t sub_74A650()
{
    size_t v0 = 0;

    MEMORYSTATUS Buffer;
    GlobalMemoryStatus(&Buffer);
    size_t v1 = Buffer.dwTotalPhys;
    GetCurrentProcess();
    size_t pid = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);

    PROCESS_MEMORY_COUNTERS ppsmemCounters;
    if (GetProcessMemoryInfo(hProcess, &ppsmemCounters, sizeof(ppsmemCounters))) {
        v0 = v1 - ppsmemCounters.PeakWorkingSetSize;
    }

    CloseHandle(hProcess);
    return v0;
}

void nglRenderPerfBar()
{
    CDECL_CALL(0x0077ECF0);
}

void nglRenderPerfInfo()
{
    printf("nglRenderPerfInfo\n");

    if constexpr (0) {
        char Dest[2048];

        static Var<char[1]> byte_8B7DF8 = {0x008B7DF8};

        static Var<int> nglVif1WorkSize = {0x00971F10};

        if (nglSyncDebug().ShowPerfInfo == 1) {
        size_t v0 = sub_74A650();
        sprintf(Dest,
                byte_8B7DF8(),
                "FINAL",
                nglSyncPerfInfo().m_fps,
                nglSyncPerfInfo().m_render_time,
                nglSyncPerfInfo().m_cpu_time,
                nglSyncPerfInfo().field_70,
                nglSyncPerfInfo().field_74,
                nglSyncPerfInfo().m_quads_time,
                nglSyncPerfInfo().m_fonts_time,
                nglSyncPerfInfo().m_num_verts,
                nglSyncPerfInfo().m_num_polys,
                nglSyncPerfInfo().field_80,
                nglSyncPerfInfo().field_0,
                nglVif1WorkSize(),
                nglSyncPerfInfo().field_8,
                nglSyncPerfInfo().field_C,
                nglSyncPerfInfo().field_10,
                nglSyncPerfInfo().field_14,
                nglDebug().field_8,
                nglDebug().field_C,
                nglDebug().field_10,
                v0);
    } else {
        sprintf(Dest,
                "%.2f FPS\n%.2fms\n",
                nglSyncPerfInfo().m_fps,
                nglSyncPerfInfo().m_render_time);
    }

    uint32_t a3;
    uint32_t a4;

    nglGetStringDimensions(nglSysFont(), Dest, &a3, &a4, 1.0, 1.0);

    nglQuad a1;
    nglInitQuad(&a1);
    float v1 = a4 + 10;
    float v2 = 620 - a3;
    nglSetQuadRect(&a1, v2, 10.0, 640.0, v1);

    nglSetQuadColor(&a1, 0xC0000000);
    nglSetQuadZ(&a1, -9999.0);
    nglListAddQuad(&a1);
    float v3 = (630 - a3);
    nglListAddString(nglSysFont(), Dest, v3, 20.0, -9999.0, -1, 1.0, 1.0);
         } else {
        CDECL_CALL(0x0076E0E0);
		}
}


void nglRenderDebug()
{
    if ( nglSyncDebug().ShowPerfInfo ) {
        nglRenderPerfInfo();
    }

    if ( nglSyncDebug().ShowPerfBar ) {
        nglRenderPerfBar();
    }
}

void sub_76DD70()
{
    nglLastFlipCycle() = nglFlipCycle();
    nglFlipCycle() = query_perf_counter().LowPart;
    nglLastFlipVBlank() = nglVBlankCount();
    nglFlipQueued() = false;
}

int __fastcall sub_781EA0(void *a1)
{
    int (__fastcall *func)(void *) = CAST(func, 0x00781EA0);
    return func(a1);
}

void nglQueueFlip()
{
    if ( nglFrameLock()
        && (!nglFrameLockImmediate() || nglVBlankCount() - nglLastFlipVBlank() < (unsigned int)nglFrameLock() ) )
    {
        nglFlipQueued() = true;
    }
    else
    {
        sub_76DD70();
    }
}

void sub_781B60()
{
    CDECL_CALL(0x00781B60);
}

void sub_782060()
{
    CDECL_CALL(0x00782060);
}

void sub_781B20()
{
    CDECL_CALL(0x00781B20);
}

void Reset3DDevice()
{
    if constexpr (0) {
        sub_782030();
        sub_77B2F0(1);
        if ( !EnableShader() ) {
            sub_81E910();
        }

        sub_781B60();
        if ( g_Windowed() )
        {
            s_d3dpresent_params().FullScreen_RefreshRateInHz = 0;
        }
        else if ( s_d3dpresent_params().PresentationInterval == 1 )
        {
            s_d3dpresent_params().FullScreen_RefreshRateInHz = 60;
        }

        if ( g_occlusionQueryTest() ) {
            g_occlusionQueryTest()->lpVtbl->Release(g_occlusionQueryTest());
        }

        g_Direct3DDevice()->lpVtbl->Reset(g_Direct3DDevice(), &s_d3dpresent_params());
        if ( !g_Direct3DDevice()->lpVtbl->CreateQuery(g_Direct3DDevice(), D3DQUERYTYPE_OCCLUSION, nullptr) ) {
            g_Direct3DDevice()->lpVtbl->CreateQuery(g_Direct3DDevice(), D3DQUERYTYPE_OCCLUSION, &g_occlusionQueryTest());
        }

        sub_782060();
        if ( !EnableShader() ) {
            sub_81E8E0(0x25A000);
        }

        ++dword_91E1D8();
        sub_781B20();
        g_renderState().Clear();
        g_renderTextureState().clear();
        sub_7726B0(0);
        dword_93AED4() = -1;
        nglSetFrameLock(nglFrameLock());
    } else {
        CDECL_CALL(0x0076E800);
    }
}

void PumpMessages()
{
    TRACE("PumpMessages");

    struct tagMSG Msg;

    while ( PeekMessageA(&Msg, nullptr, 0, 0, PM_REMOVE) ) {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
}

void nglFlip(bool a1)
{
    TRACE("nglFlip");

    if constexpr (1) {
        ++nglVBlankCount();
        g_Direct3DDevice()->lpVtbl->BeginScene(g_Direct3DDevice());
        sub_781EA0(nullptr);
        g_Direct3DDevice()->lpVtbl->EndScene(g_Direct3DDevice());

        static_assert(D3DERR_DEVICELOST == (HRESULT) 0x88760868);
        static_assert(D3DERR_DEVICENOTRESET == (HRESULT) 0x88760869);
        
        if ( !byte_971F9C()
                && g_Direct3DDevice()->lpVtbl->Present(g_Direct3DDevice(), nullptr, nullptr, nullptr, nullptr) == D3DERR_DEVICELOST )
        {
            Sleep(100u);
            if ( g_Direct3DDevice()->lpVtbl->TestCooperativeLevel(g_Direct3DDevice()) == D3DERR_DEVICENOTRESET ) {
                Reset3DDevice();
            }
        }

        PumpMessages();
        ++nglFrame();
        if ( a1 ) {                                             
            nglQueueFlip();
        }
    } else {
        CDECL_CALL(0x0076E980, a1);
    }
}

#include <bitset>

void nglListSend(bool Flip)
{
    TRACE("nglListSend");

    if constexpr (1)
    {
        if ( EnableShader() ) {
            float v10[4] {0, 0, 1, 1};
            g_Direct3DDevice()->lpVtbl->SetVertexShaderConstantF(g_Direct3DDevice(), 90, v10, 1);
        }

        nglRenderDebug();

        sub_76DE60();
#if 0
        if (nglCurScene() != nglRootScene()) {
            error("nglListSend called while one or more scenes were still active (need to call nglListEndScene).\n");
        }
#endif

        nglPerfInfo().field_28 = query_perf_counter();

        auto v3 = []() {
            auto perf_counter = query_perf_counter();
            LARGE_INTEGER v3 = bit_cast<LARGE_INTEGER>(*(uint64_t *)&perf_counter - nglPerfInfo().field_38.QuadPart);
            return v3;
        }();

        nglPerfInfo().field_38 = v3;
        nglPerfInfo().field_74 = v3.QuadPart / PCFreq();
        nglPerfInfo().field_40 = query_perf_counter();
        nglScratchBuffer().field_44 ^= 1u;
        nglScratchBuffer().field_28 = 0;
        nglScratchBuffer().field_2C = 0;
        nglScratchBuffer().m_numVertices = 0;
        nglScratchBuffer().field_30 = 0;

        nglScratchBuffer().field_4C.m_vertexBuffer->lpVtbl->Unlock(nglScratchBuffer().field_4C.m_vertexBuffer);
        nglScratchBuffer().field_48->lpVtbl->Unlock(nglScratchBuffer().field_48);
        
        nglCurScene() = nglRootScene();
        g_Direct3DDevice()->lpVtbl->BeginScene(g_Direct3DDevice());
        nglVif1RenderScene();
        g_Direct3DDevice()->lpVtbl->EndScene(g_Direct3DDevice());
        sub_781A30();

        sub_76DE80();

        float v5 = 1.0 / PCFreq();
        nglPerfInfo().field_40.QuadPart = query_perf_counter().QuadPart - nglPerfInfo().field_40.QuadPart;

        nglPerfInfo().field_70 = nglPerfInfo().field_40.QuadPart * v5;

        auto v6 = dword_975308();
        nglPerfInfo().m_quads_time = nglPerfInfo().m_counterQuads.QuadPart * v5;

        nglPerfInfo().m_fonts_time = nglPerfInfo().field_50.QuadPart * v5;

        if ( dword_975314() == dword_975308() ) {
            v6 = dword_97530C();
        }

        dword_975314() = v6;
        nglScratchMeshPos() = v6;

        //dword_972AB4 = 0;
        //dword_972ABC = 0;
        
        g_Direct3DDevice()->lpVtbl->SetStreamSource(g_Direct3DDevice(), 0, nullptr, 0, 0);
        g_Direct3DDevice()->lpVtbl->SetVertexShader(g_Direct3DDevice(), nullptr);
        g_Direct3DDevice()->lpVtbl->SetPixelShader(g_Direct3DDevice(), nullptr);

#if 0
        if ( dword_971F24() != nullptr ) {
            dword_971F24()(dword_971F28());
        }
#endif

        float v8 = []() -> double {
            return query_perf_counter().QuadPart - nglPerfInfo().field_20.QuadPart;
        }();
        
        nglPerfInfo().m_cpu_time = v8 / PCFreq();

#if 0
        if ( dword_971F1C() != nullptr )
            dword_971F1C()(dword_971F20());
#endif

        if ( Flip ) {
            nglFlip(0);
        }

        nglPerfInfo().field_20 = query_perf_counter();
        float v9 = nglFlipCycle() - nglLastFlipCycle();
        nglPerfInfo().m_render_time = g_renderTime();
        //sp_log("m_render_time = %f", nglPerfInfo().m_render_time);

        if ( v9 < 0 ) {
            v9 += flt_86F860();
        }

        sp_log("v9 = %f, PCFreq = %f", v9, PCFreq());
        nglPerfInfo().field_6C = v9 / PCFreq();
        nglPerfInfo().field_5C = nglPerfInfo().field_5C + nglPerfInfo().field_6C;
        nglPerfInfo().m_fps = 1000.f / nglPerfInfo().field_6C;
        sp_log("nglPerfInfo.m_fps == %f", nglPerfInfo().m_fps);

        nglPerfInfo().field_60 = nglPerfInfo().field_5C * 0.001f;
        if ( nglDebug().ScreenShot ) {
            nglScreenShot(nullptr);
            nglDebug().ScreenShot = 0;
        }

        nglSyncPerfInfo() = nglPerfInfo();

        nglPerfInfo().field_80 = 0;
        nglPerfInfo().m_quads_time = 0.0;
        nglPerfInfo().m_fonts_time = 0.0;
        nglPerfInfo().m_num_verts = 0;
        nglPerfInfo().m_num_polys = 0;
        nglPerfInfo().m_counterQuads.QuadPart = 0;
        nglPerfInfo().field_50.QuadPart = 0;

#if 0
        if ( dword_971F2C() ) {
            dword_971F2C()(dword_971F30());
        }
#endif

        nglCurScene() = nullptr;
    } else {
        CDECL_CALL(0x0076EA10, Flip);
    }
}

