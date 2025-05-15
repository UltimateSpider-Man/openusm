#include "levelmenu.h"

#include "debug_menu.h"
#include "resource_manager.h"
#include "common.h"
#include "game_level.h"

#include <cassert>

debug_menu* level_select_menu = nullptr;
debug_menu* hero_select_menu = nullptr;

int hero_status;
int hero_selected;

const char* hero_list[] = { "arachno_man_costume" };


level_descriptor_t *get_level_descriptors(int *)
{
    return nullptr;
}

void level_select_handler(debug_menu_entry *)
{}

void reboot_handler(debug_menu_entry *)
{}

void hero_entry_callback(debug_menu_entry *)
{
}

void hero_toggle_handler(debug_menu_entry *entry)
{
    assert(entry->get_id() < NUM_HEROES);

    hero_selected = entry->get_id();
    hero_status = 1;
}
void create_level_select_menu(debug_menu *)
{
    assert(debug_menu::root_menu != nullptr);
    
    level_select_menu = new debug_menu{"Level Select", (DWORD)debug_menu::sort_mode_t::undefined};

    debug_menu_entry *v15 = new debug_menu_entry{level_select_menu};

    debug_menu::root_menu->add_entry(v15);

    int arg0;
    auto *level_descriptors = get_level_descriptors(&arg0);
    for ( auto i = 0; i < arg0; ++i )
    {
        resource_key_type v6 = RESOURCE_KEY_TYPE_PACK;
        auto *v1 = level_descriptors[i].field_0.to_string();
        string_hash v5{v1};
        auto v11 = resource_key{v5, v6};
        auto v17 = resource_manager::get_pack_file_stats(v11, nullptr, nullptr, nullptr);
        if ( v17 )
        {
            auto *v3 = level_descriptors[i].field_60.to_string();
            mString v22{v3};
            auto *v39 = new debug_menu_entry{v22};

            v39->set_game_flags_handler(level_select_handler);
            v39->set_id(i);
            level_select_menu->add_entry(v39);
        }
    }

    mString v25{"-- REBOOT --"};
    auto *v38 = new debug_menu_entry{v25};

    v38->set_game_flags_handler(reboot_handler);
    level_select_menu->add_entry(v38);

    hero_select_menu = new debug_menu{"Hero Select", (DWORD)debug_menu::sort_mode_t::undefined };

    auto *v28 = new debug_menu_entry {hero_select_menu};

    level_select_menu->add_entry(v28);
    for ( auto i = 0u; i < 10u; ++i )
    {
        resource_key_type v6 = RESOURCE_KEY_TYPE_PACK;
        string_hash v5{(hero_list)[i]};
        auto v11 = resource_key{v5, v6};
        auto v30 = resource_manager::get_pack_file_stats(v11, nullptr, nullptr, nullptr);
        if ( v30 )
        {
            mString v35 {hero_list[i]};
            auto *v37 = new debug_menu_entry{v35};

            v37->set_game_flags_handler(hero_toggle_handler);
            v37->set_id(i);
            v37->set_frame_advance_cb(hero_entry_callback);
            hero_select_menu->add_entry(v37);
        }
    }
}