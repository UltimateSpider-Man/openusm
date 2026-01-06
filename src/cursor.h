#pragma once

#include "ngl.h"

#include <winbase.h>

struct Cursor {
    std::intptr_t m_vtbl;
    int empty0[4];
    IDirect3DTexture9 *field_14;
    nglQuad field_18;
    nglTexture field_7C;
    POINT m_cursorLoc;
    POINT field_104;
    int field_10C[2];

    bool field_114;
    char empty1[3];
    int m_screenWidth;
    int m_screenHeight;
    bool field_120;
    char empty2[3];
    float field_124;
    float field_128;
    int field_12C;
    void *field_130;
    int field_134;
    int field_138;

    //0x005A6670
    Cursor(LPCWSTR lpWideCharStr, int a3, int a4);

    //0x005A6810
    ~Cursor();

    void sub_5A67D0(int a1, int a2, int a3, int a4);

    void sub_5A6790();
	

    void sub_581C60();

    void sub_5B0D70();
};

extern Var<Cursor *> g_cursor;

extern void cursor_patch();
