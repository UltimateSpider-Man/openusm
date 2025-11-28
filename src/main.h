#pragma once




struct debug_menu_root {

static void menu_setup(int game_state, int keyboard);

static void render_current_debug_menu();

};

extern debug_menu_root* & menu_ptr;