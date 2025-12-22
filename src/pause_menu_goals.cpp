#include "pause_menu_goals.h"

#include "game.h"
#include "game_settings.h"

#include "common.h"


#include "mission_manager.h"


#include "localized_string_table.h"


#include "func_wrapper.h"

VALIDATE_SIZE(pause_menu_goals, 0x110u);



mString pause_menu_goals::get_element_desc(int a2)
{
    mString v6;
    int v4, v5;
    auto *v3 = g_game_ptr->gamefile;
    if ( a2 != 0 )
    {
        if ( a2 == 1 )
        {
            v4 = v3->field_340.field_108;
            v5 = v3->field_340.field_F8;
        }
        else if ( a2 == 2 )
        {
            v4 = v3->field_340.field_10C;
            v5 = v3->field_340.field_FC;
        }
        else
        {
            v4 = v3->field_340.field_110;
            v5 = v3->field_340.field_100;
        }
    }
    else
    {
        v4 = v3->field_340.field_104;
        v5 = v3->field_340.field_F4;
    }

    if ( v5 < v4 )
    {
        if ( v5 == v4 - 1 )
        {
            v6 = this->field_90[a2];
        }
        else
        {
            char Dest[256] {};
            sprintf(Dest, this->field_50[a2].c_str(), v4 - v5);
            mString v7 {Dest};
            v6 = v7;
        }
    }
    else
    {
        v6 = this->field_D0[a2];
    }

    return v6;
	
	THISCALL(0x0060FCD0, this ,a2);
}

mString pause_menu_goals::get_element_value(int element_index)
{
    int current_value = 0;
    int max_value = 0;
    
    bool use_story_mission_stats = !mission_manager::s_inst->is_story_active() 
        || mission_manager::s_inst->is_mission_active();
    
    game_settings* gamefile = g_game_ptr->gamefile;
    
    switch (element_index)
    {
        case 0:
            if (use_story_mission_stats)
            {
                current_value = gamefile->field_340.field_F4;
                max_value = gamefile->field_340.field_104;
            }
            else
            {
                current_value = gamefile->field_340.field_D4;
                max_value = gamefile->field_340.field_E4;
            }
            break;
            
        case 1:
            if (use_story_mission_stats)
            {
                current_value = gamefile->field_340.field_F8;
                max_value = gamefile->field_340.field_108;
            }
            else
            {
                current_value = gamefile->field_340.field_D8;
                max_value = gamefile->field_340.field_E8;
            }
            break;
            
        case 2:
            if (use_story_mission_stats)
            {
                current_value = gamefile->field_340.field_FC;
                max_value = gamefile->field_340.field_10C;
            }
            else
            {
                current_value = gamefile->field_340.field_DC;
                max_value = gamefile->field_340.field_EC;
            }
            break;
            
        case 3:
            if (use_story_mission_stats)
            {
                current_value = gamefile->field_340.field_100;
                max_value = gamefile->field_340.field_110;
            }
            else
            {
                current_value = gamefile->field_340.field_E0;
                max_value = gamefile->field_340.field_F0;
            }
            break;
            
        default:
            break;
    }
    
    char buffer[32];
    sprintf(buffer, "%d / %d", current_value, max_value);
    
    return mString{buffer};
	
	THISCALL(0x0060FB00, this ,element_index);
}




mString pause_menu_goals::Init()
{
    localized_string_table* v1 = g_game_ptr->field_7C;
    auto& strings = v1->field_0->field_0;
    
    this->field_0 = strings[275];
    
    for (int i = 0; i < 4; ++i)
    {
        this->field_10[i] = strings[276 + i];
        this->field_50[i] = strings[280 + i];
        this->field_90[i] = strings[284 + i];
        this->field_D0[i] = strings[288 + i];
    }
}


void pause_menu_goals_patch()
{

    {
        FUNC_ADDRESS(address, &pause_menu_goals::get_element_desc);
    //    REDIRECT(0x00610626, address);

    }
	
    {
        FUNC_ADDRESS(address, &pause_menu_goals::Init);
       // REDIRECT(0x00610250, address);

    }
	
    {
        FUNC_ADDRESS(address, &pause_menu_goals::get_element_value);
      // REDIRECT(0x006102D0, address);
    }
	
	
	
  
}   