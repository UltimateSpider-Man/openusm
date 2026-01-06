#pragma once

#include "femenu.h"
#include "vector3d.h"

struct menu_nav_bar;
struct FEMenuSystem;
struct PanelFile;
struct PanelQuad;
struct entity;
struct nglTexture;
struct FEText;

namespace comic_panels {
struct panel;
}

struct ptr_PanelQuad {
    PanelQuad *field_0;
    int field_4;
    bool field_8;
};

struct ptr1_PanelQuad {
    PanelQuad *field_0;
    int field_4;
};



struct alternate_costumes : FEMenu {
    FEMenuSystem *field_2C;
    PanelFile *field_30;
    menu_nav_bar *field_34;
    int field_38;
    int field_3C;
    int field_40[3];
    PanelQuad *field_4C[21];
    int field_A0[2];
    nglTexture *field_A8;
    nglTexture *field_AC;
    nglTexture *field_B0;
    nglTexture *field_B4;
    nglTexture *field_B8;
    FEText *field_BC;
    char field_C0;
    ptr_PanelQuad field_C4[6];
    int field_10C;
    int field_110;
    int field_114;
    int field_118;
    ptr1_PanelQuad field_11C[6];

    comic_panels::panel *field_14C;
    entity *field_150;
    float field_154;
    vector3d field_158;
    float field_164;
    int field_168;

    //0x00625D70
    alternate_costumes(FEMenuSystem *a2, int a3, int a4);

    void sub_640740(int idx);

    void sub_640510(int idx);

    bool sub_614AC0(int a1);

    //0x00614B60
    void set_info();

    //0x00640840
    void update_selected(int a2);
	
	
	void sub_614C60(int a2);


    //0x00646790
    //virtual
    void Init();

    //virtual
    void sub_640BC0(int a2);

    //0x00642340
    //virtual
    void onActivate();

    //0x00614A30
    //virtual
    void OnDeactivate(FEMenu *a2);

    //0x00640BC0
    //virtual
    void OnTriangle(int a2);

    //0x00640950
    //virtual
    void OnUp(int a2);

    //0x00640A00
    //virtual
    void OnDown(int a2);
};

extern void alternate_costumes_patch();
