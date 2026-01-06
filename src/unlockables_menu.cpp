#include "unlockables_menu.h"

#include "common.h"
#include "func_wrapper.h"
#include "log.h"
#include "utility.h"
#include "trace.h"
#include "panelquad.h"

#include "femenusystem.h"

#include "comic_panels.h"

#include "movie_manager.h"

#include "sound_instance_id.h"


VALIDATE_SIZE(unlockables_menu, 0x100u);

unlockables_menu::unlockables_menu(FEMenuSystem *a2, int a3, int a4)
    : FEMenu::FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00614020, this, a2, a3, a4);
}



void unlockables_menu::OnCross(int a2) {
	sp_log("unlockables_menu::OnCross():");

    static bool initialized = false;
    static string_hash fe_ps_accept;
    
    if (!initialized) {
        initialized = true;
    }
    		    static string_hash sound_id ("fe_ps_accept");

    sound_instance_id id = sub_60B960(sound_id, 1.0, 1.0);
    
    int panel_id;
    
    switch (this->field_F4) {
    case 3:
        this->field_2C->MakeActive(10);
        comic_panels::game_play_panel()->field_67 = 1;
		panel_id = 11;
        return;
        
    case 1:
        panel_id = 14;
        break;
        
    case 2:
        panel_id = 12;
        break;
        
    case 0:
        panel_id = 10;
        break;
        
    case 4:
        panel_id = 15;
        break;
        
    case 5:
        movie_manager::load_and_play_movie("x-men_trailer", "x-men_trailer", 0);
        return;
        
    case 6:
        panel_id = 16;
        break;
	case 7:
        panel_id = 17;
        break;
        
    default:
        return;
    }
    
    this->field_2C->MakeActive(panel_id);
    comic_panels::game_play_panel()->field_67 = 1;
	THISCALL(0x0062DB20, this, a2);
}

void unlockables_menu::OnTriangle(int a2) {
    sp_log("unlockables_menu::OnTriangle():");

    THISCALL(0x006253C0, this, a2);
}

void unlockables_menu::OnActivate() {
    sp_log("unlockables_menu::OnActivate():");

    THISCALL(0x0062D510, this);
}

void unlockables_menu::_Load()
{
    TRACE("unlockables_menu::Load");

    if constexpr (1)
    {
        for (auto i = 0u; i < 14u; ++i)
        {
            sp_log("%d", i);
            auto *quad = this->field_38[i];
            assert(quad != nullptr);

            quad->TurnOn(true);
        }
    }
    else
    {
        THISCALL(0x00614110, this);
    }
}

void unlockables_menu_patch() {

    {
        FUNC_ADDRESS(address, &unlockables_menu::_Load);
        set_vfunc(0x00894918, address);
    }
	{
        FUNC_ADDRESS(address, &unlockables_menu::OnCross);
        set_vfunc(0x00894954, address);
    }
    return;


    {
        FUNC_ADDRESS(address, &unlockables_menu::OnTriangle);
        //set_vfunc(0x00894958, address);
    }

    {
        FUNC_ADDRESS(address, &unlockables_menu::OnActivate);
        set_vfunc(0x00894934, address);
    }
}
