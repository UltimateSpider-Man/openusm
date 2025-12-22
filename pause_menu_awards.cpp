#include "pause_menu_awards.h"

#include "common.h"

#include "game.h"
#include "game_settings.h"

#include "localized_string_table.h"

#include "func_wrapper.h"

VALIDATE_SIZE(pause_menu_awards, 0x250u);


#include "pause_menu_awards.h"



void pause_menu_awards::Init()
{
    localized_string_table* v1 = g_game_ptr->field_7C;
    
    field_0 = v1->field_0->field_0[151];
    field_10 = v1->field_0->field_0[152];
    field_20 = v1->field_0->field_0[153];
    
    field_30[0] = v1->field_0->field_0[154];
    field_30[1] = v1->field_0->field_0[155];
    field_30[2] = v1->field_0->field_0[156];
    field_30[3] = v1->field_0->field_0[157];
    field_30[4] = v1->field_0->field_0[158];
    field_30[5] = v1->field_0->field_0[159];
    field_30[6] = v1->field_0->field_0[160];
    field_30[7] = v1->field_0->field_0[161];
    field_30[8] = v1->field_0->field_0[162];
    field_30[9] = v1->field_0->field_0[163];
    field_30[10] = v1->field_0->field_0[164];
    field_30[11] = v1->field_0->field_0[165];
    field_30[12] = v1->field_0->field_0[166];
    field_30[13] = v1->field_0->field_0[167];
    field_30[14] = v1->field_0->field_0[168];
    field_30[15] = v1->field_0->field_0[169];
    field_30[16] = v1->field_0->field_0[170];
    
    field_140[0] = v1->field_0->field_0[171];
    field_140[1] = v1->field_0->field_0[172];
    field_140[2] = v1->field_0->field_0[173];
    field_140[3] = v1->field_0->field_0[174];
    field_140[4] = v1->field_0->field_0[175];
    field_140[5] = v1->field_0->field_0[176];
    field_140[6] = v1->field_0->field_0[177];
    field_140[7] = v1->field_0->field_0[178];
    field_140[8] = v1->field_0->field_0[179];
    field_140[9] = v1->field_0->field_0[180];
    field_140[10] = v1->field_0->field_0[181];
    field_140[11] = v1->field_0->field_0[182];
    field_140[12] = v1->field_0->field_0[183];
    field_140[13] = v1->field_0->field_0[184];
    field_140[14] = v1->field_0->field_0[185];
    field_140[15] = v1->field_0->field_0[186];
    field_140[16] = v1->field_0->field_0[187];
	
	
	THISCALL(0x0060EB90,this);
}


mString pause_menu_awards::get_element_value(int a3)
{
    char v3 = 0;
    bool found = true;
    
    switch (a3)
    {
        case 0:
            v3 = g_game_ptr->gamefile->field_340.field_6E;
            break;
        case 1:
            v3 = g_game_ptr->gamefile->field_340.field_6F;
            break;
        case 2:
            v3 = g_game_ptr->gamefile->field_340.field_70;
            break;
        case 3:
            v3 = g_game_ptr->gamefile->field_340.field_71;
            break;
        case 4:
            v3 = g_game_ptr->gamefile->field_340.field_72;
            break;
        case 5:
            v3 = g_game_ptr->gamefile->field_340.field_73;
            break;
        case 6:
            v3 = g_game_ptr->gamefile->field_340.field_74;
            break;
        case 7:
            v3 = g_game_ptr->gamefile->field_340.field_75;
            break;
        case 8:
            v3 = g_game_ptr->gamefile->field_340.field_76;
            break;
        case 9:
            v3 = g_game_ptr->gamefile->field_340.field_77;
            break;
        case 10:
            v3 = g_game_ptr->gamefile->field_340.field_78;
            break;
        case 11:
            v3 = g_game_ptr->gamefile->field_340.field_79;
            break;
        case 12:
            v3 = g_game_ptr->gamefile->field_340.field_7A;
            break;
        case 13:
            v3 = g_game_ptr->gamefile->field_340.field_7B;
            break;
        case 14:
            v3 = g_game_ptr->gamefile->field_340.field_7C;
            break;
        case 15:
            v3 = g_game_ptr->gamefile->field_340.field_7D;
            break;
        case 16:
            v3 = g_game_ptr->gamefile->field_340.field_7E;
            break;
        default:
            found = false;
            break;
    }
    
    if (found && v3)
    {
        return mString(field_20);
    }
    else
    {
        return mString(field_20);
    }
	
	THISCALL(0x0060EEA0,this,a3);
}

void pause_menu_awards_patch()
{

    {
        FUNC_ADDRESS(address, &pause_menu_awards::Init);
        REDIRECT(0x006100BA, address);

    }
	
	{
        FUNC_ADDRESS(address, &pause_menu_awards::get_element_value);
      REDIRECT(0x00610280, address);
    }   
}    