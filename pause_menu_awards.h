#pragma once

#include "mstring.h"

struct pause_menu_awards
{
    mString field_0;
    mString field_10;
    mString field_20;
    mString field_30[17];
    mString field_140[17];
	
	    mString get_element_value(int a3);

          void Init();
};


extern void pause_menu_awards_patch();
