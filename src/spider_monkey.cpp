#include "spider_monkey.h"

#include "vector2di.h"
#include "ngl.h"
#include "ngl_font.h"
#include "func_wrapper.h"
#include "input_mgr.h"
#include "log.h"
#include "os_developer_options.h"
#include "trace.h"
#include "utility.h"
#include "render_text.h"


float spider_monkey::state_callback(int a1) {
    return spider_monkey::m_game_control_state()[a1];
}

float spider_monkey::delta_callback(int a1) {
    return spider_monkey::m_game_control_state()[a1] -
        spider_monkey::m_game_control_state_last_frame()[a1];
}

#include "game.h"

void spider_monkey::render()
{
    CDECL_CALL(0x004B6890);

    {
        g_game_ptr->mb->render();
    }

    {
        if (os_developer_options::instance->get_flag(mString{ "SHOW_DEBUG_INFO" }))
        {
            g_game_ptr->show_debug_info();
        }

        if (os_developer_options::instance->get_flag(mString{ "SHOW_FPS" }))
        {
            auto v40 = (g_game_ptr->field_278 == 0 ? 0.f : (1.f / g_game_ptr->field_278));
            auto v39 = (
                ((g_game_ptr->field_278 - g_game_ptr->field_27C) - g_game_ptr->field_280) >= 0.000099999997
                ? (1.0 / (g_game_ptr->field_278 - g_game_ptr->field_27C - g_game_ptr->field_280))
                : 10000.0f);

            static float dword_1584E70 = 0;
            if (v39 > dword_1584E70)
            {
                dword_1584E70 = v39;
            }

            static float dword_14EEB94 = 500;
            if (dword_14EEB94 > v39)
            {
                dword_14EEB94 = v39;
            }

            color32 v38{ 0 };
            if (v40 >= 14.2)
            {
                if (v40 >= 29.4)
                {
                    if (v40 >= 59.900002)
                    {
                        color32 v5{ 80, 255, 80, 255 };
                        v38 = v5;
                    }
                    else
                    {
                        color32 v4{ 255, 255, 32, 255 };
                        v38 = v4;
                    }
                }
                else
                {
                    color32 v3{ 255, 128, 40, 255 };
                    v38 = v3;
                }
            }
            else
            {
                color32 v2{ 255, 33, 44, 255 };
                v38 = v2;
            }

            char a1a[200]{};
            sprintf(a1a, "%2d fps (%2d.%1d)", (int)v40, (int)v39, (int)(float)(v39 * 10.0) % 10);
            auto a5 = ((int)v38.field_0[0] >> 4) | ((int)v38.field_0[1] >> 4 << 8) | ((int)v38.field_0[2] >> 4 << 16) | 0xFF000000;
            nglListAddString(nglSysFont(), 19.0, 424.0, 1.01, a5, 0.80000001, 0.80000001, a1a);
            nglListAddString(nglSysFont(), 21.0, 426.0, 1.01, a5, 0.80000001, 0.80000001, a1a);
            nglListAddString(
                nglSysFont(),
                20.0,
                425.0,
                1.0,
                v38.field_0[0] | (v38.field_0[1] << 8) | (v38.field_0[2] << 16) | 0xFF000000,
                0.80000001,
                0.80000001,
                a1a);

            if (god_mode_cheat())
            {
                if (ultra_god_mode_cheat())
                {
                    mString v16{ "Ultra-God mode" };
                    auto v11 = v38;
                    vector2di v6{ 20, 110 };
                    render_text(v16, v6, v11, 1.0, 0.5);
                }
                else if (mega_god_mode_cheat())
                {
                    mString v17{ "Mega-God mode" };
                    auto v12 = v38;
                    vector2di v7{ 20, 110 };
                    render_text(v17, v7, v12, 1.0, 0.5);
                }
                else
                {
                    mString v18{ "God mode" };
                    auto v13 = v38;
                    vector2di v8{ 20, 110 };
                    render_text(v18, v8, v13, 1.0, 0.5);
                }
            }
            else if (ultra_god_mode_cheat())
            {
                mString v19{ "Ultra-Mortal mode" };
                auto v14 = v38;
                vector2di v9{ 20, 110 };
                render_text(v19, v9, v14, 1.0, 0.5);
            }
            else if (mega_god_mode_cheat())
            {
                mString v20{ "Mega-Mortal mode" };
                auto v15 = v38;
                vector2di v10{ 20, 110 };
                render_text(v20, v10, v15, 1.0, 0.5);
            }
        }
    }
}

void spider_monkey::on_level_load() {
    CDECL_CALL(0x004B3910);
}

void spider_monkey::on_level_unload() {
    CDECL_CALL(0x004B3B20);
}

void spider_monkey::start()
{
    if constexpr (1)
    {
        input_mgr::instance->set_control_state_monkey_callback(spider_monkey::state_callback);
        input_mgr::instance->set_control_delta_monkey_callback(spider_monkey::delta_callback);
        m_running() = 1;
        m_ook_timer() = 0.0;
        m_clock().reset();
        m_runtime() = 0.0;
        m_runtime_text() = 0;
        m_runtime_monkey_text() = 0;
        os_developer_options::instance->set_int(9, 1); //MONKEY_MODE
    } else {
        CDECL_CALL(0x004B6690);
    }
}

void spider_monkey::stop() {
}

void spider_monkey::frame_advance(Float a1)
{
    TRACE("spider_monkey::frame_advance");

    CDECL_CALL(0x004B6770, a1);
}

bool spider_monkey::is_running() {
    //sp_log("spider_monkey::is_running(): %d", spider_monkey::m_running());

    return spider_monkey::m_running();
}

void spider_monkey_patch()
{
    {
        REDIRECT(0x0055D761, spider_monkey::frame_advance);
    }

    REDIRECT(0x0052B4BF, spider_monkey::render);

    REDIRECT(0x0052B5DC, spider_monkey::is_running);
}
