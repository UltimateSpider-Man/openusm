#pragma once
#include "game.h"
#if DEBUG_MENU_REIMPL == 0


static constexpr auto NUM_HEROES = 10;

extern debug_menu* level_select_menu;
extern debug_menu* hero_select_menu;
extern int hero_status;
extern int hero_selected;
extern const char* hero_list[];

void create_level_select_menu(debug_menu*);
#endif