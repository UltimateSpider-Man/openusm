#include "pause_menu_goals.h"

#include "game.h"
#include "game_settings.h"

#include "common.h"


#include "mission_manager.h"


#include "localized_string_table.h"


#include "func_wrapper.h"

VALIDATE_SIZE(pause_menu_goals, 0x13cu);


pause_menu_goals::pause_menu_goals(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x0060F820, this, a2, a3, a4);
}

mString pause_menu_goals::Init() {
    return (mString)THISCALL(0x0060F980, this);
}

mString pause_menu_goals::get_element_value(mString *out ,int a2) {
    return (mString)THISCALL(0x0060FB00, this,out,a2);
}


mString pause_menu_goals::get_element_desc(int a2) {
    return (mString)THISCALL(0x0060FCD0, this, a2);
}


void pause_menu_goals_patch()
{

    {
        FUNC_ADDRESS(address, &pause_menu_goals::get_element_desc);
     //   REDIRECT(0x006102D0, address);

    }
	
    {
        FUNC_ADDRESS(address, &pause_menu_goals::Init);
     //   REDIRECT(0x00610626, address);

    }
	
    {
        FUNC_ADDRESS(address, &pause_menu_goals::get_element_value);
     //  REDIRECT(0x00610250, address);
    }
	
	    {
	//REDIRECT(0x00610111, &pause_menu_goals::pause_menu_goals);
	
	    }
  
} 