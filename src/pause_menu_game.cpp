#include "pause_menu_game.h"
#include "game.h"
#include "common.h"
#include "localized_string_table.h"

VALIDATE_SIZE(pause_menu_game, 0x2B0u);


mString* pause_menu_game::Init()
{
    localized_string_table* stringTable = g_game_ptr->field_7C;
    auto& strings = stringTable->field_0->field_0;
    
    // Initialize field_0
    field_0 = strings[223];
    
    // Initialize field_10 array (indices 0-20)
    const int field_10_indices[] = {
        224, 225, 226, 227, 189, 190, 191, 192, 193, 194, 195,
        196, 197, 198, 199, 200, 201, 202, 203, 204, 205
    };
    
    for (int i = 0; i < 21; ++i) {
        field_10[i] = strings[field_10_indices[i]];
    }
    
    // Initialize field_160 array (indices 0-20)
    const int field_160_indices[] = {
        230, 231, 232, 233, 206, 207, 208, 209, 210, 211, 212,
        213, 214, 215, 216, 217, 218, 219, 220, 221, 222
    };
    
    for (int i = 0; i < 21; ++i) {
        field_160[i] = strings[field_160_indices[i]];
    }
    
    return &field_160[20];
}

