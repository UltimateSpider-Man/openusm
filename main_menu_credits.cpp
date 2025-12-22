#include "main_menu_credits.h"

#include "common.h"
#include "func_wrapper.h"

VALIDATE_SIZE(main_menu_credits, 0x48);

main_menu_credits::main_menu_credits(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00625360, this, a2, a3, a4);
}

Cursor main_menu_credits::OnActivate()
{

  //movie_manager::load_and_play_movie("credits", nullptr, false);
  

  
    THISCALL(0x006334E0, this);
}


int main_menu_credits::OnDeactivate(FEMenu* a2)
{
	
    
    THISCALL(0x006253A0,this, a2);

}

void main_menu_credits_patch() {

    {
        FUNC_ADDRESS(address, &main_menu_credits::OnActivate);
    //    set_vfunc(0x00894884, address);
    }
	{
        FUNC_ADDRESS(address, &main_menu_credits::OnDeactivate);
   //     set_vfunc(0x00894888, address);
    }
    return;


}