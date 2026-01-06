#pragma once

#include "debug_menu.h"


struct debug_menu_root {






};

extern debug_menu_root* & menu_ptr;

extern void hero_toggle_handler(debug_menu_entry* entry);


extern void hero_entry_callback(debug_menu_entry*);

extern void menu_setup(int game_state, int keyboard);

extern void menu_input_handler(int keyboard, int SCROLL_SPEED);

extern void render_current_debug_menu();

extern void Init2();