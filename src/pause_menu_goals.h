#pragma once

#include "mstring.h"



#include "femenu.h"

struct pause_menu_goals : FEMenu {
    mString field_0;
    mString field_10[4];
    mString field_50[4];
    mString field_90[4];
    mString field_D0[4];




pause_menu_goals(FEMenuSystem *a2, int a3, int a4);

// 0x0060F980
mString Init();


// 0x0060FB00
mString get_element_value(mString *out ,int a2);

// 0x0060FCD0
mString get_element_desc(int a2);










};



extern void pause_menu_goals_patch();
