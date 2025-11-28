#pragma once

#include "float.hpp"
#include "variable.h"

#include <windows.h>

struct Timer {
    LARGE_INTEGER field_0;
    LARGE_INTEGER field_8;
    int field_10;
    int field_14;
    int field_18;
    int field_1C;
    int field_20;
    int field_24;
    float field_28;
    float field_2C;
    float field_30;
    float field_34;
    char field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    int field_50;
    int field_54;

    //0x00582100
    Timer(Float a2, Float a3);

    float sub_5821D0();
	
	    float slow_motion();
		
			    float super_slow_motion();
		
			    float normal_motion();
				
				float speed_motion();
				
				float super_speed_motion();


    void sub_582180();
};

extern Var<Timer *> g_timer;

extern void Timer_patch();

