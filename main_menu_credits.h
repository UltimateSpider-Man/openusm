#pragma once

#include "femenu.h"

#include "cursor.h"

#include "fe_game_credits.h"

struct main_menu_credits : FEMenu {
    fe_game_credits field_2C;
    FEMenuSystem *a2;

    main_menu_credits(FEMenuSystem *a2, int a3, int a4);
	
Cursor OnActivate();



int OnDeactivate(FEMenu* a2);





};


extern void main_menu_credits_patch();