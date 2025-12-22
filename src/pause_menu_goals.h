#pragma once

#include "mstring.h"

struct pause_menu_goals
{
    mString field_0;
    mString field_10[4];
    mString field_50[4];
    mString field_90[4];
    mString field_D0[4];

    //0x0060FCD0
    mString get_element_desc(int a2);
	
	mString get_element_value(int element_index);
	
	
	mString Init();
};


extern void pause_menu_goals_patch();