#pragma once

#include "game.h"
#include "message_board.h"
#include "os_developer_options.h"

#include "float.hpp"
#include "limited_timer.h"
#include "variable.h"
#include "debug_menu.h"

inline Var<bool> god_mode_cheat{ 0x0095A6A8 };
inline Var<bool> ultra_god_mode_cheat{ 0x0095A6A9 };
inline Var<bool> mega_god_mode_cheat{ 0x0095A6AA };

struct spider_monkey {
    spider_monkey();

    //0x004B38E0
    static float state_callback(int a1);

    //0x004B38F0
    static float delta_callback(int a1);

    //0x004B3910
    static void on_level_load();

    //0x004B3B20
    static void on_level_unload();

    //0x004B6690
    static void start();

   static void stop();


    //0x004B6890
    static void render();

    //0x004B6770
    static void frame_advance(Float a1);

    //0x004B3B60
    static bool is_running();

    static inline Var<bool> m_running{0x00959E60};

    static inline Var<float> m_ook_timer{0x00959E64};

    static inline Var<limited_timer> m_clock{0x00959FE4};

    static inline Var<float> m_runtime{0x00959E6C};

    static inline Var<int> m_runtime_text{0x00959E70};
    static inline Var<int> m_runtime_monkey_text{0x00959E74};

    static inline Var<float[120]> m_game_control_state{0x00959C58};

    static inline Var<float[120]> m_game_control_state_last_frame{0x00959A78};
};

extern void spider_monkey_patch();
