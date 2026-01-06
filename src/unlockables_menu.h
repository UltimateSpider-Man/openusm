#pragma once

#include "femenu.h"

struct FEMenuSystem;
struct PanelQuad;
struct menu_nav_bar;

struct unlockables_menu : FEMenu {
    FEMenuSystem *field_2C;
    int field_30;
    menu_nav_bar *field_34;
    PanelQuad *field_38[14];
    int field_70;
    int field_74;
    int field_78;
    int field_7C;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
    int field_90;
    int field_94;
    int field_98;
    int field_9C;
    int field_A0;
    int field_A4;
    int field_A8;
    int field_AC;
    int field_B0;
    int field_B4;
    int field_B8;
    int field_BC;
    int field_C0;
    int field_C4;
    int field_C8;
    int field_CC;
    int field_D0;
    int field_D4;
    int field_D8;
    int field_DC;
    int field_E0;
    int field_E4;
    int field_E8;
    int field_EC;
    int field_F0;
    int field_F4;
    int field_F8;
    int field_FC;

    //0x00614020
    unlockables_menu(FEMenuSystem *a2, int a3, int a4);

    //0x00614110
    //virtual
    void _Load();

    //0x0062DB20
    //virtual
    void OnCross(int a2);

    //0x006253C0
    //virtual
    void OnTriangle(int a2);

    //0x0062D510
    //virtual
    void OnActivate();
	

};

extern void unlockables_menu_patch();
